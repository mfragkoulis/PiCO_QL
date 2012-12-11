# -*- coding: utf-8 -*-
#
#   Parse a user description, which conforms to the DSL, and generate the 
#   application specific filter and projection functions for the 
#   virtual tables described.
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

require 'erb'

# Models a column of the Virtual Table (VT).
class Column
  def initialize(ucase)
    @name = ""
    @line = 0                 # Line in DSL description that the column
                              # (the column's access path to be precise)
                              # is defined in.
    @data_type = ""
    @cpp_data_type = ""       # Respective C++ data type. Used only 
                              # for string so far.
    @related_to = ""          # Reference to other VT's name (FK).
    @fk_col_type = ""         # Reference to other VT's type (FK).
    @fk_method_ret = 0        # True if FK access path is a method
    		   	      # return.
                              # Required for managing temporary variables.
    @saved_results_index = -1 # Required for naming the particular 
                              # saved results instance.
    @access_path = ""         # The access statement for the column value.
    @col_type = "object"      # Record type (pointer or reference) for 
                              # special columns, the ones that refer to 
                              # other VT or UNIONS.
    @case = ucase             # switch case for union view fields
    @@int_data_types = ["int", "integer", "tinyint", "smallint", 
                        "mediumint", "bigint", "unsigned bigint", "int2",
                        "bool", "boolean", "int8", "numeric"]
    @@double_data_types = ["float", "double", "double precision", "real"]
    @@text_data_types = ["text", "date", "datetime", "clob", "string"]
    @@text_match_data_types = [/character/i, /varchar/i, /nvarchar/i, 
                               /varying character/i, /native character/i,
                               /nchar/i]
  end
  attr_accessor(:name,:line,:data_type,:cpp_data_type,
                :related_to,:fk_col_type,
		:fk_method_ret,:saved_results_index,
		:access_path,:col_type,:case)


# Used to clone a Column object. Ruby does not support deep copies.
  def construct(name, data_type, cpp_data_type, 
                related_to, fk_method_ret, 
      		fk_col_type, saved_results_index,access_path, 
		type, line, ucase)
    @name = name
    @data_type = data_type
    @cpp_data_type = cpp_data_type
    @related_to = related_to
    @fk_method_ret = fk_method_ret
    @fk_col_type = fk_col_type
    @saved_results_index = saved_results_index
    @access_path = access_path
    @col_type = type
    @line = line
    @case = ucase
  end


# Performs case analysis with respect to the column data type (and other)
# and fills the passed variables with values accordingly.
  def bind_datatypes(sqlite3_type, column_cast, sqlite3_parameters, 
                     column_cast_back, access_path)
    tmp_text_array = Array.new      # Do not process the original array.
    tmp_text_array.replace(@@text_match_data_types)
    if @related_to.length > 0       # 'this' (column) refers to other VT.
      sqlite3_type.replace("int64")
      sqlite3_parameters.replace("int")    # for 32-bit architectures.used in retrieve and search.
      column_cast.replace("(long int)")
      access_path.replace(@access_path)
      return "fk", @related_to, @col_type, @line, @fk_method_ret, @saved_results_index, @fk_col_type
    elsif @name == "base"              # 'base' column. refactor: elsif perhaps?
      sqlite3_type.replace("int64")
      column_cast.replace("(long int)")
      sqlite3_parameters.replace("int");    # for 32-bit architectures.used in retrieve.
      return "base", nil, "", nil
    elsif @name == "rownum"           # 'rownum'column
      sqlite3_type.replace("int")
      return "rownum", nil, nil, nil
    elsif @cpp_data_type == "union"
      access_path.replace(@access_path)
      return "union", @name, @col_type, @line
    else
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
      return "gen_all", nil, "", @line
    end
  end

