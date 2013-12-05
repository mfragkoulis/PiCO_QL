#include "pub_tool_hashtable.h"     // VgHashTable
#include "pub_tool_oset.h"          // OSet
#include "pub_tool_execontext.h"    // ExeContext
#include "pub_tool_basics.h"        // VG_WORDSIZE

#define MemCheckVT_decl(X) MC_Chunk* X; 
#define MemCheckVT_begin(X,Y) X = (MC_Chunk *)VG_(HT_Next)(Y)
#define MemCheckVT_advance(X,Y) X = (MC_Chunk *)VG_(HT_Next)(Y)
#define MemCheckVT_end(X) X != NULL

#define IPVT_decl(X) Addr* X;int i = 0 
#define IPVT_begin(X,Y,Z) X = &Y[Z]
#define IPVT_advance(X,Y,Z) X = &Y[Z]
#define IPVT_end(X, Y) X != Y

#define SecMapVT_decl(X) UChar* X; int i = 0
#define SecMapVT_begin(X,Y,Z) X = &Y[Z]
#define SecMapVT_advance(X,Y,Z) X = &Y[Z]
#define SecMapVT_end(X,Y) X != Y

#define PriMapVT_decl(X) SecMap* X; int i = 0
#define PriMapVT_begin(X,Y,Z) X = &Y[Z]
#define PriMapVT_advance(X,Y,Z) X = &Y[Z]
#define PriMapVT_end(X,Y) X != Y

#define AuxPriMapVT_decl(X) auxmap_L1* X; int i = 0
#define AuxPriMapVT_begin(X,Y,Z) X = &Y[Z]
#define AuxPriMapVT_advance(X,Y,Z) X = &Y[Z]
#define AuxPriMapVT_end(X,Y) X != Y

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

UChar* lookup_L2(Addr base) {
  AuxMapEnt *res;
  AuxMapEnt key;
  key.base = base;
  key.sm = 0;
  res = VG_(OSetGenLookup)(pqlPub_aux_primary_L2_map, &key); 
  (res == NULL) || (res->sm == NULL) ? return NULL : return res->sm->vabits8;
}
$

CREATE STRUCT VIEW MemCheckV (
	addr_data BIGINT FROM data,
	sizeB BIGINT FROM szB,
	excnt_alloc_id INT FROM where[0]->ecu,
	FOREIGN KEY(ec_alloc_ips_id) FROM where[0] REFERENCES IPVT POINTER,
	excnt_free_id INT FROM where[1]->ecu,
	FOREIGN KEY(ec_free_ips_id) FROM where[1] REFERENCES IPVT POINTER
)$

CREATE VIRTUAL TABLE MemCheckVT
USING STRUCT VIEW MemCheckV
WITH REGISTERED C NAME malloc_list
WITH REGISTERED C TYPE VgHashTable:MC_Chunk*
USING LOOP VG_(HT_ResetIter)(*base);for (MemCheckVT_begin(tuple_iter, *base);MemCheckVT_end(tuple_iter);MemCheckVT_advance(tuple_iter, *base))$

CREATE STRUCT VIEW IPV (
	addr_data BIGINT FROM tuple_iter
)$

CREATE VIRTUAL TABLE IPVT
USING STRUCT VIEW IPV
WITH REGISTERED C TYPE ExeContext:Addr*
USING LOOP for(IPVT_begin(tuple_iter, base->ips, i); IPVT_end(i, base->n_ips); IPVT_advance(tuple_iter, base->ips, ++i))$

CREATE STRUCT VIEW SecMapV (
	vabits TEXT FROM tuple_iter
)$

CREATE VIRTUAL TABLE SecMapVT
USING STRUCT VIEW SecMapV
WITH REGISTERED C TYPE UChar *
USING LOOP for(SecMapVT_begin(tuple_iter, base, i); SecMapVT_end(i, SM_CHUNKS); SecMapVT_advance(tuple_iter, base, ++i))$

CREATE STRUCT VIEW PriMapV (
	FOREIGN KEY(secmap_id) FROM vabits8 REFERENCES SecMapVT POINTER
)$

CREATE VIRTUAL TABLE PriMapVT
USING STRUCT VIEW PriMapV
WITH REGISTERED C NAME primary_map
WITH REGISTERED C TYPE SecMap
USING LOOP for(PriMapVT_begin(tuple_iter, base, i); PriMapVT_end(i, N_PRIMARY_MAP); PriMapVT_advance(tuple_iter, base, ++i))$

//CREATE STRUCT VIEW DistinguishedSecMapV (
//	FOREIGN KEY(no_access_sm_id) FROM tuple_iter[0].vabits8 REFERENCES SecMapVT,
//	FOREIGN KEY(undefined_sm_id) FROM tuple_iter[1].vabits8 REFERENCES SecMapVT,
//	FOREIGN KEY(defined_sm_id) FROM tuple_iter[2].vabits8 REFERENCES SecMapVT
//)$

//CREATE VIRTUAL TABLE DistinguishedSecMapVT
//USING STRUCT VIEW DistinguishedSecMapV
//WITH REGISTERED C NAME distinguished_sec_map
//WITH REGISTERED C TYPE SecMap$

CREATE STRUCT VIEW AuxMapEntryV (
	addr_data BIGINT FROM base,
	FOREIGN KEY(sec_id) FROM sm->vabits8 REFERENCES SecMapVT
)$

CREATE VIRTUAL TABLE AuxMapEntryVT
USING STRUCT VIEW AuxMapEntryV
WITH REGISTERED C TYPE AuxMapEnt$

CREATE STRUCT VIEW AuxPriMapV (
	addr_data BIGINT FROM base,
	entry_addr_data BIGINT FROM ent->base,
	FOREIGN KEY(ent_id) FROM ent REFERENCES AuxMapEntryVT POINTER,
	FOREIGN KEY(sm_vabits_id) FROM lookup_L2(tuple_iter->base) REFERENCES AuxMapEntryVT POINTER
)$

CREATE VIRTUAL TABLE AuxPriMapVT
USING STRUCT VIEW AuxPriMapV
WITH REGISTERED C NAME aux_primary_map
WITH REGISTERED C TYPE auxmap_L1
USING LOOP for(AuxPriMapVT_begin(tuple_iter,base, i); AuxPriMapVT_end(i, N_AUXMAP_L1); AuxPriMapVT_advance(tuple_iter, base, ++i))$

