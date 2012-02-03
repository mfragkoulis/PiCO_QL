class Column
  def initialize
    @name = ""
    @data_type = ""
    @related_to = ""
    @access_path = ""
    @type = ""
  end
  attr_accessor(:name,:data_type,:related_to,:access_path,:type)


# Performs case analysis with respect to the column_data type (class value)
# and fills the variables with necessary values.
  def bind_datatypes(sqlite3_type, column_cast, sqlite3_parameters, 
                     column_cast_back, access_path)
    if @type == "foreign_key" || @type == "primary_key"
      sqlite3_type.replace("text")
      sqlite3_parameters.replace(", -1, SQLITE_STATIC")
      return "fill in"
    end
    if @data_type == "int" || 
        @data_type == "integer" ||
        @data_type == "tinyint" ||
        @data_type == "smallint"||
        @data_type == "mediumint" ||
        @data_type == "bigint" ||
        @data_type == "unsigned bigint" ||
        @data_type == "int2" ||
        @data_type == "bool" || 
        @data_type == "boolean" ||
        @data_type == "int8" || 
        @data_type == "numeric"
      sqlite3_type.replace("int")
    elsif @data_type == "blob"
      sqlite3_type.replace("blob")
      column_cast.replace("(const void *)")
      sqlite3_parameters.replace(", -1, SQLITE_STATIC")
    elsif @data_type == "float" ||
        @data_type == "double"  ||
        @data_type.match(/decimal/i) ||
        @data_type == "double precision" ||
        @data_type == "real"
      sqlite3_type.replace("double")
    elsif @data_type == "text" || 
        @data_type == "date" ||
        @data_type == "datetime" ||
        @data_type.match(/character/i) ||
        @data_type.match(/varchar/i) ||
        @data_type.match(/nvarchar/i) ||
        @data_type.match(/varying character/i) ||
        @data_type.match(/native character/i) ||
        @data_type == "clob" ||
        @data_type.match(/nchar/i)
      sqlite3_type.replace("text")
      column_cast.replace("(const unsigned char *)")
      column_cast_back.replace(".c_str()")
      sqlite3_parameters.replace(", -1, SQLITE_STATIC")
    elsif @data_type == "string"
      sqlite3_type.replace("text")
      column_cast.replace("(const unsigned char *)")
      column_cast_back.replace(".c_str()")
      sqlite3_parameters.replace(", -1, SQLITE_STATIC")
    end
    access_path.replace(@access_path)
    return "compare"
  end

# Validates a column data type
# The following data types are the ones accepted by sqlite.
  def verify_data_type()
    dt = @data_type.downcase
    if dt == "int" ||
        dt == "integer" ||
        dt == "tinyint" ||
        dt == "smallint" ||
        dt == "mediumint" ||
        dt == "bigint" ||
        dt == "unsigned bigint" ||
        dt == "int2" ||
        dt == "int8" ||
        dt == "blob" ||
        dt == "float" ||
        dt == "double"  ||
        dt == "double precision" ||
        dt == "real" ||
        dt == "numeric" ||
        dt == "date" ||
        dt == "bool" ||
        dt == "boolean" ||
        dt == "datetime" ||
        dt.match(/\idecimal/) ||
        dt == "text" ||
        dt == "clob" ||
        dt.match(/character/i) ||
        dt.match(/varchar/i) ||
        dt.match(/nvarchar/i) ||
        dt.match(/varying character/i) ||
        dt.match(/native character/i) ||
        dt.match(/nchar/i) ||
        dt.match(/char/i)
      return dt
    elsif dt == "string"
      return "text"
    else
      raise TypeError.new("no such data type " + data_type.upcase + "\n")
    end
  end


# @type +: cast, backpointer after foreign_key
# Matches each column description against a pattern and extracts 
# column traits.
  def set(column)
    column.lstrip!
    column.rstrip!
    puts column
    if column.match(/\n/)
      column.gsub!(/\n/, "")
    end
    column_ptn1 = /\#(\w+)/i
    column_ptn2 = /(\w+) (\w+) from table (\w+) with base(\s*)=(\s*)(\w+)/i
    column_ptn3 = /(\w+) (\w+) from (\w+)/i
    case column
    when column_ptn1
      matchdata = column_ptn1.match(column)
      $elements.each { |el| if el.name == matchdata[1] : $elements.last.columns = $elements.last.columns_delete_last() | el.columns end }
      return
    when column_ptn2
      matchdata = column_ptn2.match(column)
      @name = matchdata[1]
      @data_type = matchdata[2]
      @related_to = matchdata[3]
      @access_path = matchdata[6]    
    when column_ptn3
      matchdata = column_ptn3.match(column)
      @name = matchdata[1]
      @data_type = matchdata[2]
      @access_path = matchdata[3]
    end
    verify_data_type()
    if @access_path.match(/self/)
      @access_path.gsub!(/self/,"")
    end
    puts "Column name is: " + @name
    puts "Column data type is: " + @data_type
    puts "Column related to: " + @related_to
    puts "Column access path is: " + @access_path
    puts "Column type is: " + @type
  end