# Register line that corresponds to the table in DSL description
  def register_line()
    $lined_description.each_index { |line|
      # Match for FK and normal data type columns.
      if $lined_description[line].match(/foreign key(\s*)\((\s*)#{Regexp.escape(@name)}(\s*)\) from #{Regexp.escape(@access_path)}|#{Regexp.escape(@name)} #{Regexp.escape(@data_type)} from #{Regexp.escape(@access_path)}(\s*)(\w*)(\s*)(\w*)/i)
        @line = line
        if $argD == "DEBUG"
          puts "Column found at line #{@line + 1} of #{$argF}"
        end
        break
        if $argD == "DEBUG"
          puts "Line #{line + 1}"
        end
      end
    }
  end


# Validates a column data type.
# The following data types are the ones accepted by sqlite.
  def verify_data_type()
    dt = @data_type.downcase
    tmp_text_array = Array.new
    tmp_text_array.replace(@@text_match_data_types)
    begin
      if dt == "string"
        @cpp_data_type.replace(dt)
        @data_type.replace("TEXT")
        return 1
      elsif @@text_data_types.include?(dt) || 
          tmp_text_array.reject! { |rgx| rgx.match(dt) != nil } != nil
        return 1
      elsif @@int_data_types.include?(dt) || 
          @@double_data_types.include?(dt) || 
          /decimal/i.match(dt) != nil
        return 0
      elsif dt == "union"
        @cpp_data_type.replace(dt)
        @data_type.replace("NUMERIC")
        return 0
      elsif dt == "enum"
# union view: union_mode data type possibility (?)
        return 0
      else
        raise TypeError.new("No such data type #{dt.upcase}\\n")
      end
    rescue
      puts "No such data type #{dt.upcase}\\n"
      exit(1)
    end
  end

  def manage_inclusion(matchdata, access_type_link)
    this_struct_view = $struct_views.last
    this_columns = this_struct_view.columns
    col_type_text = 0
    $struct_views.each { |vs| 
      if vs.name == matchdata[1]     # Search all struct_view 
                                     # definitions to find the one 
                                     # specified and include it.
        this_struct_view.columns_delete_last()
        up_to = vs.columns.length
        vs.columns.each_index { |col| 
          coln = vs.columns[col]            # Manually construct a deep 
                                            # copy of coln
          this_columns.push(Column.new("")) # and push it to current.
          access_path = ""
          if coln.access_path.match(/this\.|this->/)
            access_path = coln.access_path.gsub(/this\.|this->/, '\0<accessor>')
            access_path.gsub!("<accessor>", "#{matchdata[2]}#{access_type_link}")
          else
            access_path = "#{matchdata[2]}#{access_type_link}#{coln.access_path}"
          end
          name = "#{matchdata[1]}#{coln.name}"
          this_columns.last.construct(name,
                                      coln.data_type.clone,
                                      coln.cpp_data_type.clone,
                                      coln.related_to.clone, 
                                      coln.fk_method_ret,
                                      coln.fk_col_type.clone,
                                      coln.saved_results_index,
                                      access_path, 
                                      coln.col_type.clone,
                                      coln.line,
                                      coln.case)
        }
        col_type_text = vs.include_text_col
      end
    }
    return col_type_text
  end
  

# Matches each column description against a pattern and extracts 
# column traits.
  def set(column)
    col_type_text = 0
    column.lstrip!
    column.rstrip!
    if $argD == "DEBUG"
      puts "Column is: #{column}"
    end
    if column.match(/\n/)
      column.gsub!(/\n/, "")
    end
    column_ptn1a = /inherits struct view (\w+) from (.+) pointer/im
    column_ptn1b = /inherits struct view (\w+) from (.+)/im
    column_ptn2 = /inherits struct view (\w+)/im
    column_ptn3 = /foreign key(\s*)\((\s*)(\w+)(\s*)\) from (.+) references (\w+)(\s*)(\w*)/im
    column_ptn4 = /(\w+) (\w+) from (.+) pointer/im # for UNION column
    column_ptn5 = /(\w+) (\w+) from (.+)/im
    case column
    when column_ptn1a
      matchdata = column_ptn1a.match(column)
      col_type_text = manage_inclusion(matchdata, "->")
      return col_type_text
    when column_ptn1b
      matchdata = column_ptn1b.match(column)
      col_type_text = manage_inclusion(matchdata, ".")
      return col_type_text
    when column_ptn2                      # Include a struct_view 
                                          # definition without adapting.
      matchdata = column_ptn2.match(column)
      $struct_views.each { |vs| 
        if vs.name == matchdata[1]
          $struct_views.last.columns = 
	    $struct_views.last.columns_delete_last() | 
            vs.columns
	  col_type_text = vs.include_text_col
	end
      }
      return col_type_text
# ?            name = "#{matchdata[1]}#{coln.name}"
    when column_ptn3
      matchdata = column_ptn3.match(column)
      @name = matchdata[3]
      @data_type = "INT"
      @related_to = matchdata[6]
      @access_path = matchdata[5]
      begin
        @related_to.length > 0
        if @access_path.match(/(.+)\)/)    # Returning from a method.
          @fk_method_ret = 1
        end
        if matchdata[8].length == 0
          @col_type = "object"
	elsif matchdata[8].downcase == "pointer"
          @col_type = "pointer"   # In
        end                # columns that reference other VTs
                           # users have to declare the type for generating
                           # correct access statement.
      rescue
        puts "Referenced virtual table not registered.\\n"
        exit(1)
      end
    when column_ptn4
      matchdata = column_ptn4.match(column)
      @name = matchdata[1]
      @data_type = matchdata[2]
      @access_path = matchdata[3]
      @col_type = "pointer"
    when column_ptn5
      matchdata = column_ptn5.match(column)
      @name = matchdata[1]
      @data_type = matchdata[2]
      @access_path = matchdata[3]
    end
    register_line()
    col_type_text = verify_data_type()
    if @access_path.match(/self/)
      @access_path.gsub!(/self/,"")
    end
    if $argD == "DEBUG"
      puts "Column name is: " + @name
      puts "Column data type is: " + @data_type
      puts "Column related to: " + @related_to
      puts "Column fk_col_type: " + @fk_col_type
      puts "Column fk_method_ret: " + @fk_method_ret.to_s
      puts "Column saved_results_index: " + @saved_results_index.to_s
      puts "Column access path is: " + @access_path
      puts "Column type is: " + @col_type
      puts "Column is of text type: " + col_type_text.to_s
      puts "Column case: " + @case
    end
    return col_type_text
  end

end

# Models a virtual table.
class VirtualTable
  def initialize
    @name = ""
    @base_var_line = 0    # Line in DSL description that the C NAME 
                          # identifier is defined.
    @signature_line = 0   # Line in DSL description that the C TYPE 
                          # identifier is defined.
    @base_var = ""        # Name of the base variable alive at C++ app.
    @struct_view          # Reference to the respective 
                          # struct_view definition.
    @db = ""              # Database name to be created/connected against.
    @signature = ""       # The C++ signature of the struct.
    @container_class = "" # If a container instance as per 
                          # the SGI container concept.
    @type = ""            # The record type for the VT. 
                          # Use @type for management. It is active for 
                          # both container and object.
    @pointer = ""         # Type of the base_var.
    @iterator = ""        # Iterator name in app to use for C containers. 
    @traverse = ""        # Traverse function name for C container:
                          # generic_clist.
    @object_class = ""    # If an object instance.
    @columns = Array.new  # References to the VT columns.
    @include_text_col = 0 # True if VirtualTable includes column
    		      	  # of text data type. Required for
			  # generating code for
			  # PICO_QL_HANDLE_TEXT_ARRAY C++ flag.
    @@C_container_types = ["clist", "generic_clist"]
  end
  attr_accessor(:name,:base_var_line,:signature_line,:base_var,
                :struct_view,:db,:signature,:container_class,:type,
                :pointer,:iterator,:object_class,:columns,
                :include_text_col,:C_container_types)

# Getter for static member C_container_types at class level
  def self.C_container_types
    @@C_container_types
  end

# Support templating of member data
  def get_binding
    binding
  end

# Calls template to generates code in retrieve method. Code makes the 
# necessary arrangements for retrieve to happen successfully 
# (condition checks, reallocation)
  def finish_retrieve(fw)
    file = File.open("pico_ql_erb_templates/pico_ql_post_retrieve.erb").read
    post_retrieve = ERB.new(file, 0, '>')
    fw.puts post_retrieve.result(get_binding)
  end

  def union_retrieve(fw, union_view_name, iden, union_access_path, space)
    columns = Array.new
    switch = ""
    $union_views.each { |uv| 
      if uv.name == union_view_name
        columns = uv.columns 
        switch = uv.switch
      end 
    }
    if !union_access_path.end_with?(".") && !union_access_path.end_with?("->")
      union_access_path.concat(".")
    end
    fw.puts "#{space}    switch (#{iden}#{switch}) {"
# Will not work for union in struct1 in struct2 (user responsibility?)
# if struct1.struct2.union then access path would be 
# (any_dstr->)struct2.union . reasonable to await struct2.union_mode
# from user?
    columns.each_index { |col|
      fw.puts "#{space}    case #{columns[col].case}:"
      space.concat("  ")
      sqlite3_type = "retrieve"
      column_cast = ""
      sqlite3_parameters = ""
      column_cast_back = ""
      total_access_path = ""
      access_path_col = ""
      op, fk_col_name, column_type, line, fk_method_ret, 
      saved_results_index, fk_col_type = 
      columns[col].bind_datatypes(sqlite3_type, column_cast, 
                                   sqlite3_parameters, column_cast_back, 
                                   access_path_col)
      if $argD == "DEBUG"
        puts "sqlite3_type: " + sqlite3_type
        puts "column_cast: " + column_cast
        puts "sqlite3_parameters: " + sqlite3_parameters
        puts "column_cast_back: " + column_cast_back
        puts "access_path: " + access_path_col
        if op == "union"
          fk_col_name  = union_view_embedded
          puts "Union: " + union_view_embedded
        end
        if line != nil
          puts "line: " + (line + 1).to_s
        else 
          puts "line: nil"
        end
      end
      case op
      when "fk"
        access_path_col.insert(0, union_access_path)
        fk_retrieve(fw, access_path_col, column_type, fk_method_ret,
                    line, iden, saved_results_index, sqlite3_type,
                    column_cast, sqlite3_parameters, fk_col_name, 
                    fk_col_type, col, space)
      when "gen_all"
        access_path_col.insert(0, union_access_path)
        all_retrieve(fw, iden, access_path_col, sqlite3_type,
                     column_cast_back, sqlite3_parameters, column_cast,
                     line, space)
      when "union"
        union_view_embedded = fk_col_name
        iden.concat(union_access_path)
        union_retrieve(fw, union_view_embedded, iden, access_path_col, 
                       space)
      end
      space.chomp!("  ")
    }
    fw.puts "#{space}    }"
    fw.puts "#{space}    break;"
  end

  def all_retrieve(fw, iden, access_path, sqlite3_type,
                   column_cast_back, sqlite3_parameters, column_cast,
                   line, space)
    if access_path.match(/this\.|this->/)
      access_path.gsub!(/this\.|this->/, "#{iden}")
    else
      access_path = "#{iden}#{access_path}"
    end
    if sqlite3_type == "text"
      if column_cast_back == ".c_str()"
        string_construct_cast = ""
      else
        string_construct_cast = "(const char *)"
      end
      if $argLB == "CPP"
        fw.puts "#ifdef PICO_QL_HANDLE_TEXT_ARRAY"
        fw.puts "#{space}    textVector.push_back(#{string_construct_cast}#{access_path});"
        print_line_directive(fw, line)
      fw.puts "#{space}    sqlite3_result_text(con, (const char *)textVector.back().c_str()#{sqlite3_parameters});"
      fw.puts "#else"
      end
    end
    fw.puts "#{space}    sqlite3_result_#{sqlite3_type}(con, #{column_cast}#{access_path}#{column_cast_back}#{sqlite3_parameters});"
    print_line_directive(fw, line)
    if sqlite3_type == "text" && $argLB == "CPP"
      fw.puts "#endif"
    end
    fw.puts "#{space}    break;"
  end

  def fk_retrieve(fw, access_path, column_type, fk_method_ret,
                  line, iden, saved_results_index, sqlite3_type,
                  column_cast, sqlite3_parameters, fk_col_name, 
                  fk_col_type, col, space)
    record_type = ""
    p_type = ""
    if access_path.length == 0    # Access with (*iter) .
      if @type.match(/\*/)
        record_type = "*"
        p_type = ""
      else
        record_type = ""
        p_type = "&"
      end
    else                        # Access with (*iter)[.|->]access .
      if column_type == "pointer" 
        record_type = "*" 
        p_type = ""
      else 
        record_type = ""
        p_type = "&"
      end
    end
    fw.puts "#{space}    {"
    if $argLB == "CPP"
      fw.puts "#ifdef PICO_QL_HANDLE_POLYMORPHISM"
      def_nop = "*"
      if fk_col_type.match(/(.+)\*/)
        def_nop = ""
      end
      fw.puts "#{space}      #{fk_col_type}#{def_nop} cast = dynamic_cast<#{fk_col_type}#{def_nop}>(#{p_type}#{iden}#{access_path});"
      if $argM == "MEM_MGT" && fk_method_ret == 1
        fw.puts "#{space}      if (cast != NULL) {"
        fw.puts "#{space}        saved_results_#{saved_results_index}.push_back(*cast);"
        print_line_directive(fw, line)
        fw.puts "#ifdef ENVIRONMENT64"
        fw.puts "#{space}        sqlite3_result_#{sqlite3_type}(con, #{column_cast}&(saved_results_#{saved_results_index}.back()));"
        fw.puts "#else"
        fw.puts "#{space}        sqlite3_result_#{sqlite3_parameters}(con, #{column_cast}&(saved_results_#{saved_results_index}.back()));"
        fw.puts "#endif"
        fw.puts "#{space}      } else {"
        fw.puts "#ifdef ENVIRONMENT64"
        fw.puts "#{space}        sqlite3_result_#{sqlite3_type}(con, #{column_cast}(0));"
        fw.puts "#else"
        fw.puts "#{space}        sqlite3_result_#{sqlite3_parameters}(con, #{column_cast}(0));"
        fw.puts "#endif"
      else
        fw.puts "#ifdef ENVIRONMENT64"
        fw.puts "#{space}      sqlite3_result_#{sqlite3_type}(con, #{column_cast}cast);"
        print_line_directive(fw, line)
        fw.puts "#else"
        fw.puts "#{space}      sqlite3_result_#{sqlite3_parameters}(con, #{column_cast}cast);"
        print_line_directive(fw, line)
        fw.puts "#endif"
      end
      fw.puts "#else"
    end
    if $argM == "MEM_MGT" && fk_method_ret == 1
      fw.puts "#{space}      saved_results_#{saved_results_index}.push_back(#{record_type}#{iden}#{access_path});"
      print_line_directive(fw, line)
      fw.puts "#ifdef ENVIRONMENT64"
      fw.puts "#{space}      sqlite3_result_#{sqlite3_type}(con, #{column_cast}&(saved_results_#{saved_results_index}.back()));"
      fw.puts "#else"
      fw.puts "#{space}      sqlite3_result_#{sqlite3_parameters}(con, #{column_cast}&(saved_results_#{saved_results_index}.back()));"
      fw.puts "#endif"
    else
      fw.puts "#ifdef ENVIRONMENT64"
      fw.puts "#{space}      sqlite3_result_#{sqlite3_type}(con, #{column_cast}#{p_type}#{iden}#{access_path});"
      print_line_directive(fw, line)
      fw.puts "#else"
      fw.puts "#{space}      sqlite3_result_#{sqlite3_parameters}(con, #{column_cast}#{p_type}#{iden}#{access_path});"
      print_line_directive(fw, line)
      fw.puts "#endif"
    end
    if $argLB == "CPP"
      fw.puts "#endif"
      fw.puts "#{space}      VtblImpl *chargeVT#{col} = selector_vt[\"#{fk_col_name}\"];"
      if @base_var.length == 0
        fw.puts "#{space}      (*chargeVT#{col})(cur, 1, &charged);"
      else
        fw.puts "#{space}      map<sqlite3_vtab_cursor *, bool> *map#{@name}#{col};"
        fw.puts "#{space}      map#{@name}#{col} = NULL;"
        fw.puts "#{space}      (*chargeVT#{col})(cur, 1, map#{@name}#{col});"
      end
    else
      fw.puts "#{space}      int j = 0;"
      fw.puts "#{space}      while ((j < (int)vtAll.size) && (strcmp(vtAll.instanceNames[j], \"#{fk_col_name}\"))) {j++;}"
      fw.puts "#{space}      if (j == (int)vtAll.size) {"
      fw.puts "#{space}        printf(\"In search: VT %s not registered.\\nExiting now.\\n\", ((picoQLTable *)cur->pVtab)->zName);"
      fw.puts "#{space}        return SQLITE_ERROR;"
      fw.puts "#{space}      }"
      fw.puts "#{space}      struct Vtbl *chargeVT#{col} = vtAll.instances[j];"
      if @base_var.length == 0
        fw.puts "#{space}      chargeVT#{col}->report_charge(cur, 1, ((#{@name}_vt *)vtbl)->charged, ((#{@name}_vt *)vtbl)->chargedSize, chargeVT#{col});"
      else
        #      fw.puts "#{space}      map<sqlite3_vtab_cursor *, bool> *map#{@name}#{col};"
        #      fw.puts "#{space}      map#{@name}#{col} = NULL;"
        fw.puts "#{space}      chargeVT#{col}->report_charge(cur, 1, NULL, 0, chargeVT#{col});"
      end
    end
    fw.puts "#{space}      break;"
    fw.puts "#{space}    }"
  end

# Method performs case analysis to generate 
# the correct form of the variable
  def configure_retrieve(access_path, op)
    iden = ""
    type_check = ""
    if @container_class.length > 0
      if @@C_container_types.include?(@container_class)
        if $argLB == "CPP"
          access_path.length == 0 ? iden =  "*(rs->resIter)" : iden = "(*(rs->resIter))."
        else
          access_path.length == 0 ? iden =  "((#{@name}ResultSetImpl *)rs)->res[rs->current]" : iden = "((#{@name}ResultSetImpl *)rs)->res[rs->current]."
        end
      else
        access_path.length == 0 ? iden =  "**(rs->resIter)" : iden = "(**(rs->resIter))."
      end
    else
      access_path.length == 0 ? iden = "any_dstr" : iden = "any_dstr->"
    end
    case op
    when /gen_all|union/
      if @container_class.length > 0
        if access_path.length == 0
          if @type.match(/\*/)
            iden = "*#{iden}"
          end
        else
          if (access_path == "first" && 
              @type.gsub(/ /,"").match(/\*,/)) ||
              (access_path == "second" &&
               @type.gsub(/ /,"").match(/(.+)\*/))
            iden = "*#{iden}"
          elsif !access_path.match(/first/) &&
              !access_path.match(/second/) &&
              @type.gsub(/ /,"").match(/(.+)\*/)
            iden.chomp!(".")
            iden.concat("->")
          end
        end
      end
    when "fk"
      # is object after transformations
      if access_path.length > 0 &&
        !access_path.match(/first/) &&
          !access_path.match(/second/) &&
          @type.gsub(/ /,"").match(/(.+)\*/)
        iden.chomp!(".")
        iden.concat("->")
      end
    end
    return iden
  end

# Print line directive
  def print_line_directive(fw, line)
    if line != nil
      if $argD == "DEBUG"
        fw.puts "#line #{line + 1} \"#{$argF}\""
      end
    end
  end

# Generates code to retrieve each VT struct.
# Each retrieve case matches a specific column of the VT.
  def retrieve_columns(fw)
    fw.puts "  switch (nCol) {"
    @columns.each_index { |col|
      fw.puts "  case #{col}:"
      sqlite3_type = "retrieve"
      column_cast = ""
      sqlite3_parameters = ""
      column_cast_back = ""
      access_path = ""
      op, fk_col_name, column_type, line, fk_method_ret, 
      saved_results_index, fk_col_type = 
      @columns[col].bind_datatypes(sqlite3_type, column_cast, 
                                   sqlite3_parameters, column_cast_back, 
                                   access_path)
      iden = ""
      iden = configure_retrieve(access_path, op)
      case op
      when "base"
        fw.puts "#ifdef ENVIRONMENT64"
        # sqlite3_type = "int64" always in this case.
        fw.puts "    sqlite3_result_#{sqlite3_type}(con, #{column_cast}any_dstr);"
        fw.puts "#else"
        # sqlite3_parameters = "int" always in this case.
        fw.puts "    sqlite3_result_#{sqlite3_parameters}(con, #{column_cast}any_dstr);"
        fw.puts "#endif"
        fw.puts "    break;"
      when "rownum"
        fw.puts "    sqlite3_result_#{sqlite3_type}(con, rs->current);"
        fw.puts "    break;"
      when "fk"
        fk_retrieve(fw, access_path, column_type, fk_method_ret,
                    line, iden, saved_results_index, sqlite3_type,
                    column_cast, sqlite3_parameters, fk_col_name, 
                    fk_col_type, col, "")
      when "gen_all"
        all_retrieve(fw, iden, access_path, sqlite3_type,
                     column_cast_back, sqlite3_parameters, column_cast,
                     line, "")
      when "union"
        union_view_name = fk_col_name
        union_retrieve(fw, union_view_name, iden, access_path, "")
      end
    }
  end

# Calls template to generate code in retrieve method. 
# Code makes the necessary arrangements for retrieve to happen 
# successfully (condition checks, reallocation).
  def setup_retrieve(fw)
    file = File.open("pico_ql_erb_templates/pico_ql_pre_retrieve_#{$argLB.downcase}.erb").read
    pre_retrieve = ERB.new(file, 0 , '>')
    fw.puts pre_retrieve.result(get_binding)
  end

# Calls template to generates code in retrieve method. Code makes the 
# necessary arrangements for retrieve to happen successfully 
# (condition checks, reallocation)
  def finish_search(fw)
    file = File.open("pico_ql_erb_templates/pico_ql_post_search.erb").read
    post_search = ERB.new(file, 0, '>')
    fw.puts post_search.result(get_binding)
  end

  def configure_result_set()
    if @container_class.length > 0
      if @@C_container_types.include?(@container_class)
        if $argLB == "CPP"
          case @container_class
          when "clist"
            @pointer.match(/\*/) ? retype = "" : retype = "*"
            add_to_result_setF = "<space>    rs->res.push_back(iter);\n<space>    rs->resBts.push_back(1);\n<space>  } else {\n<space>    rs->resBts.push_back(0);\n<space>  }\n<space>  iter = iter->#{@iterator};\n<space>}"
          when "generic_clist"
            add_to_result_setF = "<space>    rs->res.push_back(iter);\n<space>    rs->resBts.push_back(1);\n<space>  } else {\n<space>    rs->resBts.push_back(0);\n<space>  }\n<space>}"            
          end
        else
          case @container_class
          when "clist"
            add_to_result_setF = "<space>    rs->size++;\n<space>    rs->actualSize++;\n<space>    ((#{@name}ResultSetImpl *)rs)->res = (#{@signature}#{retype}*)sqlite3_realloc(((#{@name}ResultSetImpl *)rs)->res, sizeof(#{@signature}#{retype})*rs->size);\n<space>    ((#{@name}ResultSetImpl *)rs)->res[rs->size - 1] = iter;\n<space>  }\n<space>  iter = iter->#{@iterator};\n<space>}"
          when "generic_clist"
            add_to_result_setF = "<space>    rs->size++;\n<space>    rs->actualSize++;\n<space>    ((#{@name}ResultSetImpl *)rs)->res = (#{@signature}#{retype}*)sqlite3_realloc(((#{@name}ResultSetImpl *)rs)->res, sizeof(#{@signature}#{retype})*rs->size);\n<space>    ((#{@name}ResultSetImpl *)rs)->res[rs->size - 1] = iter;\n<space>  }\n<space>}"
          end
        end
      else
        add_to_result_setF = "<space>    rs->res.push_back(iter);\n<space>    rs->resBts.set(index, 1);\n<space>  }\n<space>  index++;\n<space>}"
      end
      if $argLB == "CPP"
        add_to_result_setN = "<space>    resIterC = rs->res.erase(resIterC);\n<space>    rs->resBts.reset(index);\n<space>  } else\n<space>    resIterC++;\n<space>  index = rs->resBts.find_next(index);\n<space>}"
      else
        add_to_result_setN = "<space>    rs->actualSize--;\n<space>    ((#{@name}ResultSetImpl *)rs)->res[index] = NULL;\n<space>  }\n<space>  index++;\n<space>  while (((#{@name}ResultSetImpl *)rs)->res[index] == NULL) {index++;}\n<space>}"
      end
    else
      add_to_result_setF = "<space>  stcsr->size = 1;\n<space>}"
      add_to_result_setN = "<space>  stcsr->size = 0;\n<space>}"
    end
    return add_to_result_setF, add_to_result_setN
  end

  def configure_search(op, access_path, fk_type)
    idenF = ""
    idenN = ""
    access_pathF = ""
    access_pathN = ""
    if @container_class.length > 0
      if @@C_container_types.include?(@container_class)
        access_path.length == 0 ? idenF = "iter" : idenF = "iter."
        if $argLB == "CPP"
        access_path.length == 0 ? idenN = "(*resIterC)" : idenN = "(*resIterC)."
        else
          access_path.length == 0 ? idenN = "((#{@name}ResultSetImpl *)rs)->res[index]" : idenN = "((#{@name}ResultSetImpl *)rs)->res[index]."
        end
      else
        access_path.length == 0 ? idenF = "(*iter)" : idenF = "(*iter)."
        access_path.length == 0 ? idenN = "(**resIterC)" : idenN = "(**resIterC)."
      end
    else
      access_path.length == 0 ? idenF = "any_dstr" : idenF = "any_dstr->"
      access_path.length == 0 ? idenN = "any_dstr" : idenN = "any_dstr->"
    end
    case op
    when "union"
      if access_path.length > 0 
        if (@type.gsub(/ /,"").match(/(\w+)\*,/) &&
            access_path.match(/first/)) || 
            (@type.gsub(/ /,"").match(/,(\w+)\*/) &&
            access_path.match(/second/)) ||
            @type.gsub(/ /,"").match(/(\w+)\*/)
          idenF.chomp!(".")
          idenF.concat("->")
          idenN.chomp!(".")
          idenN.concat("->")
        end
      end
      if fk_type == "pointer"
        access_path.concat("->")
      else
        access_path.concat(".")
      end
    when "all"
      if @container_class.length > 0
        if access_path.length == 0
          if @type.match(/\*/)
            idenF = "*#{idenF}"
            idenN = "*#{idenN}"
          end
        else
          if (access_path == "first" && 
              @type.gsub(/ /,"").match(/\*,/)) ||
              (access_path == "second" &&
               @type.gsub(/ /,"").match(/(.+)\*/))
            idenF = "*#{idenF}"
            idenN = "*#{idenN}"
          elsif !access_path.match(/first/) &&
              !access_path.match(/second/) &&
              @type.gsub(/ /,"").match(/(.+)\*/)
            idenF.chomp!(".")
            idenF.concat("->")
            idenN.chomp!(".")
            idenN.concat("->")
          end
        end
      end
    when "fk"
# is object after transformations
      if access_path.length > 0 &&
        !access_path.match(/first/) &&
          !access_path.match(/second/) &&
          @type.gsub(/ /,"").match(/(.+)\*/)
        idenF.chomp!(".")
        idenF.concat("->")
        idenN.chomp!(".")
        idenN.concat("->")
      end
      if (access_path.length == 0 && 
          !@type.gsub(/ /,"").match(/(.+)\*/)) ||
          (access_path.length > 0 && 
           fk_type == "object")
        idenF = "&#{idenF}"
        idenN = "&#{idenN}"
      end
    end
    if access_path.match(/this\.|this->/)
      access_pathF = access_path.gsub(/this\.|this->/, "#{idenF}")
      access_pathN = access_path.gsub(/this\.|this->/, "#{idenN}")
    else
      access_pathF = "#{idenF}#{access_path}"
      access_pathN = "#{idenN}#{access_path}"
    end
    return access_pathF, access_pathN, idenF, idenN
  end

  def display_traverse(iterator, head, accessor)
    loop = @traverse
    matchdata = loop.match(/\((.+)\)/)
    if matchdata
      args = matchdata[1].split(/,/)
      args.each { |rg|
        case rg
        when /<iterator>/
          loop.gsub!("#{rg}", "#{iterator}")
        when /<head>/
          loop.gsub!("#{rg}", "#{head}")
        else
          cleaned = rg.gsub(/<|>/, "")
          loop.gsub!("#{rg}", "#{head}#{accessor}#{cleaned}")
        end
      }
    end
    if $argD == "DEBUG"
      puts "#{loop}"
    end
    return loop
  end

  def configure_iteration()
    if @container_class.length > 0
      if @@C_container_types.include?(@container_class)
# for C containers resBts has size 1 to differ from error conditions.
# Since we don't know size before hand we push_back as in result set.
# We need to start pushing from scratch.
# TODO:support for carray?
        if $argLB == "CPP"
          iterationF = "<space>iter = any_dstr;\n<space>rs->resBts.clear();\n<space>while (iter != NULL) {"
        else
          if @container_class == "generic_clist"
            loop = display_traverse("iter", "any_dstr", "->")
            iterationF = "<space>#{loop} {"
          else
            iterationF = "<space>iter = any_dstr;\n<space>while (iter != NULL) {"
          end
        end
      else
        iterationF = "<space>for (iter = any_dstr->begin(); iter != any_dstr->end(); iter++) {"
      end
      if $argLB == "CPP"
        iterationN = "<space>index = rs->resBts.find_first();\n<space>resIterC = rs->res.begin();\n<space>while (resIterC != rs->res.end()) {"
      else
        iterationN = "<space>index = 0;\n<space>while (((#{@name}ResultSetImpl *)rs)->res[index] == NULL) {index++;}\n<space>while (index < (int)rs->size) {"
      end
      return iterationF, iterationN
    end
    return "", ""
  end

  def gen_rownum(fw)
    fw.puts "      rowNum = sqlite3_value_int(val);"
    if !@@C_container_types.include?(@container_class)
      fw.puts "      if (rowNum > (int)rs->resBts.size()) {"
    else
      fw.puts "      iter = any_dstr;"
      fw.puts "      while (iter != NULL) {"
      fw.puts "#{$s}if (rowNum == i) {"
      if $argLB == "CPP"
        fw.puts "#{$s}  found = true;"
      else
        fw.puts "#{$s}  found = 1;"
      end
      fw.puts "#{$s}  break;"
      fw.puts "#{$s}}"
      fw.puts "#{$s}iter = iter->#{@iterator};"
      fw.puts "#{$s}i++;"
      fw.puts "      }"
      fw.puts "      if (!found) {"
    end
    fw.puts "        rs->res.clear();"
    fw.puts "        rs->resBts.clear();"
    fw.puts "        return SQLITE_OK;" 
    fw.puts "      }"
    if !@@C_container_types.include?(@container_class)
      fw.puts "      iter = any_dstr->begin();"
      fw.puts "      for (int i = 0; i < rowNum; i++)"
      fw.puts "        iter++;"
    end
    fw.puts "      if (first_constr == 1) {"
    if @@C_container_types.include?(@container_class)
      fw.puts "#{$s}rs->resBts.resize(rowNum + 1, 0);"
    end
    fw.puts "#{$s}rs->res.push_back(iter);"
    fw.puts "#{$s}rs->resBts.set(rowNum, 1);"
    fw.puts "      } else {"
    fw.puts "#{$s}if (rs->resBts.test(rowNum)) {"
    fw.puts "#{$s}  rs->resBts.reset();"
    fw.puts "#{$s}  rs->resBts.set(rowNum, 1);"
    fw.puts "#{$s}  rs->res.clear();"
    fw.puts "#{$s}  rs->res.push_back(iter);"
    fw.puts "#{$s}  rs->resIter = rs->res.begin();"
    fw.puts "#{$s}} else {"
    fw.puts "#{$s}  rs->resBts.clear();"
    fw.puts "#{$s}  rs->res.clear();"
    fw.puts "#{$s}}"
    fw.puts "      }"
    fw.puts "      break;"
  end

  def gen_base(fw)
    fw.puts "      if (first_constr == 1) {"
    iterationF, useless = configure_iteration()
    add_to_result_setF, useless = configure_result_set
    if @container_class.length > 0
      fw.puts "#{iterationF.gsub(/<space>/, "#{$s}")}"
    end
# Customizing..each gsub targets a single case so safe.
# CPP, C_containers
    add_to_result_setF.gsub!(/\n<space>  \} else \{\n<space>    rs->resBts.push_back\(0\);\n<space>  \}/, "")
