#include "pub_tool_hashtable.h"     // VgHashTable
#include "pub_tool_execontext.h"    // ExeContext

#define MemCheckVT_decl(X) MC_Chunk* X; 
#define MemCheckVT_begin(X,Y) X = (MC_Chunk *)VG_(HT_Next)(Y)
#define MemCheckVT_advance(X,Y) X = (MC_Chunk *)VG_(HT_Next)(Y)
#define MemCheckVT_end(X) X != NULL

#define IPVT_decl(X) Addr* X;int i = 0 
#define IPVT_begin(X,Y,Z) X = &Y[Z]
#define IPVT_advance(X,Y,Z) X = &Y[Z]
#define IPVT_end(X, Y) X != Y

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
