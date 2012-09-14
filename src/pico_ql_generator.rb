# -*- coding: utf-8 -*-
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

require 'erb'

# Models a column of the Virtual Table (VT).
class Column
  def initialize
    @name = ""
    @line = 0                 # Line in DSL description that the column
                              # (the column's access path to be precise)
                              # is defined in.
    @data_type = ""
    @cpp_data_type = ""       # Respective C++ data type. Used only 
                              # for string so far.
    @related_to = ""          # Reference to other VT's name (FK).
    @access_path = ""         # The access statement for the column value.
    @col_type = ""                # Record type (pointer or reference) for 
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
  attr_accessor(:name,:line,:data_type,:related_to,:access_path,:col_type)


# Used to clone a Column object. Ruby does not support deep copies.
  def construct(name, data_type, related_to, access_path, type, line)
    @name = name
    @data_type = data_type
    @related_to = related_to
    @access_path = access_path
    @col_type = type
    @line = line
  end

# Fills variables


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
      return "fk", @related_to, @col_type, @line
    end
    if @name == "base"              # 'base' column. refactor: elsif perhaps?
      sqlite3_type.replace("int64")
      column_cast.replace("(long int)")
      sqlite3_parameters.replace("int");    # for 32-bit architectures.used in retrieve.
      return "base", nil, nil, nil
    elsif @name == "rownum"           # 'rownum'column
      sqlite3_type.replace("int")
      return "rownum", nil, nil, nil
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
    return "gen_all", nil, nil, @line
  end


