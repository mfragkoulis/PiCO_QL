#include <map>
#include <string>
#include <deque>
#include <vector>
#include <stack>
#include <iterator>
#include <iostream>
#include <fstream>
#include <list>
#include <set>
#include <utility>
#include <functional>
#include <algorithm>		// set_difference
#include <cctype>
#include <sstream>		// ostringstream
#include <cstdio>		// perror, rename
#include <cstdlib>		// atoi
#include <cerrno>		// errno

#include <boost/iostreams/positioning.hpp>

#include "swill.h"
#include "getopt.h"

#include <regex.h>

#include "cpp.h"
#include "debug.h"
#include "error.h"
#include "ytab.h"
#include "attr.h"
#include "metrics.h"
#include "fileid.h"
#include "filemetrics.h"
#include "idquery.h"
#include "call.h"
$

CREATE STRUCT VIEW File (
	path STRING FROM get_path(),
	id INTEGER FROM get_id(),
	fname STRING FROM get_fname(),
	dir STRING FROM get_dir(),
	NSTATEMENT INTEGER FROM metrics().get_int_metric(FileMetrics::em_nstatement),
	NCOPIES INTEGER FROM metrics().get_int_metric(FileMetrics::em_ncopies),
	NPFUNCTION INTEGER FROM metrics().get_int_metric(FileMetrics::em_npfunction),
	NFFUNCTION INTEGER FROM metrics().get_int_metric(FileMetrics::em_nffunction),
	NPVAR INTEGER FROM metrics().get_int_metric(FileMetrics::em_npvar),
	NFVAR INTEGER FROM metrics().get_int_metric(FileMetrics::em_nfvar),
	NAGGREGATE INTEGER FROM metrics().get_int_metric(FileMetrics::em_naggregate),
	NAMEMBER INTEGER FROM metrics().get_int_metric(FileMetrics::em_namember),
	NENUM INTEGER FROM metrics().get_int_metric(FileMetrics::em_nenum),
	NEMEMBER INTEGER FROM metrics().get_int_metric(FileMetrics::em_nemember),
	NINCFILE INTEGER FROM metrics().get_int_metric(FileMetrics::em_nincfile),
	readonly INTEGER FROM get_readonly()
)$

// File details gathered during parsing for GUI operation
CREATE VIRTUAL TABLE File
USING STRUCT VIEW File
WITH REGISTERED C NAME files
WITH REGISTERED C TYPE vector<Fileid>$


CREATE STRUCT VIEW Eclass (
	len INTEGER FROM get_len(),
	size INTEGER FROM get_size(),
	unused BOOLEAN FROM is_unused(),
        // True if this equivalence class is unintentionally unused
	isDeclaredUnused BOOLEAN FROM get_attribute(is_declared_unused),
        // Declared with __unused__ attribute
	isMacroToken BOOLEAN FROM get_attribute(is_macro_token),
        // Identifier stored in a macro

	isReadonly BOOLEAN FROM get_attribute(is_readonly),
        // Read-only; true if any member
										// comes from an ro file
	// The four C namespaces
	isSuetag BOOLEAN FROM get_attribute(is_suetag),
        // Struct/union/enum tag
	isSumember BOOLEAN FROM get_attribute(is_sumember),
        // Struct/union member
	isLabel BOOLEAN FROM get_attribute(is_label),
        // Goto label
	isOrdinary BOOLEAN FROM get_attribute(is_ordinary),
        // Ordinary identifier

	isMacro BOOLEAN FROM get_attribute(is_macro),
        // Name of an object or function-like macro
	isUndefined_macro BOOLEAN FROM get_attribute(is_undefined_macro),
        // Macro (heuristic: ifdef, defined)
	isMacro_arg BOOLEAN FROM get_attribute(is_macro_arg),
        // Macro argument
	// The following are valid if isOrdinary is true:
	isCscope BOOLEAN FROM get_attribute(is_cscope),
        // Compilation-unit (file) scoped
				// identifier  (static)
	isLscope BOOLEAN FROM get_attribute(is_lscope),
        // Linkage-unit scoped identifier
	isTypedef BOOLEAN FROM get_attribute(is_typedef),
        // Typedef
	isEnum BOOLEAN FROM get_attribute(is_enum),
        // Enumeration member
	isYacc BOOLEAN FROM get_attribute(is_yacc),
        // Yacc identifier
	isFunction BOOLEAN FROM get_attribute(is_function),
        // Function
        FOREIGN KEY(members) FROM get_members() REFERENCES Tokid
)$

CREATE STRUCT VIEW Identifier (
	id STRING FROM get_id(),
	xfile BOOLEAN FROM get_xfile()
)$

CREATE STRUCT VIEW IdentifierProperties (
      INCLUDES STRUCT VIEW Eclass FROM first POINTER,
      INCLUDES STRUCT VIEW Identifier FROM second
)$

CREATE VIRTUAL TABLE IdentifierProperties
USING STRUCT VIEW IdentifierProperties
WITH REGISTERED C NAME ids
WITH REGISTERED C TYPE map<Eclass *, Identifier>$

CREATE STRUCT VIEW Tokid (
	// Make it an integer on all systems
	offset INTEGER FROM (int)boost::iostreams::position_to_offset(this.get_streampos()),
        INCLUDES STRUCT VIEW File FROM get_fileid()
)$

