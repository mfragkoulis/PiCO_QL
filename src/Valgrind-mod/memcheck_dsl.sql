#include "pub_tool_hashtable.h"     // VgHashTable

#define MemCheckVT_decl(X) MC_Chunk* X; 
#define MemCheckVT_begin(X,Y) X = (MC_Chunk *)VG_(HT_Next)(Y)
#define MemCheckVT_advance(X,Y) X = (MC_Chunk *)VG_(HT_Next)(Y)
#define MemCheckVT_end(X) X != NULL

typedef
   struct _MC_Chunk {
      struct _MC_Chunk* next;
      Addr         data;            // Address of the actual block.
      SizeT        szB : (sizeof(SizeT)*8)-2; // Size requested; 30 or 62 bits.
//      MC_AllocKind allockind : 2;   // Which operation did the allocation.
//      ExeContext*  where[0];
      /* Variable-length array. The size depends on MC_(clo_keep_stacktraces).
         This array optionally stores the alloc and/or free stack trace. */
   }
   MC_Chunk;

$

CREATE STRUCT VIEW MemCheckV (
	addr_data BIGINT FROM data,
	sizeB BIGINT FROM szB
)$

CREATE VIRTUAL TABLE MemCheckVT
USING STRUCT VIEW MemCheckV
WITH REGISTERED C NAME malloc_list
WITH REGISTERED C TYPE VgHashTable:MC_Chunk*
USING LOOP VG_(HT_ResetIter)(*base);for (MemCheckVT_begin(tuple_iter, *base);MemCheckVT_end(tuple_iter);MemCheckVT_advance(tuple_iter, *base))$