# Checks if the col is a foreign key to a given table 
# and returns the access path if that is the case
  def search_fk_col(table_name_id)
    if @type == "foreign_key" && @related_to == table_name_id
      return true, @access_path
    end
    return false
  end

# Returns valid column format for use in VT create queries.
  def print_col_info()
      return @name + " " + @data_type
  end

end


class View
  def initialize
    @name = ""
    @db = ""
    @virtual_tables = Array.new
    @where_clauses = Array.new
  end

  def match_view(view_description)
    view_ptn = /^create view (\w+)\.(\w+) as select \* from (.+) where(\s*) (.+)/im
    puts view_description
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
    where.match(/ and /i) ? @where_clauses = where.split(/ and /) : @where_clauses = where
    puts "View name is: " + @name
    puts "View lives in database named: " + @db
    @virtual_tables.each { |vt| puts "View of virtual tables: " + vt }
    @where_clauses.each { |wh| puts "View of where clauses: " + wh }
  end
end


class VirtualTable
  def initialize
    @name = ""
    @base_var = ""
    @element
    @db = ""
    @signature = ""
    @stl_class = ""
    @type = ""
    @pointer = ""
    @object_class = ""
    @template_args = ""
    @children = Array.new
    @@stl_single_classes = ["list" , "deque" , "vector" , "set" , 
                            "multiset"]
    @@stl_double_classes = ["map" , "multimap"]
    @@stl_sequence_classes = ["list", "vector", "deque"]
    @@stl_associative_classes = ["set" , "multiset" , "map" , "multimap"]
  end

  attr_accessor(:name,:base_var,:element,:db,:signature,:stl_class,:type,:pointer,:object_class,:template_args,:children)


# Generates code to search each VT struct.
# Each search case matches a specific column of the VT.
  def search_columns(fw)
    col = 0
    fw.puts "#{$s}switch( iCol ){"
    col_array = @element.columns
    col_array.each_index { |col|
      fw.puts "#{$s}case #{col}:"
# if collection/map...
      if @stl_class.length > 0
        fw.puts @s + "    iter = any_dstr->begin();"
        fw.puts @s + "    for(int i=0; i<size;i++){"
      end
      sqlite3_type = ""
      column_cast = ""
      sqlite3_parameters = ""
      column_cast_back = ""
      access_path = ""
      op = col_array[col].bind_datatypes(sqlite3_type, column_cast, 
                                   sqlite3_parameters, column_cast_back, 
                                   access_path)
      if @signature.length > 0
        access_path.length == 0 ? iden = "(*iter)" : iden = "(*iter)."
      else
        access_path.length == 0 ? iden = "any_dstr" : iden = "any_dstr->"
      end
      #      puts "sqlite3_type: " + sqlite3_type
      #      puts "column_cast: " + column_cast
      #      puts "sqlite3_parameters: " + sqlite3_parameters
      #      puts "column_cast_back: " + column_cast_back
      #      puts "access_path: " + access_path
      if @stl_class.length > 0
        if op == "compare"
          fw.puts @s + @s + "if (compare(" + column_cast + iden + 
            access_path + column_cast_back + ", op, sqlite3_value_" + 
            sqlite3_type + "(val)) )"
          fw.puts @s + @s + "    temp_res[count++] = i;"
        elsif op == "fill in"
          fw.puts @s + @s + "temp_res[count++] = i;"
        end
        fw.puts @s + @s + "iter++;"
        fw.puts @s + "    }"
      else
        if op == "compare"
          fw.puts @s + "    if (compare(" + column_cast + iden + 
            access_path + column_cast_back + ", op, sqlite3_value_" + 
            sqlite3_type + "(val)) )"
          fw.puts @s + @s + "temp_res[count++] = i;"        
        elsif op == "fill in"
          fw.puts @s + "    temp_res[count++] = i;"
        end
      end
      fw.puts @s + "    assert(count <= stcsr->max_size);"
      fw.puts @s + "    break;"
    }
    fw.puts @s + "}"
  end



