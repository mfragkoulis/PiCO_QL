#
#   Parse a user description, which conforms to the DSL, and generate the 
#   application specific filter and projection functions for the virtual tables
#   described.
#
#   Copyright 2012 Marios Fragkoulis
#
#   Licensed under the Apache License, Version 2.0
#   (the "License");you may not use this file except in
#   compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in
#   writing, software distributed under the License is
#   distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
#   express or implied.
#   See the License for the specific language governing
#   permissions and limitations under the License.

# All code blocks for generation gathered in this class in class methods
# in the form of HERE-documents.
class CodeToGenerate

  def CodeToGenerate.Cursor_current
    return  <<-AG5
    int index = stcsr->current;
    iter = any_dstr->begin();
    for (int i = 0; i < stcsr->resultSet[index]; i++) {
        iter++;
    }
AG5
  end


  def CodeToGenerate.Error_case
    return <<-EC
    if (stl->zErr) {
        sqlite3_free(stl->zErr);
        return SQLITE_MISUSE;
    }
EC
  end


  def CodeToGenerate.Stl_fill_resultset
    return <<-SFR
        for (int j = 0; j < size; j++) {
            stcsr->resultSet[j] = j;
            stcsr->size++;
	}
        assert(stcsr->size <= stcsr->max_size);
        assert(&stcsr->resultSet[stcsr->size] <= &stcsr->resultSet[stcsr->max_size]);
SFR
  end


  def CodeToGenerate.Typesafe_block
    return <<-TB
            vtd_iter = vt_directory.find(stl->zName);
            if ((vtd_iter == vt_directory.end()) || (vtd_iter->second == 0)) {
                printf("Invalid cast to %s\\n", stl->zName);
                return SQLITE_MISUSE;
            }
            vt_directory[stl->zName] = 0;
TB
  end


  def CodeToGenerate.Resultset_alloc
    return <<-RAL
        int *temp_res;
	temp_res = (int *)sqlite3_malloc(sizeof(int)  * stcsr->max_size);
        if (!temp_res) {
            printf("Error in allocating memory\\n");
            return SQLITE_NOMEM;
        }
RAL
  end


  def CodeToGenerate.Class_sign(signature)
    return <<-CS
STL class signature not properly given:
template error in #{signature} \\n\\n NOW EXITING. \\n
CS
  end


  def CodeToGenerate.Cls_search
    return <<-CLS
        if ((re = compare_res(count, stcsr, temp_res)) != 0)
            return re;
        sqlite3_free(temp_res);
    }
    return SQLITE_OK;
}


CLS
  end

  
  def CodeToGenerate.Els_case
    return <<-ELS
    } else {
        stl->data = NULL;
        stl->embedded = 1;
    }
    vt_directory[stl->zName] = 0;
}

ELS
  end


  def CodeToGenerate.Prepare_thread(tables)
      return <<-PRTD
