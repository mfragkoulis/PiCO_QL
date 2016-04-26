#include "pub_tool_hashtable.h"     // VgHashTable
#include "pub_tool_oset.h"          // OSet
#include "pub_tool_execontext.h"    // ExeContext
#include "pub_tool_basics.h"        // VG_WORDSIZE
#include "pub_tool_debuginfo.h"     // VG_(get_fnname), VG_(get_fnname_w_offset)
#include "pub_core_options.h"       // coregrind/ VG_(clo_sym_offsets)
#include "pub_tool_errormgr.h"      // typedef struct _Error Error, typedef struct _Supp Supp
#include "pub_tool_xarray.h"        // typedef struct _XArray XArray

#define MemProfileVT_decl(X) MC_Chunk* X;
#define MemProfileVT_begin(X,Y) X = (MC_Chunk *)VG_(HT_Next)(Y)
#define MemProfileVT_advance(X,Y) X = (MC_Chunk *)VG_(HT_Next)(Y)
#define MemProfileVT_end(X) X != NULL

#define ExecutionContext_decl(X) Addr X;int i = 0 
#define ExecutionContext_begin(X,Y,Z) X = Y[Z]
#define ExecutionContext_advance(X,Y,Z) X = Y[Z]
#define ExecutionContext_end(X, Y) X != Y

#define ErrorProfile_decl(X) Error *X; int i = 0
#define ErrorProfile_begin(X,Y,Z) X = Y[Z]
#define ErrorProfile_advance(X,Y,Z) X = Y[Z]
#define ErrorProfile_end(X,Y) X != Y

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


// Different kinds of blocks.
typedef enum {
   Block_Mallocd = 111,
   Block_Freed,
   Block_MempoolChunk,
   Block_UserG
} BlockKind;


typedef
   enum {
      // Nb: the order is important -- it dictates the order of loss records
      // of equal sizes.
      Reachable    =0,  // Definitely reachable from root-set.
      Possible     =1,  // Possibly reachable from root-set;  involves at
                        //   least one interior-pointer along the way.
      IndirectLeak =2,  // Leaked, but reachable from another leaked block
                        //   (be it Unreached or IndirectLeak).
      Unreached    =3,  // Not reached, ie. leaked.
                        //   (At best, only reachable from itself via a cycle.)
  }
  Reachedness;

/* When a LossRecord is put into an OSet, these elements represent the key. */
typedef
   struct _LossRecordKey {
      Reachedness  state;        // LC_Extra.state value shared by all blocks.
      ExeContext*  allocated_at; // Where they were allocated.
   }
   LossRecordKey;

/* A loss record, used for generating err msgs.  Multiple leaked blocks can be
 * merged into a single loss record if they have the same state and similar
 * enough allocation points (controlled by --leak-resolution). */
typedef
   struct _LossRecord {
      LossRecordKey key;  // Key, when used in an OSet.
      SizeT szB;          // Sum of all MC_Chunk.szB values.
      SizeT indirect_szB; // Sum of all LC_Extra.indirect_szB values.
      UInt  num_blocks;   // Number of blocks represented by the record.
      SizeT old_szB;          // old_* values are the values found during the
      SizeT old_indirect_szB; // previous leak search. old_* values are used to
      UInt  old_num_blocks;   // output only the changed/new loss records
   }
   LossRecord;

/* The classification of a faulting address. */
typedef
   enum {
      Addr_Undescribed, // as-yet unclassified
      Addr_Unknown,     // classification yielded nothing useful
      Addr_Block,       // in malloc'd/free'd block
      Addr_Stack,       // on a thread's stack
      Addr_DataSym,     // in a global data sym
      Addr_Variable,    // variable described by the debug info
      Addr_SectKind     // last-ditch classification attempt
   }
   AddrTag;


typedef
   struct _AddrInfo
   AddrInfo;

struct _AddrInfo {
   AddrTag tag;
   union {
      // As-yet unclassified.
      struct { } Undescribed;

      // On a stack.
      struct {
         ThreadId tid;        // Which thread's stack?
      } Stack;

      // This covers heap blocks (normal and from mempools) and user-defined
      // blocks.
      struct {
         BlockKind   block_kind;
         const HChar* block_desc;    // "block", "mempool" or user-defined
         SizeT       block_szB;
         PtrdiffT    rwoffset;
         ExeContext* allocated_at;  // might be null_ExeContext.
         ExeContext* freed_at;      // might be null_ExeContext.
      } Block;

