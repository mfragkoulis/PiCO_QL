#include "pub_tool_hashtable.h"     // VgHashTable
#include "pub_tool_oset.h"          // OSet
#include "pub_tool_execontext.h"    // ExeContext
#include "pub_tool_basics.h"        // VG_WORDSIZE
#include "pub_tool_debuginfo.h"     // VG_(get_fnname), VG_(get_fnname_w_offset)
#include "pub_core_options.h"       // coregrind/ VG_(clo_sym_offsets)
#include "pub_tool_errormgr.h"      // typedef struct _Error Error, typedef struct _Supp Supp
#include "pub_tool_xarray.h"        // typedef struct _XArray XArray
#include "pub_tool_poolalloc.h"     // typedef struct _PoolAlloc PoolAlloc
                                    // found as extern in mc_include.h
#include "pub_tool_tooliface.h"     // typedef struct VgCallbackClosure
                                    // found as extern in mc_include.h
#include "mc_include.h"             // MC_(helperc_LOADV8)


#define MemProfileVT_decl(X) MC_Chunk* X;
#define MemProfileVT_begin(X,Y) X = (MC_Chunk *)VG_(HT_Next)(Y)
#define MemProfileVT_advance(X,Y) X = (MC_Chunk *)VG_(HT_Next)(Y)
#define MemProfileVT_end(X) X != NULL

#define ExecutionContext_decl(X) Addr X;int i = 0 
#define ExecutionContext_begin(X,Y,Z) X = Y[Z]
#define ExecutionContext_advance(X,Y,Z) X = Y[Z]
#define ExecutionContext_end(X, Y) X != Y

#define AddrVbitsVT_decl(X) Addr X
#define AddrVbitsVT_begin(X,Y,Z) X = Y->data, Z = Y->data + Y->szB
#define AddrVbitsVT_advance(X) X += 1     // each Vbits entry tracks the state of 1 Bytes. 1 shadow bit for each memory bit. So hopping 1 Byte after each iteration.
#define AddrVbitsVT_end(X,Y) X < Y

//#define AddrVAbitsVT_decl(X) Addr X
//#define AddrVAbitsVT_begin(X,Y,Z) X = Y->data, Z = Y->data + Y->szB
//#define AddrVAbitsVT_advance(X) X += 4     // each VAbits entry tracks the state of 4 Bytes. So hopping 4 Bytes after each iteration.
//#define AddrVAbitsVT_end(X,Y) X < Y

#define SecMapVT_decl(X) int *X; int i = 0
#define SecMapVT_begin(X,Y,Z) X = (int *)&Y[Z]
#define SecMapVT_advance(X,Y,Z) X = (int *)&Y[Z]
#define SecMapVT_end(X,Y) X != Y

#define PriMapVT_decl(X) SecMap* X; int i = 0
#define PriMapVT_begin(X,Y,Z) X = Y[Z]
#define PriMapVT_advance(X,Y,Z) X = Y[Z]
#define PriMapVT_end(X,Y) X != Y

#define AuxPriL1MapVT_decl(X) auxmap_L1* X; int i = 0
#define AuxPriL1MapVT_begin(X,Y,Z) X = &Y[Z]
#define AuxPriL1MapVT_advance(X,Y,Z) X = &Y[Z]
#define AuxPriL1MapVT_end(X,Y) X != Y

#define AuxPriL2MapVT_decl(X) AuxMapEnt* X;
#define AuxPriL2MapVT_begin(X,Y) X = (AuxMapEnt *)VG_(OSetGen_Next)(Y)
#define AuxPriL2MapVT_advance(X,Y) X = (AuxMapEnt *)VG_(OSetGen_Next)(Y)
#define AuxPriL2MapVT_end(X) X != NULL

#define VBitVT_decl(X) int X;int i = 0
#define VBitVT_begin(X,Y,Z) X = Y[Z]
#define VBitVT_advance(X,Y,Z) X = Y[Z]
#define VBitVT_end(X, Y) X < Y

#define SecVBitNodeVT_decl(X) SecVBitNode* X;
#define SecVBitNodeVT_begin(X,Y) X = (SecVBitNode *)VG_(OSetGen_Next)(Y)
#define SecVBitNodeVT_advance(X,Y) X = (SecVBitNode *)VG_(OSetGen_Next)(Y)
#define SecVBitNodeVT_end(X) X != NULL

#define w32PerCacheLineVT_decl(X) int X; int i = 0
#define w32PerCacheLineVT_begin(X,Y,Z) X = Y[Z]
#define w32PerCacheLineVT_advance(X,Y,Z) X = Y[Z]
#define w32PerCacheLineVT_end(X,Y) X != Y

#define descrPerCacheLineVT_decl(X) int X; int i = 0
#define descrPerCacheLineVT_begin(X,Y,Z) X = Y[Z]
#define descrPerCacheLineVT_advance(X,Y,Z) X = Y[Z]
#define descrPerCacheLineVT_end(X,Y) X != Y

