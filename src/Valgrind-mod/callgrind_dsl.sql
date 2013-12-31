#include "global.h"                 // typedef struct _thread_info thread_info,
		                    // CLG_(get_threads)(), Statistics, N_FN_ENTRIES
#include "pub_tool_threadstate.h"   // VG_N_THREADS

#define ThreadVT_decl(X) thread_info *X;int i = 0
#define ThreadVT_begin(X,Y,Z) X = Y[Z]
#define ThreadVT_end(X,Y) X < Y
#define ThreadVT_advance(X,Y,Z) X = Y[Z]

#define FnStackVT_decl(X) fn_node *X;int i = 0
#define FnStackVT_begin(X,Y,Z) X = Y[Z]
#define FnStackVT_end(X,Y) X < Y
#define FnStackVT_advance(X,Y,Z) X = Y[Z]

#define FnChainVT_decl(X) fn_node *X

#define FnDepsVT_decl(X) fn_node *X;int i = 1 // i = 0 the function we are at
#define FnDepsVT_begin(X,Y,Z) X = Y[Z]
#define FndepsVT_end(X,Y) X < Y
#define FnDepsVT_advance(X,Y,Z) X = Y[Z]

#define FileFunctionsVT_decl(X) fn_node *X;int i = 0 
#define FileFunctionsVT_begin(X,Y,Z) X = Y[Z]
#define FileFunctionsVT_end(X,Y) X < Y
#define FileFunctionsVT_advance(X,Y,Z) X = Y[Z]

#define FullCostVT_decl(X) long X;int i = 0
#define FullCostVT_begin(X,Y,Z) X = (long)Y[Z]
#define FullCostVT_end(X,Y) X < Y
#define FullCostVT_advance(X,Y,Z) X = (long)Y[Z]

$

CREATE STRUCT VIEW FnNodeV (
  name TEXT FROM name,
  in_file TEXT FROM file->name,
  number INT FROM number,
  FOREIGN KEY(fnchain_id) FROM next REFERENCES FnChainVT POINTER,
  FOREIGN KEY(last_cxt_id) FROM last_cxt REFERENCES FnDepsVT POINTER,
  FOREIGN KEY(pure_cxt_id) FROM pure_cxt REFERENCES FnDepsVT POINTER,
  FOREIGN KEY(file_node_id) FROM file REFERENCES FileFunctionsVT POINTER,
  dump_before INT FROM dump_before,
  dump_after INT FROM dump_after,
  zero_before INT FROM zero_before,
  toggle_collect INT FROM toggle_collect,
  skip INT FROM skip,
  pop_on_jump INT FROM pop_on_jump,
  is_malloc INT FROM is_malloc,
  is_realloc INT FROM is_realloc,
  is_free INT FROM is_free,
  fgroup INT FROM group,
  separate_callers INT FROM separate_callers,
  separate_recursions INT FROM separate_recursions
)$

CREATE VIRTUAL TABLE FnStackVT
USING STRUCT VIEW FnNodeV
WITH REGISTERED C TYPE fn_stack:fn_node*
USING LOOP for(FnStackVT_begin(tuple_iter, base->top, i); FnStackVT_end(i, base->size); FnStackVT_advance(tuple_iter, base->top, ++i))$

CREATE VIRTUAL TABLE FnChainVT
USING STRUCT VIEW FnNodeV
WITH REGISTERED C TYPE fn_node
USING LOOP for(tuple_iter = base; tuple_iter != NULL; tuple_iter = tuple_iter->next)$

CREATE VIRTUAL TABLE FnDepsVT
USING STRUCT VIEW FnNodeV
WITH REGISTERED C TYPE Context:fn_node*
USING LOOP for(FnDepsVT_begin(tuple_iter, base->fn, i); FnStackVT_end(i, base->size); FnStackVT_advance(tuple_iter, base->fn, ++i))$

CREATE VIRTUAL TABLE FileFunctionsVT
USING STRUCT VIEW FnNodeV
WITH REGISTERED C TYPE file_node:fn_node*
USING LOOP for(FileFunctionsVT_begin(tuple_iter, base->fns, i); FileFunctionsVT_end(i, N_FN_ENTRIES); FileFunctionsVT_advance(tuple_iter, base->fns, ++i))$

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