      // In a global .data symbol.  This holds the first 127 chars of
      // the variable's name (zero terminated), plus a (memory) offset.
      struct {
         HChar    name[128];
         PtrdiffT offset;
      } DataSym;

      // Is described by Dwarf debug info.  XArray*s of HChar.
      struct {
         XArray* /* of HChar */ descr1;
         XArray* /* of HChar */ descr2;
      } Variable;

      // Could only narrow it down to be the PLT/GOT/etc of a given
      // object.  Better than nothing, perhaps.
      struct {
         HChar      objname[128];
         VgSectKind kind;
      } SectKind;

      // Classification yielded nothing useful.
      struct { } Unknown;

   } Addr;
};


typedef
  struct _MC_Error {
   // Nb: we don't need the tag here, as it's stored in the Error type! Yuk.
   //MC_ErrorTag tag;

   union {
      // Use of an undefined value:
      // - as a pointer in a load or store
      // - as a jump target
      struct {
         SizeT szB;   // size of value in bytes
         // Origin info
         UInt        otag;      // origin tag
         ExeContext* origin_ec; // filled in later
      } Value;

      // Use of an undefined value in a conditional branch or move.
      struct {
         // Origin info
         UInt        otag;      // origin tag
         ExeContext* origin_ec; // filled in later
      } Cond;

      // Addressability error in core (signal-handling) operation.
      // It would be good to get rid of this error kind, merge it with
      // another one somehow.
      struct {
      } CoreMem;

      // Use of an unaddressable memory location in a load or store.
      struct {
         Bool     isWrite;    // read or write?
         SizeT    szB;        // not used for exec (jump) errors
         Bool     maybe_gcc;  // True if just below %esp -- could be a gcc bug
         AddrInfo ai;
      } Addr;

      // Jump to an unaddressable memory location.
      struct {
         AddrInfo ai;
      } Jump;

      // System call register input contains undefined bytes.
      struct {
         // Origin info
         UInt        otag;      // origin tag
         ExeContext* origin_ec; // filled in later
      } RegParam;

      // System call register input contains undefined bytes.
      struct {
         // Origin info
         UInt        otag;      // origin tag
         ExeContext* origin_ec; // filled in later
      } MemParam;

      // Problem found from a client request like CHECK_MEM_IS_ADDRESSABLE.
      struct {
         Bool     isAddrErr;  // Addressability or definedness error?
         AddrInfo ai;
         // Origin info
         UInt        otag;      // origin tag
         ExeContext* origin_ec; // filled in later
      } User;

      // Program tried to free() something that's not a heap block (this
      // covers double-frees). */
      struct {
         AddrInfo ai;
      } Free;

      // Program allocates heap block with one function
      // (malloc/new/new[]/custom) and deallocates with not the matching one.
      struct {
         AddrInfo ai;
      } FreeMismatch;

      // Call to strcpy, memcpy, etc, with overlapping blocks.
      struct {
         Addr  src;   // Source block
         Addr  dst;   // Destination block
         SizeT szB;   // Size in bytes;  0 if unused.
      } Overlap;

      // A memory leak.
      struct {
         UInt        n_this_record;
         UInt        n_total_records;
         LossRecord* lr;
      } Leak;

      // A memory pool error.
      struct {
         AddrInfo ai;
      } IllegalMempool;

   } Err;
} MC_Error;


struct _Error {
   struct _Error* next;
   // Unique tag.  This gives the error a unique identity (handle) by
   // which it can be referred to afterwords.  Currently only used for
   // XML printing.
   UInt unique;
   // NULL if unsuppressed; or ptr to suppression record.
   Supp* supp;
   Int count;

   // The tool-specific part
   ThreadId tid;           // Initialised by core
   ExeContext* where;      // Initialised by core
   ErrorKind ekind;        // Used by ALL.  Must be in the range (0..)
   Addr addr;              // Used frequently
   const HChar* string;    // Used frequently
   void* extra;            // For any tool-specific extras
};

/* For each caller specified for a suppression, record the nature of
   the caller name.  Not of interest to tools. */
typedef
   enum {
      NoName,     /* Error case */
      ObjName,    /* Name is of an shared object file. */
      FunName,    /* Name is of a function. */
      DotDotDot   /* Frame-level wildcard */
   }
   SuppLocTy;

typedef
   struct {
      SuppLocTy ty;
      Bool      name_is_simple_str; /* True if name is a string without
                                       '?' and '*' wildcard characters. */
      HChar*    name; /* NULL for NoName and DotDotDot */
   }
   SuppLoc;