# Generates code in search method. Code makes the necessary arrangements 
# for search to happen successfully (condition checks, reallocation)
  def setup_search(fw)

# optimisation: refrain from calling get_datastructure_size in each call.
# However for real time apps this is necessary.
    error_case = <<-EC
    if ( stl->zErr ) {
        sqlite3_free(stl->zErr);
        return SQLITE_MISUSE;
    }
EC

    stl_fill_resultset = <<-SFR
        for (int j=0; j<size; j++){
            stcsr->resultSet[j] = j;
            stcsr->size++;
	}
        assert(stcsr->size <= stcsr->max_size);
        assert(&stcsr->resultSet[stcsr->size] <= &stcsr->resultSet[stcsr->max_size]);
SFR

    typesafe_block = <<-TB
            vtd_iter = vt_directory.find(stl->zName);
            if ( (vtd_iter == vt_directory.end()) || (vtd_iter->second == 0) ) {
                printf("Invalid cast to %s\\n", stl->zName);
                return SQLITE_MISUSE;
            }
            vt_directory[stl->zName] = 0;
TB

    resultset_alloc = <<-RAL
        int *temp_res;
	temp_res = (int *)sqlite3_malloc(sizeof(int)  * stcsr->max_size);
        if ( !temp_res ){
            printf("Error in allocating memory\\n");
            return SQLITE_NOMEM;
        }
RAL

    fw.puts "    stlTable *stl = (stlTable *)cur->pVtab;"
    fw.puts "    stlTableCursor *stcsr = (stlTableCursor *)cur;"
    /\*/.match(@pointer) == nil ? retype = "*" : retype = ""
    if @base_var.length > 0
      fw.puts "    #{@signature}#{retype} any_dstr = (#{@signature}#{retype})stcsr->source;"
      if @stl_class.length > 0
        fw.puts "    #{@signature}:: iterator iter;"
#      else
#        raise "ERROR: not recorded structure type: stl or object"
      end
    else
      fw.puts "    #{@signature}#{retype} any_dstr;"
      if @stl_class.length > 0
        fw.puts "    #{@signature}:: iterator iter;"
#      else
#        raise "ERROR: not recorded structure type: stl or object"
      end
    end
    fw.puts "    int op, iCol, count = 0, i = 0, re = 0;"
    if @base_var.length == 0 : fw.puts error_case end
    fw.puts "    if ( val==NULL ){"
    if @base_var.length > 0
      if @stl_class.length > 0
        fw.puts "#{$s}int size = get_datastructure_size(cur);"
        fw.puts stl_fill_resultset
      else
        fw.puts "#{$s}stcsr->size++;"
      end
    else
      fw.puts "#{$s}printf(\"Seaching VT #{@name} with no BASE constraint...makes no sense.\\n\");"
      fw.puts "#{$s}return SQLITE_MISUSE;"
    end
    fw.puts "    } else {"
    if @base_var.length == 0
      fw.puts "#{$s}check_alloc((const char *)constr, op, iCol);"
      fw.puts "#{$s}if ( equals_base(stl->azColumn[iCol]) ) {"
      if $arg == "typesafe" : fw.puts typesafe_block end
      fw.puts "#{$s}    stcsr->source = (void *)sqlite3_value_int64(val);"
      fw.puts "#{$s}    any_dstr = (#{@signature}#{retype})stcsr->source;"
      if @stl_class.length > 0
        fw.puts "#{$s}    int size = get_datastructure_size(cur);"
        fw.puts "#{$s}    realloc_resultset(cur);"
      end
      fw.puts "#{$s}}"
    end
    fw.puts resultset_alloc
  end


# validate the signature of an stl structure and extract signature traits.
# Also for objects, extract class name.
  def verify_signature()

    class_sign = <<-CS
STL class signature not properly given:
template error in #{@signature} \n\n NOW EXITING. \n
CS

    case @signature
    when /(\w+)<(.+)>(\**)/
      matchdata = /(\w+)<(.+)>(\**)/.match(@signature)
      @stl_class = matchdata[1]
      @type = matchdata[2]
      @pointer = matchdata[3]
