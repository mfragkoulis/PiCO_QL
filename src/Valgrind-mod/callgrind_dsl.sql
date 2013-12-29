#include "global.h"                 // typedef struct _thread_info thread_info,
		                    // CLG_(get_threads)(),
#include "pub_tool_threadstate.h"   // VG_N_THREADS

#define ThreadVT_decl(X) thread_info *X;int i = 0
#define ThreadVT_begin(X,Y,Z) X = Y[Z]
#define ThreadVT_end(X,Y) X < Y
#define ThreadVT_advance(X,Y,Z) X = Y[Z]

$

CREATE STRUCT VIEW FnStackV (
	size INT FROM size
)$

CREATE STRUCT VIEW FnStackVT
USING STRICT VIEW FnStackV
WITH REGISTERED C TYPE fn_stack$

CREATE STRUCT VIEW ThreadV (
	FOREIGN KEY(fn_stack_id) FROM fns REFERENCES FnStackVT
)$

CREATE VIRTUAL TABLE ThreadVT
USING STRUCT VIEW ThreadV
WITH REGISTERED C NAME all_threads
WITH REGISTERED C TYPE thread_info **:thread_info *
USING LOOP for(ThreadVT_begin(tuple_iter, base, i); ThreadVT_end(i, VG_N_THREADS); ThreadVT_advance(tuple_iter, base, ++i))$