/* Suppressions.  Tools can get/set tool-relevant parts with functions
   declared in include/pub_tool_errormgr.h.  Extensible via the 'extra' field.
   Tools can use a normal enum (with element values in the normal range
   (0..)) for 'skind'. */
struct _Supp {
   struct _Supp* next;
   Int count;     // The number of times this error has been suppressed.
   HChar* sname;  // The name by which the suppression is referred to.

   // Index in VG_(clo_suppressions) giving filename from which suppression
   // was read, and the lineno in this file where sname was read.
   Int    clo_suppressions_i;
   Int    sname_lineno;

   // Length of 'callers'
   Int n_callers;
   // Array of callers, for matching stack traces.  First one (name of fn
   // where err occurs) is mandatory;  rest are optional.
   SuppLoc* callers;

   /* The tool-specific part */
   SuppKind skind;   // What kind of suppression.  Must use the range (0..).
   HChar* string;    // String -- use is optional.  NULL by default.
   void* extra;      // Anything else -- use is optional.  NULL by default.
};


typedef
    struct {
      Error** errlist;
      int size;
      int malloced;
} Error_PiCO_QL;
Error_PiCO_QL* errors;



//#define BUF_LEN 4096
#define BUF_LEN 1024
static HChar alloc_by[BUF_LEN];
static HChar obj_name[BUF_LEN];
static HChar dir_name[BUF_LEN];
static HChar file_name[BUF_LEN];
static HChar fn_name[BUF_LEN];
static HChar exec_obj_name[BUF_LEN];
static HChar exec_dir_name[BUF_LEN];
static HChar exec_file_name[BUF_LEN];
static HChar exec_fn_name[BUF_LEN];

static HChar * getObjName(Addr data, HChar *buf_obj) {
  Bool  know_objname = VG_(get_objname)(data, buf_obj, BUF_LEN);
  if (!know_objname) strcpy(buf_obj, "N/A");
  return buf_obj;
};

static HChar * getFileName(Addr data, HChar *buf_filename, HChar *buf_dirname) {
  UInt lineno;
  Bool know_dirinfo = True;
  Bool know_srcloc = VG_(get_filename_linenum)(
                           data,
                           buf_filename,  BUF_LEN,
                           buf_dirname, BUF_LEN, &know_dirinfo,
                           &lineno);
  if (!know_srcloc) strcpy(buf_filename, "N/A");
  return buf_filename;
};

static int getLOCNo(Addr data, HChar *buf_filename, HChar *buf_dirname) {
  UInt lineno = -1;
  Bool know_dirinfo = True;
  VG_(get_filename_linenum)(
                            data,
                            buf_filename,  BUF_LEN,
                            buf_dirname, BUF_LEN, &know_dirinfo,
                            &lineno);
  return lineno;
};

static HChar * getDirName(Addr data, HChar * buf_filename, HChar * buf_dirname) {
  UInt lineno;
  Bool know_dirinfo = False;
  Bool know_srcloc  = VG_(get_filename_linenum)(
                           data,
                           buf_filename,  BUF_LEN,
                           buf_dirname, BUF_LEN, &know_dirinfo,
                           &lineno);
  if (!know_srcloc) strcpy(buf_dirname, "N/A");
  return buf_dirname;
};

static HChar * getFnName(Addr data, HChar *buf_fn) {
  Bool know_fnname  = VG_(clo_sym_offsets)
		? VG_(get_fnname_w_offset) (data, buf_fn, BUF_LEN)
                : VG_(get_fnname) (data, buf_fn, BUF_LEN);
  if (!know_fnname) strcpy(buf_fn, "N/A");
  return buf_fn;
};

static long send_MC_Error(Error *e, int ucase) {
  if (VG_(get_error_kind)(e) == ucase) return (long)VG_(get_error_extra)(e);
  else return 0;
};

static long get_stub(MC_Error *e) {
  return (long)e;
};


$