// Thread. Creates the queries and passes them on to SQTL.
void * thread_sqlite(void *data) {
    const char **queries, **table_names;
    queries = (const char **)sqlite3_malloc(sizeof(char *) *
                   #{tables.length.to_s});
    table_names = (const char **)sqlite3_malloc(sizeof(char *) *
                   #{tables.length.to_s});
    int failure = 0;
PRTD
  end


  # Maybe adjust so that create queries are grouped by database.
  def CodeToGenerate.Init_sqtl(tables) 
    return <<-INSQTL
    failure = register_table( "#{tables[0].db}" , #{tables.length.to_s}, queries, table_names, data);
    printf(\"Thread sqlite returning..\\n\");
    sqlite3_free(queries);
    sqlite3_free(table_names);
    return (void *)failure;
}


// The API with user application code. Creates the SQTL thread.
int call_sqtl() {
    pthread_t sqlite_thread;
    int re_sqlite = pthread_create(&sqlite_thread, NULL, thread_sqlite, NULL);
    pthread_join(sqlite_thread, NULL);
    return re_sqlite;
}

INSQTL
  end


  def CodeToGenerate.Directives(directives)
    return <<-DIR
/*                                                         
 *   Implement the filter and projection functions for 
 *   each of the registered virtual tables.
 *                                                         
 *   Copyright 2012 Marios Fragkoulis
 *                                                         
 *   Licensed under the Apache License, Version 2.0        
 *   (the "License");you may not use this file except in   
 *   compliance with the License.                          
 *   You may obtain a copy of the License at               
 *                                                         
 *       http://www.apache.org/licenses/LICENSE-2.0        
 *                                                         
 *   Unless required by applicable law or agreed to in     
 *   writing, software distributed under the License is    
 *   distributed on an "AS IS" BASIS.                      
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either  
 *   express or implied.                                   
 *   See the License for the specific language governing   
 *  permissions and limitations under the License.         
 */

#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>
#include <map>
#{directives}
#include "stl_search.h"
#include "user_functions.h"
#include "workers.h"


using namespace std;

struct name_cmp {
    bool operator()(const char *a, const char *b) {
        return strcmp(a, b) < 0;
    }
};

static map<const char *, int, name_cmp> vt_directory;
static map<const char *, int, name_cmp>::iterator vtd_iter;
DIR
  end


  def CodeToGenerate.Equals_base
    return <<-EQB
// Each embedded virtual table has a column named 'base'.
// This function checks if a provided column name is indeed 'base'.
int equals_base(const char *zCol) {
    int length = (int)strlen(zCol) + 1;
    char copy[length], *token;
    memcpy(copy, zCol, length);
    token = strtok(copy, " ");
    if (token != NULL) {
        if (!strcmp(token, "base"))
            return true;
        else
            return false;
    } else
        return SQLITE_NOMEM;
}


EQB
  end
    

  def CodeToGenerate.Makefile
    return <<-MKF
ifdef RELEASE
CXXFLAGS=-D_NDEBUG -O2
CFLAGS=-D_NDEBUG -O2
else
CXXFLAGS=-W -g -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_GLIBCXX_DEBUG -D_GLIBCXX_CONCEPT_CHECKS -D_GLIBCXX_FULLY_DYNAMIC_STRING -DTEST
CFLAGS=-W -g -DTEST
ifdef TYPESAFE
CXXFLAGS+=-DTYPESAFE
CFLAGS+=-DTYPESAFE
endif
endif
ifdef DEBUG
CXXFLAGS+=-DDEBUG
CFLAGS+=-DDEBUG
endif

OBJ=main.o stl_search.o stl_to_sql.o user_functions.o workers.o stl_test.o

executable: $(OBJ)
        $(CXX) -lswill -lsqlite3 $(CXXFLAGS) $(OBJ) -o $@

stl_search.o: stl_search.cpp stl_search.h user_functions.h workers.h

stl_to_sql.o: stl_to_sql.c stl_to_sql.h stl_search.h

user_functions.o: user_functions.c user_functions.h stl_test.h

workers.o: workers.cpp workers.h stl_search.h

stl_test.o: stl_test.c stl_test.h
MKF
  end

end


# Models a column of the Virtual Table (VT).
class Column
  def initialize
    @name = ""
    @data_type = ""
    @cpp_data_type = ""       # Respective C++ data type. Used only 
                              # for string so far.
    @related_to = ""          # Reference to other VT(like a FK).
    @access_path = ""         # The access statement for the column value.
    @type = ""                # Record type (pointer or reference) for 
                              # special columns, the ones that refer to 
                              # other VT.
    @@int_data_types = ["int", "integer", "tinyint", "smallint", 
                        "mediumint", "bigint", "unsigned bigint", "int2",
                        "bool", "boolean", "int8", "numeric"]
    @@double_data_types = ["float", "double", "double precision", "real"]
    @@text_data_types = ["text", "date", "datetime", "clob", "string"]
    @@text_match_data_types = [/character/i, /varchar/i, /nvarchar/i, 
                               /varying character/i, /native character/i,
                               /nchar/i]
  end
  attr_accessor(:name,:data_type,:related_to,:access_path,:type)


# Used to clone a Column object. Ruby does not support deep copies.
def construct(name, data_type, related_to, access_path, type)
    @name = name
    @data_type = data_type
    @related_to = related_to
    @access_path = access_path
    @type = type
end


# Performs case analysis with respect to the column data type (and other)
# and fills the passed variables with values accordingly.
  def bind_datatypes(sqlite3_type, column_cast, sqlite3_parameters, 
                     column_cast_back, access_path)
    tmp_text_array = Array.new      # Do not process the original array.
    tmp_text_array.replace(@@text_match_data_types)
    if @related_to.length > 0       # 'this' (column) refers to other VT.
      if sqlite3_type == "search" 
        return "fk", nil, nil 
      elsif sqlite3_type == "retrieve"
        /_ptr/.match(@name) ? @type = "pointer" : @type = "object"   # In
                        # columns that reference other VTs
                        # users have to declare the type for generating
                        # correct access statement.
        sqlite3_type.replace("int64")
        column_cast.replace("(long int)")
        access_path.replace(@access_path)
        return "fk", @related_to, @type
      end
    end
    if @name == "base"               # 'base' column.
      sqlite3_type.replace("int64")
      column_cast.replace("(long int)")
      return "base", nil, nil
    end
    dt = @data_type.downcase         # Normal data column.
    if @@int_data_types.include?(dt)
      sqlite3_type.replace("int")
    elsif (dt == "blob")
      sqlite3_type.replace("blob")
      column_cast.replace("(const void *)")
      sqlite3_parameters.replace(", -1, SQLITE_STATIC")
    elsif @@double_data_types.include?(dt) ||
        /decimal/i.match(dt)
      sqlite3_type.replace("double")
    elsif @@text_data_types.include?(dt) || 
        tmp_text_array.reject! { |rgx| rgx.match(dt) != nil } != nil
                                    # If match against RegExp, reject.
                                    # Then, if array changed, enter.
      case sqlite3_type
      when "search"
        column_cast.replace("(const unsigned char *)")
      when "retrieve" 
        column_cast.replace("(const char *)")
      end
      sqlite3_type.replace("text")
      if @cpp_data_type == "string"
        column_cast_back.replace(".c_str()") 
      end
      sqlite3_parameters.replace(", -1, SQLITE_STATIC")
    end
    access_path.replace(@access_path)
    return "gen_all", nil, nil
  end


# Validates a column data type.
# The following data types are the ones accepted by sqlite.
  def verify_data_type()
    dt = @data_type.downcase
    tmp_text_array = Array.new
    tmp_text_array.replace(@@text_match_data_types)
    if dt == "string"
      @cpp_data_type.replace(dt)
      @data_type.replace("TEXT")
    elsif @@int_data_types.include?(dt) || 
        @@double_data_types.include?(dt) || 
        /decimal/i.match(dt) != nil ||
        @@text_data_types.include?(dt) || 
        tmp_text_array.reject! { |rgx| rgx.match(dt) != nil } != nil
      return dt
    else
      raise TypeError.new("No such data type #{dt.upcase}\\n")
    end
  end
  

# Matches each column description against a pattern and extracts 
# column traits.
  def set(column)
    column.lstrip!
    column.rstrip!
    if $argD == "DEBUG"
      puts "Column is: #{column}"
    end
    if column.match(/\n/)
      column.gsub!(/\n/, "")
    end
    column_ptn1 = /\$(\w+) FROM (.+)/im
    column_ptn2 = /\$(\w+)/im
    column_ptn3 = /(\w+) (\w+) from table (\w+) with base(\s*)=(\s*)(.+)/im
    column_ptn4 = /(\w+) (\w+) from (.+)/im
    case column
    when column_ptn1
      matchdata = column_ptn1.match(column)
      index = 0
      this_element = $elements.last
      this_columns = this_element.columns
      $elements.each { |el| 
        if el.name == matchdata[1]     # Search all element 
                                       # definitions to find the one 
                                       # specified and include it.
          index = this_element.columns.length - 2
          if index < 0 : index = 0 end
          this_element.columns_delete_last()
          el.columns.each_index { |col| 
            coln = el.columns[col]         # Manually construct a deep copy
            this_columns.push(Column.new)  # of coln and push it to 'this'.
            this_columns.last.construct(coln.name.clone, 
                                        coln.data_type.clone, 
                                        coln.related_to.clone, 
                                        coln.access_path.clone, 
                                        coln.type.clone)
          }
        end
      }
      this_columns.each_index { |col|     # Adapt access path.
        if col > index 
          this_columns[col].access_path.replace(matchdata[2] + 
                                                this_columns[col].access_path)
        end
      }
      return
    when column_ptn2                      # Merely include an element 
                                          # definition.
      matchdata = column_ptn2.match(column)
      $elements.each { |el| 
        if el.name == matchdata[1] : 
            $elements.last.columns = 
            $elements.last.columns_delete_last() | 
            el.columns end
      }
      return
    when column_ptn3
      matchdata = column_ptn3.match(column)
      @name = matchdata[1]
      @data_type = matchdata[2]
      @related_to = matchdata[3]
      @access_path = matchdata[6]
    when column_ptn4
      matchdata = column_ptn4.match(column)
      @name = matchdata[1]
      @data_type = matchdata[2]
      @access_path = matchdata[3]
    end
    verify_data_type()
    if @access_path.match(/self/)
      @access_path.gsub!(/self/,"")
    end
    if $argD == "DEBUG"
      puts "Column name is: " + @name
      puts "Column data type is: " + @data_type
      puts "Column related to: " + @related_to
      puts "Column access path is: " + @access_path
      puts "Column type is: " + @type
    end
  end

end

# Models a view.
class View
  def initialize
    @name = ""
    @db = ""
    @virtual_tables = Array.new
    @where_clauses = Array.new
  end
  attr_accessor(:name,:db,:virtual_tables,:where_clauses)

  def match_view(view_description)
    view_ptn = 
      /^create view (\w+)\.(\w+) as select \* from (.+) where(\s*) (.+)/im
    if $argD == "DEBUG"
      puts "View description is: #{view_description}"
    end
    matchdata = view_ptn.match(view_description)
    @name = matchdata[2]
    @db = matchdata[1]
    vts = matchdata[3]
    where = matchdata[5]
    if vts.match(/,/) 
      @virtual_tables = vts.split(/,/) 
    else
      raise "Invalid input for virtual tables: " + vts
    end
    if where.match(/ and /im)
      @where_clauses = where.split(/ and /)
    else      
      @where_clauses = where
    end
    if $argD == "DEBUG"
      puts "View name is: " + @name
      puts "View lives in database named: " + @db
      @virtual_tables.each { |vt| puts "View of virtual tables: " + vt }
      @where_clauses.each { |wh| puts "View of where clauses: " + wh }
    end
  end

end

# Models a virtual table.
class VirtualTable
  def initialize
    @name = ""
    @base_var = ""        # Name of the base variable alive at C++ app.
    @element              # Reference to the respective element definition.
    @db = ""              # Database name to be created/connected.
    @signature = ""       # The C++ signature of the struct.
    @stl_class = ""       # If an STL struct instance.
    @type = ""            # The record type for the VT.
    @pointer = ""         # Type of the base_var.
    @object_class = ""    # If an object instance.
    @template_args = ""   # For STL structs, number of template arguments.
    @columns = Array.new  # References to the VT columns.
    @@stl_single_classes = ["list" , "deque" , "vector" , "set" , 
                            "multiset"]
    @@stl_double_classes = ["map" , "multimap"]
    @@stl_sequence_classes = ["list", "vector", "deque"]
    @@stl_associative_classes = ["set" , "multiset" , "map" , "multimap"]
  end
  attr_accessor(:name,:base_var,:element,:db,:signature,:stl_class,:type,
                :pointer,:object_class,:template_args,:columns)


# Method performs case analysis to generate 
# the correct form of the variable
  def configure(access_path)
    iden = ""
    if @stl_class.length > 0
      access_path.length == 0 ? iden =  "*iter" : iden = "(*iter)."
    else
      access_path.length == 0 ? iden = "any_dstr" : iden = "any_dstr->"
    end
    return iden
  end


# Generates code to retrieve each VT struct.
# Each retrieve case matches a specific column of the VT.
  def retrieve_columns(fw)
    fw.puts "    switch (n) {"
    col_array = @columns
    col_array.each_index { |col|
      fw.puts "    case #{col}:"
      sqlite3_type = "retrieve"
      column_cast = ""
      sqlite3_parameters = ""
      column_cast_back = ""
      access_path = ""
      op, fk_col_name, column_type = 
      @columns[col].bind_datatypes(
                                   sqlite3_type, column_cast, 
                                   sqlite3_parameters, column_cast_back, 
                                   access_path)
      case op
      when "base"
        fw.puts "#{$s}sqlite3_result_#{sqlite3_type}(con, #{column_cast}any_dstr);"
        fw.puts "#{$s}break;"
      when "fk"
        iden = configure(access_path)
        if fk_col_name != nil
          fw.puts "#{$s}if ((vtd_iter = vt_directory.find(\"#{fk_col_name}\")) != vt_directory.end())"
          fw.puts "#{$s}    vtd_iter->second = 1;"
          if access_path.length == 0    # Access with (*iter) .
            @type.match(/\*/) ? record_type = "" : record_type = "&"
          else                          # Access with (*iter)[.|->]access .
            column_type == "pointer" ? record_type = "" : record_type = "&"
          end
        end
        fw.puts "#{$s}sqlite3_result_#{sqlite3_type}(con, #{column_cast}#{record_type}#{iden}#{access_path}#{column_cast_back}#{sqlite3_parameters});"
        fw.puts "#{$s}break;"
      when "gen_all"
        iden = configure(access_path)
        fw.puts "#{$s}sqlite3_result_#{sqlite3_type}(con, #{column_cast}#{iden}#{access_path}#{column_cast_back}#{sqlite3_parameters});"
        fw.puts "#{$s}break;"
      end
    }
  end


# Adds/subtracts pointer semantics from signature accordingly.
  def cast_signature()
    if /\*/.match(@pointer) == nil
      sign_retype = "#{@signature}*"
      sign_untype = @signature
    else 
      sign_retype = @signature
      sign_untype = @signature.chomp("*")
    end
    return sign_retype, sign_untype
  end

# Generates code in retrieve method. Code makes the necessary arrangements 
# for retrieve to happen successfully (condition checks, reallocation)
  def setup_retrieve(fw)
    fw.puts "    stlTableCursor *stcsr = (stlTableCursor *)cur;"
    sign_retype, sign_untype  = cast_signature()
    fw.puts "    #{sign_retype} any_dstr = (#{sign_retype})stcsr->source;"
    if @stl_class.length > 0
      fw.puts "    #{sign_untype}:: iterator iter;"
      fw.puts CodeToGenerate.Cursor_current
    end
  end


# Generates spaces to convene properly aligned code generation.
  def vt_type_spacing(fw)
    fw.print $s
    if @stl_class.length > 0
      fw.print $s
    else
      fw.print "    "
    end
  end


# Generates code to search each VT struct.
# Each search case matches a specific column of the VT.
  def search_columns(fw)
    fw.puts "#{$s}switch (iCol) {"
    @columns.each_index { |col|
      fw.puts "#{$s}case #{col}:"
      sqlite3_type = "search"
      column_cast = ""
      sqlite3_parameters = ""
      column_cast_back = ""
      access_path = ""
      op, useless, uselesss = 
      @columns[col].bind_datatypes(sqlite3_type, 
                                   column_cast, sqlite3_parameters, 
                                   column_cast_back, access_path)
      if op == "fk"
        fw.puts "#{$s}    printf(\"Restricted area. Searching VT #{@name} column #{@columns[col].name}...makes no sense.\\n\");"
        fw.puts "#{$s}    return SQLITE_MISUSE;"
        next
      end
      if @stl_class.length > 0
        fw.puts "#{$s}    iter = any_dstr->begin();"
        fw.puts "#{$s}    for (int i = 0; i < size; i++) {"
        access_path.length == 0 ? iden = "(*iter)" : iden = "(*iter)."
      else
        access_path.length == 0 ? iden = "any_dstr" : iden = "any_dstr->"
      end
      if $argD == "DEBUG"
        puts "sqlite3_type: " + sqlite3_type
        puts "column_cast: " + column_cast
        puts "sqlite3_parameters: " + sqlite3_parameters
        puts "column_cast_back: " + column_cast_back
        puts "access_path: " + access_path
      end
      case op
      when "gen_all"
        vt_type_spacing(fw)
        fw.print "if (compare(#{column_cast}#{iden}#{access_path}#{column_cast_back}, op, sqlite3_value_#{sqlite3_type}(val)) )"
        fw.puts
        vt_type_spacing(fw)
        fw.print "    temp_res[count++] = i;"
      when "base"
        vt_type_spacing(fw)
        fw.print "temp_res[count++] = i;"
      end
      fw.puts
      if @stl_class.length > 0
        fw.puts "#{$s}#{$s}iter++;"
        fw.puts "#{$s}    }"
      end
      fw.puts "#{$s}    assert(count <= stcsr->max_size);"
      fw.puts "#{$s}    break;"
    }
    fw.puts "#{$s}}"
  end

  
# Generates code in search method. Code makes the necessary arrangements 
# for search to happen successfully (condition checks, reallocation).
  def setup_search(fw)
    fw.puts "    stlTable *stl = (stlTable *)cur->pVtab;"
    fw.puts "    stlTableCursor *stcsr = (stlTableCursor *)cur;"
    sign_retype, sign_untype = cast_signature()
    fw.puts "    #{sign_retype} any_dstr = (#{sign_retype})stcsr->source;"
    if @stl_class.length > 0
      fw.puts "    #{sign_untype}:: iterator iter;"
    end
    fw.puts "    int op, iCol, i = 0, count = 0, re = 0;"
    if @base_var.length > 0
      fw.puts "    int size = get_datastructure_size(cur);"
    else
      fw.puts "    int size;"
    end
    if @base_var.length == 0 : fw.puts CodeToGenerate.Error_case end
    fw.puts "    if (val == NULL) {"
    if @base_var.length > 0
      if @stl_class.length > 0
        fw.puts CodeToGenerate.Stl_fill_resultset
      else
        fw.puts "#{$s}stcsr->size++;"
      end
    else
      fw.puts "#{$s}printf(\"Searching VT #{@name} with no BASE constraint...makes no sense.\\n\");"
      fw.puts "#{$s}return SQLITE_MISUSE;"
    end
    fw.puts "    } else {"
    fw.puts "#{$s}check_alloc((const char *)constr, op, iCol);"
    if @base_var.length == 0
      fw.puts "#{$s}if (equals_base(stl->azColumn[iCol])) {"
      if $argT == "TYPESAFE" : fw.puts CodeToGenerate.Typesafe_block end
      fw.puts "#{$s}    stcsr->source = (void *)sqlite3_value_int64(val);"
      fw.puts "#{$s}    any_dstr = (#{sign_retype})stcsr->source;"
      if @stl_class.length > 0
        fw.puts "#{$s}    realloc_resultset(cur);"
      end
      fw.puts "#{$s}}"
      fw.puts "#{$s}size = get_datastructure_size(cur);"
    end
    fw.puts CodeToGenerate.Resultset_alloc
  end


# Validate the signature of an stl structure and extract signature traits.
# Also for objects, extract class name.
  def verify_signature()
    case @signature
    when /(\w+)<(.+)>(\**)/m
      matchdata = /(\w+)<(.+)>(\**)/m.match(@signature)
      @stl_class = matchdata[1]
      @type = matchdata[2]
      @pointer = matchdata[3]
      if @@stl_single_classes.include?(@stl_class)
        @template_args = "single"
      elsif @@stl_double_classes.include?(@stl_class)
        @template_args = "double"
      else
        raise TypeError.new("No such container class: " + @stl_class +
                            "\n\n NOW EXITING. \n")
      end
      if @@stl_sequence_classes.include?(@stl_class)
        @container_type="sequence"
      elsif @@stl_associative_classes.include?(@stl_class)
        @container_type="associative"
      end
      if (@template_args == "single" && /(.+),(.+)/.match(@type)) ||
          (@template_args == "double" && !(/(.+),(.+)/.match(@type)))
        raise ArgumentError.new(CodeToGenerate.Class_sign(@signature))
      end
      if $argD == "DEBUG"
        puts "Table STL class name is: " + @stl_class
        puts "Table no of template args is: " + @template_args
        puts "Table container type is: " + @container_type
        puts "Table record is of type: " + @type
        puts "Table type is of type pointer: " + @pointer
      end
    when /(\w+)\*|(\w+)/
      matchdata = /(\w+)(\**)/.match(@signature)
      @object_class = @signature
      @type = @signature
      @pointer = matchdata[2]
      if $argD == "DEBUG"
        puts "Table object class name : " + @object_class
        puts "Table record is of type: " + @type
        puts "Table type is of type pointer: " + @pointer
      end
    when /(.+)/
      raise "Template instantiation faulty.\\n"
    end
  end

# Matches VT definitions against prototype patterns.
  def match_table(table_description)
    table_ptn1 = /^create table (\w+)\.(\w+) with base(\s*)=(\s*)(\w+) as select \* from (.+)/im
    table_ptn2 = /^create table (\w+)\.(\w+) as select \* from (.+)/im
    if $argD == "DEBUG"
      puts "Table description is: #{table_description}"
    end
    case table_description
    when table_ptn1
      matchdata = table_ptn1.match(table_description)
      @name = matchdata[2]
      @db = matchdata[1]
      @base_var = matchdata[5]
      @signature = matchdata[6]
    when table_ptn2
      matchdata = table_ptn2.match(table_description)
      @name = matchdata[2]
      @db = matchdata[1]
      @signature = matchdata[3]
    end
    verify_signature()
    if @type.match(/\*/)                    # Use type. It is active for 
      vtable_type = @type.chomp('*')        # both stl_struct and object.
    else
      vtable_type = @type
    end
    $elements.each { |el| if el.name == @name : @element = el end }   
                   # Try to match element definition using the VT's name 
                   # first, its type then.
    if @element == nil
      $elements.each { |el| if el.name == vtable_type : @element = el end }
    end
    if @element == nil
      raise "Cannot match element for table #{@name}.\\n"
  end
    if @base_var.length == 0            # base column for embedded structs.
      @columns.push(Column.new).last.set("base INT FROM self") 
    end
    @columns = @columns | @element.columns
    if $argD == "DEBUG"
      puts "Table name is: " + @name
      puts "Table lives in database named: " + @db
      puts "Table base variable name is: " + @base_var
      puts "Table signature name is: " + @signature
      puts "Table follows element: " + @element.name
    end
  end

end

# Models an element table.
class Element
  def initialize
    @name = ""
    @columns = Array.new
  end
  attr_accessor(:name,:columns)

  # Removes the last entry in the columns table and returns the table 
  # itself. Useful when including an element definition to remove the 
  # entry left empty.
  def columns_delete_last()
    @columns.delete(@columns.last)
    return @columns
  end

  # Matches an element definition against the prototype pattern and 
  # extracts the characteristics.
  def match_element(element_description)
    if $argD == "DEBUG"
      puts "Element description is: #{element_description}"
    end
    pattern = /^create element table (\w+)(\s*)\((.+)\)/im
    matchdata = pattern.match(element_description)
    if matchdata
      # First record of table_data contains the whole description of the 
      # element.
      # Second record contains the element's name
      @name = matchdata[1]
      columns_str = Array.new
      if matchdata[3].match(/,/)
        columns_str = matchdata[3].split(/,/)
      else
        columns_str[0] = matchdata[3]
      end
    end
    columns_str.each { |x| @columns.push(Column.new).last.set(x) }
    if $argD == "DEBUG"
      puts "Columns follow:"
      @columns.each { |x| p x }
    end
  end

end

# Models the input description.
class InputDescription
  def initialize(description)
    # original description tokenised in an Array
    @description = description
    # array with entries the identity of each virtual table
    @tables = Array.new
    @directives = ""
  end
  attr_accessor(:description,:tables,:directives)

# Calls the family of methods that generate the application-specific 
# retrieve method for each VT struct.
  def print_retrieve_functions(fw)
    @tables.each { |vt|
      fw.puts "// Retrieves column values of virtual table #{vt.name}."
      fw.puts "int #{vt.name}_retrieve(sqlite3_vtab_cursor *cur, int n, sqlite3_context *con) {"
      vt.setup_retrieve(fw)
      vt.retrieve_columns(fw)
      fw.puts "    }"
      fw.puts "    return SQLITE_OK;"
      fw.puts "}\n\n\n"
    }
    fw.puts "int retrieve(sqlite3_vtab_cursor *cur, int n, sqlite3_context *con) {"
    fw.puts "    stlTable *stl = (stlTable *)cur->pVtab;"
    @tables.each { |vt|
      fw.puts "    if (!strcmp(stl->zName, \"#{vt.name}\"))"
      fw.puts "        return #{vt.name}_retrieve(cur, n, con);"
    }
    fw.puts "}"
  end


# Calls the family of methods that generate the application-specific 
# search method for each VT struct.
  def print_search_functions(fw)
    @tables.each { |vt|
      fw.puts "// Filters column values of virtual table #{vt.name}."
      fw.puts "int #{vt.name}_search(sqlite3_vtab_cursor *cur, char *constr, sqlite3_value *val) {"
      vt.setup_search(fw)
      vt.search_columns(fw)
      fw.puts CodeToGenerate.Cls_search
    }
    fw.puts "int search(sqlite3_vtab_cursor* cur, char *constr, sqlite3_value *val) {"
    fw.puts "    stlTable *stl = (stlTable *)cur->pVtab;"
    @tables.each { |vt|
      fw.puts "    if (!strcmp(stl->zName, \"#{vt.name}\"))"
      fw.puts "#{$s}return #{vt.name}_search(cur, constr, val);"
    }
    fw.puts "}"
  end


# Generates the function that retrieves the size of 
# data structures registered with sqtl.
  def print_get_size(fw)
    fw.puts "// Returns the size (records) for each virtual table."
    fw.puts "int get_datastructure_size(sqlite3_vtab_cursor *cur) {"
    fw.puts "    stlTableCursor *stc = (stlTableCursor *)cur;"
    fw.puts "    stlTable *stl = (stlTable *)cur->pVtab;"
    count = 0
    @tables.each_index { |vt|
      if @tables[vt].stl_class.length > 0
        if count == 0
          fw.puts "    if (!strcmp(stl->zName, \"#{@tables[vt].name}\")) {"
	  count += 1
        else
          fw.puts "    } else if (!strcmp(stl->zName, \"#{@tables[vt].name}\")) {"
        end
        /\*/.match(@tables[vt].pointer) == nil ? retype = "*" : retype = "" 
        fw.puts "#{$s}#{@tables[vt].signature}#{retype} any_dstr = (#{@tables[vt].signature}#{retype})stc->source;"
        fw.puts "#{$s}return (int)any_dstr->size();"
      end
    }
    fw.puts "    }"
    fw.puts "    return 1;"
    fw.puts "}"
  end


# Generates the function that assigns a base variable to 
# the respective VT struct.
  def print_register_vt(fw)
    fw.puts "// Registers the base variables of user application code"
    fw.puts "// with their virtual table representation."
    fw.puts "void register_vt(stlTable *stl) {"
    count = 0
    @tables.each_index { |vt| 
      if @tables[vt].base_var.length > 0
        if count == 0
          fw.puts "    if (!strcmp(stl->zName, \"#{@tables[vt].name}\")) {"
	  count += 1
        else
          fw.puts "    } else if (!strcmp(stl->zName, \"#{@tables[vt].name}\")) {"
        end
        /\*/.match(@tables[vt].pointer) == nil ? retype = "&" : retype = "" 
        fw.puts "#{$s}stl->data = (void *)#{retype}#{@tables[vt].base_var};"
        fw.puts "#{$s}stl->embedded = 0;"
      end
    }
    fw.puts CodeToGenerate.Els_case
  end


# Generates the thread function that starts the SQTL thread.
  def print_thread(fw)
    fw.puts CodeToGenerate.Prepare_thread(@tables)
# <db>.<table> does not work for some reason. test.
    @tables.each_index { |vt| 
#      query =  "CREATE VIRTUAL TABLE #{@tables[vt].db}.#{@tables[vt].name} USING stl("
      query =  "CREATE VIRTUAL TABLE #{@tables[vt].name} USING stl("
      @tables[vt].columns.each { |c| 
        query += "#{c.name} #{c.data_type}, "
      }
      query = query.chomp(", ") + ")"
      fw.puts "    queries[#{vt}] = \"#{query}\";"
      fw.puts "    table_names[#{vt}] = \"#{@tables[vt].name}\";"
    }
    fw.puts CodeToGenerate.Init_sqtl(@tables)
  end


# Generates the external base variables as prescribed 
# from user in the description
  def print_extern_variables(fw)
    @tables.each { |vt| 
      if vt.base_var.length > 0
        fw.puts "extern #{vt.signature} #{vt.base_var};" 
      end 
    }
  end


# Generates application-specific code to complement the SQTL library.
# There is a call to each of the above generative functions.
  def generate()
    myfile = File.open("stl_search.cpp", "w") do |fw|
      fw.puts CodeToGenerate.Directives(@directives)
      fw.puts "\n\n"
      print_extern_variables(fw)
      fw.puts "\n\n"
      print_thread(fw)
      fw.puts "\n\n"
      print_register_vt(fw)
      fw.puts "\n\n"
      print_get_size(fw)
      fw.puts "\n\n"
      fw.puts CodeToGenerate.Equals_base
      print_search_functions(fw)
      fw.puts "\n\n"
      print_retrieve_functions(fw)
    end
    puts "Created/updated stl_search.cpp ."
    myFile = File.open("makefile.append", "w") do |fw|
      fw.puts CodeToGenerate.Makefile
      fw.puts
    end
    puts "Created/updated makefile.append ."
  end


# The method cleans the user description from duplicate 
# and unnecessary spaces and conducts case analysis 
# according to which, the description is promoted to the 
# appropriate class. Required directives are also extracted.
  def register_datastructures()
    if $argD == "DEBUG"
      puts "Description before whitespace cleanup: "
      @description.each { |x| p x }
    end
    token_d = @description
    token_d = token_d.select { |x| x.length > 0 }
    @directives = token_d[0]
    token_d.delete_at(0)
    if $argD == "DEBUG"
      puts "Directives: #{@directives}"
    end
    x = 0
    while x < token_d.length            # Cleaning white space.
      token_d[x].lstrip!
      token_d[x].rstrip!
      if /\n|\t|\r|\f/.match(token_d[x])
        token_d[x].gsub!(/\n|\t|\r|\f/, "") 
      end
      token_d[x].squeeze!(" ")           
      if / ,|, /.match(token_d[x]) : token_d[x].gsub!(/ ,|, /, ",") end
      if / \(/.match(token_d[x]) : token_d[x].gsub!(/ \(/, "(") end
      if /\( /.match(token_d[x]) : token_d[x].gsub!(/\( /, "(") end
      if /\) /.match(token_d[x]) : token_d[x].gsub!(/\) /, ")") end
      if / \)/.match(token_d[x]) : token_d[x].gsub!(/ \)/, ")") end
      x += 1
    end
    @description = token_d
    if $argD == "DEBUG"
      puts "Description after whitespace cleanup: "
      @description.each { |x| p x }
    end
    $elements = Array.new
    views = Array.new
    w = 0
    @description.each { |stmt|
      if $argD == "DEBUG"
        puts "\nDESCRIPTION No: " + w.to_s + "\n"
      end
      stmt.lstrip!
      stmt.rstrip!
      case stmt
      when /^create element table/im
        $elements.push(Element.new).last.match_element(stmt)
      when /^create table/im
        @tables.push(VirtualTable.new).last.match_table(stmt)
      when /^create view/im
        views.push(View.new).last.match_view(stmt)
      end
      w += 1
    }
  end
  
end


# Take cases on command-line arguments.
def take_cases(argv)
  case argv
  when /debug/i
    $argD = "DEBUG"
  when /typesafe/i
    $argT = "TYPESAFE"
  end
end


# The main method.
if __FILE__ == $0
  $argF = ARGV[0]
  take_cases(ARGV[1])
  take_cases(ARGV[2])
  if !File.file?($argF)
    raise "File #{$argF} does not exist.\\n"
  end
  description = File.open($argF, "r") { |fw| fw.read }
  if description.match(/;/)
    token_description = description.split(/;/)
  else
    raise "Invalid description..delimeter ';' not used."
  end
  $s = "        "
  ip = InputDescription.new(token_description)
  ip.register_datastructures()
  ip.generate()
end