CREATE VIRTUAL TABLE Tokid
USING STRUCT VIEW Tokid
WITH REGISTERED C TYPE set<Tokid>$

CREATE STRUCT VIEW TokidEquivalenceClasses (
      INCLUDES STRUCT VIEW Tokid FROM first,
      INCLUDES STRUCT VIEW Eclass FROM second POINTER
)$

CREATE VIRTUAL TABLE TokidEquivalenceClasses
USING STRUCT VIEW TokidEquivalenceClasses
WITH REGISTERED C NAME tm
WITH REGISTERED C TYPE map<Tokid, Eclass *>$

CREATE STRUCT VIEW Call (
	name STRING FROM get_name(),
	isDefined BOOLEAN FROM is_defined(),
	isDeclared BOOLEAN FROM is_declared(),
	isFileScoped BOOLEAN FROM is_file_scoped(),
	isCFun BOOLEAN FROM is_cfun(),
	isMacro BOOLEAN FROM is_macro(),
	NSTMT DOUBLE FROM metrics().get_metric(FunMetrics::em_nstmt),
	NOP DOUBLE FROM metrics().get_metric(FunMetrics::em_nop),
	NUOP DOUBLE FROM metrics().get_metric(FunMetrics::em_nuop),
	NNCONST DOUBLE FROM metrics().get_metric(FunMetrics::em_nnconst),
	NCLIT DOUBLE FROM metrics().get_metric(FunMetrics::em_nclit),
	NIF DOUBLE FROM metrics().get_metric(FunMetrics::em_nif),
	NELSE DOUBLE FROM metrics().get_metric(FunMetrics::em_nelse),
	NSWITCH DOUBLE FROM metrics().get_metric(FunMetrics::em_nswitch),
	NCASE DOUBLE FROM metrics().get_metric(FunMetrics::em_ncase),
	NDEFAULT DOUBLE FROM metrics().get_metric(FunMetrics::em_ndefault),
	NBREAK DOUBLE FROM metrics().get_metric(FunMetrics::em_nbreak),
	NFOR DOUBLE FROM metrics().get_metric(FunMetrics::em_nfor),
	NWHILE DOUBLE FROM metrics().get_metric(FunMetrics::em_nwhile),
	NDO DOUBLE FROM metrics().get_metric(FunMetrics::em_ndo),
	NCONTINUE DOUBLE FROM metrics().get_metric(FunMetrics::em_ncontinue),
	NGOTO DOUBLE FROM metrics().get_metric(FunMetrics::em_ngoto),
	NRETURN DOUBLE FROM metrics().get_metric(FunMetrics::em_nreturn),
	NPID DOUBLE FROM metrics().get_metric(FunMetrics::em_npid),
	NFID DOUBLE FROM metrics().get_metric(FunMetrics::em_nfid),
	NMID DOUBLE FROM metrics().get_metric(FunMetrics::em_nmid),
	NID DOUBLE FROM metrics().get_metric(FunMetrics::em_nid),
	NUPID DOUBLE FROM metrics().get_metric(FunMetrics::em_nupid),
	NUFID DOUBLE FROM metrics().get_metric(FunMetrics::em_nufid),
	NUMID DOUBLE FROM metrics().get_metric(FunMetrics::em_numid),
	NUID DOUBLE FROM metrics().get_metric(FunMetrics::em_nuid),
	NGNSOC DOUBLE FROM metrics().get_metric(FunMetrics::em_ngnsoc),
	NPARAM DOUBLE FROM metrics().get_metric(FunMetrics::em_nparam),
	MAXNEST DOUBLE FROM metrics().get_metric(FunMetrics::em_maxnest),
	NLABEL DOUBLE FROM metrics().get_metric(FunMetrics::em_nlabel),
	FANIN DOUBLE FROM metrics().get_metric(FunMetrics::em_fanin),
	FANOUT DOUBLE FROM metrics().get_metric(FunMetrics::em_fanout),
	CCYCL1 DOUBLE FROM metrics().get_metric(FunMetrics::em_ccycl1),
	CCYCL2 DOUBLE FROM metrics().get_metric(FunMetrics::em_ccycl2),
	CCYCL3 DOUBLE FROM metrics().get_metric(FunMetrics::em_ccycl3),
	CSTRUC DOUBLE FROM metrics().get_metric(FunMetrics::em_cstruc),
	CHAL DOUBLE FROM metrics().get_metric(FunMetrics::em_chal),
	IFLOW DOUBLE FROM metrics().get_metric(FunMetrics::em_iflow),
	entityTypeName STRING FROM entity_type_name(),
	INCLUDES STRUCT VIEW Tokid FROM get_tokid(),
        INCLUDES STRUCT VIEW File FROM get_fileid()
)$

CREATE STRUCT VIEW FunctionMap (
      INCLUDES STRUCT VIEW Tokid FROM first,
      INCLUDES STRUCT VIEW Call FROM second POINTER
)$

CREATE VIRTUAL TABLE FunctionMap
USING STRUCT VIEW FunctionMap
WITH REGISTERED C NAME fun_map
WITH REGISTERED C TYPE map<Tokid, Call *>$