#      if @stl_class.match(/map/i)
#        @type = "pair<#{@type}>"
#      end
      if @@stl_single_classes.include?(@stl_class)
        @template_args = "single"
      elsif @@stl_double_classes.include?(@stl_class)
        @template_args = "double"
      else
        raise TypeError.new("no such container class: " + @stl_class +
                            "\n\n NOW EXITING. \n")
      end
      if @@stl_sequence_classes.include?(@stl_class)
        @container_type="sequence"
      elsif @@stl_associative_classes.include?(@stl_class)
        @container_type="associative"
      end
      if (@template_args == "single" && /(.+),(.+)/.match(@type)) ||
          (@template_args == "double" && !(/(.+),(.+)/.match(@type)))
        raise ArgumentError.new(class_sign)
      end
      puts "Table STL class name is: " + @stl_class
      puts "Table no of template args is: " + @template_args
      puts "Table container type is: " + @container_type
      puts "Table record is of type: " + @type
      puts "Table type is of type pointer: " + @pointer
    when /(\w+)\*|(\w+)/
      matchdata = /(\w+)(\**)/.match(@signature)
      @object_class = @signature
      @type = @signature
      @pointer = matchdata[2]
      puts "Table object class name : " + @object_class
      puts "Table record is of type: " + @type
      puts "Table type is of type pointer: " + @pointer
    when /(.+)/
      raise "Template instantiation faulty.\n"
    end
  end


  def match_table(table_description)
    table_ptn1 = /^create table (\w+)\.(\w+) with base(\s*)=(\s*)(\w+) as select \* from (.+)/im
    table_ptn2 = /^create table (\w+)\.(\w+) as select \* from (.+)/im
    puts table_description
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
      @signature = matchdata[3].gsub(/\s/,"")
    end
    verify_signature()
    @type.match(/\*/) ? element_type = @type.chomp('*') : element_type = @type 
    $elements.each { |el| if el.name == @name : @element = el end }
    if @element == nil
      $elements.each { |el| if el.name == element_type : @element = el end }
    end
    puts "Table name is: " + @name
    puts "Table lives in database named: " + @db
    puts "Table base variable name is: " + @base_var
    puts "Table signature name is: " + @signature
    puts "Table follows element: " + @element.name
  end

end

class Element
  def initialize
    @name = ""
    @columns = Array.new
  end
  attr_accessor(:name,:columns)


  def spacing(index)
    spacing = 0
    blanks = @s
    while spacing < index
      blanks += "    "
      spacing += 1
    end
    return blanks
  end


# Generates code to retrieve each VT struct.
# Each retrieve case matches a specific column of the VT.
  def retrieve_columns(fw)
    col = 0
    fw.puts "    switch( n ){"
    while col < @columns.length
      fw.puts "    case " + col.to_s + ":"
      sqlite3_type = ""
      column_cast = ""
      sqlite3_parameters = ""
      column_cast_back = ""
      access_path = ""
      op = @columns[col].bind_datatypes(sqlite3_type, column_cast, 
                                   sqlite3_parameters, column_cast_back, 
                                   access_path)
      if op == "fill in"
        iden = "\"N/A\""
      elsif op == "compare"
        if @signature.length > 0
          if access_path.length == 0
            iden = "(*iter)"
          else
            iden = "(*iter)."
          end
        else
          if access_path.length == 0
            iden = "any_dstr"
          else
            iden = "any_dstr->"
          end
        end
      end
# Patch. Too little exception in bind_datatypes to spoil code reuse
      if column_cast_back == ".c_str()"
        column_cast = "(const char *)"
      end
      fw.puts @s + "sqlite3_result_" + sqlite3_type + "(con, " + 
        column_cast + iden + access_path + column_cast_back + 
        sqlite3_parameters + ");"
      fw.puts @s + "break;"
      col += 1
    end
  end

# Generates code in retrieve method. Code makes the necessary arrangements 
# for retrieve to happen successfully (condition checks, reallocation)
  def setup_retrieve(fw, ds_array)

    #HereDoc1

        auto_gen5 = <<-AG5
    int index = stcsr->current;
    iter = any_dstr->begin();
    for(int i=0; i<stcsr->resultSet[index]; i++){
        iter++;
    }