# All cases except CPP, C containers: no compare to close if in base
    if !@@C_container_types.include?(@container_class)
      add_to_result_setF.gsub!(/\n<space>\}/, "")
    else
# CPP, C containers: configure spacing
      if @container_class == "clist"
        add_to_result_setF.gsub!(/\n<space>  \}\n<space>  iter = iter->#{@iterator};\n<space>\}/, "\n<space>    }\n<space>    iter = iter->#{@iterator};\n<space>  }")
      elsif @container_class == "generic_clist"
        add_to_result_setF.gsub!(/\n<space>  \}\n<space>\}/, "\n<space>    }\n<space>  }")
      end
# C, C containers : remove extra '}'
      if $argLB == "C"
        if @container_class == "clist"
          add_to_result_setF.gsub!(/\n<space>    \}\n<space>    iter = iter->#{@iterator};\n<space>  \}/, "\n<space>    iter = iter->#{@iterator};\n<space>  }")
        elsif @container_class == "generic_clist"
          add_to_result_setF.gsub!(/\n<space>    \}\n<space>  \}/, "\n<space>  }")
        end
      end
    end
# CPP, CPP_containers
    add_to_result_setF.gsub!(/\n<space>  index\+\+;/, "")
    add_to_result_setF.gsub!(/index/, "index++")
    fw.puts "#{add_to_result_setF.gsub("<space>", "      ")}"
    fw.puts "      } else {"
    fw.puts "#{$s}printf(\"Constraint for BASE column on embedded data structure has not been placed first. Exiting now.\\n\");"
    fw.puts "#{$s}return SQLITE_MISUSE;"
    fw.puts "      }"
    fw.puts "      break;"
  end
  
  def gen_union_col_constr(fw, union_view_name, root_access_path, 
                           union_access_path, add_to_result_set, 
                           iteration, notC)
    columns = Array.new
    switch = ""
    $union_views.each { |uv| 
      if uv.name == union_view_name
        columns = uv.columns 
        switch = uv.switch
      end 
    }
    space = ""
    space.replace($s)
    if @container_class.length > 0
      space.concat("  ")
    end
    fw.puts "#{space}switch (#{root_access_path}#{switch}) {"
    columns.each { |col|
      fw.puts "#{space}case #{col.case}:"
      space.concat("  ")
      sqlite3_type = "search"
      column_cast = ""
      sqlite3_parameters = ""
      column_cast_back = ""
      access_path_col = ""
      total_access_path = ""
      op, union_view_embedded, col_type, line, fk_method_ret, useless3,
      useless4 = 
      col.bind_datatypes(sqlite3_type, 
                         column_cast, sqlite3_parameters, 
                         column_cast_back, access_path_col)
      total_access_path.replace(union_access_path).concat(access_path_col)
      if op == "fk" && col_type == "object"
        total_access_path = "&#{total_access_path}"
      elsif op == "union"
        if fk_type == "pointer"
          total_access_path.concat("->")
        else
          total_access_path.concat(".")
        end
      end
      if $argD == "DEBUG"
        puts "sqlite3_type: " + sqlite3_type
        puts "column_cast: " + column_cast
        puts "sqlite3_parameters: " + sqlite3_parameters
        puts "column_cast_back: " + column_cast_back
        puts "access_path: " + access_path_col
        puts "col_type: " + col_type
        if op == "union"
          puts "Union: " + union_view_embedded
        end
        if line != nil
          puts "line: " + (line + 1).to_s
        else 
          puts "line: nil"
        end
      end
      case op
      when "fk"
        gen_fk_col_constr(fw, fk_method_ret, total_access_path, col_type, 
                          column_cast, column_cast_back, sqlite3_type, 
                          sqlite3_parameters, line, add_to_result_set, 
                          space, notC, iteration)
      when "gen_all"
        gen_all_constr(fw, column_cast, total_access_path, 
                       column_cast_back, sqlite3_type, notC, 
                       iteration, add_to_result_set, line, space)
      when "union"
        gen_union_col_constr(fw, union_view_embedded, union_access_path, 
                             total_access_path, add_to_result_set, 
                             iteration, notC)
      end
      if @container_class.length > 0
        space.concat("  ")
      end
      fw.puts "#{space}break;"
      space.chomp!("  ")
    }
    fw.puts "#{space}}"
  end

  def gen_union(fw, union_view_name, union_access_path, col_type)
    full_union_access_pathF, full_union_access_pathN, idenF, idenN = configure_search("union", union_access_path, col_type)
    add_to_result_setF, add_to_result_setN = configure_result_set()
    iterationF, iterationN = configure_iteration()
    fw.puts "      if (first_constr == 1) {"
    space = "#{$s}"
    if @container_class.length > 0
      add_to_result_setF.chomp!("\n<space>  iter = iter->#{@iterator};\n<space>}")
      add_to_result_setN.chomp!("\n<space>}")
      fw.puts "#{iterationF.gsub(/<space>/, "#{space}")}"
