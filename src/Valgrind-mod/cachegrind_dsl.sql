#include "pub_tool_oset.h"          // OSet 

#define CacheProfile_decl(X) LineCC* X; 
#define CacheProfile_begin(X,Y) X = (LineCC *)VG_(OSetGen_Next)(Y)
#define CacheProfile_advance(X,Y) X = (LineCC *)VG_(OSetGen_Next)(Y)
#define CacheProfile_end(X) X != NULL

typedef
   struct {
      ULong a;  /* total # memory accesses of this kind */
      ULong m1; /* misses in the first level cache */
      ULong mL; /* misses in the second level cache */
   }
   CacheCC;

typedef
   struct {
      ULong b;  /* total # branches of this kind */
      ULong mp; /* number of branches mispredicted */
   }
   BranchCC;

//------------------------------------------------------------
// Primary data structure #1: CC table
// - Holds the per-source-line hit/miss stats, grouped by file/function/line.
// - an ordered set of CCs.  CC indexing done by file/function/line (as
//   determined from the instrAddr).
// - Traversed for dumping stats at end in file/func/line hierarchy.

typedef struct {
   HChar* file;
   HChar* fn;
   Int    line;
}
CodeLoc;

typedef struct {
   CodeLoc  loc; /* Source location that these counts pertain to */
   CacheCC  Ir;  /* Insn read counts */
   CacheCC  Dr;  /* Data read counts */
   CacheCC  Dw;  /* Data write/modify counts */
   BranchCC Bc;  /* Conditional branch counts */
   BranchCC Bi;  /* Indirect branch counts */
} LineCC;
$

CREATE STRUCT VIEW CacheV (
	codeLocationFile TEXT FROM loc.file,
	codeLocationFunction TEXT FROM loc.fn,
	codeLocationLine INT FROM loc.line,
	cacheInstructionReadAccesses BIGINT FROM Ir.a,
	cacheInstructionReadMissL1 BIGINT FROM Ir.m1,
	cacheInstructionReadMissL2 BIGINT FROM Ir.mL,
	cacheDataReadAccesses BIGINT FROM Dr.a,
	cacheDataReadMissL1 BIGINT FROM Dr.m1,
	cacheDataReadMissL2 BIGINT FROM Dr.mL,
	cacheDataWriteAccesses BIGINT FROM Dw.a,
	cacheDataWriteMissL1 BIGINT FROM Dw.m1,
	cacheDataWriteMissL2 BIGINT FROM Dw.mL,
)$

CREATE VIRTUAL TABLE CacheProfile
USING STRUCT VIEW CacheV
WITH REGISTERED C NAME cachegrind_out_table
WITH REGISTERED C TYPE OSet:LineCC*
USING LOOP VG_(OSetGen_ResetIter)(base);for (CacheProfile_begin(tuple_iter, base);CacheProfile_end(tuple_iter);CacheProfile_advance(tuple_iter, base))$

CREATE VIEW FilterOrderCacheQ AS
        SELECT codeLocationFile, codeLocationFunction, codeLocationLine,
                cacheInstructionReadAccesses, cacheInstructionReadMissL1,
                cacheInstructionReadMissL2
        FROM CacheProfile
        WHERE cacheInstructionReadAccesses > 1000000
        ORDER BY cacheInstructionReadMissL1, cacheInstructionReadMissL2;$

CREATE VIEW FilterFuncCacheQ AS
        SELECT codeLocationFile, codeLocationFunction, codeLocationLine,
               SUM(cacheDataWriteAccesses) AS dataWriteAccesses,
               SUM(cacheDataWriteMissL1) AS dataWriteMissesL1,
               SUM(cacheDataWriteMissL2) AS dataWriteMissesL2,
               SUM(cacheDataWriteMissL1)*100/SUM(cacheDataWriteAccesses) AS MissRateL1,
               SUM(cacheDataWriteMissL2)*100/SUM(cacheDataWriteAccesses) AS MissRateL2
        FROM CacheProfile
        WHERE codeLocationFunction LIKE '%memset%'
        GROUP BY codeLocationFile, codeLocationFunction, codeLocationLine
        ORDER BY dataWriteMissesL1 DESC;$

CREATE VIEW GroupFuncCacheQ AS
        SELECT codeLocationFile, codeLocationFunction,
               SUM(cacheInstructionReadAccesses), SUM(cacheInstructionReadMissL1), SUM(cacheInstructionReadMissL2),
               SUM(cacheDataReadAccesses), SUM(cacheDataReadMissL1), SUM(cacheDataReadMissL2),
               SUM(cacheDataWriteAccesses), SUM(cacheDataWriteMissL1), SUM(cacheDataWriteMissL2)
        FROM CacheProfile
        GROUP BY codeLocationFile, codeLocationFunction
        ORDER BY SUM(cacheDataWriteMissL1) DESC;$

CREATE VIEW GroupFuncDataCacheQ AS
        SELECT codeLocationFile, codeLocationFunction,
               SUM(cacheDataWriteAccesses) AS dataWriteAccesses,
               SUM(cacheDataWriteMissL1) AS dataWriteMissesL1,
               SUM(cacheDataWriteMissL2) AS dataWriteMissesL2,
               SUM(cacheDataWriteMissL1)*100/SUM(cacheDataWriteAccesses) AS MissRateL1,
               SUM(cacheDataWriteMissL2)*100/SUM(cacheDataWriteAccesses) AS MissRateL2
        FROM CacheProfile
        GROUP BY codeLocationFile, codeLocationFunction
        HAVING dataWriteAccesses > 1000000
        ORDER BY dataWriteMissesL1 DESC;$