#define OCacheL1VT_decl(X) OCacheLine* X; int set = 0; int line = 0
#define OCacheL1VT_begin(X,Y,Z,W) X = &Y->set[Z].line[W]
#define OCacheL1VT_advance(X,Y,Z,W) X = &Y->set[Z].line[W]
#define OCacheL1VT_end(X,Y) X != Y

#define SuppressionVT_decl(X) Supp *X

#define ErrorVT_decl(X) Error *X; int i = 0
#define ErrorVT_begin(X,Y,Z) X = Y[Z]
#define ErrorVT_advance(X,Y,Z) X = Y[Z]
#define ErrorVT_end(X,Y) X != Y

#define VA_BITS2_NOACCESS     0x0      // 00b
#define VA_BITS2_UNDEFINED    0x1      // 01b
#define VA_BITS2_DEFINED      0x2      // 10b
#define VA_BITS2_PARTDEFINED  0x3      // 11b

#define V_BITS8_DEFINED       0
#define V_BITS8_UNDEFINED     0xFF

#define SM_DIST_NOACCESS   0
#define SM_DIST_UNDEFINED  1
#define SM_DIST_DEFINED    2

#define SM_OFF(aaa)           (((aaa) & 0xffff) >> 2)
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
#define MAX_PRIMARY_ADDRESS (Addr)((((Addr)65536) * N_PRIMARY_MAP)-1)



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

#define BYTES_PER_SEC_VBIT_NODE     16
typedef
   struct {
      Addr  a;
      UChar vbits8[BYTES_PER_SEC_VBIT_NODE];
   }
   SecVBitNode;

#define OC_LINES_PER_SET 2
#define OC_N_SET_BITS    20
#define OC_N_SETS        (1 << OC_N_SET_BITS)
#define OC_BITS_PER_LINE 5
#define OC_W32S_PER_LINE (1 << (OC_BITS_PER_LINE - 2))


typedef
   struct {
      Addr  tag;
      UInt  w32[OC_W32S_PER_LINE];
      UChar descr[OC_W32S_PER_LINE];
   }
   OCacheLine;

typedef
   struct {
      OCacheLine line[OC_LINES_PER_SET];
   }
   OCacheSet;

typedef
   struct {
      OCacheSet set[OC_N_SETS];
   }
   OCache;


// Different kinds of blocks.
typedef enum {
   Block_Mallocd = 111,
   Block_Freed,
   Block_MempoolChunk,
   Block_UserG
} BlockKind;


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
static Addr blockEnd;
//static Addr addrSize;
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

static HChar * get_descr(HChar *text, HChar *plc) {
  if (text != NULL) return text;
  else return plc;
};



static long send_MC_Error(Error *e, int ucase) {
  if (VG_(get_error_kind)(e) == ucase) return (long)VG_(get_error_extra)(e);
  else return 0;
};

static long get_stub(MC_Error *e) {
  return (long)e;
};

static Bool is_valid_oc_tag ( Addr tag ) {
   return 0 == (tag & ((1 << OC_BITS_PER_LINE) - 1));
}

/* Classify and also sanity-check 'line'.  Return 'e' (empty) if not
   in use, 'n' (nonzero) if it contains at least one valid origin tag,
   and 'z' if all the represented tags are zero. */
static UChar classify_OCacheLine ( OCacheLine* line )
{
   UWord i;
   if (line->tag == 1 /* invalid */ )
      return 'e'; /* EMPTY */
   tl_assert(is_valid_oc_tag(line->tag));
   for (i = 0; i < OC_W32S_PER_LINE; i++) {
      tl_assert(0 == ((~0xF) & line->descr[i]));
      if (line->w32[i] > 0 && line->descr[i] > 0)
         return 'n'; /* NONZERO - contains useful info */
   }
   return 'z'; /* ZERO - no useful info */
}

static long sm_offset(Addr base) {
  base &= ~(Addr)0xFFFF;
  return base;
};

static int inPrim(Addr base) {
  if (base <= MAX_PRIMARY_ADDRESS) return 1;
  else return 0;
};