#      space.concat("  ")
    end
    notC = ""
    iteration = ""
    gen_union_col_constr(fw, union_view_name, idenF,
                         full_union_access_pathF, add_to_result_setF, 
                         iteration, notC)
    if @container_class.length > 0
      if @@C_container_types.include?(@container_class)
        fw.puts "#{$s}  iter = iter->#{@iterator};"
      end
      fw.puts "#{$s}}"
      fw.puts "      } else {"
      fw.puts "#{iterationN.gsub(/<space>/, "#{space}")}"
#      space.concat("  ")
    else
      fw.puts "      } else if (stcsr->size == 1) {"
    end
    notC = "!"
    gen_union_col_constr(fw, union_view_name, idenN, 
                         full_union_access_pathN, add_to_result_setN, 
                         iteration, notC)
    if @container_class.length > 0
      fw.puts "#{$s}}"
    end
    fw.puts "      }"
    fw.puts "      break;"
  end

  def gen_all_constr(fw, column_cast, access_path, column_cast_back, 
                     sqlite3_type, notC, iteration, add_to_result_set,
                     line, space)
    if @container_class.length > 0
      if iteration.length > 0
        fw.puts "#{iteration.gsub(/<space>/, "#{space}")}"
        space.concat("  ")
      end
# not for union
    end
    fw.puts "#{space}if (#{notC}compare_#{sqlite3_type}(#{column_cast}#{access_path}#{column_cast_back}, op, sqlite3_value_#{sqlite3_type}(val))) {"
    print_line_directive(fw, line)
    if @container_class.length > 0
      space.chomp!("  ")
    end
    fw.puts "#{add_to_result_set.gsub("<space>", "#{space}")}"
  end

  def gen_all(fw, column_cast, access_path, 
              column_cast_back, sqlite3_type, line)
    access_pathF = ""
    access_pathN = ""
    add_to_result_setF = ""
    add_to_result_setN = ""
    iterationF = ""
    iterationN = ""
    space = ""
    space.replace($s)
    access_pathF, access_pathN, idenF, idenN = configure_search("all", access_path, "")
    add_to_result_setF, add_to_result_setN = configure_result_set()
    iterationF, iterationN = configure_iteration()
    fw.puts "      if (first_constr == 1) {"
    notC = ""
    gen_all_constr(fw, column_cast, access_pathF, column_cast_back, 
                   sqlite3_type, notC, iterationF, add_to_result_setF, 
                   line, space)
    if @container_class.length > 0
      fw.puts "      } else {"
    else
      fw.puts "      } else if (stcsr->size == 1) {"
    end
    notC = "!"
    gen_all_constr(fw, column_cast, access_pathN, column_cast_back, 
                   sqlite3_type, notC, iterationN, add_to_result_setN, 
                   line, space)
    fw.puts "      }"
    fw.puts "      break;"    
  end
  
  def gen_fk_col_constr(fw, fk_method_ret, access_path, fk_type, 
                        column_cast, column_cast_back, sqlite3_type, 
                        sqlite3_parameters, line, add_to_result_set, 
                        space, notC, iteration)
    if $argM == "MEM_MGT" && fk_method_ret == 1
      fw.puts "#{space}{"
      space.concat("  ")
      fw.puts "#{space}typeof(#{access_path}) t = #{access_path};"
    end
    if @container_class.length > 0
      if iteration.length > 0
        fw.puts "#{iteration.gsub("<space>", "#{space}")}"
        space.concat("  ")
      end
    end
    fw.puts "#ifdef ENVIRONMENT64"
    if $argM == "MEM_MGT" && fk_method_ret == 1    # Returning from a method.
      fw.puts "#{space}if (#{notC}compare_#{sqlite3_type}(#{column_cast}t#{column_cast_back}, op, #{column_cast.chomp('&')}sqlite3_value_#{sqlite3_type}(val))) {"
    else
      fw.puts "#{space}if (#{notC}compare_#{sqlite3_type}(#{column_cast}#{access_path}#{column_cast_back}, op, #{column_cast.chomp('&')}sqlite3_value_#{sqlite3_type}(val))) {"
    end
    print_line_directive(fw, line)
    fw.puts "#else"
    if $argM == "MEM_MGT" && fk_method_ret == 1
      fw.puts "#{space}if (#{notC}compare_#{sqlite3_parameters}(#{column_cast}t#{column_cast_back}, op, #{column_cast.chomp('&')}sqlite3_value_#{sqlite3_parameters}(val))) {"
    else
      fw.puts "#{space}if (#{notC}compare_#{sqlite3_parameters}(#{column_cast}#{access_path}#{column_cast_back}, op, #{column_cast.chomp('&')}sqlite3_value_#{sqlite3_parameters}(val))) {"
    end
    if @container_class.length > 0
      space.chomp!("  ")
    end
    print_line_directive(fw, line)
    fw.puts "#endif"
    fw.puts "#{add_to_result_set.gsub("<space>", "#{space}")}"
    if @container_class.length > 0
      if iteration.length > 0
        space.chomp!("  ")
      end
    end
    if @container_class.length == 0
      space.chomp!("  ")
    end
    if $argM == "MEM_MGT" && fk_method_ret == 1
      fw.puts "#{space}}"
      space.chomp!("  ")
    end
  end

  def gen_fk(fw, fk_type, fk_method_ret, column_cast, column_cast_back, 
             sqlite3_type, sqlite3_parameters, line, access_path)
    access_pathF = ""
    access_pathN = ""
    add_to_result_setF = ""
    add_to_result_setN = ""
    iterationF = ""
    iterationN = ""
    space = ""
    access_pathF, access_pathN, idenF, idenN = configure_search("fk", access_path, fk_type)
    add_to_result_setF, add_to_result_setN = configure_result_set()
    iterationF, iterationN = configure_iteration()
    fw.puts "      if (first_constr) {"
    space.replace($s)
    notC = ""
    gen_fk_col_constr(fw, fk_method_ret, access_pathF, fk_type, 
                      column_cast, column_cast_back, sqlite3_type, 
                      sqlite3_parameters, line, add_to_result_setF, 
                      space, notC, iterationF)
    if @container_class.length > 0
      fw.puts "      } else {"
    else
      fw.puts "      } else if (stcsr->size == 1) {"
    end
    space.concat("  ")
    notC = "!"
    gen_fk_col_constr(fw, fk_method_ret, access_pathN, fk_type, 
                      column_cast, column_cast_back, sqlite3_type, 
                      sqlite3_parameters, line, add_to_result_setN, 
                      space, notC, iterationN)
    fw.puts "      }"
    fw.puts "      break;"
  end