CREATE STRUCT VIEW MemProfileV (
        addr_data BIGINT FROM data,
        obj_name TEXT FROM {getObjName(tuple_iter->where[0]->ips[1], obj_name)},
        dir_name TEXT FROM {getDirName(tuple_iter->where[0]->ips[1], file_name, dir_name)},
        file_name TEXT FROM {getFileName(tuple_iter->where[0]->ips[1], file_name, dir_name)},
        fn_name TEXT FROM {getFnName(tuple_iter->where[0]->ips[1], fn_name)},
        line_no INT FROM {getLOCNo(tuple_iter->where[0]->ips[1], file_name, dir_name)},
        alloc_by TEXT FROM {getFnName(tuple_iter->where[0]->ips[0], alloc_by)},
        sizeB BIGINT FROM szB,
        allocKind INT FROM allockind,
        excnt_alloc_id INT FROM where[0]->ecu,
        excnt_free_id INT FROM where[1]->ecu
)$

CREATE VIRTUAL TABLE MemProfileVT
USING STRUCT VIEW MemProfileV
WITH REGISTERED C NAME malloc_list
WITH REGISTERED C TYPE VgHashTable:MC_Chunk*
USING LOOP VG_(HT_ResetIter)(*base);for (MemProfileVT_begin(tuple_iter, *base);MemProfileVT_end(tuple_iter);MemProfileVT_advance(tuple_iter, *base))$



CREATE STRUCT VIEW IPV (
	object_file TEXT FROM {getObjName(tuple_iter, exec_obj_name)},
	directory TEXT FROM {getDirName(tuple_iter, exec_file_name, exec_dir_name)},
	file TEXT FROM {getFileName(tuple_iter, exec_file_name, exec_dir_name)},
	function TEXT FROM {getFnName(tuple_iter, exec_fn_name)},
	line_of_code INT FROM {getLOCNo(tuple_iter, exec_file_name, exec_dir_name)}
//	execnt_id INT FROM ecu
)$

CREATE VIRTUAL TABLE ExecutionContext
USING STRUCT VIEW IPV
WITH REGISTERED C TYPE ExeContext:Addr
USING LOOP for(ExecutionContext_begin(tuple_iter, base->ips, i); ExecutionContext_end(i, base->n_ips); ExecutionContext_advance(tuple_iter, base->ips, ++i))$

CREATE STRUCT VIEW ErrorValueV (
        size_bytes BIGINT FROM Err.Value.szB,
        origin_tag INT FROM Err.Value.otag,
        FOREIGN KEY(origin_execution_context_id) FROM Err.Value.origin_ec REFERENCES ExecutionContext POINTER
)$


CREATE VIRTUAL TABLE ValueError
USING STRUCT VIEW ErrorValueV
WITH REGISTERED C TYPE MC_Error$

CREATE STRUCT VIEW ErrorCondV (
        origin_tag INT FROM Err.Cond.otag,
        FOREIGN KEY(origin_execution_context_id) FROM Err.Cond.origin_ec REFERENCES ExecutionContext POINTER
)$

CREATE VIRTUAL TABLE ConditionError
USING STRUCT VIEW ErrorCondV
WITH REGISTERED C TYPE MC_Error$

CREATE STRUCT VIEW ErrorCoreMemV (
        stub BIGINT FROM get_stub(tuple_iter)
)$

CREATE VIRTUAL TABLE CoreMemoryError
USING STRUCT VIEW ErrorCoreMemV
WITH REGISTERED C TYPE MC_Error$

CREATE STRUCT VIEW ErrorAddrV (
        is_write INT FROM Err.Addr.isWrite,
        size_bytes BIGINT FROM Err.Addr.szB,
        maybe_Gcc INT FROM Err.Addr.maybe_gcc,
// instrument AddrInfo and use includes struct view or join
        address_info_tag INT FROM Err.Addr.ai.tag
)$

CREATE VIRTUAL TABLE AddressError
USING STRUCT VIEW ErrorAddrV
WITH REGISTERED C TYPE MC_Error$

CREATE STRUCT VIEW ErrorJumpV (
        stub BIGINT FROM get_stub(tuple_iter),
        address_info_tag INT FROM Err.Jump.ai.tag
)$

CREATE VIRTUAL TABLE JumpError
USING STRUCT VIEW ErrorJumpV
WITH REGISTERED C TYPE MC_Error$

CREATE STRUCT VIEW ErrorUserV (
        is_address_error INT FROM Err.User.isAddrErr,
        address_info_tag INT FROM Err.User.ai.tag,
        origin_tag INT FROM Err.User.otag,
        FOREIGN KEY(origin_execution_context_id) FROM Err.User.origin_ec REFERENCES ExecutionContext POINTER
)$

CREATE VIRTUAL TABLE UserError
USING STRUCT VIEW ErrorUserV
WITH REGISTERED C TYPE MC_Error$


