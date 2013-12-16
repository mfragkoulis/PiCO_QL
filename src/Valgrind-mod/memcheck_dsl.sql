#include "pub_tool_hashtable.h"     // VgHashTable
#include "pub_tool_oset.h"          // OSet
#include "pub_tool_execontext.h"    // ExeContext
#include "pub_tool_basics.h"        // VG_WORDSIZE

#define MemProfileVT_decl(X) MC_Chunk* X; 
#define MemProfileVT_begin(X,Y) X = (MC_Chunk *)VG_(HT_Next)(Y)
#define MemProfileVT_advance(X,Y) X = (MC_Chunk *)VG_(HT_Next)(Y)
#define MemProfileVT_end(X) X != NULL

#define AddrVAbitsVT_decl(X) Addr X
#define AddrVAbitsVT_begin(X,Y,Z) X = Y->data, Z = Y->data + Y->szB
#define AddrVAbitsVT_advance(X) X += 4     // each VAbits entry tracks the state of 4 Bytes. So hopping 4 Bytes after each iteration.
#define AddrVAbitsVT_end(X,Y) X < Y

#define IPVT_decl(X) Addr* X;int i = 0 
#define IPVT_begin(X,Y,Z) X = &Y[Z]
#define IPVT_advance(X,Y,Z) X = &Y[Z]
#define IPVT_end(X, Y) X != Y

#define SecMapVT_decl(X) int *X; int i = 0
#define SecMapVT_begin(X,Y,Z) X = (int *)&Y[Z]
#define SecMapVT_advance(X,Y,Z) X = (int *)&Y[Z]
#define SecMapVT_end(X,Y) X != Y

#define PriMapVT_decl(X) SecMap* X; int i = 0
#define PriMapVT_begin(X,Y,Z) X = Y[Z]
#define PriMapVT_advance(X,Y,Z) X = Y[Z]
#define PriMapVT_end(X,Y) X != Y

#define AuxPriL1MapVT_decl(X) auxmap_L1* X; int i = 0
#define AuxPriL1MapVT_begin(X,Y,Z) X = &Y[Z]
#define AuxPriL1MapVT_advance(X,Y,Z) X = &Y[Z]
#define AuxPriL1MapVT_end(X,Y) X != Y

#define AuxPriL2MapVT_decl(X) AuxMapEnt* X;
#define AuxPriL2MapVT_begin(X,Y) X = (AuxMapEnt *)VG_(OSetGen_Next)(Y)
#define AuxPriL2MapVT_advance(X,Y) X = (AuxMapEnt *)VG_(OSetGen_Next)(Y)
#define AuxPriL2MapVT_end(X) X != NULL
 
#define VBitVT_decl(X) int X;int i = 0 
#define VBitVT_begin(X,Y,Z) X = Y[Z]
#define VBitVT_advance(X,Y,Z) X = Y[Z]
#define VBitVT_end(X, Y) X < Y

#define SecVBitNodeVT_decl(X) SecVBitNode* X;
#define SecVBitNodeVT_begin(X,Y) X = (SecVBitNode *)VG_(OSetGen_Next)(Y)
#define SecVBitNodeVT_advance(X,Y) X = (SecVBitNode *)VG_(OSetGen_Next)(Y)
#define SecVBitNodeVT_end(X) X != NULL
 
#define VA_BITS2_NOACCESS     0x0      // 00b
#define VA_BITS2_UNDEFINED    0x1      // 01b
#define VA_BITS2_DEFINED      0x2      // 10b
#define VA_BITS2_PARTDEFINED  0x3      // 11b

#define V_BITS8_DEFINED       0
#define V_BITS8_UNDEFINED     0xFF

#define SM_DIST_NOACCESS   0
#define SM_DIST_UNDEFINED  1
#define SM_DIST_DEFINED    2

#define SM_OFF(aaa)           (((aaa) & 0xffff) >> 2)
#define SM_CHUNKS             16384
#if VG_WORDSIZE == 4