# Generates code to search each VT struct.
# Each search case matches a specific column of the VT.
  def search_columns(fw)
    fw.puts "    switch (nCol) {"
    @columns.each_index { |col|
      fw.puts "    case #{col}:"
      sqlite3_type = "search"
      column_cast = ""
      sqlite3_parameters = ""
      column_cast_back = ""
      access_path = ""
      space = "      "
      op, union_view_name, col_type, line, fk_method_ret, useless3,
      useless4 = 
      @columns[col].bind_datatypes(sqlite3_type, 
                                   column_cast, sqlite3_parameters, 
                                   column_cast_back, access_path)
      if $argD == "DEBUG"
        puts "sqlite3_type: " + sqlite3_type
        puts "column_cast: " + column_cast
        puts "sqlite3_parameters: " + sqlite3_parameters
        puts "column_cast_back: " + column_cast_back
        puts "access_path: " + access_path
        if op == "union"
          puts "Union: " + union_view_name
        end
        if line != nil
          puts "line: " + (line + 1).to_s
        else 
          puts "line: nil"
        end
      end
      case op
      when "fk"
        gen_fk(fw, col_type, fk_method_ret, column_cast, 
               column_cast_back, 
               sqlite3_type, sqlite3_parameters, line, access_path)
      when "gen_all"
        gen_all(fw, column_cast, access_path, 
                column_cast_back, sqlite3_type, line)
      when "union"
        gen_union(fw, union_view_name, access_path, col_type)
      when "base"
        gen_base(fw)
      when "rownum"
        gen_rownum(fw)
      end
    }
    fw.puts "    }"
  end