AG5

    fw.puts "    stlTable *stl = (stlTable *)cur->pVtab;"
    fw.puts "    dsData *d = (dsData *)stl->data;"
    if @signature.length > 0
      fw.puts "    stlTableCursor *stcsr = (stlTableCursor *)cur;"
      fw.puts "    " + @signature +
        " *any_dstr = (" + @signature + " *)d->attr->memory;"
      fw.puts "    " + @signature + ":: iterator iter;"
      fw.puts auto_gen5
    elsif @object_class.length > 0
      fw.puts "    " + @object_class +
        " *any_dstr = (" + @object_class + " *)d->attr->memory;"
    else
      puts "ERROR: not recorded structure type: stl or object"
      exit(1)
    end
  end



  def columns_delete_last()
    @columns.delete(@columns.last)
    return @columns
  end


  def match_element(element_description)
    puts element_description
    pattern = /^create element table (\w+)(\s*)\((.+)\)/im
    matchdata = pattern.match(element_description)
    if matchdata
      # First record of table_data contains the whole description of the element
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
    @columns.each { |x| p x }
  end

end

class InputDescription
  def initialize(description)
    # original description tokenised in an Array
    @description = description
    # array with entries the identity of each virtual table
    @tables = Array.new
    @directives = ""
#    @tokenised_dir = Array.new
    @s = "        "
  end

# Processes and generates the directives 
# (helper functions, user-defined classes) for the makefile
  def print_directives(fw, op)
    td = 0
    while td < @tokenised_dir.length
      current = @tokenised_dir[td].chomp(".h\"")
      current.gsub!(/\#include \"/,"")
      if op == 1
        fw.print current + ".o "
      elsif op == 2
        fw.print current + ".o: " + current + 
          ".cpp " + current + ".h \n" + 
          "\tg++ -W -g -c " + current + ".cpp \n\n"
      elsif op == 3
        fw.print current + ".h "
      end
      td += 1
    end
  end

# Distinguishes between directives for helper classes 
# and directives for user defined classes (empty line between the two)
  def tokenise_directive()
    if @directives.match(/\n\n/)
      directives = @directives.split(/\n\n/)
      if directives[1].match(/\n/)
        @tokenised_dir = directives[1].split(/\n/)
      else
        @tokenised_dir[0] = directives[1]
      end
    end
  end

# Generates the application-specific retrieve method for each VT struct.
  def print_retrieve_functions(fw)
    w = 0
    while w < @ds_chars.length
      curr_ds = @ds_chars[w]
      fw.puts "int " + curr_ds.name +
        "_retrieve(sqlite3_vtab_cursor *cur, int n, sqlite3_context *con){"
      curr_ds.setup_retrieve(fw, @ds_chars)
      curr_ds.retrieve_columns(fw)
      fw.puts "    }"
      fw.puts "    return SQLITE_OK;"
      fw.puts "}\n\n\n"
      w += 1
    end
    fw.puts "int retrieve(sqlite3_vtab_cursor *cur, int n, sqlite3_context *con){"
    fw.puts "    stlTable *stl = (stlTable *)cur->pVtab;"
    w = 0
    while w < @ds_chars.length
      curr_ds = @ds_chars[w]
      fw.puts "    if( !strcmp(stl->zName, \"" + curr_ds.name + "\") )"
      fw.puts "        return " + curr_ds.name +
        "_retrieve(cur, n, con);"
      w += 1
    end
    fw.puts "}"
  end

# Generates the application-specific search method for each VT struct.
  def print_search_functions(fw)

    cls_search = <<-CLS
        if ( (re = compare_res(count, stcsr, temp_res)) != 0 )
            return re;
        sqlite3_free(temp_res);
    }
    return SQLITE_OK;
}