/* cover the entire address space */
#  define N_PRIMARY_BITS  16

#else

/* Just handle the first 64G fast and the rest via auxiliary
   primaries.  If you change this, Memcheck will assert at startup.
   See the definition of UNALIGNED_OR_HIGH for extensive comments. */
#  define N_PRIMARY_BITS  20

#endif


/* Do not change this. */
#define N_PRIMARY_MAP  ( ((UWord)1) << N_PRIMARY_BITS)
#define MAX_PRIMARY_ADDRESS (Addr)((((Addr)65536) * N_PRIMARY_MAP)-1)

typedef
   enum {
      MC_AllocMalloc = 0,
      MC_AllocNew    = 1,
      MC_AllocNewVec = 2,
      MC_AllocCustom = 3
   }
   MC_AllocKind;

struct _ExeContext {
   struct _ExeContext* chain;
   /* A 32-bit unsigned integer that uniquely identifies this
      ExeContext.  Memcheck uses these for origin tracking.  Values
      must be nonzero (else Memcheck's origin tracking is hosed), must
      be a multiple of four, and must be unique.  Hence they start at
      4. */
   UInt ecu;
   /* Variable-length array.  The size is 'n_ips'; at
      least 1, at most VG_DEEPEST_BACKTRACE.  [0] is the current IP,
      [1] is its caller, [2] is the caller of [1], etc. */
   UInt n_ips;
   Addr ips[0];
};

typedef
   struct _MC_Chunk {
      struct _MC_Chunk* next;
      Addr         data;            // Address of the actual block.
      SizeT        szB : (sizeof(SizeT)*8)-2; // Size requested; 30 or 62 bits.
      MC_AllocKind allockind : 2;   // Which operation did the allocation.
      ExeContext*  where[0];
      /* Variable-length array. The size depends on MC_(clo_keep_stacktraces).
         This array optionally stores the alloc and/or free stack trace. */
   }
   MC_Chunk;

typedef
   struct {
      UChar vabits8[SM_CHUNKS];
   }
   SecMap;

typedef
   struct {
      Addr    base;
      SecMap* sm;
   }
   AuxMapEnt;

#define N_AUXMAP_L1 24
/* Tunable parameter: How big is the L1 queue?*/
typedef
   struct {
          Addr       base;
          AuxMapEnt* ent; // pointer to the matching auxmap_L2 node
       }
       auxmap_L1;

#define BYTES_PER_SEC_VBIT_NODE     16
typedef
   struct {
      Addr  a;
      UChar vbits8[BYTES_PER_SEC_VBIT_NODE];
   }
   SecVBitNode;


static long sm_offset(Addr base) {
  base &= ~(Addr)0xFFFF;
  return base;
};

static int inPrim(Addr base) {
  if (base <= MAX_PRIMARY_ADDRESS) return 1;
  else return 0;
};

static short getVAbits(Addr base) {
  UChar vabits = -1;
  if (inPrim(base)) {
    UWord pm_off = base >> 16;
    UWord sm_off = SM_OFF(base);
#if VG_DEBUG_MEMORY >= 1
    tl_assert(pm_off < N_PRIMARY_MAP);
#endif
    SecMap *sm = pqlPub_primary_map[ pm_off ];
    vabits = sm->vabits8[ sm_off ];
/*  char m[100];
    sprintf(m, "%d", (int)vabits);
    printf("returning vabits8: %c (%s)\n", vabits, m);*/
  } else {
    AuxMapEnt  key;
    AuxMapEnt* res;
    Word       i;
    tl_assert(base > MAX_PRIMARY_ADDRESS);
    base &= ~(Addr)0xFFFF;
    for (i = 0; i < N_AUXMAP_L1; i++) {
      if (pqlPub_aux_primary_L1_map[i].base == base) {
        break;
      }
    }
    if (i < N_AUXMAP_L1) {
      res = pqlPub_aux_primary_L1_map[i].ent;
    } else {
      key.base = base;
      key.sm   = 0;
      res = VG_(OSetGen_Lookup)(pqlPub_aux_primary_L2_map, &key);
    }
    if ((res) && (res->sm) && (res->sm != &pqlPub_distinguished_sec_map[SM_DIST_NOACCESS])) {
      UWord sm_off = SM_OFF(base);
      vabits = res->sm->vabits8[ sm_off ];
    }
  }
  return vabits;
};