# Register line that corresponds to the table in DSL description
  def register_line()
    $lined_description.each_index { |line|
      if $lined_description[line].match(/foreign key(\s*)\((\s*)#{Regexp.escape(@name)}(\s*)\) from #{Regexp.escape(@access_path)}|#{Regexp.escape(@name)} #{Regexp.escape(@data_type)} from #{Regexp.escape(@access_path)}/i)
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
      else
        raise TypeError.new("No such data type #{dt.upcase}\\n")
      end
    rescue
      puts "No such data type #{dt.upcase}\\n"
      exit(1)
    end
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
    column_ptn1 = /inherits struct view (\w+) from (.+)/im
    column_ptn2 = /inherits struct view (\w+)/im
    column_ptn3 = /foreign key(\s*)\((\s*)(\w+)(\s*)\) from (.+) references (\w+)(\s*)(\w*)/im
    column_ptn4 = /(\w+) (\w+) from (.+)/im
    case column
    when column_ptn1
      matchdata = column_ptn1.match(column)
      index = 0
      this_struct_view = $struct_views.last
      this_columns = this_struct_view.columns
      $struct_views.each { |vs| 
        if vs.name == matchdata[1]     # Search all struct_view 
                                       # definitions to find the one 
                                       # specified and include it.
          index = this_struct_view.columns.length - 2
          if index < 0 : index = 0 end
          this_struct_view.columns_delete_last()
          vs.columns.each_index { |col| 
            coln = vs.columns[col]         # Manually construct a deep copy
            this_columns.push(Column.new)  # of coln and push it to 'this'.
            this_columns.last.construct(coln.name.clone, 
                                        coln.data_type.clone, 
                                        coln.related_to.clone, 
                                        coln.access_path.clone, 
                                        coln.col_type.clone,
                                        coln.line)
          }
	  col_type_text = vs.include_text_col
        end
      }
      this_columns.each_index { |col|     # Adapt access path.
        if col > index 
          this_columns[col].access_path.replace(matchdata[2] + 
                                                this_columns[col].access_path)
        end
      }
      return col_type_text
    when column_ptn2                      # Merely include a struct_view 
                                          # definition.
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
    when column_ptn3
      matchdata = column_ptn3.match(column)
      @name = matchdata[3]
      @data_type = "INT"
      @related_to = matchdata[6]
      @access_path = matchdata[5]
      begin
        @related_to.length > 0
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
      puts "Column access path is: " + @access_path
      puts "Column type is: " + @col_type
      puts "Column is of text type: " + col_type_text.to_s
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
    @struct_view              # Reference to the respective struct_view definition.
    @db = ""              # Database name to be created/connected.
    @signature = ""       # The C++ signature of the struct.
    @container_class = "" # If a container instance as per 
                          # the SGI container concept.
    @type = ""            # The record type for the VT. 
                          # Use @type for management. It is active for 
                          # both container and object.
    @pointer = ""         # Type of the base_var.
    @object_class = ""    # If an object instance.
    @columns = Array.new  # References to the VT columns.
    @include_text_col = 0 # True if VirtualTable includes column
    		      	  # of text data type. Required for
			  # generating code for
			  # PICO_QL_HANDLE_POLYMORPHISM C++ flag.
  end
  attr_accessor(:name,:base_var_line,:signature_line,:base_var,:struct_view,
                :db,:signature,:container_class,:type,
                :pointer,:object_class,:columns,:include_text_col)

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

# Method performs case analysis to generate 
# the correct form of the variable
  def configure(access_path)
    iden = ""
    if @container_class.length > 0
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
      op, fk_col_name, column_type, line = 
      @columns[col].bind_datatypes(
                                   sqlite3_type, column_cast, 
                                   sqlite3_parameters, column_cast_back, 
                                   access_path)
      if $argD == "DEBUG" && line != nil
        fw.puts "// Line #{line + 1} #{$argF}"
      end
      case op
      when "base"
        fw.puts "#ifdef ENVIRONMENT64"
        # sqlite3_type = "int64" always in this case.
        fw.puts "#{$s}sqlite3_result_#{sqlite3_type}(con, #{column_cast}any_dstr);"
        fw.puts "#else"
        # sqlite3_parameters = "int" always in this case.
        fw.puts "#{$s}sqlite3_result_#{sqlite3_parameters}(con, #{column_cast}any_dstr);"
        fw.puts "#endif"
        fw.puts "#{$s}break;"
      when "rownum"
        fw.puts "#{$s}sqlite3_result_#{sqlite3_type}(con, stcsr->resultSet[index]);"
        fw.puts "#{$s}break;"
      when "fk"
        fw.puts "       {"
        iden = configure(access_path)
        if fk_col_name != nil       # ??
          if access_path.length == 0    # Access with (*iter) .
            @type.match(/\*/) ? record_type = "*" : record_type = ""
          else                          # Access with (*iter)[.|->]access .
            column_type == "pointer" ? record_type = "*" : record_type = ""
          end
        end
        fk_type = $table_index[fk_col_name]
        fk_type.end_with?('*') ? add_pointer = "" : add_pointer = "*"
        fw.puts "#{$s}#{fk_type} #{add_pointer}#{fk_col_name.downcase}_tmp = (#{fk_type} #{add_pointer})sqlite3_malloc(sizeof(#{fk_type.chomp('*')}));"
        fw.puts "#{$s}*#{fk_col_name.downcase}_tmp = #{record_type}#{iden}#{access_path};"
        fw.puts "#ifdef ENVIRONMENT64"
        fw.puts "#{$s}sqlite3_result_#{sqlite3_type}(con, #{column_cast}#{fk_col_name.downcase}_tmp);"
        fw.puts "#else"
        fw.puts "#{$s}sqlite3_result_#{sqlite3_parameters}(con, #{column_cast}#{fk_col_name.downcase}_tmp);"
        fw.puts "#endif"
        fw.puts "#{$s}tmp.push_back((void *)#{fk_col_name.downcase}_tmp);"
        fw.puts "#{$s}break;"
        fw.puts "       }"
      when "gen_all"
        iden = configure(access_path)
        if access_path.match(/this\.|this->/)
          access_path.gsub!(/this\.|this->/, "#{iden}")
        else access_path = "#{iden}#{access_path}"
        end
	if sqlite3_type == "text"
	  if column_cast_back == ".c_str()"
	    string_construct_cast = ""
	  else
	    string_construct_cast = "(const char *)"
	  end
	  fw.puts "#ifdef PICO_QL_HANDLE_POLYMORPHISM"
	  fw.puts "#{$s}tr->push_back(new string(#{string_construct_cast}#{access_path}));"
          fw.puts "#{$s}sqlite3_result_text(con, (const char *)(*tr->back()).c_str()#{sqlite3_parameters});"
          fw.puts "#else"
	end
        fw.puts "#{$s}sqlite3_result_#{sqlite3_type}(con, #{column_cast}#{access_path}#{column_cast_back}#{sqlite3_parameters});"
        if sqlite3_type == "text"
	  fw.puts "#endif"
	end
        fw.puts "#{$s}break;"
      end
    }
  end

# Calls template to generate code in retrieve method. 
# Code makes the necessary arrangements for retrieve to happen successfully 
# (condition checks, reallocation)
  def setup_retrieve(fw)
    file = File.open("pico_ql_erb_templates/pico_ql_pre_retrieve.erb").read
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

# Generates spaces to convene properly aligned code generation.
  def vt_type_spacing(fw)
    fw.print $s
    if @container_class.length > 0
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
      op, useless, fk_type, line = 
      @columns[col].bind_datatypes(sqlite3_type, 
                                   column_cast, sqlite3_parameters, 
                                   column_cast_back, access_path)
      if op == "fk"
        if fk_type == "object" : column_cast.concat("&") end
        op = "gen_all"
      end
      if @container_class.length > 0 && op != "rownum"
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
        if line != nil
          puts "line: " + (line + 1).to_s
        else 
          puts "line: nil"
        end
      end
      if $argD == "DEBUG" && line != nil
        fw.puts "// Line #{line + 1} #{$argF}"
      end
      case op
      when "gen_all"
        if sqlite3_type == "int64"
          fw.puts "#ifdef ENVIRONMENT64"
        end
        vt_type_spacing(fw)
        if access_path.match(/this\.|this->/)
          access_path.gsub!(/this\.|this->/, "#{iden}")
        else 
          access_path = "#{iden}#{access_path}"
        end
        fw.print "if (compare(#{column_cast}#{access_path}#{column_cast_back}, op, sqlite3_value_#{sqlite3_type}(val)) )"
        fw.puts
        if sqlite3_type == "int64"
          fw.puts "#else"
          vt_type_spacing(fw)
          fw.print "if (compare(#{column_cast}#{access_path}#{column_cast_back}, op, sqlite3_value_#{sqlite3_parameters}(val)) )"
          fw.puts
          fw.puts "#endif"
        end
        vt_type_spacing(fw)
        fw.print "    temp_res[count++] = i;"
      when "base"
        vt_type_spacing(fw)
        fw.print "temp_res[count++] = i;"
      when "rownum"
        fw.print "#{$s}    temp_res[count++] = sqlite3_value_int(val);"
      end
      fw.puts
      if @container_class.length > 0 && op != "rownum"
        fw.puts "#{$s}#{$s}iter++;"
        fw.puts "#{$s}    }"
      end
      fw.puts "#{$s}    assert(count <= stcsr->max_size);"
      fw.puts "#{$s}    break;"
    }
    fw.puts "#{$s}}"
  end

# Calls template to generate code in search method. 
# Code makes the necessary arrangements for search to happen successfully 
# (condition checks, reallocation).
  def setup_search(fw)
    file = File.open("pico_ql_erb_templates/pico_ql_pre_search.erb").read
    pre_search = ERB.new(file, 0, '>')
    fw.puts pre_search.result(get_binding)
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
        @container_class = matchdata[1]
        @type = matchdata[2]
        @pointer = matchdata[3]
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
        raise "Template instantiation faulty: #{@signature}.\\n"
      end
    rescue
      puts "Template instantiation faulty: #{@signature}.\\n"
      exit(1)
    end
  end

# Matches VT definitions against prototype patterns.
  def match_table(table_description)
    table_ptn1 = /^create virtual table (\w+)\.(\w+) using struct view (\w+) with registered c name (.+) with registered c type (.+)/im
    table_ptn2 = /^create virtual table (\w+)\.(\w+) using struct view (\w+) with registered c type (.+)/im
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
    when table_ptn2
      matchdata = table_ptn2.match(table_description)
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
    if @base_var.length == 0            # base column for embedded structs.
      @columns.push(Column.new).last.set("base INT FROM self") 
    end
    if @container_class.length > 0     # rownum column for container structs.
      @columns.push(Column.new).last.set("rownum INT FROM self") 
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

# Models a struct_view table.
class StructView
  def initialize
    @name = ""
    @columns = Array.new
    @include_text_col = 0 # True if StructView includes column
    		      	  # of text data type. Required for
			  # generating code for
			  # PICO_QL_HANDLE_POLYMORPHISM C++ flag.
  end
  attr_accessor(:name,:columns,:include_text_col)

  # Removes the last entry in the columns table and returns the table 
  # itself. Useful when including a struct_view definition to remove the 
  # entry left empty.
  def columns_delete_last()
    @columns.delete(@columns.last)
    return @columns
  end

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
      # struct_view.
      # Second record contains the struct_view's name
      @name = matchdata[1]
      columns_str = Array.new
      if matchdata[3].match(/,/)
        columns_str = matchdata[3].split(/,/)
      else
        columns_str[0] = matchdata[3]
      end
    end
    columns_str.each { |x| @include_text_col += @columns.push(Column.new).last.set(x) }
    if $argD == "DEBUG"
      puts "StructView #{@name} registered."
      puts "StructView includes #{@include_text_col} columns of type text."
      puts "Columns follow:"
      @columns.each { |x| p x }
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
    @directives = ""
  end
  attr_accessor(:description,:tables,:directives)

# Support templating of member data
  def get_binding
    binding
  end

# Calls template to generate code in retrieve method. 
# Code makes the necessary arrangements for retrieve to happen successfully 
# (condition checks, reallocation)
  def wrap_retrieve(fw)
    file = File.open("pico_ql_erb_templates/pico_ql_wrapper_retrieve.erb").read
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
    file = File.open("pico_ql_erb_templates/pico_ql_wrapper_search.erb").read
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

# Generates the LICENSE copyright notice and directives as prescribed 
# from user in the description
def print_directives_utils(fw)
  file = File.open("pico_ql_erb_templates/pico_ql_directives_utils.erb").read
  directives = ERB.new(file, 0, '>')
  fw.puts directives.result(get_binding)
end

# Generates application-specific code to complement the SQTL library.
# There is a call to each of the above generative functions.
  def generate()
    myfile = File.open("pico_ql_search.cpp", "w") do |fw|
      print_directives_utils(fw)
      print_search_functions(fw)
      print_retrieve_functions(fw)
    end
    puts "Created/updated pico_ql_search.cpp ."
    myFile = File.open("pico_ql_makefile.append", "w") do |fw|
      file = File.open("pico_ql_erb_templates/pico_ql_makefile.erb").read
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
    token_d = token_d.select { |x| x.length > 0 }
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
	"    // Line #{(line += 1).to_s} #{$argF}" + nl
        }
      end
      puts "Directives: #{@directives}"
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
    @description = token_d.select{ |x| x.length > 0 }
    if $argD == "DEBUG"
      puts "Description after whitespace cleanup: "
      @description.each { |x| p x }
    end
    $struct_views = Array.new
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
  when /typesafe/i
    $argT = "TYPESAFE"    # Currently withdrawn option
  end
end


# The main method.
if __FILE__ == $0
  $argF = ARGV[0]
  take_cases(ARGV[1])
  take_cases(ARGV[2])
  begin
    $lined_description = File.open($argF, "r") { |fw| 
      fw.readlines.each{ |line| 
        if line.match(/\/\/(.+)/)
          line.gsub!(/\/\/(.+)/, "")
        end
      }
    }
  rescue Exception => e
    puts e.message
    exit(1)
  end
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