CLS

    @tables.each { |vt|
      fw.puts "int #{vt.name}_search(sqlite3_vtab_cursor *cur, char *constr, sqlite3_value *val){"
      vt.setup_search(fw)
#      vt.search_columns(fw)
      fw.puts cls_search
    }
    fw.puts "int search(sqlite3_vtab_cursor* cur, char *constr, sqlite3_value *val){"
    fw.puts "    stlTable *stl = (stlTable *)cur->pVtab;"
    @tables.each { |vt|
      fw.puts "    if( !strcmp(stl->zName, \"#{vt.name}\") )"
      fw.puts "#{$s}return #{vt.name}_search(cur, constr, val);"
    }
    fw.puts "}"
  end


  def print_get_size(fw)
    fw.puts "int get_datastructure_size(sqlite3_vtab_cursor *cur){"
    fw.puts "    stlTableCursor *stc = (stlTableCursor *)cur;"
    fw.puts "    stlTable *stl = (stlTable *)cur->pVtab;"
    @tables.each_index { |vt|
      if @tables[vt].stl_class.length > 0
        if vt == 0
          fw.puts "    if ( !strcmp(stl->zName, \"#{@tables[vt].name}\") ) {"
        else
          fw.puts "    } else if ( !strcmp(stl->zName, \"#{@tables[vt].name}\") ) {"
        end
        /\*/.match(@tables[vt].pointer) == nil ? retype = "*" : retype = "" 
        fw.puts "#{@s}#{@tables[vt].signature}#{retype} any_dstr = (#{@tables[vt].signature}#{retype})stc->source;"
        fw.puts "#{@s}return (int)any_dstr->size();"
      end
    }
    fw.puts "    }"
    fw.puts "    return 1;"
    fw.puts "}"
  end


  def print_register_vt(fw)
    
    els_case = <<-ELS
    } else {
        stl->data = NULL;
        stl->embedded = 1;
    }
    vt_directory[stl->zName] = 0;
}

ELS
    
    fw.puts "void register_vt(stlTable *stl) {"
    @tables.each_index { |vt| 
      if @tables[vt].base_var.length > 0
        if vt == 0
          fw.puts "    if ( !strcmp(stl->zName, \"#{@tables[vt].name}\") ) {"
        else
          fw.puts "    } else if ( !strcmp(stl->zName, \"#{@tables[vt].name}\") ) {"
        end
        /\*/.match(@tables[vt].pointer) == nil ? retype = "&" : retype = "" 
        fw.puts "#{@s}stl->data = (void *)#{retype}#{@tables[vt].base_var};"
        fw.puts "#{@s}stl->embedded = 0;"
      end
    }
    fw.puts els_case
  end


  def print_thread(fw)

    auto_gen1_1 = <<-AG11
