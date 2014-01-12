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

#define FnCxtVT_decl(X) fn_node *X;int i = 1 // i = 0 the function we are at
#define FnCxtVT_begin(X,Y,Z) X = Y[Z]
#define FnCxtVT_end(X,Y) X < Y
#define FnCxtVT_advance(X,Y,Z) X = Y[Z]

#define FileFunctionsVT_decl(X) fn_node *X;int i = 0 
#define FileFunctionsVT_begin(X,Y,Z) X = Y[Z]
#define FileFunctionsVT_end(X,Y) X < Y
#define FileFunctionsVT_advance(X,Y,Z) X = Y[Z]

#define InstrInfoVT_decl(X) InstrInfo *X;int i = 0 
#define InstrInfoVT_begin(X,Y,Z) X = &Y[Z]
#define InstrInfoVT_end(X,Y) X < Y
#define InstrInfoVT_advance(X,Y,Z) X = &Y[Z]

#define JmpInfoVT_decl(X) CJmpInfo *X;int i = 0 
#define JmpInfoVT_begin(X,Y,Z) X = &Y[Z]
#define JmpInfoVT_end(X,Y) X < Y
#define JmpInfoVT_advance(X,Y,Z) X = &Y[Z]

#define JmpDataVT_decl(X) JmpData *X;int i = 0 
#define JmpDataVT_begin(X,Y,Z) X = (JmpData *)&Y[Z]
#define JmpDataVT_end(X,Y) ((X) || (Y)) == 0 ? 1 : X < Y
#define JmpDataVT_advance(X,Y,Z) X = (JmpData *)&Y[Z]

#define CallStackVT_decl(X) call_entry *X;int i = 0 
#define CallStackVT_begin(X,Y,Z) X = &Y[Z]
#define CallStackVT_end(X,Y) X < Y
#define CallStackVT_advance(X,Y,Z) X = &Y[Z]

#define JccFromBbccListVT_decl(X) jCC *X 

#define BbccListBbVT_decl(X) BBCC *X 

#define BbccRecArrayVT_decl(X) BBCC *X;int i = 0 
#define BbccRecArrayVT_begin(X,Y,Z) X = Y[Z]
#define BbccRecArrayVT_end(X,Y) X < Y
#define BbccRecArrayVT_advance(X,Y,Z) X = Y[Z]

#define ExecStackVT_decl(X) exec_state *X;int i = 0 
#define ExecStackVT_begin(X,Y,Z) X = Y[Z]
#define ExecStackVT_end(X,Y) X < Y
#define ExecStackVT_advance(X,Y,Z) X = Y[Z]

#define FnArrayVT_decl(X) int X;int i = 0 
#define FnArrayVT_begin(X,Y,Z) X = Y[Z]
#define FnArrayVT_end(X,Y) X < Y
#define FnArrayVT_advance(X,Y,Z) X = Y[Z]

#define JccHashVT_decl(X) jCC *X;int i = 0 
#define JccHashVT_begin(X,Y,Z) X = Y[Z]
#define JccHashVT_end(X,Y) X < Y
#define JccHashVT_advance(X,Y,Z) X = Y[Z]

#define BbccHashVT_decl(X) BBCC *X;int i = 0 
#define BbccHashVT_begin(X,Y,Z) X = Y[Z]
#define BbccHashVT_end(X,Y) X < Y
#define BbccHashVT_advance(X,Y,Z) X = Y[Z]

//#define FullCostVT_decl(X) long X;int i = 0
//#define FullCostVT_begin(X,Y,Z) X = (long)Y[Z]
//#define FullCostVT_end(X,Y) X < Y
//#define FullCostVT_advance(X,Y,Z) X = (long)Y[Z]

static int get_mangled_no(BBCC *bbcc) {
  return bbcc->cxt->base_number + bbcc->rec_index;
};

static long get_cost_offset(FullCost *f, int index) {
  return (long)f[index];
};

$