static short getVbits(Addr base) {
  return (short)MC_(helperc_LOADV8)(base);
};
/*
static short getVAbits(Addr base) {
  UChar vabits = -1;
  if (inPrim(base)) {
    UWord pm_off = base >> 16;
    UWord sm_off = SM_OFF(base);
#if VG_DEBUG_MEMORY >= 1
    tl_assert(pm_off < N_PRIMARY_MAP);
#endif
    SecMap *sm = pqlPub_primary_map[ pm_off ];
    vabits = sm->vabits8[ sm_off ];
/ *  char m[100];
    sprintf(m, "%d", (int)vabits);
    printf("returning vabits8: %c (%s)\n", vabits, m);* /
  } else {
    AuxMapEnt  key;
    AuxMapEnt* res;
    Word       i;
    tl_assert(base > MAX_PRIMARY_ADDRESS);
    base &= ~(Addr)0xFFFF;
    for (i = 0; i < N_AUXMAP_L1; i++) {
      if (pqlPub_aux_primary_L1_map[i].base == base) {
        break;
      }
    }
    if (i < N_AUXMAP_L1) {
      res = pqlPub_aux_primary_L1_map[i].ent;
    } else {
      key.base = base;
      key.sm   = 0;
      res = VG_(OSetGen_Lookup)(pqlPub_aux_primary_L2_map, &key);
    }
    if ((res) && (res->sm) && (res->sm != &pqlPub_distinguished_sec_map[SM_DIST_NOACCESS])) {
      UWord sm_off = SM_OFF(base);
      vabits = res->sm->vabits8[ sm_off ];
    }
  }
  return vabits;
};


static short extract_vabits2(Addr base, short vabits8) {
   UInt shift = (base & 3) << 1;          // shift by 0, 2, 4, or 6
   vabits8 >>= shift;                     // shift the two bits to the bottom
/ *   char m[100];
   sprintf(m, "In extract: for addr %ld returning vabits2 %d.\n", (long)base, (int)(0x3 & vabits8));
   printf("%s", m);* /
   return 0x3 & vabits8;                  // mask out the rest
};

static short getVAbits2(Addr base, int indexB) {
  if (base + indexB >= addrSize) return -1;
  short vabits8 = getVAbits(base);
/ *  char m[100];
  sprintf(m, "vabits8 in 1B is: %d, addr is %ld, threshold size is %ld.\n", (int)vabits8, (long)base, (long)addrSize);
  printf("%s", m);* /
  return extract_vabits2(base + indexB, vabits8);
};

static UWord getVbits8(Addr base, int indexB) {
  UWord vabits2 = getVAbits2(base, indexB);
  if (vabits2 == VA_BITS2_PARTDEFINED) {
    Addr aAligned = VG_ROUNDDN(base, BYTES_PER_SEC_VBIT_NODE);
    Int amod     = base % BYTES_PER_SEC_VBIT_NODE;
    SecVBitNode* n = VG_(OSetGen_Lookup)(pqlPub_sec_vbit_table, &aAligned);
    UChar vbits8;
    tl_assert2(n, "get_sec_vbits8: no node for address %p (%p)\n", aAligned, base);
    / * Shouldn't be fully defined or fully undefined--those cases shouldn't
     * make it to the secondary V bits table.
     * /
    vbits8 = n->vbits8[amod];
//    sth is wrong with this implementation; the assert below is triggered.
//    tl_assert(V_BITS8_DEFINED != vbits8 && V_BITS8_UNDEFINED != vbits8);
    return vbits8;
  }
  return -1;
};
*/

$

CREATE STRUCT VIEW MemProfileV (
        addr_data BIGINT FROM data,
        obj_name TEXT FROM {getObjName(tuple_iter->where[0]->ips[1], obj_name)},
        dir_name TEXT FROM {getDirName(tuple_iter->where[0]->ips[1], file_name, dir_name)},
        file_name TEXT FROM {getFileName(tuple_iter->where[0]->ips[1], file_name, dir_name)},
        fn_name TEXT FROM {getFnName(tuple_iter->where[0]->ips[1], fn_name)},
        line_no INT FROM {getLOCNo(tuple_iter->where[0]->ips[1], file_name, dir_name)},
        alloc_by TEXT FROM {getFnName(tuple_iter->where[0]->ips[0], alloc_by)},
        inPrim BIGINT FROM inPrim(tuple_iter->data),
        FOREIGN KEY(vabits_id) FROM tuple_iter REFERENCES AddrVbitsVT POINTER,
        //FOREIGN KEY(vabits_id) FROM tuple_iter REFERENCES AddrVAbitsVT POINTER,
        sizeB BIGINT FROM szB,
        allocKind INT FROM allockind,
        excnt_alloc_id INT FROM where[0]->ecu,
        FOREIGN KEY(ec_alloc_ips_id) FROM where[0] REFERENCES ExecutionContext POINTER,
        excnt_free_id INT FROM where[1]->ecu,
        FOREIGN KEY(ec_free_ips_id) FROM where[1] REFERENCES ExecutionContext POINTER
)$

CREATE VIRTUAL TABLE MemProfileVT
USING STRUCT VIEW MemProfileV
WITH REGISTERED C NAME malloc_list
WITH REGISTERED C TYPE VgHashTable:MC_Chunk*
USING LOOP VG_(HT_ResetIter)(*base);for (MemProfileVT_begin(tuple_iter, *base);MemProfileVT_end(tuple_iter);MemProfileVT_advance(tuple_iter, *base))$

CREATE STRUCT VIEW AddrVAbitsV (
        addr_data BIGINT FROM tuple_iter,
        inPrimaryTable BIGINT FROM inPrim(tuple_iter),
        vbits INT FROM getVbits(tuple_iter)
        //vabits INT FROM getVAbits(tuple_iter),
        //vabits_1B INT FROM {getVAbits2(tuple_iter, 0)},
        //vbits_1B INT FROM {getVbits8(tuple_iter, 0)},
        //vabits_2B INT FROM {getVAbits2(tuple_iter, 1)},
        //vbits_2B INT FROM {getVbits8(tuple_iter, 1)},
        //vabits_3B INT FROM {getVAbits2(tuple_iter, 2)},
        //vbits_3B INT FROM {getVbits8(tuple_iter, 2)},
        //vabits_4B INT FROM {getVAbits2(tuple_iter, 3)},
        //vbits_4B INT FROM {getVbits8(tuple_iter, 3)}
)$