CREATE STRUCT VIEW ErrorLeakV (
        records_leaked INT FROM Err.Leak.n_total_records,
// instrument LossRecord
        size_bytes BIGINT FROM Err.Leak.lr->szB
)$

CREATE VIRTUAL TABLE LeakError
USING STRUCT VIEW ErrorLeakV
WITH REGISTERED C TYPE MC_Error$

CREATE STRUCT VIEW ErrorIllegalMempoolV (
        stub BIGINT FROM get_stub(tuple_iter),
        address_info_tag INT FROM Err.IllegalMempool.ai.tag
)$

CREATE VIRTUAL TABLE IllegalMemoryPoolError
USING STRUCT VIEW ErrorIllegalMempoolV
WITH REGISTERED C TYPE MC_Error$



CREATE STRUCT VIEW ErrorV (
	name TEXT FROM string,
	id INT FROM unique,
	address_error BIGINT FROM addr,
	thread_id INT FROM tid,
	count INT FROM count,
	FOREIGN KEY(execution_context_id) FROM where REFERENCES ExecutionContext POINTER,
	kind INT FROM ekind,
        FOREIGN KEY(value_error_id) FROM {send_MC_Error(tuple_iter, 0)} REFERENCES ValueError POINTER,
        FOREIGN KEY(condition_error_id) FROM {send_MC_Error(tuple_iter, 1)} REFERENCES ConditionError POINTER,
        FOREIGN KEY(core_memory_error_id) FROM {send_MC_Error(tuple_iter, 2)} REFERENCES CoreMemoryError POINTER,
        FOREIGN KEY(address_error_id) FROM {send_MC_Error(tuple_iter, 3)} REFERENCES AddressError POINTER,
        FOREIGN KEY(jump_error_id) FROM {send_MC_Error(tuple_iter, 4)} REFERENCES JumpError POINTER,
        FOREIGN KEY(user_error_id) FROM {send_MC_Error(tuple_iter, 7)} REFERENCES UserError POINTER,
        FOREIGN KEY(leak_error_id) FROM {send_MC_Error(tuple_iter, 11)} REFERENCES LeakError POINTER,
        FOREIGN KEY(illegal_memory_pool_error_id) FROM {send_MC_Error(tuple_iter, 12)} REFERENCES IllegalMemoryPoolError POINTER

)$

//   switch (VG_(get_error_kind)(err)) {
CREATE VIRTUAL TABLE ErrorProfile
USING STRUCT VIEW ErrorV
WITH REGISTERED C NAME errorList
WITH REGISTERED C TYPE Error_PiCO_QL:Error*
USING LOOP for(ErrorProfile_begin(tuple_iter, base->errlist, i); ErrorProfile_end(i, base->size); ErrorProfile_advance(tuple_iter, base->errlist, ++i))$
	

CREATE VIEW InspectErrorView AS
	SELECT
		(SELECT CASE WHEN kind = 0 THEN 'Err_Value'
			     WHEN kind = 1 THEN 'Err_Cond'
			     WHEN kind = 2 THEN 'Err_CoreMem'
			     WHEN kind = 3 THEN 'Err_Addr'
			     WHEN kind = 4 THEN 'Err_Jump'
			     WHEN kind = 7 THEN 'Err_User'
			     WHEN kind = 11 THEN 'Err_Leak'
			     WHEN kind = 12 THEN 'Err_IllegalMempool' END) error_kind_tag, *
	FROM ErrorProfile;$

	
CREATE VIEW RangeSizeMemProfileQ AS
	SELECT (sizeB / 256) * 256 AS size_ranges,
	  COUNT(*) AS blocks_in_range
	FROM MemProfileVT
	GROUP BY size_ranges
	ORDER BY size_ranges;$

CREATE VIEW SizePerLOCMemProfileQ AS
	SELECT fn_name, line_no, addr_data,
		file_name, obj_name,
		 SUM(sizeB)*4 AS totalSizePerLOC
	FROM MemProfileVT
	GROUP BY file_name, fn_name, line_no
        HAVING totalSizePerLOC > 1000000
	ORDER BY totalSizePerLOC DESC;$

CREATE VIEW GroupFunctionMemProfileQ AS
	SELECT fn_name, line_no, addr_data,
		file_name, obj_name, alloc_by,
		 SUM(sizeB)
	FROM MemProfileVT
        WHERE fn_name LIKE '%insert%'
	GROUP BY file_name, fn_name
	ORDER BY SUM(sizeB) DESC;$