CREATE STRUCT VIEW FnNodeV (
	name TEXT FROM name,
	in_file TEXT FROM file->name,
	in_obj TEXT FROM file->obj->name, // ++ ObjFilesVT, addr, size, offset
	number INT FROM number,
	FOREIGN KEY(fnchain_id) FROM next REFERENCES FnChainVT POINTER,
	FOREIGN KEY(last_cxt_id) FROM last_cxt REFERENCES FnCxtVT POINTER,
	FOREIGN KEY(pure_cxt_id) FROM pure_cxt REFERENCES FnCxtVT POINTER,
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

CREATE VIRTUAL TABLE FnNodeVT
USING STRUCT VIEW FnNodeV
WITH REGISTERED C TYPE fn_node$

CREATE VIRTUAL TABLE FnStackVT
USING STRUCT VIEW FnNodeV
WITH REGISTERED C TYPE fn_stack:fn_node*
USING LOOP for(FnStackVT_begin(tuple_iter, base->top, i); FnStackVT_end(i, base->size); FnStackVT_advance(tuple_iter, base->top, ++i))$

CREATE VIRTUAL TABLE FnChainVT
USING STRUCT VIEW FnNodeV
WITH REGISTERED C TYPE fn_node
USING LOOP for(tuple_iter = base; tuple_iter != NULL; tuple_iter = tuple_iter->next)$

CREATE VIRTUAL TABLE FnCxtVT
USING STRUCT VIEW FnNodeV
WITH REGISTERED C TYPE Context:fn_node*
USING LOOP for(FnCxtVT_begin(tuple_iter, base->fn, i); FnCxtVT_end(i, base->size); FnCxtVT_advance(tuple_iter, base->fn, ++i))$

CREATE VIRTUAL TABLE FileFunctionsVT
USING STRUCT VIEW FnNodeV
WITH REGISTERED C TYPE file_node:fn_node*
USING LOOP for(FileFunctionsVT_begin(tuple_iter, base->fns, i); FileFunctionsVT_end(i, N_FN_ENTRIES); FileFunctionsVT_advance(tuple_iter, base->fns, ++i))$

CREATE STRUCT VIEW InstrInfoV (
	instr_offset INT FROM instr_offset,
	instr_size INT FROM instr_size,
	cost_offset INT FROM cost_offset
  //EventSet* eventset;
)$

CREATE VIRTUAL TABLE InstrInfoVT
USING STRUCT VIEW InstrInfoV
WITH REGISTERED C TYPE BB:InstrInfo*
USING LOOP for(InstrInfoVT_begin(tuple_iter, base->instr, i); InstrInfoVT_end(i, base->instr_len); InstrInfoVT_advance(tuple_iter, base->instr, ++i))$

CREATE STRUCT VIEW JmpInfoV (
	instr_index INT FROM instr,
	jmp_kind INT FROM jmpkind
)$

CREATE VIRTUAL TABLE JmpInfoVT
USING STRUCT VIEW JmpInfoV
WITH REGISTERED C TYPE BB:CJmpInfo*
USING LOOP for(JmpInfoVT_begin(tuple_iter, base->jmp, i); JmpInfoVT_end(i, base->cjmp_count); JmpInfoVT_advance(tuple_iter, base->jmp, ++i))$

CREATE STRUCT VIEW BbV (
	addr BIGINT FROM bb_addr(tuple_iter),
	in_obj TEXT FROM obj->name,
	in_obj_offset BIGINT FROM offset,
	section INT FROM sect_kind,
	instr_count INT FROM instr_count,
	FOREIGN KEY(fn_node_id) FROM fn REFERENCES FnNodeVT POINTER,
	line INT FROM line,
	is_fn_entry INT FROM is_entry,
	FOREIGN KEY(bbcc_list_id) FROM bbcc_list REFERENCES BbccListBbVT POINTER,
	cjmp_count INT FROM cjmp_count,
	FOREIGN KEY(jmp_array_id) FROM tuple_iter REFERENCES JmpInfoVT POINTER,
	cjmp_inverted INT FROM cjmp_inverted,
	instr_len INT FROM instr_len,
	cost_count INT FROM cost_count,
	FOREIGN KEY(instr_array_id) FROM tuple_iter REFERENCES InstrInfoVT POINTER
)$
	
CREATE VIRTUAL TABLE BbVT
USING STRUCT VIEW BbV
WITH REGISTERED C TYPE BB$

CREATE STRUCT VIEW JmpDataV (
       ecounter INT FROM ecounter,
       FOREIGN KEY(jcc_list_id) FROM jcc_list REFERENCES JccFromBbccListVT POINTER
)$

CREATE VIRTUAL TABLE JmpDataVT
USING STRUCT VIEW JmpDataV
WITH REGISTERED C TYPE BBCC*:JmpData*
USING LOOP for(JmpDataVT_begin(tuple_iter, base->jmp, i); JmpDataVT_end(i, base->bb->cjmp_count); JmpDataVT_advance(tuple_iter, base, ++i))$

CREATE STRUCT VIEW BbccV (
	FOREIGN KEY(bb_id) FROM bb REFERENCES BbVT POINTER,
	FOREIGN KEY(cxt_id) FROM cxt REFERENCES FnCxtVT POINTER,
	tid INT FROM tid,
	mangled_no INT FROM get_mangled_no(tuple_iter),
	recursion_index INT FROM rec_index,
	FOREIGN KEY(recursion_array_id) FROM tuple_iter REFERENCES BbccRecArrayVT POINTER,
	ret_counter BIGINT FROM ret_counter,
	FOREIGN KEY(next_bbcc_id) FROM next_bbcc REFERENCES BbccListBbVT POINTER,
	FOREIGN KEY(lru_next_bbcc_id) FROM lru_next_bbcc REFERENCES BbccVT POINTER,
	FOREIGN KEY(skippedfn_cost_id) FROM skipped REFERENCES FullCostVT POINTER,
	FOREIGN KEY(cost_id) FROM cost REFERENCES FullCostVT POINTER,
	ecounter_sum BIGINT FROM ecounter_sum,
	FOREIGN KEY(jmp_data_id) FROM tuple_iter REFERENCES JmpDataVT POINTER
)$

CREATE VIRTUAL TABLE BbccVT
USING STRUCT VIEW BbccV
WITH REGISTERED C TYPE BBCC$

CREATE VIRTUAL TABLE BbccListBbVT
USING STRUCT VIEW BbccV
WITH REGISTERED C TYPE BBCC
USING LOOP for(tuple_iter = base; tuple_iter != NULL; tuple_iter = base->next_bbcc)$

CREATE VIRTUAL TABLE BbccRecArrayVT
USING STRUCT VIEW BbccV
WITH REGISTERED C TYPE BBCC*:BBCC*
USING LOOP for(BbccRecArrayVT_begin(tuple_iter, base->rec_array, i); BbccRecArrayVT_end(i, base->cxt->fn[0]->separate_recursions); BbccRecArrayVT_advance(tuple_iter, base->rec_array, ++i))$

CREATE STRUCT VIEW JccV (
	jmp_kind INT FROM jmpkind,
	jmp INT FROM jmp,
	call_counter BIGINT FROM call_counter,
	FOREIGN KEY(next_jcc_from_bbcc_list_id) FROM next_from REFERENCES JccFromBbccListVT POINTER,
	FOREIGN KEY(from_bbcc_id) FROM from REFERENCES BbccVT POINTER,
	FOREIGN KEY(to_bbcc_id) FROM from REFERENCES BbccVT POINTER,
	FOREIGN KEY(cost_id) FROM cost REFERENCES FullCostVT POINTER
)$

CREATE VIRTUAL TABLE JccVT
USING STRUCT VIEW JccV
WITH REGISTERED C TYPE jCC$

CREATE VIRTUAL TABLE JccFromBbccListVT
USING STRUCT VIEW JccV
WITH REGISTERED C TYPE jCC
USING LOOP for(tuple_iter = base; tuple_iter != NULL; tuple_iter = tuple_iter->next_from)$

CREATE STRUCT VIEW CallEntryV (
	FOREIGN KEY(jcc_id) FROM jcc REFERENCES JccVT POINTER,
	FOREIGN KEY(enter_cost_id) FROM enter_cost REFERENCES FullCostVT POINTER,
	sp_addr BIGINT FROM sp,
	ret_addr BIGINT FROM ret_addr,
	FOREIGN KEY(nonskipped_bbcc_id) FROM nonskipped REFERENCES BbccVT POINTER,
	FOREIGN KEY(cxt_id) FROM cxt REFERENCES FnCxtVT POINTER,
        fn_sp INT FROM fn_sp
)$

CREATE VIRTUAL TABLE CallStackVT
USING STRUCT VIEW CallEntryV
WITH REGISTERED C TYPE call_stack:call_entry*
USING LOOP for(CallStackVT_begin(tuple_iter, base->entry, i); CallStackVT_end(i, base->sp); CallStackVT_advance(tuple_iter, base->entry, ++i))$
$

CREATE STRUCT VIEW ExecStateV (
	sig INT FROM sig,
	orig_sp INT FROM orig_sp,
	FOREIGN KEY(cost_id) FROM cost REFERENCES FullCostVT POINTER,
	collect INT FROM collect,
	FOREIGN KEY(cxt_id) FROM cxt REFERENCES FnCxtVT POINTER,
	jmps_passed INT FROM jmps_passed,
	FOREIGN KEY(bbcc_id) FROM bbcc REFERENCES BbccVT POINTER,
	FOREIGN KEY(nonskipped_bbcc_id) FROM nonskipped REFERENCES BbccVT POINTER,
	call_stack_bottom INT FROM call_stack_bottom
)$

CREATE VIRTUAL TABLE ExecStackVT
USING STRUCT VIEW ExecStateV
WITH REGISTERED C TYPE exec_stack:exec_state*
USING LOOP for(ExecStackVT_begin(tuple_iter, base->entry, i); ExecStackVT_end(i, MAX_SIGHANDLERS); ExecStackVT_advance(tuple_iter, base->entry, ++i))$

CREATE STRUCT VIEW FnArrayV (
	active_count INT FROM tuple_iter
)$

CREATE VIRTUAL TABLE FnArrayVT
USING STRUCT VIEW FnArrayV
WITH REGISTERED C TYPE fn_array:int
USING LOOP for(FnArrayVT_begin(tuple_iter, base->array, i); FnArrayVT_end(i, base->size); FnArrayVT_advance(tuple_iter, base->array, ++i))$

CREATE STRUCT VIEW JccHashV (
	size INT FROM size,
	entries INT FROM entries
)$

CREATE VIRTUAL TABLE JccHashVT
USING STRUCT VIEW JccV
WITH REGISTERED C TYPE jcc_hash:jCC*
USING LOOP for(JccHashVT_begin(tuple_iter, base->table, i); JccHashVT_end(i, base->size); JccHashVT_advance(tuple_iter, base->table, ++i))$

CREATE STRUCT VIEW BbccHashV (
	size INT FROM size,
	entries INT FROM entries
)$

CREATE VIRTUAL TABLE BbccHashVT
USING STRUCT VIEW BbccV
WITH REGISTERED C TYPE bbcc_hash:BBCC*
USING LOOP for(BbccHashVT_begin(tuple_iter, base->table, i); BbccHashVT_end(i, base->size); BbccHashVT_advance(tuple_iter, base->table, ++i)) {
               for(;tuple_iter != NULL; tuple_iter = tuple_iter->next)$
//                     for(BbccHashVT_begin(tuple_iter, tuple_iter->rec_array, h); BbccHashVT_end(h, tuple_iter->cxt->fn[0]->separate_recursions); BbccHashVT_advance(tuple_iter, tuple_iter->rec_array, ++h))$


CREATE STRUCT VIEW FullCostV (
	//event TEXT FROM get
	ir BIGINT FROM {get_cost_offset(tuple_iter, 0)}
)$

CREATE VIRTUAL TABLE FullCostVT
USING STRUCT VIEW FullCostV
WITH REGISTERED C TYPE FullCost$
//WITH REGISTERED C TYPE FullCost:long
//USING LOOP for(FullCostVT_begin(tuple_iter, base, i); FullCostVT_end(i, CLG_(sets).full->size); FullCostVT_advance(tuple_iter, base, ++i))$

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

CREATE VIEW OrderedMangleBBCCHashV AS
	SELECT DISTINCT addr, FN.name, line, 
		BC.mangled_no, number, in_file, 
		BB.in_obj, BB.in_obj_offset, instr_count
	FROM ThreadVT T 
	JOIN BbccHashVT BC 
	ON BC.base=T.bbcc_hash_id 
	JOIN BbVT BB 
	ON BB.base = BC.bb_id 
	JOIN FnNodeVT FN 
	ON FN.base = BB.fn_node_id 
	WHERE NOT (BC.ecounter_sum
	OR BC.ret_counter)
	AND is_fn_entry
	ORDER BY BB.in_obj, in_file, FN.name;$

CREATE VIEW OrderedExeCounterBBCCHashV AS
	SELECT addr, name, line, 
		R.mangled_no, number, in_file, 
		BB.in_obj, BB.in_obj_offset, instr_count, 
		SUM(R.ecounter_sum), SUM(R.ret_counter)
	FROM ThreadVT T 
	JOIN BbccHashVT BC 
	ON BC.base=T.bbcc_hash_id 
	JOIN BbccRecArrayVT R 
	ON R.base = BC.recursion_array_id 
	JOIN BbVT BB 
	ON BB.base = R.bb_id 
	JOIN FnNodeVT FN 
	ON FN.base = BB.fn_node_id 
	JOIN FullCostVT FC 
	ON FC.base = R.cost_id 
	WHERE (R.ecounter_sum 
	OR R.ret_counter)
	GROUP BY BC.bb_id
	ORDER BY SUM(R.ecounter_sum) DESC
	LIMIT 20;$

CREATE VIEW OrderedCallCounterJCCHashV AS
	SELECT addr, name, line, 
		mangled_no, BB.in_obj, 
		SUM(call_counter), SUM(ecounter_sum), Ir 
	FROM ThreadVT T 
	JOIN JccHashVT JC 
	ON JC.base=T.jcc_hash_id 
	JOIN BbccVT BC 
	ON BC.base=JC.from_bbcc_id 
	JOIN BbVT BB 
	ON BB.base = BC.bb_id 
	JOIN FnNodeVT FN 
	ON FN.base = BB.fn_node_id 
	JOIN FullCostVT FC 
	ON FC.base = JC.cost_id
	GROUP BY BC.bb_id
	ORDER BY SUM(call_counter) DESC;