void * thread_sqlite(void *data){
    const char **queries, **table_names;
    queries = (const char **)sqlite3_malloc(sizeof(char *) *
                   #{@tables.length.to_s});
    table_names = (const char **)sqlite3_malloc(sizeof(char *) *
                   #{@tables.length.to_s});
    int failure = 0;
AG11


   #HereDoc2
# maybe adjust so that create queries are grouped by database.
      auto_gen2 = <<-AG2
    failure = register_table( "#{@tables[0].db}" ,  #{@tables.length.to_s}, queries, table_names, data);
    printf(\"Thread sqlite returning..\\n\");
    sqlite3_free(queries);
    sqlite3_free(table_names);
    return (void *)failure;
}


int call_sqtl() {
    pthread_t sqlite_thread;
    int re_sqlite = pthread_create(&sqlite_thread, NULL, thread_sqlite, NULL);
    signal(SIGPIPE,SIG_IGN);
    pthread_join(sqlite_thread, NULL);
    return re_sqlite;
}

AG2

    fw.puts auto_gen1_1
# <db>.<table> always valid?
# <db>.<table> does not work for some reason. test.
    @tables.each_index { |vt| 
      query =  "CREATE VIRTUAL TABLE #{@tables[vt].db}.#{@tables[vt].name} USING stl(" 
      @tables[vt].element.columns.each { |c| query += "#{c.name} #{c.data_type}," }
      query = query.chomp(",") + ")"
      fw.puts "    queries[#{vt}] = \"#{query}\";"
      fw.puts "    table_names[#{vt}] = \"#{@tables[vt].name}\";"
    }
    fw.puts auto_gen2
  end


  def print_extern_variables(fw)
    @tables.each { |vt| if vt.base_var.length > 0 : fw.puts "extern #{vt.signature} #{vt.base_var};" end }
  end



# Generates application-specific code to complement the SQTL library.
  def generate()

   #HereDoc1
      auto_gen1 = <<-AG1

using namespace std;


//#define DEBUGGING                                                             


struct name_cmp {
    bool operator()(const char *a, const char *b) {
        return strcmp(a, b) < 0;
    }
};

static map<const char *, int, name_cmp> vt_directory;
static map<const char *, int>::iterator vtd_iter;

AG1


    #HereDoc3
    directives = <<-dir
#include <assert.h>
#include <stdio.h>
#include <string>
#include "stl_search.h"
#include "user_functions.h"
#include "workers.h"
#{@directives}

using namespace std;


/*
#define DEBUGGING
*/

dir


    #HereDoc5
  makefile_part = <<-mkf

stl_search.o: stl_search.cpp stl_search.h user_functions.h workers.h
        g++ -W -g -c stl_search.cpp

user_functions.o: user_functions.c user_functions.h stl_test.h
        gcc -W -g -c user_functions.c

workers.o: workers.cpp workers.h stl_search.h
        g++ -W -g -c workers.cpp

stl_test.o: stl_test.c stl_test.h
        gcc -W -g -c stl_test.c

stl_to_sql.o: stl_to_sql.c stl_to_sql.h stl_search.h
        gcc -g -c stl_to_sql.c
mkf

    myfile = File.open("stl_search_gen.cpp", "w") do |fw|
      fw.puts "\#include \"stl_search.h\""
      fw.puts "\#include <string>"
      fw.puts "\#include <assert.h>"
      fw.puts "\#include <stdio.h>"
      fw.puts @directives
      fw.puts
      fw.puts auto_gen1
      print_extern_variables(fw)
      fw.puts "\n\n"
      print_thread(fw)
      fw.puts "\n\n"
      print_register_vt(fw)
      fw.puts "\n\n"
      print_get_size(fw)
      fw.puts "\n\n"
      print_search_functions(fw)
      fw.puts "\n\n"
#      print_retrieve_functions(fw)
    end
    myFile = File.open("makefile.append", "w") do |fw|
      fw.print "executable: main.o stl_search.o stl_to_sql.o user_functions.o workers.o stl_test.o"
#      tokenise_directive()
#      print_directives(fw, 1)
      fw.print "\n    g++ -lswill -lsqlite3 -W -g main.o stl_search.o stl_to_sql.o user_functions.o workers.o stl_test.o"
#      print_directives(fw, 1)
      fw.puts "-o test\n\n"
      fw.print "main.o: main.cpp stl_search.h "
#      print_directives(fw, 3)
      fw.puts "\n\tg++ -W -g -c main.cpp"
      fw.puts makefile_part
      fw.puts
#      print_directives(fw, 2)

    end
  end


# User description first comes here. Description is cleaned from 
# surplus spaces and is split to extract directives to external 
# application and library files. 
# Each VT description is separated, matched against specific patterns
# and all elements are recorded including column specifications.
  def register_datastructures()
    puts "description before whitespace cleanup: "
    @description.each { |x| p x }
    token_d = @description
    token_d = token_d.select { |x| x.length > 0 }
    @directives = token_d[0]
    token_d.delete_at(0)
    puts "Directives: " + @directives
    x = 0
    while x < token_d.length
      token_d[x].lstrip!
      token_d[x].rstrip!
      if /\n|\t|\r|\f/.match(token_d[x]) : token_d[x].gsub!(/\n|\t|\r|\f/, "") end
      token_d[x].squeeze!(" ")
      if / ,|, /.match(token_d[x]) : token_d[x].gsub!(/ ,|, /, ",") end
      if / \(/.match(token_d[x]) : token_d[x].gsub!(/ \(/, "(") end
      if /\( /.match(token_d[x]) : token_d[x].gsub!(/\( /, "(") end
      if /\) /.match(token_d[x]) : token_d[x].gsub!(/\) /, ")") end
      if / \)/.match(token_d[x]) : token_d[x].gsub!(/ \)/, ")") end
      x += 1
    end
    @description = token_d
    puts "description after whitespace cleanup: "
    @description.each { |x| p x }
    $elements = Array.new
    views = Array.new
    w = 0
    while w < @description.length
      puts "\nDESCRIPTION No: " + w.to_s + "\n"
      @description[w].lstrip!
      @description[w].rstrip!
      des = @description[w]
      case des
      when /^create element table/i
        $elements.push(Element.new).last.match_element(des)
      when /^create table/i
        @tables.push(VirtualTable.new).last.match_table(des)
      when /^create view/i
        views.push(View.new).last.match_view(des)
      end
      w += 1
    end
  end
end

if __FILE__ == $0
  if !File.file?("input.txt")
    raise "File 'input.txt' does not exist.\n"
  end
  description = File.open("input.txt", "r") { |fw| fw.read }
  if description.match(/;/)
    token_description = description.split(/;/)
  else
    raise "Invalid description..delimeter ';' not used."
  end
  $arg = "typesafe"
  $s = "        "
  ip = InputDescription.new(token_description)
  ip.register_datastructures()
  ip.generate()
end
