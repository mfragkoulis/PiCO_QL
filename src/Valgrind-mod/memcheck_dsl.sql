#include "pub_tool_hashtable.h"     // VgHashTable
#include "pub_tool_oset.h"          // OSet
#include "pub_tool_execontext.h"    // ExeContext
#include "pub_tool_basics.h"        // VG_WORDSIZE

#define MemProfileVT_decl(X) MC_Chunk* X; 
#define MemProfileVT_begin(X,Y) X = (MC_Chunk *)VG_(HT_Next)(Y)
#define MemProfileVT_advance(X,Y) X = (MC_Chunk *)VG_(HT_Next)(Y)
#define MemProfileVT_end(X) X != NULL

#define IPVT_decl(X) Addr* X;int i = 0 
#define IPVT_begin(X,Y,Z) X = &Y[Z]
#define IPVT_advance(X,Y,Z) X = &Y[Z]
#define IPVT_end(X, Y) X != Y

#define SecMapVT_decl(X) int *X; int i = 0
#define SecMapVT_begin(X,Y,Z) X = &Y[Z]
#define SecMapVT_advance(X,Y,Z) X = &Y[Z]
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

static long sm_offset(Addr base) {
  base &= ~(Addr)0xFFFF;
  return base;
};

static int inPrim(Addr base) {
  if (base <= MAX_PRIMARY_ADDRESS) return 1;
  else return 0;
};

static short getVAbits8InPrim(Addr base) {
  if (inPrim(base)) {
    UWord pm_off = base >> 16;
    UWord sm_off = SM_OFF(base);
#if VG_DEBUG_MEMORY >= 1
    tl_assert(pm_off < N_PRIMARY_MAP);
#endif
    SecMap *sm = pqlPub_primary_map[ pm_off ];
    UChar vabits = sm->vabits8[ sm_off ];
    printf("returning vabits8: %c.\n", vabits);
    return vabits;
  } else {
    return -1;
  }
};
$

CREATE STRUCT VIEW MemProfileV (
	addr_data BIGINT FROM data,
	inPrim BIGINT FROM inPrim(tuple_iter->data),
	vabits8 INT FROM getVAbits8InPrim(tuple_iter->data),
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

CREATE STRUCT VIEW IPV (
	addr_data BIGINT FROM tuple_iter
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