static short extract_vabits2(Addr base, UChar vabits8) {
   UInt shift = (base & 3) << 1;          // shift by 0, 2, 4, or 6
   vabits8 >>= shift;                     // shift the two bits to the bottom
/*   char m[100];
   sprintf(m, "In extract: for addr %ld returning vabits2 %d.\n", (long)base, (int)(0x3 & vabits8));
   printf("%s", m);*/
   return 0x3 & vabits8;                  // mask out the rest
};

static Addr addrSize;
static short getVAbits2(Addr base, int indexB) {
  if (base + indexB >= addrSize) return -1;
  UChar vabits8 = getVAbits(base);
/*  char m[100];
  sprintf(m, "vabits8 in 1B is: %d, addr is %ld, threshold size is %ld.\n", (int)vabits8, (long)base, (long)addrSize);
  printf("%s", m);*/
  return extract_vabits2(base + indexB, vabits8);
};

static short getVbits8(Addr base, int indexB) {
  short vabits2 = getVAbits2(base, indexB);
  if (vabits2 == VA_BITS2_PARTDEFINED) {
    Addr aAligned = VG_ROUNDDN(base, BYTES_PER_SEC_VBIT_NODE);
    Int amod     = base % BYTES_PER_SEC_VBIT_NODE;
    SecVBitNode* n = VG_(OSetGen_Lookup)(pqlPub_sec_vbit_table, &aAligned);
    UChar vbits8;
    tl_assert2(n, "get_sec_vbits8: no node for address %p (%p)\n", aAligned, base);
    /* Shouldn't be fully defined or fully undefined -- those cases shouldn't
     * make it to the secondary V bits table.
     */
    vbits8 = n->vbits8[amod];
//    tl_assert(V_BITS8_DEFINED != vbits8 && V_BITS8_UNDEFINED != vbits8);
    return vbits8;
  }
  return -1;
};

$

CREATE STRUCT VIEW MemProfileV (
	addr_data BIGINT FROM data,
	inPrim BIGINT FROM inPrim(tuple_iter->data),
	FOREIGN KEY(vabits_id) FROM tuple_iter REFERENCES AddrVAbitsVT POINTER,
	sizeB BIGINT FROM szB,
	allocKind INT FROM allockind,
	excnt_alloc_id INT FROM where[0]->ecu,
	FOREIGN KEY(ec_alloc_ips_id) FROM where[0] REFERENCES IPVT POINTER,
	excnt_free_id INT FROM where[1]->ecu,
	FOREIGN KEY(ec_free_ips_id) FROM where[1] REFERENCES IPVT POINTER
)$

CREATE VIRTUAL TABLE MemProfileVT
USING STRUCT VIEW MemProfileV
WITH REGISTERED C NAME malloc_list
WITH REGISTERED C TYPE VgHashTable:MC_Chunk*
USING LOOP VG_(HT_ResetIter)(*base);for (MemProfileVT_begin(tuple_iter, *base);MemProfileVT_end(tuple_iter);MemProfileVT_advance(tuple_iter, *base))$

CREATE STRUCT VIEW AddrVAbitsV (
	addr_data BIGINT FROM tuple_iter,
	inPrim BIGINT FROM inPrim(tuple_iter),
	vabits INT FROM getVAbits(tuple_iter),
	vabits_1B INT FROM {getVAbits2(tuple_iter, 0)},
	vbits_1B INT FROM {getVbits8(tuple_iter, 0)},
	vabits_2B INT FROM {getVAbits2(tuple_iter, 1)},
	vbits_2B INT FROM {getVbits8(tuple_iter, 1)},
	vabits_3B INT FROM {getVAbits2(tuple_iter, 2)},
	vbits_3B INT FROM {getVbits8(tuple_iter, 2)},
	vabits_4B INT FROM {getVAbits2(tuple_iter, 3)},
	vbits_4B INT FROM {getVbits8(tuple_iter, 3)}
)$