CREATE VIRTUAL TABLE AddrVbitsVT
USING STRUCT VIEW AddrVAbitsV
WITH REGISTERED C TYPE MC_Chunk*:Addr
USING LOOP for (AddrVbitsVT_begin(tuple_iter, base, blockEnd); AddrVbitsVT_end(tuple_iter, blockEnd); AddrVbitsVT_advance(tuple_iter))$

//CREATE VIRTUAL TABLE AddrVAbitsVT
//USING STRUCT VIEW AddrVAbitsV
//WITH REGISTERED C TYPE MC_Chunk*:Addr
//USING LOOP for (AddrVAbitsVT_begin(tuple_iter, base, addrSize); AddrVAbitsVT_end(tuple_iter, addrSize); AddrVAbitsVT_advance(tuple_iter))$
// addrSize is static global variable defined above in the boilerplate part of the DSL


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

CREATE STRUCT VIEW SecMapV (
        vabits INT FROM tuple_iter
)$

CREATE VIRTUAL TABLE SecMapVT
USING STRUCT VIEW SecMapV
WITH REGISTERED C TYPE SecMap:int*
USING LOOP for(SecMapVT_begin(tuple_iter, base->vabits8, i); SecMapVT_end(i, SM_CHUNKS); SecMapVT_advance(tuple_iter, base->vabits8, ++i))$

CREATE STRUCT VIEW PriMapV (
        FOREIGN KEY(secmap_id) FROM tuple_iter REFERENCES SecMapVT POINTER,
//      addr_off BIGINT FROM TODO
)$

CREATE VIRTUAL TABLE PriMapVT
USING STRUCT VIEW PriMapV
WITH REGISTERED C NAME primary_map
WITH REGISTERED C TYPE SecMap**:SecMap*
USING LOOP for(PriMapVT_begin(tuple_iter, base, i); PriMapVT_end(i, N_PRIMARY_MAP); PriMapVT_advance(tuple_iter, base, ++i))$

CREATE STRUCT VIEW AuxMapEntryV (
        addr_data BIGINT FROM base
)$

CREATE VIRTUAL TABLE AuxMapEntryVT
USING STRUCT VIEW AuxMapEntryV
WITH REGISTERED C TYPE AuxMapEnt$

CREATE STRUCT VIEW AuxPriMapV (
        addr_data BIGINT FROM sm_offset(tuple_iter->base),
        entry_addr_data BIGINT FROM ent->base
)$

CREATE VIRTUAL TABLE AuxPriL1MapVT
USING STRUCT VIEW AuxPriMapV
WITH REGISTERED C NAME aux_primary_L1_map
WITH REGISTERED C TYPE auxmap_L1
USING LOOP for(AuxPriL1MapVT_begin(tuple_iter,base, i); AuxPriL1MapVT_end(i, N_AUXMAP_L1); AuxPriL1MapVT_advance(tuple_iter, base, ++i))$

CREATE VIRTUAL TABLE AuxPriL2MapVT
USING STRUCT VIEW AuxMapEntryV
WITH REGISTERED C NAME aux_primary_L2_map
WITH REGISTERED C TYPE OSet:AuxMapEnt*
USING LOOP VG_(OSetGen_ResetIter)(base);for (AuxPriL2MapVT_begin(tuple_iter, base);AuxPriL2MapVT_end(tuple_iter);AuxPriL2MapVT_advance(tuple_iter, base))$

CREATE STRUCT VIEW DistinguishedSecMapV (
        FOREIGN KEY(no_access_sm_id) FROM tuple_iter REFERENCES SecMapVT POINTER
        //FOREIGN KEY(undefined_sm_id) FROM {tuple_iter[1]} REFERENCES SecMapVT,
        //FOREIGN KEY(defined_sm_id) FROM {tuple_iter[2]} REFERENCES SecMapVT
)$

CREATE VIRTUAL TABLE DistinguishedSecMapVT
USING STRUCT VIEW DistinguishedSecMapV
WITH REGISTERED C NAME distinguished_sec_map
WITH REGISTERED C TYPE SecMap$

CREATE STRUCT VIEW VBitV (
        vbit INT FROM tuple_iter
)$

CREATE VIRTUAL TABLE VBitVT
USING STRUCT VIEW VBitV
WITH REGISTERED C TYPE int*:int
USING LOOP for(VBitVT_begin(tuple_iter,base, i); VBitVT_end(i, BYTES_PER_SEC_VBIT_NODE); VBitVT_advance(tuple_iter, base, ++i))$

CREATE STRUCT VIEW SecVBitNodeV (
        addr_data BIGINT FROM a,
        FOREIGN KEY(vbits_id) FROM vbits8 REFERENCES VBitVT POINTER
)$

