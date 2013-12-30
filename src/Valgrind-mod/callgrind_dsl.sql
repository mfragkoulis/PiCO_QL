#include "global.h"                 // typedef struct _thread_info thread_info,
		                    // CLG_(get_threads)(), Statistics
#include "pub_tool_threadstate.h"   // VG_N_THREADS

#define ThreadVT_decl(X) thread_info *X;int i = 0
#define ThreadVT_begin(X,Y,Z) X = Y[Z]
#define ThreadVT_end(X,Y) X < Y
#define ThreadVT_advance(X,Y,Z) X = Y[Z]

#define FullCostVT_decl(X) long X;int i = 0
#define FullCostVT_begin(X,Y,Z) X = (long)Y[Z]
#define FullCostVT_end(X,Y) X < Y
#define FullCostVT_advance(X,Y,Z) X = (long)Y[Z]

$

CREATE STRUCT VIEW FnStackV (
	size INT FROM size
)$

CREATE VIRTUAL TABLE FnStackVT
USING STRUCT VIEW FnStackV
WITH REGISTERED C TYPE fn_stack$

CREATE STRUCT VIEW CallStackV (
	size INT FROM size,
	sp INT FROM sp
)$

CREATE VIRTUAL TABLE CallStackVT
USING STRUCT VIEW CallStackV
WITH REGISTERED C TYPE call_stack$

CREATE STRUCT VIEW ExecStackV (
	sp INT FROM sp
)$

CREATE VIRTUAL TABLE ExecStackVT
USING STRUCT VIEW ExecStackV
WITH REGISTERED C TYPE exec_stack$

CREATE STRUCT VIEW FnArrayV (
	size INT FROM size
)$

CREATE VIRTUAL TABLE FnArrayVT
USING STRUCT VIEW FnArrayV
WITH REGISTERED C TYPE fn_array$

CREATE STRUCT VIEW JccHashV (
	size INT FROM size,
	entries INT FROM entries
)$

CREATE VIRTUAL TABLE JccHashVT
USING STRUCT VIEW JccHashV
WITH REGISTERED C TYPE jcc_hash$

CREATE STRUCT VIEW BbccHashV (
	size INT FROM size,
	entries INT FROM entries
)$

CREATE VIRTUAL TABLE BbccHashVT
USING STRUCT VIEW BbccHashV
WITH REGISTERED C TYPE bbcc_hash$

CREATE STRUCT VIEW FullCostV (
	//event TEXT FROM get
	cost BIGINT FROM tuple_iter
)$

CREATE VIRTUAL TABLE FullCostVT
USING STRUCT VIEW FullCostV
WITH REGISTERED C TYPE FullCost:long
USING LOOP for(FullCostVT_begin(tuple_iter, base, i); FullCostVT_end(i, CLG_(sets).full->size); FullCostVT_advance(tuple_iter, base, ++i))$

CREATE STRUCT VIEW ThreadV (
	FOREIGN KEY(fn_stack_id) FROM fns REFERENCES FnStackVT,
	FOREIGN KEY(call_stack_id) FROM calls REFERENCES CallStackVT,
	FOREIGN KEY(exec_stack_id) FROM states REFERENCES ExecStackVT,
	FOREIGN KEY(fn_array_id) FROM fn_active REFERENCES FnArrayVT,
	FOREIGN KEY(jcc_hash_id) FROM jccs REFERENCES JccHashVT,
	FOREIGN KEY(bbcc_hash_id) FROM bbccs REFERENCES BbccHashVT,
	FOREIGN KEY(lastdump_cost_id) FROM lastdump_cost REFERENCES FullCostVT POINTER,
	FOREIGN KEY(sighandler_cost_id) FROM sighandler_cost REFERENCES FullCostVT POINTER
)$

CREATE VIRTUAL TABLE ThreadVT
USING STRUCT VIEW ThreadV
WITH REGISTERED C NAME all_threads
WITH REGISTERED C TYPE thread_info **:thread_info *
USING LOOP for(ThreadVT_begin(tuple_iter, base, i); ThreadVT_end(i, VG_N_THREADS); ThreadVT_advance(tuple_iter, base, ++i))$

CREATE STRUCT VIEW StatisticsV (
	context_counter INT FROM context_counter
)$

CREATE VIRTUAL TABLE StatisticsVT
USING STRUCT VIEW StatisticsV
WITH REGISTERED C NAME statistics
WITH REGISTERED C TYPE Statistics$