CREATE VIRTUAL TABLE AddrVAbitsVT
USING STRUCT VIEW AddrVAbitsV
WITH REGISTERED C TYPE MC_Chunk*:Addr
USING LOOP for (AddrVAbitsVT_begin(tuple_iter, base, addrSize); AddrVAbitsVT_end(tuple_iter, addrSize); AddrVAbitsVT_advance(tuple_iter))$
// addrSize is static global variable defined above in the boilerplate part of the DSL

CREATE STRUCT VIEW IPV (
	addr_data BIGINT FROM tuple_iter
//	execnt_id INT FROM ecu
)$

CREATE VIRTUAL TABLE IPVT
USING STRUCT VIEW IPV
WITH REGISTERED C TYPE ExeContext:Addr*
USING LOOP for(IPVT_begin(tuple_iter, base->ips, i); IPVT_end(i, base->n_ips); IPVT_advance(tuple_iter, base->ips, ++i))$

CREATE STRUCT VIEW SecMapV (
	vabits INT FROM tuple_iter
)$

CREATE VIRTUAL TABLE SecMapVT
USING STRUCT VIEW SecMapV
WITH REGISTERED C TYPE SecMap:int*
USING LOOP for(SecMapVT_begin(tuple_iter, base->vabits8, i); SecMapVT_end(i, SM_CHUNKS); SecMapVT_advance(tuple_iter, base->vabits8, ++i))$

CREATE STRUCT VIEW PriMapV (
	FOREIGN KEY(secmap_id) FROM tuple_iter REFERENCES SecMapVT POINTER,
//	addr_off BIGINT FROM TODO
)$

CREATE VIRTUAL TABLE PriMapVT
USING STRUCT VIEW PriMapV
WITH REGISTERED C NAME primary_map
WITH REGISTERED C TYPE SecMap**:SecMap*
USING LOOP for(PriMapVT_begin(tuple_iter, base, i); PriMapVT_end(i, N_PRIMARY_MAP); PriMapVT_advance(tuple_iter, base, ++i))$

CREATE STRUCT VIEW AuxMapEntryV (
	addr_data BIGINT FROM base
)$

CREATE VIRTUAL TABLE AuxMapEntryVT
USING STRUCT VIEW AuxMapEntryV
WITH REGISTERED C TYPE AuxMapEnt$

CREATE STRUCT VIEW AuxPriMapV (
	addr_data BIGINT FROM sm_offset(tuple_iter->base),
	entry_addr_data BIGINT FROM ent->base
)$

CREATE VIRTUAL TABLE AuxPriL1MapVT
USING STRUCT VIEW AuxPriMapV
WITH REGISTERED C NAME aux_primary_L1_map
WITH REGISTERED C TYPE auxmap_L1
USING LOOP for(AuxPriL1MapVT_begin(tuple_iter,base, i); AuxPriL1MapVT_end(i, N_AUXMAP_L1); AuxPriL1MapVT_advance(tuple_iter, base, ++i))$

CREATE VIRTUAL TABLE AuxPriL2MapVT
USING STRUCT VIEW AuxMapEntryV
WITH REGISTERED C NAME aux_primary_L2_map
WITH REGISTERED C TYPE OSet:AuxMapEnt*
USING LOOP VG_(OSetGen_ResetIter)(base);for (AuxPriL2MapVT_begin(tuple_iter, base);AuxPriL2MapVT_end(tuple_iter);AuxPriL2MapVT_advance(tuple_iter, base))$