CREATE VIRTUAL TABLE SecVBitNodeVT
USING STRUCT VIEW SecVBitNodeV
WITH REGISTERED C NAME sec_vbit_table
WITH REGISTERED C TYPE OSet:SecVBitNode*
USING LOOP VG_(OSetGen_ResetIter)(base);for (SecVBitNodeVT_begin(tuple_iter, base);SecVBitNodeVT_end(tuple_iter);SecVBitNodeVT_advance(tuple_iter, base))$

CREATE STRUCT VIEW w32PerCacheLineV (
        w32 INT FROM tuple_iter
)$

CREATE VIRTUAL TABLE w32PerCacheLineVT
USING STRUCT VIEW w32PerCacheLineV
WITH REGISTERED C TYPE OCacheLine:int
USING LOOP for(w32PerCacheLineVT_begin(tuple_iter, base->w32, i); w32PerCacheLineVT_end(i, OC_W32S_PER_LINE); w32PerCacheLineVT_advance(tuple_iter, base->w32, ++i))$

CREATE STRUCT VIEW descrPerCacheLineV (
        descr INT FROM tuple_iter
)$

CREATE VIRTUAL TABLE descrPerCacheLineVT
USING STRUCT VIEW descrPerCacheLineV
WITH REGISTERED C TYPE OCacheLine:int
USING LOOP for(descrPerCacheLineVT_begin(tuple_iter, base->descr, i); descrPerCacheLineVT_end(i, OC_W32S_PER_LINE); descrPerCacheLineVT_advance(tuple_iter, base->w32, ++i))$


CREATE STRUCT VIEW OCacheL1V (
        addr_tag BIGINT FROM tag,
        classification INT FROM classify_OCacheLine(tuple_iter),
        FOREIGN KEY(w32_id) FROM tuple_iter REFERENCES w32PerCacheLineVT POINTER,
        FOREIGN KEY(descr_id) FROM tuple_iter REFERENCES descrPerCacheLineVT POINTER
)$