# Calls template to generate code in search method. 
# Code makes the necessary arrangements for search to happen successfully 
# (condition checks, reallocation).
  def setup_search(fw)
    file = File.open("pico_ql_erb_templates/pico_ql_pre_search_#{$argLB.downcase}.erb").read
    pre_search = ERB.new(file, 0, '>')
    fw.puts pre_search.result(get_binding)
  end

# Calls template to generate code in result set iterator methods. 
# Code makes the necessary arrangements for managing a result set. 
  def result_set_iter(fw)
    file = File.open("pico_ql_erb_templates/pico_ql_result_set_iter_#{$argLB.downcase}.erb").read
    result_set_iter = ERB.new(file, 0, '>')
    fw.puts result_set_iter.result(get_binding)
  end

# Register line that corresponds to the table in DSL description
  def register_line()
    $lined_description.each_index { |line|
      if $lined_description[line].match(/with registered c name #{Regexp.escape(@base_var)}/i)
        @base_var_line = line
        if $argD == "DEBUG"
          puts "Virtual table's C NAME found at line #{@base_var_line + 1} of #{$argF}"
        end
        break
        if $argD == "DEBUG"
          puts "Line #{line + 1}"
        end
      end
    }
    $lined_description.each_index { |line|
      if $lined_description[line].match(/with registered c type #{Regexp.escape(@signature)}/i)
        @signature_line = line
        if $argD == "DEBUG"
          puts"LINE is #{$lined_description[line]}"
          puts "Virtual table's C TYPE found at line #{@signature_line + 1} of #{$argF}"
        end
        break
        if $argD == "DEBUG"
          puts "Line #{line + 1}"
        end
      end
    }
  end

# Validate the signature of a container structure and extract signature traits.
# Also for objects, extract class name.
  def verify_signature()
    begin
      case @signature
      when /(\w+)<(.+)>(\**)/m
        matchdata = /(\w+)<(.+)>(\**)/m.match(@signature)
        if @@C_container_types.include?(matchdata[1])
          @signature = matchdata[2]
          if @signature.match(/(\w+)(\s*)\*/)
            @pointer = "*"
          end
        else
          @pointer = matchdata[3]
        end
        @container_class = matchdata[1]
        @type = matchdata[2]
        if $argD == "DEBUG"
          puts "Virtual table container class name is: " + @container_class
          puts "Virtual table record is of type: " + @type
          puts "Virtual table type is of type pointer: " + @pointer
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
        raise "Template instantiation faulty: #{@signature}.\n"
      end
    rescue
      puts "Template instantiation faulty: #{@signature}.\n"
      exit(1)
    end
  end

  def process_traverse()
    @traverse.gsub!(/(\s+)/, "")
    matchdata = @traverse.match(/\((.+)\)/)
    if matchdata
      args = matchdata[1].split(/,/)
      args.each { |rg|
        @traverse.gsub!("#{rg}", "<#{rg}>")
      }
    else
      puts "Invalid traverse function: #{@traverse}."
      puts "Exiting now."
      exit(1)
    end
  end

# Matches VT definitions against prototype patterns.
  def match_table(table_description)
    table_ptn1 = /^create virtual table (\w+)\.(\w+) using struct view (\w+) with registered c name (.+) with registered c type (.+) using c traverse function (.+)/im
    table_ptn2 = /^create virtual table (\w+)\.(\w+) using struct view (\w+) with registered c type (.+) using c traverse function (.+)/im
    table_ptn3 = /^create virtual table (\w+)\.(\w+) using struct view (\w+) with registered c name (.+) with registered c type (.+) using c iterator name (\w+)/im
    table_ptn4 = /^create virtual table (\w+)\.(\w+) using struct view (\w+) with registered c type (.+) using c iterator name (\w+)/im
    table_ptn5 = /^create virtual table (\w+)\.(\w+) using struct view (\w+) with registered c name (.+) with registered c type (.+)/im
    table_ptn6 = /^create virtual table (\w+)\.(\w+) using struct view (\w+) with registered c type (.+)/im
    if $argD == "DEBUG"
      puts "Table description is: #{table_description}"
    end
    struct_view_name = ""
    case table_description
    when table_ptn1
      matchdata = table_ptn1.match(table_description)
      @db = matchdata[1]
      @name = matchdata[2]
      struct_view_name = matchdata[3]
      @base_var = matchdata[4]
      @signature = matchdata[5]
      @traverse = matchdata[6]
      process_traverse()
    when table_ptn2
      matchdata = table_ptn2.match(table_description)
      @db = matchdata[1]
      @name = matchdata[2]
      struct_view_name = matchdata[3]
      @signature = matchdata[4]
      @traverse = matchdata[5]
      process_traverse()
    when table_ptn3
      matchdata = table_ptn3.match(table_description)
      @db = matchdata[1]
      @name = matchdata[2]
      struct_view_name = matchdata[3]
      @base_var = matchdata[4]
      @signature = matchdata[5]
      @iterator = matchdata[6]
    when table_ptn4
      matchdata = table_ptn4.match(table_description)
      @db = matchdata[1]
      @name = matchdata[2]
      struct_view_name = matchdata[3]
      @signature = matchdata[4]
      @iterator = matchdata[5]
    when table_ptn5
      matchdata = table_ptn5.match(table_description)
      @db = matchdata[1]
      @name = matchdata[2]
      struct_view_name = matchdata[3]
      @base_var = matchdata[4]
      @signature = matchdata[5]
    when table_ptn6
      matchdata = table_ptn6.match(table_description)
      @db = matchdata[1]
      @name = matchdata[2]
      struct_view_name = matchdata[3]
      @signature = matchdata[4]
    end
    verify_signature()
    $struct_views.each { |sv| if sv.name == struct_view_name : @struct_view = sv end }
    begin
      if @struct_view == nil
        raise "Cannot match struct_view for table #{@name}.\\n"
      end
    rescue
      if @name.length == 0 
        @name = "<empty_name>" 
        puts "Perhaps you have forgotten to type the database name, e.g. CREATE VIRTUAL TABLE DB_NAME.TABLE_NAME"
      end
      puts "Cannot match struct_view for table #{@name}."
      exit(1)
    end
    $table_index[@name] = @signature
    if @base_var.length == 0        # base column for embedded structs.
      @columns.push(Column.new("")).last.set("base INT FROM self") 
# perhaps PRIMARY KEY(base)
    end
    if @container_class.length > 0 && $argLB == "CPP"
      @columns.push(Column.new("")).last.set("rownum INT FROM self") 
    end
    @include_text_col = @struct_view.include_text_col
    @columns = @columns | @struct_view.columns
    register_line()
    if $argD == "DEBUG"
      puts "Table name is: " + @name
      puts "Table's C NAME defined at line #{@base_var_line + 1} of #{$argF}"
      puts "Table's C TYPE defined at line #{@signature_line + 1} of #{$argF}"
      puts "Table lives in database named: " + @db
      puts "Table base variable name is: " + @base_var
      puts "Table signature name is: " + @signature
      puts "Table follows struct_view: " + @struct_view.name
    end
  end

end

class StructView
  def initialize()
    @name = ""
    @columns = Array.new
    @include_text_col = 0 # True if StructView includes column
    		      	  # of text data type. Required for
			  # generating code for
			  # PICO_QL_HANDLE_TEXT_ARRAY C++ flag.
  end
  attr_accessor(:name,:columns,:include_text_col)

  # Matches a struct view definition against the prototype pattern and 
  # extracts the characteristics.
  def match_struct_view(struct_view_description)
    if $argD == "DEBUG"
      puts "Struct view description is: #{struct_view_description}"
    end
    pattern = /^create struct view (\w+)(\s*)\((.+)(\s*)\)/im
    matchdata = pattern.match(struct_view_description)
    if matchdata
      # First record of table_data contains the whole description of the 
      # structview.
      # Second record contains the struct view's name
      @name = matchdata[1]
      columns_str = Array.new
      if matchdata[3].match(/,/)
        columns_str = matchdata[3].split(/,/)
      else
        columns_str[0] = matchdata[3]
      end
    end
    begin
      columns_str.each { |x| @include_text_col += @columns.push(Column.new("")).last.set(x) }
    rescue Exception => e
      puts e.message
      exit(1)
    end
    if $argD == "DEBUG"
      puts "Struct view #{@name} registered."
      puts "Struct view includes #{@include_text_col} columns of type text."
      puts "Columns follow:"
      @columns.each { |x| p x }
    end
  end

  # Removes the last entry in the columns table and returns the table 
  # itself. Useful when including a struct_view definition to remove the 
  # entry left empty.
  def columns_delete_last()
    @columns.delete(@columns.last)
    return @columns
  end

end


# Models a (standard relational) view definition.
class RelationalView

  def initialize(definition)
    @stmt = definition
    @name = ""
  end
  attr_accessor(:stmt,:name)

  def extract_name()
    pattern = /^create view (\w+) as/im
    matchdata = pattern.match(@stmt)
    if matchdata
      # First record of table_data contains the whole description of the 
      # struct_view.
      # Second record contains the view's name
      @name = matchdata[1]
    end
  end

end

class UnionView

  def initialize()
    @name = ""
    @switch = ""
    @columns = Array.new
    @include_text_col = 0 # True if StructView includes column
                          # of text data type. Required for
			  # generating code for
			  # PICO_QL_HANDLE_TEXT_ARRAY C++ flag.
  end
  attr_accessor(:name,:switch,:columns,:include_text_col)

  def match_union_view(union_view_description)
    if $argD == "DEBUG"
      puts "Union view description is: #{union_view_description}"
    end
    union_view_description.lstrip!
    union_view_description.chomp!(")")
    union_view_description.rstrip!
    if union_view_description.match(/when/i)
      utokened = union_view_description.split(/when/i)
    else
      puts "Union description not matched: #{union_view_description}"
      puts "Exiting now"
      exit(1)
    end
    header = utokened[0]
    header.rstrip!
    utokened.delete_at(0)
    pattern = /^create union view (\w+)(\s*)\((\s*)case (.+)/im
    matchdata = pattern.match(header)
    if matchdata
      # First record of table_data contains the whole description of the 
      # structview.
      # Second record contains the struct view's name
      @name = matchdata[1]
      @switch = matchdata[4]
      columns_str = Array.new
    else
      puts "Union description header not matched: #{header}"
      puts "Exiting now"
      exit(1)
    end
    utokened.each { |x| 
      matchdata = / (\w+) then (.+)/im.match(x)
      if matchdata
        @include_text_col += @columns.push(Column.new(matchdata[1])).last.set(matchdata[2])
      else
        puts "Invalid description for union column: #{x}"
        puts "Exiting now"
        exit(1)
      end
    }
    if $argD == "DEBUG"
      puts "Union view #{@name} registered."
      puts "Union view includes #{@include_text_col} columns of type text."
      puts "Columns follow:"
      @columns.each { |x| p x }
    end
  end

end

class Lock
  def initialize
    @lock_function = ""
    @unlock_function = ""
    @active = 0
  end
  attr_accessor(:lock_function, :unlock_function, :active)

  def match_lock(lock_description)
    lock_ptn = /use c lock (.+) unlock (.+)/im
    case lock_description
    when lock_ptn
      matchdata = lock_ptn.match(lock_description)
      @lock_function = matchdata[1]
      @unlock_function = matchdata[2]
      @active = 1
    end
    if $argD == "DEBUG"
      puts "Lock description: #{lock_description}"
      puts "Lock function: #{@lock_function}"
      puts "Unlock function: #{@unlock_function}"
      puts "Active: #{@active}"
      puts "Universal lock registered."
    end
  end
  
end

# Models the input description.
class InputDescription
  def initialize(description)
    # original description tokenised in an Array using ';' delimeter
    @description = description
    # array with entries the identity of each virtual table
    @tables = Array.new
    @views = Array.new
    @directives = ""
  end
  attr_accessor(:description,:tables,:views,:directives)

# Support templating of member data
  def get_binding
    binding
  end

# Calls template to generate code in retrieve method. 
# Code makes the necessary arrangements for retrieve to happen successfully 
# (condition checks, reallocation)
  def wrap_retrieve(fw)
    file = File.open("pico_ql_erb_templates/pico_ql_wrapper_retrieve_#{$argLB.downcase}.erb").read
    wrapper_retrieve = ERB.new(file, 0, '>')
    fw.puts wrapper_retrieve.result(get_binding)
  end


# Calls the family of methods that generate the application-specific 
# retrieve method for each VT struct.
  def print_retrieve_functions(fw)
    @tables.each { |vt|
      vt.setup_retrieve(fw)
      vt.retrieve_columns(fw)
      vt.finish_retrieve(fw)
    }
    wrap_retrieve(fw)
  end


# Calls template to generate code in search method. 
# Code makes the necessary arrangements for retrieve to happen successfully 
# (condition checks, reallocation)
  def wrap_search(fw)
    file = File.open("pico_ql_erb_templates/pico_ql_wrapper_search_#{$argLB.downcase}.erb").read
    wrapper_search = ERB.new(file, 0, '>')
    fw.puts wrapper_search.result(get_binding)
  end


# Calls the family of methods that generate the application-specific 
# search method for each VT struct.
  def print_search_functions(fw)
    @tables.each { |vt|
      vt.setup_search(fw)
      vt.search_columns(fw)
      vt.finish_search(fw)
    }
    wrap_search(fw)
  end

# Calls template to generate code in result set iterator methods. 
# Code makes the necessary arrangements for managing a result set. 
  def wrap_result_set_iter(fw)
    file = File.open("pico_ql_erb_templates/pico_ql_wrapper_result_set_iter_#{$argLB.downcase}.erb").read
    wrapper_result_set_iter = ERB.new(file, 0, '>')
    fw.puts wrapper_result_set_iter.result(get_binding)
  end

# Calls the family of methods that generate the application-specific 
# result set iterator methods for each VT struct.
  def print_result_set_iter(fw)
    @tables.each { |vt|
      vt.result_set_iter(fw)
    }
    wrap_result_set_iter(fw)
  end
  
# Generates the LICENSE copyright notice and directives as prescribed 
# from user in the description for pico_ql_internal.cpp
  def print_directives_utils(fw)
    file = File.open("pico_ql_erb_templates/pico_ql_directives_utils_#{$argLB.downcase}.erb").read
    directives = ERB.new(file, 0, '>')
    fw.puts directives.result(get_binding)
  end

# Generates the LICENSE copyright notice and application interface 
# functions in pico_ql_search.cpp
  def print_register_serve(fw)
    file = File.open("pico_ql_erb_templates/pico_ql_register_serve_#{$argLB.downcase}.erb").read
    app_interface = ERB.new(file, 0, '>')
    fw.puts app_interface.result(get_binding)
  end

# Generates application-specific code to complement the SQTL library.
# There is a call to each of the above generative functions.
  def generate()
    myfile = File.open("pico_ql_search.#{$argLB.downcase}", "w") do |fw|
      print_register_serve(fw)
    end
    puts "Created/updated pico_ql_search.#{$argLB.downcase} ."
    myfile = File.open("pico_ql_internal.#{$argLB.downcase}", "w") do |fw|
      print_directives_utils(fw)
      print_result_set_iter(fw)
      print_search_functions(fw)
      print_retrieve_functions(fw)
    end
    puts "Created/updated pico_ql_internal.#{$argLB.downcase} ."
    myFile = File.open("pico_ql_makefile.append", "w") do |fw|
      file = File.open("pico_ql_erb_templates/pico_ql_makefile_#{$argLB.downcase}.erb").read
      makefile = ERB.new(file, 0, '>')
      fw.puts makefile.result(get_binding)
    end
    puts "Created/updated pico_ql_makefile.append ."
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
    token_d = token_d.select { |x| x =~ /(\S+)/ }
    token_d[0].lstrip!
    if token_d[0].start_with?("#include")
      @directives = token_d[0]
      # Putback the ';' after the namespace.
      if @directives.match(/using namespace (.+)/)
        @directives.gsub!(/using namespace (.+)/, 'using namespace \1;')
      end
      token_d.delete_at(0)
      if $argD == "DEBUG"
        line = 0              # Put line directives in include directives.
        if @directives.match(/\n/)
          @directives.gsub!(/\n/){ |nl|
            "\n#line #{(line += 1).to_s} \"#{$argF}\"" + nl
          }
        end
        puts "Directives: #{@directives}"
      end
    end
    x = 0
    while x < token_d.length            # Cleaning white space.
      if /\n|\t|\r|\f/.match(token_d[x])
        token_d[x].gsub!(/\n|\t|\r|\f/, " ") 
      end
      token_d[x].lstrip!
      token_d[x].rstrip!
      token_d[x].squeeze!(" ")
      if / ,|, /.match(token_d[x]) : token_d[x].gsub!(/ ,|, /, ",") end
      x += 1
    end
    @description = token_d.select{ |x| x =~ /(\S+)/ }
    if $argD == "DEBUG"
      puts "Description after whitespace cleanup: "
      @description.each { |x| p x }
    end
    $struct_views = Array.new
    $union_views = Array.new
    $lock = Lock.new
    $table_index = Hash.new
    w = 0
    @description.each { |stmt|
      if $argD == "DEBUG"
        puts "\nDESCRIPTION No: " + w.to_s + "\n"
      end
      stmt.lstrip!
      stmt.rstrip!
      case stmt
      when /^create struct view/im
        $struct_views.push(StructView.new).last.match_struct_view(stmt)
      when /^create virtual table/im
        @tables.push(VirtualTable.new).last.match_table(stmt)
      when /^create view (\w+) as/im
        @views.push(RelationalView.new(stmt)).last.extract_name()
      when /^create union view/im
        $union_views.push(UnionView.new).last.match_union_view(stmt)
      when /^use c lock/im
        $lock.match_lock(stmt)
      end
      w += 1
    }
    if $argD == "DEBUG"
      puts "Table index entries:"
      $table_index.each_pair { |k,v| p "#{k}-#{v}"}
    end
  end
  
end


# Take cases on command-line arguments.
def take_cases(argv)
  case argv
  when /debug/i
    $argD = "DEBUG"
  when /no_mem_mgt/i
    $argM = "NO_MEM_MGT"
  when "C"
    $argLB = "C"
  end
end


# The main method.
if __FILE__ == $0
  $argF = ARGV[0]
  $argM = "MEM_MGT"
  $argLB = "CPP"
  ARGV.each_index { |arg| if arg > 0 : take_cases(ARGV[arg]) end }
  begin
    $lined_description = File.open($argF, "r") { |fw| 
      fw.readlines.each{ |line| 
        # Drop single-line comments, multi-line comments not supported.
        if line.match(/\/\/(.+)/)
          line.gsub!(/\/\/(.+)/, "")
        end
      }
    }
#    $lined_description = $lined_description.select { |line| !line.empty? }
  rescue Exception => e
    puts e.message
    exit(1)
  end
  # Strip white-space.
  $lined_description.each { |line|
    line.squeeze!(" ")
    if / ,|, /.match(line) : line.gsub!(/ ,|, /, ",") end
    #      if / \(/.match(line) : line.gsub!(/ \(/, "(") end
    if /\( /.match(line) : line.gsub!(/\( /, "(") end
    #      if /\) /.match(line) : line.gsub!(/\) /, ")") end
    if / \)/.match(line) : line.gsub!(/ \)/, ")") end
  }
  description = $lined_description.to_s
  # Remove the ';' from the namespace before splitting.
  if description.match(/using namespace (.+);/)
    description.gsub!(/using namespace (.+);/, 'using namespace \1')
  end
  begin
    token_description = description.split(/;/)
  rescue
    puts "Invalid description..delimeter ';' not used."
    exit(1)
  end
  $s = "        "
  ip = InputDescription.new(token_description)
  ip.register_datastructures()
  ip.generate()
end