CREATE STRUCT VIEW DistinguishedSecMapV (
	FOREIGN KEY(no_access_sm_id) FROM tuple_iter REFERENCES SecMapVT POINTER
	//FOREIGN KEY(undefined_sm_id) FROM {tuple_iter[1]} REFERENCES SecMapVT,
	//FOREIGN KEY(defined_sm_id) FROM {tuple_iter[2]} REFERENCES SecMapVT
)$

CREATE VIRTUAL TABLE DistinguishedSecMapVT
USING STRUCT VIEW DistinguishedSecMapV
WITH REGISTERED C NAME distinguished_sec_map
WITH REGISTERED C TYPE SecMap$

CREATE STRUCT VIEW VBitV (
	vbit INT FROM tuple_iter
)$

CREATE VIRTUAL TABLE VBitVT
USING STRUCT VIEW VBitV
WITH REGISTERED C TYPE int*:int
USING LOOP for(VBitVT_begin(tuple_iter,base, i); VBitVT_end(i, BYTES_PER_SEC_VBIT_NODE); VBitVT_advance(tuple_iter, base, ++i))$

CREATE STRUCT VIEW SecVBitNodeV (
	addr_data BIGINT FROM a,
	FOREIGN KEY(vbits_id) FROM vbits8 REFERENCES VBitVT POINTER
)$

CREATE VIRTUAL TABLE SecVBitNodeVT
USING STRUCT VIEW SecVBitNodeV
WITH REGISTERED C NAME sec_vbit_table
WITH REGISTERED C TYPE OSet:SecVBitNode*
USING LOOP VG_(OSetGen_ResetIter)(base);for (SecVBitNodeVT_begin(tuple_iter, base);SecVBitNodeVT_end(tuple_iter);SecVBitNodeVT_advance(tuple_iter, base))$

CREATE VIEW VAbitTags AS
	SELECT base, addr_data, inPrim, vabits,
        	(SELECT case WHEN vabits_1B = 0 THEN 'noaccess'
		     	WHEN vabits_1B = 1 THEN 'undefined'
		     	WHEN vabits_1B = 2 THEN 'defined'
		     	WHEN vabits_1B = 3 THEN 'partdefined' end) VATag_1B,
        	(SELECT case WHEN vbits_1B = 0 THEN 'defined'
		     	WHEN vbits_1B = -1 THEN 'undefined'
		     	ELSE vbits_1B end) VTag_1B,
        	(SELECT case WHEN vabits_2B = 0 THEN 'noaccess'
		 	WHEN vabits_2B = 1 THEN 'undefined'
		     	WHEN vabits_2B = 2 THEN 'defined'
		     	WHEN vabits_2B = 3 THEN 'partdefined' end) VATag_2B,
        	(SELECT case WHEN vbits_2B = 0 THEN 'defined'
		     	WHEN vbits_2B = -1 THEN 'undefined'
		     	ELSE vbits_2B end) VTag_2B,
        	(SELECT case WHEN vabits_3B = 0 THEN 'noaccess'
		     	WHEN vabits_3B = 1 THEN 'undefined'
		     	WHEN vabits_3B = 2 THEN 'defined'
		     	WHEN vabits_3B = 3 THEN 'partdefined' end) VATag_3B,
        	(SELECT case WHEN vbits_3B = 0 THEN 'defined'
		     	WHEN vbits_3B = -1 THEN 'undefined'
		     	ELSE vbits_3B end) VTag_3B,
        	(SELECT case WHEN vabits_4B = 0 THEN 'noaccess'
		     	WHEN vabits_4B = 1 THEN 'undefined'
		     	WHEN vabits_4B = 2 THEN 'defined'
		     	WHEN vabits_4B = 3 THEN 'partdefined' end) VATag_4B,
        	(SELECT case WHEN vbits_4B = 0 THEN 'defined'
		     	WHEN vbits_4B = -1 THEN 'undefined'
		     	ELSE vbits_4B end) VTag_4B
	FROM AddrVAbitsVT;
	