CREATE VIRTUAL TABLE OCacheL1VT
USING STRUCT VIEW OCacheL1V
WITH REGISTERED C NAME ocache_L1
WITH REGISTERED C TYPE OCache:OCacheLine*
USING LOOP for(set = 0; set < OC_N_SETS; set++) {
           for(OCacheL1VT_begin(tuple_iter, base, set, line); OCacheL1VT_end(line, OC_LINES_PER_SET); OCacheL1VT_advance(tuple_iter, base, set, ++line))$


CREATE STRUCT VIEW ErrorValueV (
        size_bytes BIGINT FROM Err.Value.szB,
        origin_tag INT FROM Err.Value.otag,
        FOREIGN KEY(origin_execution_context_id) FROM Err.Value.origin_ec REFERENCES ExecutionContext POINTER
)$


CREATE VIRTUAL TABLE ErrorValueVT
USING STRUCT VIEW ErrorValueV
WITH REGISTERED C TYPE MC_Error$

CREATE STRUCT VIEW ErrorCondV (
        origin_tag INT FROM Err.Cond.otag,
        FOREIGN KEY(origin_execution_context_id) FROM Err.Cond.origin_ec REFERENCES ExecutionContext POINTER
)$

CREATE VIRTUAL TABLE ErrorCondVT
USING STRUCT VIEW ErrorCondV
WITH REGISTERED C TYPE MC_Error$

CREATE STRUCT VIEW ErrorCoreMemV (
        stub BIGINT FROM get_stub(tuple_iter)
)$

CREATE VIRTUAL TABLE ErrorCoreMemVT
USING STRUCT VIEW ErrorCoreMemV
WITH REGISTERED C TYPE MC_Error$

CREATE STRUCT VIEW ErrorAddrV (
        is_write INT FROM Err.Addr.isWrite,
        size_bytes BIGINT FROM Err.Addr.szB,
        maybe_Gcc INT FROM Err.Addr.maybe_gcc,
// instrument AddrInfo and use includes struct view or join
        address_info_tag INT FROM Err.Addr.ai.tag
)$

CREATE VIRTUAL TABLE ErrorAddrVT
USING STRUCT VIEW ErrorAddrV
WITH REGISTERED C TYPE MC_Error$

CREATE STRUCT VIEW ErrorJumpV (
        stub BIGINT FROM get_stub(tuple_iter),
        address_info_tag INT FROM Err.Jump.ai.tag
)$

CREATE VIRTUAL TABLE ErrorJumpVT
USING STRUCT VIEW ErrorJumpV
WITH REGISTERED C TYPE MC_Error$

CREATE STRUCT VIEW ErrorUserV (
        is_address_error INT FROM Err.User.isAddrErr,
        address_info_tag INT FROM Err.User.ai.tag,
        origin_tag INT FROM Err.User.otag,
        FOREIGN KEY(origin_execution_context_id) FROM Err.User.origin_ec REFERENCES ExecutionContext POINTER
)$

CREATE STRUCT VIEW ErrorRegParamV (
        originTag INT FROM Err.RegParam.otag,
        FOREIGN KEY(origin_execontext_id) FROM Err.RegParam.origin_ec REFERENCES ExecutionContext POINTER
)$

CREATE VIRTUAL TABLE ErrorRegParamVT
USING STRUCT VIEW ErrorRegParamV
WITH REGISTERED C TYPE MC_Error$

CREATE STRUCT VIEW ErrorMemParamV (
        originTag INT FROM Err.MemParam.otag,
        FOREIGN KEY(origin_execontext_id) FROM Err.MemParam.origin_ec REFERENCES ExecutionContext POINTER
)$

CREATE VIRTUAL TABLE ErrorMemParamVT
USING STRUCT VIEW ErrorMemParamV
WITH REGISTERED C TYPE MC_Error$

CREATE VIRTUAL TABLE ErrorUserVT
USING STRUCT VIEW ErrorUserV
WITH REGISTERED C TYPE MC_Error$

CREATE STRUCT VIEW ErrorFreeV (
        stub BIGINT FROM get_stub(tuple_iter),
        addrInfoTag INT FROM Err.Free.ai.tag
)$

CREATE VIRTUAL TABLE ErrorFreeVT
USING STRUCT VIEW ErrorFreeV
WITH REGISTERED C TYPE MC_Error$

CREATE STRUCT VIEW ErrorFreeMismatchV (
        stub BIGINT FROM get_stub(tuple_iter),
        addrInfoTag INT FROM Err.FreeMismatch.ai.tag
)$

CREATE VIRTUAL TABLE ErrorFreeMismatchVT
USING STRUCT VIEW ErrorFreeMismatchV
WITH REGISTERED C TYPE MC_Error$

CREATE STRUCT VIEW ErrorOverlapV (
        addrSrc BIGINT FROM Err.Overlap.src,
        addrDest BIGINT FROM Err.Overlap.dst,
        sizeB BIGINT FROM Err.Overlap.szB
)$

CREATE VIRTUAL TABLE ErrorOverlapVT
USING STRUCT VIEW ErrorOverlapV
WITH REGISTERED C TYPE MC_Error$

CREATE STRUCT VIEW ErrorLeakV (
        records_leaked INT FROM Err.Leak.n_total_records,
// instrument LossRecord
        size_bytes BIGINT FROM Err.Leak.lr->szB
)$

CREATE VIRTUAL TABLE ErrorLeakVT
USING STRUCT VIEW ErrorLeakV
WITH REGISTERED C TYPE MC_Error$

CREATE STRUCT VIEW ErrorIllegalMempoolV (
        stub BIGINT FROM get_stub(tuple_iter),
        address_info_tag INT FROM Err.IllegalMempool.ai.tag
)$

CREATE VIRTUAL TABLE ErrorIllegalMempoolVT
USING STRUCT VIEW ErrorIllegalMempoolV
WITH REGISTERED C TYPE MC_Error$

CREATE STRUCT VIEW ErrorV (
        name TEXT FROM string,
        id INT FROM unique,
        addr_data BIGINT FROM addr,
        thread_id INT FROM tid,
        count INT FROM count,
        FOREIGN KEY(execontext_id) FROM where REFERENCES ExecutionContext POINTER,
        kind INT FROM ekind,
        mc_kind INT FROM VG_(get_error_kind)(tuple_iter),
        FOREIGN KEY(value_id) FROM {send_MC_Error(tuple_iter, 0)} REFERENCES ErrorValueVT POINTER,
        FOREIGN KEY(cond_id) FROM {send_MC_Error(tuple_iter, 1)} REFERENCES ErrorCondVT POINTER,
        FOREIGN KEY(coremem_id) FROM {send_MC_Error(tuple_iter, 2)} REFERENCES ErrorCoreMemVT POINTER,
        FOREIGN KEY(addr_id) FROM {send_MC_Error(tuple_iter, 3)} REFERENCES ErrorAddrVT POINTER,
        FOREIGN KEY(jump_id) FROM {send_MC_Error(tuple_iter, 4)} REFERENCES ErrorJumpVT POINTER,
        FOREIGN KEY(regparam_id) FROM {send_MC_Error(tuple_iter, 5)} REFERENCES ErrorRegParamVT POINTER,
        FOREIGN KEY(memparam_id) FROM {send_MC_Error(tuple_iter, 6)} REFERENCES ErrorMemParamVT POINTER,
        FOREIGN KEY(user_id) FROM {send_MC_Error(tuple_iter, 7)} REFERENCES ErrorUserVT POINTER,
        FOREIGN KEY(free_id) FROM {send_MC_Error(tuple_iter, 8)} REFERENCES ErrorFreeVT POINTER,
        FOREIGN KEY(freemismatch_id) FROM {send_MC_Error(tuple_iter, 9)} REFERENCES ErrorFreeMismatchVT POINTER,
        FOREIGN KEY(overlap_id) FROM {send_MC_Error(tuple_iter, 10)} REFERENCES ErrorOverlapVT POINTER,
        FOREIGN KEY(leak_id) FROM {send_MC_Error(tuple_iter, 11)} REFERENCES ErrorLeakVT POINTER,
        FOREIGN KEY(illegalmempool_id) FROM {send_MC_Error(tuple_iter, 12)} REFERENCES ErrorIllegalMempoolVT POINTER
)$


//   switch (VG_(get_error_kind)(err)) {
CREATE VIRTUAL TABLE ErrorVT
USING STRUCT VIEW ErrorV
WITH REGISTERED C NAME errorList
WITH REGISTERED C TYPE Error_PiCO_QL:Error*
USING LOOP for(ErrorVT_begin(tuple_iter, base->errlist, i); ErrorVT_end(i, base->size); ErrorVT_advance(tuple_iter, base->errlist, ++i))$
	

CREATE STRUCT VIEW SuppressionV (
        name TEXT FROM sname,
        description TEXT FROM {HChar plc[] = "N/A";
                                get_descr(tuple_iter->string, plc)},
        kind INT FROM skind,
        count INT FROM count,
        nCallers INT FROM n_callers
)$

CREATE VIRTUAL TABLE SuppressionVT
USING STRUCT VIEW SuppressionV
WITH REGISTERED C NAME suppressionList
WITH REGISTERED C TYPE Supp
USING LOOP for(tuple_iter = base; tuple_iter != NULL; tuple_iter = tuple_iter->next)$

CREATE VIEW VBitTagged AS
        SELECT base, addr_data, inPrimaryTable, vBits,
                (SELECT CASE WHEN vBits = 0 THEN 'defined'
                        WHEN vBits = 255 THEN 'undefined'
                        ELSE 'partdefined' END) vBitTag
        FROM AddrVbitsVT;$

//CREATE VIEW VAbitTags AS
//        SELECT base, addr_data, inPrimaryTable, vabits,
//                (SELECT CASE WHEN vabits = 170 THEN 'defined'
//                        ELSE 'some_undefined' END) VATag,
//                (SELECT CASE WHEN vabits & 3 = 0 THEN 'noaccess'
//                        WHEN vabits & 3 = 1 THEN 'undefined'
//                        WHEN vabits & 3 = 2 THEN 'defined'
//                        WHEN vabits & 3 = 3 THEN 'partdefined' END) VATag_1B,
//                (SELECT CASE WHEN vbits_1B = 0 THEN 'defined-VG_BUG?'
//                        WHEN vbits_1B = -1 THEN '-'
//                        WHEN vbits_1B = 255 THEN 'undefined-VG_BUG?'
//                        ELSE vbits_1B END) VTag_1B,
//                (SELECT CASE WHEN (vabits >> 2) & 3 = 0 THEN 'noaccess'
//                        WHEN (vabits >> 2) & 3 = 1 THEN 'undefined'
//                        WHEN (vabits >> 2) & 3 = 2 THEN 'defined'
//                        WHEN (vabits >> 2) & 3 = 3 THEN 'partdefined' END) VATag_2B,
//                (SELECT CASE WHEN vbits_2B = 0 THEN 'defined-VG_BUG?'
//                        WHEN vbits_2B = -1 THEN '-'
//                        WHEN vbits_2B = 255 THEN 'undefined-VG_BUG?'
//                        ELSE vbits_2B END) VTag_2B,
//                (SELECT CASE WHEN (vabits >> 4) & 3 = 0 THEN 'noaccess'
//                        WHEN (vabits >> 4) & 3 = 1 THEN 'undefined'
//                        WHEN (vabits >> 4) & 3 = 2 THEN 'defined'
//                        WHEN (vabits >> 4) & 3 = 3 THEN 'partdefined' END) VATag_3B,
//                (SELECT CASE WHEN vbits_3B = 0 THEN 'defined-VG_BUG?'
//                        WHEN vbits_3B = -1 THEN '-'
//                        WHEN vbits_3B = 255 THEN 'undefined-VG_BUG?'
//                        ELSE vbits_3B END) VTag_3B,
//                (SELECT CASE WHEN (vabits >> 6) & 3 = 0 THEN 'noaccess'
//                        WHEN (vabits >> 6) & 3 = 1 THEN 'undefined'
//                        WHEN (vabits >> 6) & 3 = 2 THEN 'defined'
//                        WHEN (vabits >> 6) & 3 = 3 THEN 'partdefined' END) VATag_4B,
//                (SELECT CASE WHEN vbits_4B = 0 THEN 'defined-VG_BUG?'
//                        WHEN vbits_4B = -1 THEN '-'
//                        WHEN vbits_4B = 255 THEN 'undefined-VG_BUG?'
//                        ELSE vbits_4B END) VTag_4B
//        FROM AddrVAbitsVT;$

CREATE VIEW ClassifyOCacheLine AS
        SELECT addr_tag, classification,
                (SELECT CASE WHEN classification=101 THEN 'no_useful_info'
                        WHEN classification=122 THEN 'empty'
                        ELSE 'useful_info' END) clf_tag,
                w32_id, descr_id
        FROM OCacheL1VT;$


CREATE VIEW InspectErrorQ AS
        SELECT
                (SELECT CASE WHEN kind = 0 THEN 'Err_Value'
                             WHEN kind = 1 THEN 'Err_Cond'
                             WHEN kind = 2 THEN 'Err_CoreMem'
                             WHEN kind = 3 THEN 'Err_Addr'
                             WHEN kind = 4 THEN 'Err_Jump'
                             WHEN kind = 5 THEN 'Err_RegParam'
                             WHEN kind = 6 THEN 'Err_MemParam'
                             WHEN kind = 7 THEN 'Err_User'
                             WHEN kind = 8 THEN 'Err_Free'
                             WHEN kind = 9 THEN 'Err_FreeMismatch'
                             WHEN kind = 10 THEN 'Err_Overlap'
                             WHEN kind = 11 THEN 'Err_Leak'
                             WHEN kind = 12 THEN 'Err_IllegalMempool' END) mcErrorTag, *
        FROM ErrorVT
        JOIN ErrorValueVT EV
        ON EV.base = value_id
        JOIN ErrorCondVT EC
        ON EC.base = cond_id
        JOIN ErrorCoreMemVT ECM
        ON ECM.base = coremem_id
        JOIN ErrorAddrVT EA
        ON EA.base = addr_id
        JOIN ErrorJumpVT EJ
        ON EJ.base = jump_id
        JOIN ErrorRegParamVT ERP
        ON ERP.base = regparam_id
        JOIN ErrorMemParamVT EMP
        ON EMP.base = memparam_id
        JOIN ErrorUserVT EU
        ON EU.base = user_id
        JOIN ErrorFreeVT EF
        ON EF.base = free_id
        JOIN ErrorFreeMismatchVT EFM
        ON EFM.base = freemismatch_id
        JOIN ErrorOverlapVT EO
        ON EO.base = overlap_id
        JOIN ErrorLeakVT EL
        ON EL.base = leak_id
        JOIN ErrorIllegalMempoolVT EIM
        ON EIM.base = illegalmempool_id;


CREATE VIEW OrderSizeGetStackMemProfileQ AS
        SELECT M.addr_data, IP.fn_name, IP.line_no,
                IP.addr_data, IP.file_name,
                IP.obj_name, sizeB
        FROM MemProfileVT M
        JOIN ExecutionContext IP
        ON base = ec_alloc_ips_id
        ORDER BY sizeB DESC, M.addr_data;$

	
CREATE VIEW RangeSizeMemProfileQ AS
	SELECT fn_name, sizeB/1000000 AS size_ranges,
	  COUNT(*) AS blocks_in_range
	FROM MemProfileVT
	GROUP BY size_ranges
	ORDER BY size_ranges;$

CREATE VIEW SizePerLOCMemProfileQ AS
	SELECT fn_name, line_no, addr_data,
		file_name, obj_name,
		 SUM(sizeB) AS totalSizePerLOC
	FROM MemProfileVT
	GROUP BY file_name, fn_name, line_no
        HAVING totalSizePerLOC > 1000000
	ORDER BY totalSizePerLOC DESC;$

CREATE VIEW GroupFunctionMemProfileQ AS
	SELECT fn_name, line_no, addr_data,
		file_name, obj_name, alloc_by,
		sizeB
	FROM MemProfileVT
        WHERE fn_name LIKE '%lookup%'
	ORDER BY sizeB DESC;$

CREATE VIEW bytesWastedMemProfileQ AS
        SELECT block_addr, sizeB, fn_name, line_no,
                SUM(bytesWasted)
        FROM (
                SELECT M.addr_data AS block_addr, sizeB, fn_name,
                       line_no, vbits, vBitTag,
                       CAST(vbitTag = 'undefined' AS INTEGER) AS bytesWasted
                FROM MemProfileVT M
                JOIN VBitTagged
                ON base=vabits_id
                WHERE vBitTag = 'undefined'
        ) BW
        GROUP BY block_addr
        ORDER BY SUM(bytesWasted) DESC;$


//CREATE VIEW LocatePDBMemProfileQ AS
//        SELECT M.addr_data, fn_name,
//                sizeB, COUNT(*) AS PDBs
//        FROM MemProfileVT M
//        JOIN VAbitTags
//        ON base=vabits_id
//        WHERE VAtag_1B = 'partdefined'
//        OR VAtag_2B = 'partdefined'
//        OR VAtag_3B = 'partdefined'
//        OR VAtag_4B = 'partdefined'
//        GROUP BY M.addr_data
//        ORDER BY PDBs;$

//CREATE VIEW wordBytesWastedMemProfileQ AS
//        SELECT block_addr, sizeB, fn_name, line_no,
//                SUM(wordBytesWasted), COUNT(*) AS words
//        FROM (
//                SELECT M.addr_data AS block_addr, sizeB, fn_name,
//                       line_no,
//                        CAST(VAtag_1B <> 'defined' AS INTEGER) +
//                        CAST(VAtag_2B <> 'defined' AS INTEGER) +
//                        CAST(VAtag_3B <> 'defined' AS INTEGER) +
//                        CAST(VAtag_4B <> 'defined' AS INTEGER) wordBytesWasted
//                FROM MemProfileVT M
//                JOIN VAbitTags
//                ON base=vabits_id
//                WHERE VATag <> 'defined'
//        ) BW
//        GROUP BY block_addr
//        ORDER BY SUM(wordBytesWasted) DESC, words DESC;$

