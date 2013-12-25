#include "pub_tool_oset.h"          // OSet 

#define CachegrindVT_decl(X) LineCC* X; 
#define CachegrindVT_begin(X,Y) X = (LineCC *)VG_(OSetGen_Next)(Y)
#define CachegrindVT_advance(X,Y) X = (LineCC *)VG_(OSetGen_Next)(Y)
#define CachegrindVT_end(X) X != NULL

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
	codeLocFile TEXT FROM loc.file,
	codeLocFunc TEXT FROM loc.fn,
	codeLocLine INT FROM loc.line,
	cacheInstrRMemAc BIGINT FROM Ir.a,
	cacheInstrRMemMissL1 BIGINT FROM Ir.m1,
	cacheInstrRMemMissL2 BIGINT FROM Ir.mL,
	cacheDataRMemAc BIGINT FROM Dr.a,
	cacheDataRMemMissL1 BIGINT FROM Dr.m1,
	cacheDataRMemMissL2 BIGINT FROM Dr.mL,
	cacheDataWMemAc BIGINT FROM Dw.a,
	cacheDataWMemMissL1 BIGINT FROM Dw.m1,
	cacheDataWMemMissL2 BIGINT FROM Dw.mL,
	branchCondTotal BIGINT FROM Bc.b,
	branchCondMisPred BIGINT FROM Bc.mp,
	branchIndirTotal BIGINT FROM Bi.b,
	branchIndirMisPred BIGINT FROM Bi.mp
)$

CREATE VIRTUAL TABLE CachegrindVT
USING STRUCT VIEW CacheV
WITH REGISTERED C NAME cachegrind_out_table
WITH REGISTERED C TYPE OSet:LineCC*
USING LOOP VG_(OSetGen_ResetIter)(base);for (CachegrindVT_begin(tuple_iter, base);CachegrindVT_end(tuple_iter);CachegrindVT_advance(tuple_iter, base))$

CREATE VIEW FilterOrderCacheQ AS
	SELECT *
	FROM CachegrindVT
	WHERE cacheInstrRMemAc > 100000
	ORDER BY cacheInstrRMemMissL1, cacheInstrRMemMissL2;$

CREATE VIEW FilterFuncCacheQ AS
	SELECT *
	FROM CachegrindVT
	WHERE codeLocFunc LIKE '%lookup%'
	ORDER BY cacheDataRMemMissL1 desc;$

CREATE VIEW GroupFuncCacheQ AS
	SELECT codeLocFile, codeLocFunc, codeLocLine,
	       SUM(cacheInstrRMemAc) AS instrReadAcc, SUM(cacheInstrRMemMissL1) AS instrReadMissL1, SUM(cacheInstrRMemMissL2) AS instrReadMissL2,
	       SUM(cacheDataRMemAc) AS dataReadAcc, SUM(cacheDataRMemMissL1) AS dataReadMissL1, SUM(cacheDataRMemMissL2) AS dataReadMissL2,
	       SUM(cacheDataWMemAc) AS dataWriteAcc, SUM(cacheDataWMemMissL1) AS dataWriteMissL1, SUM(cacheDataWMemMissL2) AS dataWriteMissL2
	FROM CachegrindVT
	GROUP BY codeLocFunc
	ORDER BY dataWriteMissL1 desc;$

