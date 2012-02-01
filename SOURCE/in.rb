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
      puts $err_state
      raise TypeError.new("no such data type " + data_type.upcase + "\n")
    end
  end


# @type +: cast, backpointer after foreign_key
# Matches each column description against a pattern and extracts 
# column traits.
  def set(column)
    puts column
    column.lstrip!
    column.rstrip!
    if column.match(/\n/)
      column.gsub!(/\n/, "")
    end
    column_ptn1 = /\#(\w+)/i
    column_ptn2 = /(\w+) (\w+) from table (\w+) with base(\s*)=(\s*)(\w+)/i
    column_ptn3 = /(\w+) (\w+) from (\w+)/i
    case column
    when column_ptn1
      matchdata = column_ptn1.match(column)
      $elements.each { |el| if el.name == matchdata[1] : $elements.last.columns = $elements.last.columns | el.columns end }
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
    
=begin
    matchdata = pattern.match(column)
    column_data = Array.new
    l = 0
    cd = 0
    if matchdata
      while l < matchdata.length
        if matchdata[l] != nil
          column_data[cd] = matchdata[l]
          # strip whitespace from front and back
          column_data[cd].lstrip!
          column_data[cd].rstrip!
          if column_data[cd].length == 0
            column_data.delete_at(cd)
            cd -= 1
          end        
          puts "column_data[" + cd.to_s + "] = " + column_data[cd]
          cd += 1
        end
        l += 1
      end
      @name = column_data[1]
      @data_type = verify_data_type(column_data[2])
      if column_data.length == 5
        @related_to = column_data[3]
        @access_path = column_data[4]
      elsif column_data.length == 4
        if @type == "foreign_key"
          @related_to = column_data[3]
        else
# if @type not key(!&) but data column then..
# note: no access path on primary key.
          @access_path = column_data[3]
          if @type == "primitive"
            @access_path.gsub!(/self\$|self/,"")
          end
          if @access_path.match(/\$/)
            puts "$"
            @access_path.gsub!(/\$/,".")
          end
        end
      elsif column_data.length == 3 && @type == "primary_key"
      else
        puts "Invalid format.\n"  
      end
      puts "Column name is: " + @name
      puts "Column data type is: " + @data_type
      puts "Column related to: " + @related_to
      puts "Column access path is: " + @access_path
      puts "Column type is: " + @type
    else
      puts "Invalid format. One or more of identifiers 'FROM''(space)' missing\n"
      exit(1)
    end
=end
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
#    puts "type is " + @type
    if @type == "primary_key"
      return @name + " INTEGER PRIMARY KEY"
    elsif @type == "foreign_key"
      return @name + " REFERENCES " + @related_to 
    elsif @type == "standard" || @type == "primitive"
      return @name + " " + @data_type
    else
      puts "Unrecognised state\n"
      exit(1)
    end
  end

end


class Child
  def initialize
    @name = ""
    @access = ""
  end
  attr_accessor(:name, :access)

end

class View
  def initialize
    @name = ""
    @db = ""
    @virtual_tables = Array.new
    @where_clauses = Array.new
  end

  def match_view(view_description)
    view_description.lstrip!
    view_description.rstrip!
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
    where.match(/ /) ? @where_clauses = where.split(/ /) : @where_clauses = where
    puts "View name is: " + @name
    puts "View lives in database named: " + @db
    @virtual_tables.each { |vt| puts "View of virtual tables: " + vt }
    @where_clauses.each { |wh| puts "View of virtual tables: " + wh }
  end
end


class VirtualTable
  def initialize
    @name = ""
    @base_var = ""
    @element = ""
    @db = ""
    @signature = ""
    @stl_class = ""
    @type = ""
    @object_class = ""
    @template_args = ""
    @children = Array.new
  end

  attr_accessor(:name,:base_var,:element,:db,:signature,:stl_class,:type,:object_class,:template_args,:children)


# validate the signature of an stl structure and extract signature traits.
# Also for objects, extract class name.
  def verify_signature()

    class_sign = <<-CS
STL class signature not properly given:
template error in #{@signature} \n\n NOW EXITING. \n
CS
    sign = @signature
    if sign.include?("<") && sign.include?(">")
      container_split = sign.split(/</)
      @stl_class = container_split[0]
      @type = container_split[1].chomp!(">")
#      if @stl_class.match(/map/i)
#        @type = "pair<#{@type}>"
#      end
      if @stl_class == "list" || @stl_class == "deque"  ||
          @stl_class == "vector" || @stl_class == "slist" ||
          @stl_class == "set" || @stl_class == "multiset" ||
          @stl_class == "hash_set" || @stl_class == "hash_multiset" ||
          @stl_class == "bitset"
        @template_args = "single"
      elsif @stl_class == "map" ||
          @stl_class == "multimap" || @stl_class == "hash_map" ||
          @stl_class == "hash_multimap"
        @template_args = "double"
      else
        puts $err_state
        raise TypeError.new("no such container class: " + @stl_class +
                            "\n\n NOW EXITING. \n")
      end
      if (@template_args== "single" && container_split[1].include?(",")) ||
          (@template_args == "double" && 
           (!container_split[1].include?(",") &&
            # After splitting with '<', making sure template 
            # instantiation is not empty
            (container_split[1].chomp!(">").length != 0)))
        # double:if not normal case and not nested case raise
        raise ArgumentError.new(class_sign)
      end
      if @stl_class=="list" || @stl_class=="deque"  ||
          @stl_class=="vector" || @stl_class=="slist" ||
          @stl_class=="bitset"
        @container_type="sequence"
      elsif @stl_class=="map" ||
          @stl_class=="multimap" || @stl_class=="hash_map" ||
          @stl_class=="hash_multimap" ||
          @stl_class=="set" || @stl_class=="multiset" ||
          @stl_class=="hash_set" || @stl_class=="hash_multiset"
        @container_type="associative"
      elsif @stl_class=="bitset"
        @container_type="bitset"
      end
      puts "Table STL class name is: " + @stl_class
      puts "Table no of template args is: " + @template_args
      puts "Table container type is: " + @container_type
      puts "Table record is of type: " + @type
    else
      if sign.match(/(<*) | (>*)/)
        puts "Template instantiation identifier '<' or '>' missing\n"
        exit(1)
      end
      @object_class = sign
      @type = @object_class
      puts "Table object class name : " + @object_class
      puts "Table record is of type: " + @type
    end
  end


  def match_table(table_description)
    table_description.lstrip!
    table_description.rstrip!
    table_ptn1 = /^create table (\w+)\.(\w+) with base(\s*)=(\s*)(\w+) as select \* from (.+)/im
    table_ptn2 = /^create table (\w+)\.(\w+) as select \* from (.+)/im
    puts table_description
    case table_description
    when table_ptn1
      puts "1"
      matchdata = table_ptn1.match(table_description)
      @name = matchdata[2]
      @db = matchdata[1]
      @base_var = matchdata[5]
      @signature = matchdata[6]
    when table_ptn2
      puts "2"
      matchdata = table_ptn2.match(table_description)
      @name = matchdata[2]
      @db = matchdata[1]
      @signature = matchdata[3].gsub(/\s/,"")
    end
    verify_signature()
    @type.match(/\*/) ? element_type = @type.chomp!('*') : element_type = @type 
    $elements.each { |el| if el.name == @name : @element = el end }
    if @element == nil
      $elements.each { |el| if el.name == element_type : @element = el end }
    end
    puts "Table name is: " + @name
    puts "Table lives in database named: " + @db
    puts "Table base variable name is: " + @base_var
    puts "Table signature name is: " + @signature
    puts "Table follows element: " + @element.name
=begin
#      process_parents(parents)
#      process_columns(matchdata[10], columns)
      return
    else
      pattern = /^ (\w+)\.(\w+)(\s*):(\s*)(.+)(\s*)\{(\s*)(.+)\}/im
      matchdata = pattern.match(vt_description)
    end
    if matchdata
      # First record of table_data contains the whole description of the virtual table
      # Second record contains the database name in which the virtual table will be created
      @db = matchdata[1]
      # Third record contains the virtual table name
      @name = matchdata[2]
      # Sixth record contains the signature, call to gsub to strip any whitespaces
      @parents = nil
      table_signature = matchdata[5].gsub(/\s/,"")
      verify_signature(table_signature)
      process_columns(matchdata[8], columns)
    else
      puts "Invalid format. One or more of identifiers ':', 'TABLE', 'FROM''(space)' missing\n"
      exit(1)
    end
#=begin
       mt = 0
       while mt < matchdata.length
         puts "matchdata[" + td.to_s + "] = " + matchdata[td]
         mt += 1
       end
=end
  end

end

class Element
  def initialize
    @name = ""
    # content: map<string,Truck*>
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


  def print_paths(ds_chars, paths, fw, init)
    parent_type = ""
    ds_length = ds_chars.length
    if @type.match(/\*/) == nil
      type  = @type + "*"
      # VT Truck results from pointer Truck * in stl struct.
      # This is standard.
    else 
      type = @type
    end
    if (init == 0)
      fw.puts "    if ( !strcmp(table_name, \"#{@name}\") ) {"
    else
      fw.puts "    else if ( !strcmp(table_name, \"#{@name}\") ) {"
    end
    if @nonNative == nil || @nonNative == 0
      fw.puts @s + "set<#{type}> *resultset = (set<#{type}> *)stc->resultSet;"
    elsif @nonNative == 1
      parent = @parents[0]
      w = 0
      while w < ds_length
        if ds_chars[w].name == parent
          parent_type = ds_chars[w].type
          parent_signature = ds_chars[w].signature
          if parent_type.match(/\*/) == nil
            parent_type = parent_type + "*"
          end
          break
        end
        w += 1
      end
      if w == ds_length
        "VT name #{parent} unknown"
        exit(1)
      end
      fw.puts @s + "map<#{type},#{parent_type}> *resultset = (map<#{type},#{parent_type}> *)stc->resultSet;"
    elsif @nonNative > 1
# HANDLE
    end
    if type.match(/pair/)
      fw.puts @s + type + " p;"
    end
    pths = 0
    indexD = 0
    indexR = 0
    indexR_hold = 0
    while pths < paths.length
      path = paths[pths]
      pth = path.length - 1
      if path.length > 1
# not autonomous
        fw.puts @s + "pr = 0;"
        fw.puts @s + "while (pr < pr_size) {"
        fw.puts @s + "    if ( !strcmp(parents[pr]->dsName, \"#{path[pth]}\") )"
        fw.puts @s + @s + "break;"
        fw.puts @s + "    pr++;"
        fw.puts @s + "}"
        fw.puts @s + "parent_attr = parents[pr];"
      end
      while pth >= 0
        w = 0
        parent = path[pth];
        while w < ds_length
          if ds_chars[w].name == parent && ds_chars[w].signature.length > 0
            parent_signature = ds_chars[w].signature
# Object VTs can be accessed from iterators using their access path
            if pth == path.length - 1
              fw.puts @s + "#{parent_signature} *any_dstr#{indexD.to_s} = (#{parent_signature} *)parent_attr->memory;"
            else
              fw.puts @s + "#{parent_signature} *any_dstr#{indexD.to_s};"
            end
            fw.puts @s + "#{parent_signature}::iterator iter#{indexD.to_s};"
            indexD += 1
            break
          end
          w += 1
        end
        pth -= 1
      end
      pth = path.length - 1
      access_bridge = ""
      access = ""
      indexS = 0
      while pth >= 0
        w = 0
        parent = path[pth];
        while w < ds_length
          if ds_chars[w].name == parent
            if ds_chars[w].signature.length > 0
              parent_signature = ds_chars[w].signature
              spaces = spacing(indexS)
              if pth < path.length - 1
                fw.puts spaces + "any_dstr#{indexR.to_s} = (#{parent_signature} *)(*iter#{(indexR-1).to_s})#{access_bridge}#{access};"
                access = ""
                access_bridge = ""
              end
              fw.puts spaces + "ds_size#{indexR.to_s} = any_dstr#{indexR.to_s}->size();"
              fw.puts spaces + "iter#{indexR.to_s} = any_dstr#{indexR.to_s}->begin();"
              fw.puts spaces + "for (int i#{indexR.to_s}; i#{indexR.to_s}<ds_size#{indexR.to_s}; i#{indexR.to_s}++;) {"
              indexR += 1
              indexS += 1
            else 
              
            end
            if pth > 0
# We are getting ahead of ourselves to provide access to the VT 
# to be printed next. For case 0, there is no next.
              child = path[pth - 1]
              parent_children = ds_chars[w].children
              pc_size = ds_chars[w].children.length
              pc = 0
              while pc < pc_size
                if child == parent_children[pc].name
                  access = parent_children[pc].access
                  if access.length > 0
                    child_type = ds_chars[w].type
                    if child_type.match(/\*/) == nil
                      child_type = child_type + "*"
                    end
                    if child_type.match(/pair<(\s*)(\w+)(\s*),(\s*)(\w+)(\s*)>(\s*)\*/)
                      access_bridge = "->"
                    elsif child_type.match(/pair<(\s*)(\w+)(\s*),(\s*)(\w+)(\s*)>(\s*)/)
                      access_bridge = "."
                    elsif child_type.match(/(\s*)(\w+)(\s*)\*(\s*)/)
                      access_bridge = "->"
                    elsif child_type.match(/(\s*)(\w+)(\s*)/)
                      access_bridge = "."
                    end
                  end
                  break
                end
                pc += 1
              end
            end
            break
          end
          w += 1
        end
        pth -= 1
      end
      if @nonNative == nil || @nonNative < 1
        if type.match(/pair/)
          fw.puts spacing(indexS) + "p = *iter#{(indexR-1).to_s};"
          fw.puts spacing(indexS) + "resultset->insert(&p#{access_bridge}#{access});"
        else
          fw.puts spacing(indexS) + "resultset->insert((*iter#{(indexR-1).to_s})#{access_bridge}#{access});"
        end
      elsif @nonNative == 1
        if type.match(/pair/)
          if parent_signature.match(/map/)
            fw.puts spacing(indexR) + "p = *iter#{(indexR-1).to_s};"
            fw.puts spacing(indexR) + "pp = *iter#{(indexR-2).to_s};"
            fw.puts spacing(indexR) + "resultset->insert(pair<#{type},#{parent_type}>(&p#{access_bridge}#{access},&pp));"
          else
            fw.puts spacing(indexR) + "p = *iter#{(indexR-1).to_s};"
            fw.puts spacing(indexR) + "resultset->insert(pair<#{type},#{parent_type}>(&p#{access_bridge}#{access},*iter#{(indexR-2).to_s}));"
          end
        else
          fw.puts spacing(indexR) + "resultset->insert(pair<#{type},#{parent_type}>((*iter#{(indexR-1).to_s})#{access_bridge}#{access},*iter#{(indexR-2).to_s}));"
        end
      else
        #HANDLE
      end
      prev_indexR_hold = indexR_hold
      indexR_hold = indexR
      temp_indexR = indexR
=begin
      puts "temp_indexR: #{temp_indexR}"
      puts "prev_indexR_hold: #{prev_indexR_hold}"
      puts "indexR_hold: #{indexR_hold}"
=end
      indexR_diff = indexR_hold - prev_indexR_hold
      while indexR_diff > 0 && temp_indexR - 1 >= 0
        fw.puts spacing(indexS) + "iter#{(temp_indexR-1).to_s}++;"
        fw.puts spacing(indexS - 1) + "}"
        temp_indexR -= 1
        indexR_diff -= 1
        indexS -= 1
      end
      pths += 1
    end
    fw.puts @s + "stc->size = resultset->size();"
    fw.puts "    }"
=begin
    puts "For VT #{@name} paths are #{paths.length}: "
    pths = 0
    while pths < paths.length
      pth = 0
      puts "Path #{pths.to_s}:"
      while pth < paths[pths].length
        puts paths[pths][pth]
        pth += 1
      end
      puts "<-------->"
      pths += 1
    end
=end
  end
  

  def get_paths(ds_chars, paths, path)
    pr = 0
    if @parents == nil
      pr_size = 0
    else
      pr_size = @parents.length
    end
    holder_path = Array.new
    holder_path.replace(path)
    if pr_size == 0
=begin
      h = 0
      while h < holder_path.length
        puts "holder_path[#{h.to_s}]: #{holder_path[h]}"
        h += 1
      end 
=end
      paths.push(holder_path)
    end
    while pr < pr_size
      curr_parent = @parents[pr]
      if path.index(curr_parent) == nil
        path.push(curr_parent)
      else
        puts "Cyclic relationship."
        # HANDLE
      end
      w = 0
      while w < ds_chars.length
        if ds_chars[w].name == curr_parent
          ds_chars[w].get_paths(ds_chars, paths, path)
          path.replace(holder_path)
          break
        end
        w += 1
      end
      pr += 1
    end
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

# Generates code to search each VT struct.
# Each search case matches a specific column of the VT.
  def search_columns(fw)
    col = 0
    fw.puts @s + "switch( iCol ){"
    while col < @columns.length
      fw.puts @s + "case " + col.to_s + ":"
# if collection/map...
      if @signature.length > 0
        fw.puts @s + "    iter = any_dstr->begin();"
        fw.puts @s + "    for(int i=0; i<(int)any_dstr->size();i++){"
      end
      sqlite3_type = ""
      column_cast = ""
      sqlite3_parameters = ""
      column_cast_back = ""
      access_path = ""
      op = @columns[col].bind_datatypes(sqlite3_type, column_cast, 
                                   sqlite3_parameters, column_cast_back, 
                                   access_path)
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
      #      puts "sqlite3_type: " + sqlite3_type
      #      puts "column_cast: " + column_cast
      #      puts "sqlite3_parameters: " + sqlite3_parameters
      #      puts "column_cast_back: " + column_cast_back
      #      puts "access_path: " + access_path
      if @signature.length > 0
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
      col += 1
    end
    fw.puts @s + "}"
  end

# Generates code in search method. Code makes the necessary arrangements 
# for search to happen successfully (condition checks, reallocation)
  def setup_search(fw, ds_array)

# optimisation: refrain from calling get_datastructure_size in each call.
# However for real time apps this is necessary.
    #HereDoc1

  stl_fill_resultset = <<-SFR
        for (int j=0; j<get_datastructure_size(cur->pVtab); j++){
            stcsr->resultSet[j] = j;
            stcsr->size++;
	}
        assert(stcsr->size <= stcsr->max_size);
        assert(&stcsr->resultSet[stcsr->size] <= &stcsr->resultSet[stcsr->max_size]);
SFR


    #HereDoc2

  object_match = <<-OM
        stcsr->resultSet[count++] = i;
        stcsr->size++;
OM

    #HereDoc5

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
    fw.puts "    dsData *d = (dsData *)stl->data;"
    dereference = ""
      if @parents == nil
        parents_no = 0
      else
        parents_no = @parents.length
      end
    if @signature.length > 0
      fw.puts "    " + @signature +
        " *any_dstr = (" + @signature + " *)d->attr->memory;"
      fw.puts "    " + @signature + ":: iterator iter;"
    elsif @object_class.length > 0
      fw.puts "    " + @object_class +
        " *any_dstr = (" + @object_class + " *)d->attr->memory;"
    else
      puts "ERROR: not recorded structure type: stl or object"
      exit(1)
    end
    fw.puts "    int op, iCol, count = 0, i = 0, re = 0;"
    if parents_no > 0 && @signature.length > 0
      fw.puts "    realloc_resultset(cur);"
    end
    fw.puts "    if ( val==NULL ){"
    if @signature.length > 0
      fw.puts stl_fill_resultset
    else
      fw.puts object_match
    end
    fw.puts "    }else{"
    fw.puts @s + "check_alloc((const char *)constr, op, iCol);"
    fw.puts resultset_alloc
  end


# Constructs VT create queries.
  def gen_create_query()
# <db>.<table> always valid?
# <db>.<table> does not work for some reason. test.
    query =  "CREATE VIRTUAL TABLE " + @db + "." + @name  + " USING stl("
    c = 0
    while c < @columns.length
      query += @columns[c].print_col_info() + ","
      c += 1
    end
    query = query.chomp(",") + ")"
    puts query
    return query
  end

# Searches each column of a VT to find a specific FK and 
# return its access statement
  def search_fk(table_name_id)
    col = 0
    found = false
    while col < @columns.length && !found
      found, access = @columns[col].search_fk_col(table_name_id)
      col += 1
    end
    if !found
      access = nil
    end
    return access
  end

# Stores column information in a specific data structure
  def register_columns(columns)
    col = 0
    while col < columns.length
      @columns[col] = Column.new
      @columns[col].set(columns[col])
      col += 1
    end
  end

# Extracts column information for a VT
  def process_columns(columns_str, columns)
    split_col = Array.new
    if col_string.match(/,/)
      split_col = col_string.split(/,/)
    else
      split_col[0] = col_string
    end
    columns.replace(split_col)
    l = 0
    while l < columns.length
      puts "columns[" + l.to_s + "] = " + columns[l]
      l += 1
    end
  end  

# Extracts parent information for a VT
  def process_parents(parents)
    parents.gsub!(/\s/,"")
    split_parents = Array.new
    if parents.match(/,/)
      split_parents = parents.split(/,/)
    else 
      split_parents[0] = parents
    end
    @parents.replace(split_parents)
    p = 0
    while p < @parents.length
      puts "parents[" + p.to_s + "] = " + @parents[p]
      p += 1
    end
  end


  def match_element(element_description)
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
    # array with entries the processed characteristics of each virtual table
    @directives = ""
    @tokenised_dir = Array.new
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

    w = 0
    while w < @ds_chars.length
      curr_ds = @ds_chars[w]
      fw.puts "int " + curr_ds.name +
        "_search(sqlite3_vtab_cursor *cur, char *constr, sqlite3_value *val){"
      curr_ds.setup_search(fw, @ds_chars)
      curr_ds.search_columns(fw)
      fw.puts cls_search
      w += 1
    end
    fw.puts "int search(sqlite3_vtab_cursor* cur, char *constr, sqlite3_value *val){"
    fw.puts "    stlTable *stl = (stlTable *)cur->pVtab;"
    w = 0
    while w < @ds_chars.length
      curr_ds = @ds_chars[w]
      fw.puts "    if( !strcmp(stl->zName, \"" + curr_ds.name + "\") )"
      fw.puts @s + "return " + curr_ds.name +
        "_search(cur, constr, val);"
      w += 1
    end
    fw.puts "}"
  end


  def print_free_resultset(fw)
    setup = <<-ST
void free_resultset(sqlite3_vtab_cursor *cur) {
    stlTableCursor *stc = (stlTableCursor *)cur;
    stlTable *stl=(stlTable *)cur->pVtab;
    dsData *d = (dsData *)stl->data;
    int nntv_size = d->nntv_size;
    const char *table_name = stl->zName;
ST
    fw.puts setup
    w = 0
    while w < @ds_chars.length
      curr_ds = @ds_chars[w]
      if w == 0
        fw.puts "    if( !strcmp(table_name, \"" + curr_ds.name + "\") )"
      else
        fw.puts "    else if( !strcmp(table_name, \"" + curr_ds.name + "\") )"
      end
      type = curr_ds.type
      if type.match(/pair<(\s*)(\w+)(\s*),(\s*)(\w+)(\s*)>(\s*)\*/)
      elsif type.match(/pair<(\s*)(\w+)(\s*),(\s*)(\w+)(\s*)>(\s*)/)
        type = type + "*"
      elsif type.match(/(\s*)(\w+)(\s*)\*(\s*)/)
      elsif type.match(/(\s*)(\w+)(\s*)/)
        type = type + "*"
      end
      if curr_ds.nonNative == nil || curr_ds.nonNative == 0
        fw.puts @s + "delete( (set<#{type}> *)stc->resultSet );"
      elsif curr_ds.nonNative == 1
        ww = 0
        while ww < @ds_chars.length
          if curr_ds.parents[0] == @ds_chars[ww].name
            break
          end
          ww += 1
        end
        curr_pr = @ds_chars[ww]
        if !curr_pr.type.match(/\*/)
          pr_type = curr_pr.type + "*"
        else
          pr_type = curr_pr.type
        end
        fw.puts @s + "delete( (map<#{type},#{pr_type}> *)stc->resultSet );"
      elsif curr_ds.nonNative > 1
        # HANDLE
      end
      w += 1
    end
    fw.puts "}"
  end


  def print_reset_nonNative(fw)
    
    reset = <<-RS
void reset_nonNative(sqlite3_vtab_cursor *cur) {
    stlTable *stl=(stlTable *)cur->pVtab;
    dsData *d = (dsData *)stl->data;
    nonNative **nntv = d->nntv;
    int nntv_size = d->nntv_size, i = 0;
    for (i = 0; (nntv_size > 1) && (i < nntv_size); i++) {
        nntv[i]->active = 0;
    }
}
RS
    fw.puts reset
  end

  
  def print_fill_resultset(fw)

    setup = <<-SP
    stlTable *stl = (stlTable *)cur->pVtab;
    const char *table_name = stl->zName;
    stlTableCursor *stc = (stlTableCursor *)cur;
    dsData *d = (dsData *)stl->data;
    attrCarrier **parents, *parent_attr, *curr_attr = d->attr;;
    int pr, pr_size, ds_size;
    parents = d->parents;
    pr_size = d->parents_size;
    table_name = stl->zName;
SP

    fw.puts "int fill_resultset(sqlite3_vtab_cursor *cur) {"
    fw.puts setup
    w = 0
    paths = Array.new
    path = Array.new
    while w < @ds_chars.length
      curr_ds = @ds_chars[w]
      parents = curr_ds.parents
      path[0] = curr_ds.name
      puts "Getting paths for: #{curr_ds.name}..."
      curr_ds.get_paths(@ds_chars, paths, path)
      curr_ds.print_paths(@ds_chars, paths, fw, w)
      paths.clear
      path.clear
      w += 1
    end
    fw.puts "    return SQLITE_OK;"
    fw.puts "}"
  end

# Generates the function to reallocate space for the struct 
# that carries info for the user defined structs of the 
# application registered with SQTL.
  def print_realloc_carrier(fw)

realloc_carrier = <<-RC
int realloc_carrier(sqlite3_vtab *pVtab, void *ds, const char *tablename, char **pzErr) {
    dsArray **ddsC = (dsArray **)ds;
    dsArray *dsC = *ddsC;
    int re;
#ifdef DEBUGGING
    printf("dsC->size: %i\\n", dsC->ds_size);
#endif
    if (dsC->init)
        if ( (re = set_carriers(ddsC, pzErr)) != SQLITE_OK )
            return re;
    return set_dependencies(pVtab, *ddsC, tablename, pzErr);
}
RC
    fw.puts realloc_carrier
  end


# Generates the function which sets up the references 
# between related VT constructs.
  def print_set_carriers(fw)

    setting_up = <<-SS
    dsArray *dsC = *ddsC, *tmp_dsC;
    dsData **tmp_data, **dsC_data;
    int x_size, nByte, size, c, i;
    char *c_temp;
    x_size = dsC->ds_size;
SS

    curve_ds = <<-CDS
    tmp_dsC = (dsArray *)sqlite3_malloc(nByte);
    if (tmp_dsC != NULL) {
        memset(tmp_dsC, 0, nByte);
        tmp_dsC->init = 0;
        tmp_dsC->ds_size = 6;
        size = tmp_dsC->ds_size;
        tmp_dsC->ds = (dsData **)&tmp_dsC[1];
        tmp_data = tmp_dsC->ds;
        tmp_data[0] = (dsData *)&tmp_data[size];
        for (i = 1; i < size; i++)
            tmp_data[i] = (dsData *)&tmp_data[i-1][1];
        tmp_data[0]->attr = (attrCarrier *)&tmp_data[i-1][1];
        for (i = 1; i < size; i++)
            tmp_data[i]->attr = (attrCarrier *)&tmp_data[i-1]->attr[1];
        c_temp = (char *)&tmp_data[i-1]->attr[1];
        dsC_data = dsC->ds;
CDS
   
    exception_handlingExiting = <<-EHE
#ifdef DEBUGGING
        printf("c_temp: %lx <= tmp_dsC: %lx \\n", c_temp, &((char *)tmp_dsC)[nByte]);
#endif
        assert(c_temp <= &((char *)tmp_dsC)[nByte]);
        sqlite3_free(dsC);
        *ddsC = tmp_dsC;
#ifdef DEBUGGING
        printf("\\nReallocating carrier..now size %i \\n\\n", tmp_dsC->size);
#endif
    }else{
        sqlite3_free(tmp_dsC);
        *pzErr = sqlite3_mprintf("Error (re)allocating memory\\n");
        return SQLITE_NOMEM;
    }
    return SQLITE_OK;
}
EHE

    fw.puts "int set_carriers(dsArray **ddsC, char **pzErr) {"
    fw.puts setting_up
    w = 0
    names_total = 0
    structs_no = @ds_chars.length
    while w < structs_no
      names_total += @ds_chars[w].name.length + 1
      w += 1
    end
    fw.puts "    nByte = sizeof(dsArray) + (sizeof(dsData *) + sizeof(dsData) + sizeof(attrCarrier)) * #{structs_no.to_s} + #{names_total.to_s};"
    fw.puts curve_ds
    w = 0
    fw.puts @s + "c = 0;"
# Duplicate effort below necessary to achieve simplicity.
# We are copying autonomous structs first, registered by user in main.
# Afterwards, we are copying the nested, registered in description.
# Verify that order does indeed not matter from now on.
    while w < structs_no
      curr_ds = @ds_chars[w]
      parents = curr_ds.parents
      if parents == nil
        parents_no = 0
      else
        parents_no = curr_ds.parents.length
      end
      if parents_no == 0
        fw.puts @s + "copy_structs(&tmp_data[c], &c_temp, dsC_data[c]->attr->dsName, dsC_data[c]->attr->memory);"
        fw.puts @s + "c++;"
      end
      w += 1
    end
    w = 0
    while w < structs_no
      curr_ds = @ds_chars[w]
      if parents_no > 0
        fw.puts @s + "copy_structs(&tmp_data[c], &c_temp, \"" + curr_ds.name + "\", NULL);"
        fw.puts @s + "c++;"
      end
      w += 1
    end
    fw.puts @s + "assert(c == " + structs_no.to_s + ");"
    fw.puts exception_handlingExiting
  end


# Generates the function which copies the contents of the original 
# data structure description (filled by user in main) to the extended 
# one which will contain full information, also for nested structures.
  def print_copy_structs(fw)

    copy = <<-CP
void copy_structs(dsData **ddsC, char **c_temp, const char *name, long int *mem\
) {
    dsData *dss = *ddsC;
    attrCarrier *attr_dss = dss->attr;
    int len;
    attr_dss->dsName = *c_temp;
    len = (int)strlen(name) + 1;
    memcpy(*c_temp, name, len);
    *c_temp += len;
    if ( mem != NULL ) {
        attr_dss->memory = mem;
        attr_dss->set_memory = NULL;
    }
    *ddsC = dss;
}
CP
    fw.puts copy
  end

# Generates the function which sets up the references 
# between related VT constructs.
  def print_set_dependencies(fw)

    setup_setting = <<-SS
    stlTable *stl = (stlTable *)pVtab;
    dsData **tmp_data = dsC->ds, *curr_data;
    attrCarrier **parents, *curr_attr;
    nonNative **nntv;
    int c = 0, pr, dc, pr_size, nn, nntv_size;
    int dsC_size = dsC->ds_size;
    while (c < dsC_size) {
        if ( !strcmp(table_name, tmp_data[c]->attr->dsName) )
            break;
        c++;
    }
    curr_data = tmp_data[c];
    curr_attr = curr_data->attr;
SS


    fw.puts "int set_dependencies(sqlite3_vtab *pVtab, dsArray *dsC, const char *table_name, char **pzErr) {"
    fw.puts setup_setting
    w = 0
    paths = Array.new
    path = Array.new
    while w < @ds_chars.length
      curr_ds = @ds_chars[w]
      parents = curr_ds.parents
      if parents == nil
        parents_no = 0
      else
        parents_no = curr_ds.parents.length
      end
      nonNative = curr_ds.nonNative
      if w == 0
        fw.puts "    if( !strcmp(table_name, \"" + curr_ds.name + "\") ) {"
      else
        fw.puts "    } else if( !strcmp(table_name, \"" + curr_ds.name + "\") ) {"
      end
      fw.puts @s + "stl->data = curr_data;"
      if parents_no > 0
        fw.puts @s + "curr_data->parents_size = " + parents_no.to_s + ";"
        fw.puts @s + "pr_size = curr_data->parents_size;"
        fw.puts @s + "curr_data->parents = (attrCarrier **)sqlite3_malloc(sizeof(attrCarrier *) * pr_size);"
        fw.puts @s + "parents = curr_data->parents;"
        fw.puts @s + "pr = 0;"
        pr = 0
        while pr < parents_no
          ww = 0
          while ww < @ds_chars.length
            if @ds_chars[ww].name == parents[pr]
              path[0] = parents[pr]
              @ds_chars[ww].get_paths(@ds_chars, paths, path)
            end
            ww += 1
          end
          pths = 0
          while pths < paths.length
            path = paths[pths]
            fw.puts @s + "dc = 0;"
            fw.puts @s + "while (dc < dsC_size) {"
            fw.puts @s + "    if ( !strcmp(tmp_data[dc]->attr->dsName, \"" + 
              path[path.length - 1] + "\") )"
            fw.puts @s + @s + "break;"
            fw.puts @s + "    dc++;"
            fw.puts @s + "}"
            fw.puts @s + "parents[pr] = tmp_data[dc]->attr;"
            fw.puts @s + "pr++;"
            pths += 1
          end
          paths.clear
          path.clear
          pr += 1
        end
        fw.puts @s + "assert (pr == pr_size);"
      else
        fw.puts @s + "curr_data->parents_size = 0;"
        fw.puts @s + "curr_data->parents = NULL;"
      end
      if nonNative == nil || nonNative == 0
        fw.puts @s + "curr_data->nntv_size = 0;"
        fw.puts @s + "curr_data->nntv = NULL;"
      else
        fw.puts @s + "curr_data->nntv_size = " + nonNative.to_s + ";"
        fw.puts @s + "nntv_size = curr_data->nntv_size;"
        fw.puts @s + "curr_data->nntv = (nonNative **)sqlite3_malloc(sizeof(nonNative *) * nntv_size);"
        fw.puts @s + "nntv = curr_data->nntv;"
        fw.puts @s + "nntv[0] = (nonNative *)&nntv[nntv_size];"
        fw.puts @s + "nn = 0;"
        pr = 0
        while pr < nonNative
          fw.puts @s + "dc = 0;"
          fw.puts @s + "while (dc < dsC_size) {"
          fw.puts @s + "    if ( !strcmp(tmp_data[dc]->attr->dsName, \"" + 
            parents[pr] + "\") )"
          fw.puts @s + @s + "break;"
          fw.puts @s + "    dc++;"
          fw.puts @s + "}"
          fw.puts @s + "nntv[nn]->name = tmp_data[dc]->attr->dsName;"
          fw.puts @s + "nntv[nn]->active = 0;"
          fw.puts @s + "nn++;"
          pr += 1
        end
        fw.puts @s + "assert (nn == nntv_size);"
      end
      w += 1
    end
    fw.puts "    }"
    fw.puts "    return SQLITE_OK;"
    fw.puts "}"
  end


# Generates application-specific code to complement the SQTL library.
  def generate()

   #HereDoc1
      auto_gen1 = <<-AG1

using namespace std;


void * thread_sqlite(void *data){
    const char **queries, **table_names;
    queries = (const char **)sqlite3_malloc(sizeof(char *) *
                   #{@ds_chars.length.to_s});
    table_names = (const char **)sqlite3_malloc(sizeof(char *) *
                   #{@ds_chars.length.to_s});
    int failure = 0;
AG1

   #HereDoc2
# maybe adjust so that create queries are grouped by database.
      auto_gen2 = <<-AG2
    failure = register_table( "#{@ds_chars[0].db}" ,  #{@ds_chars.length.to_s}, queries, table_names, data);
    printf(\"Thread sqlite returning..\\n\");
    sqlite3_free(queries);
    sqlite3_free(table_names);
    return (void *)failure;
}


/* comparison function for datastructure if needed
struct classcomp{
    bool operator() (const USER_CLASS& uc1, const USER_CLASS& uc2) const{
        return (uc1.get_known_type()<uc2.get_known_type());
    }
};
// in main: include classcomp in template arguments
*/


int main(){
// allocations and initialisations
  int re_sqlite;
  void *data;
  char *c_temp;

  //names of data structures to be registered
  const char *name1 = "to be filled_in";
  int n_name1 = (int)strlen(name1) + 1;
  // etc for subsequent data structures. eg:
  // const char *name2 = "to be filled_in";
  // int n_name2 = (int)strlen(name2) + 1;
  // length of data structures names

  dsArray *dsC;
  dsData **ddsC;
  int nByte = sizeof(dsArray) + (sizeof(dsData *) + sizeof(dsData) + sizeof(attrCarrier)) * <number of stl structures to register> + n_name1;
  // etc for subsequent data structures. eg: + n_name2;
  dsC = (dsArray *)sqlite3_malloc(nByte);
  memset(dsC, 0, nByte);
  pthread_t sqlite_thread;

// assignment of data structure characteristics to dsC
  // number of data structures to register
  dsC->ds_size = <number of stl structures to register>;
  int size = dsC->ds_size;
  dsC->init = 1;
  dsC->ds = (dsData **)&dsC[1];
  ddsC = dsC->ds;
  ddsC[0] = (dsData *)&ddsC[size];
  int dsi;
  for (dsi = 1; dsi < size; dsi++)
    ddsC[dsi] = (dsData *)&ddsC[dsi-1][1];
  ddsC[0]->attr = (attrCarrier *)&ddsC[dsi-1][1];
  for (dsi = 1; dsi < size; dsi++)
    ddsC[dsi]->attr = (attrCarrier *)&ddsC[dsi-1]->attr[1];
  ddsC[0]->attr->memory = (long int *) <address of stl structure to register first>;
  // etc for subsequent data structures. eg:
  // ddsC[1]->attr->memory = (long int *) <address of stl structure to register first>;
  c_temp = (char *)&ddsC[dsi-1]->attr[1];
  ddsC[0]->attr->dsName = c_temp;
  memcpy(c_temp, name1, n_name1);
  c_temp += n_name1;
  // etc for subsequent data structures
  // ddsC[1]->attr->dsName = c_temp;
  memcpy(c_temp, name2, n_name2);
  c_temp += n_name2;
  assert(c_temp <= &((char *)dsC)[nByte]);
  dat = (void *)&dsC;
  signal(SIGPIPE, SIG_IGN);
  re_sqlite = pthread_create(&sqlite_thread, NULL, thread_sqlite, dat);
  pthread_join(sqlite_thread, NULL);
  printf(\"Thread sqlite returned %i\\n\", re_sqlite);
}


AG2

    #HereDoc3
    directives = <<-dir
using namespace std;


/*
#define DEBUGGING
*/

dir

    #HereDoc4
        auto_gen3 = <<-AG3


int realloc_resultset(sqlite3_vtab_cursor *cur) {
    stlTable *stl = (stlTable *)cur->pVtab;
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    int arraySize;
    int *res;
    arraySize = get_datastructure_size(cur->pVtab);
    if ( arraySize != stcsr->max_size ){
        res = (int *)sqlite3_realloc(stcsr->resultSet, sizeof(int) * arraySize);
        if (res!=NULL){
            stcsr->resultSet = res;
            memset(stcsr->resultSet, -1,
                   sizeof(int) * arraySize);
            stcsr->max_size = arraySize;
#ifdef DEBUGGING
            printf("\\nReallocating resultSet..now max size %i \\n\\n", stcsr->max_size);
#endif
        }else{
            sqlite3_free(res);
            printf("Error (re)allocating memory\\n");
            return SQLITE_NOMEM;
        }
    }
    return SQLITE_OK;
}


int compare(int dstr_value, int op, int value){
    switch( op ){
    case 0:
        return dstr_value<value;
    case 1:
        return dstr_value<=value;
    case 2:
        return dstr_value==value;
    case 3:
        return dstr_value>=value;
    case 4:
        return dstr_value>value;
    }
}


int compare(long int dstr_value, int op, long int value){
    switch( op ){
    case 0:
        return dstr_value<value;
    case 1:
        return dstr_value<=value;
    case 2:
        return dstr_value==value;
    case 3:
        return dstr_value>=value;
    case 4:
        return dstr_value>value;
    }
}


int compare(double dstr_value, int op, double value){
    switch( op ){
    case 0:
        return dstr_value<value;
    case 1:
        return dstr_value<=value;
    case 2:
        return dstr_value==value;
    case 3:
        return dstr_value>=value;
    case 4:
        return dstr_value>value;
    }
}


int compare(const void *dstr_value, int op, const void *value){
    switch( op ){
    case 0:
        return dstr_value<value;
    case 1:
        return dstr_value<=value;
    case 2:
        return dstr_value==value;
    case 3:
        return dstr_value>=value;
    case 4:
        return dstr_value>value;
    }
}


int compare(const unsigned char *dstr_value, int op,
                   const unsigned char *value){
    switch( op ){
    case 0:
        return strcmp((const char *)dstr_value,(const char *)value)<0;
    case 1:
        return strcmp((const char *)dstr_value,(const char *)value)<=0;
    case 2:
        return strcmp((const char *)dstr_value,(const char *)value)==0;
    case 3:
        return strcmp((const char *)dstr_value,(const char *)value)>=0;
    case 4:
        return strcmp((const char *)dstr_value,(const char *)value)>0;
    }
}


int compare_res(int count, stlTableCursor *stcsr, int *temp_res) {
    int ia, ib;
    int *i_res;
    int i_count = 0;
    if ( (stcsr->size == 0) && (stcsr->first_constr == 1) ){
        memcpy(stcsr->resultSet, temp_res, sizeof(int) *
               stcsr->max_size);
        stcsr->size = count;
        stcsr->first_constr = 0;
    }else if (stcsr->size > 0){
        i_res = (int *)sqlite3_malloc(sizeof(int) *
                                      stcsr->max_size);
        if ( i_res == NULL ) {
            sqlite3_free(i_res);
            printf("Error (re)allocating memory\\n");
            return SQLITE_NOMEM;
        }
        for(int a=0; a<stcsr->size; a++){
            for(int b=0; b<count; b++){
                ia = stcsr->resultSet[a];
                ib = temp_res[b];
                if( ia==ib ){
                    i_res[i_count++] = ia;
                }else if( ia < ib )
                    b = count;
            }
        }
        assert( i_count <= stcsr->max_size );
        memcpy(stcsr->resultSet, i_res, sizeof(int) *
               i_count);
        stcsr->size = i_count;
        sqlite3_free(i_res);
    }
    return SQLITE_OK;
}


void check_alloc(const char *constr, int &op, int &iCol) {
    switch( constr[0] - 'A' ){
    case 0:
        op = 0;
        break;
    case 1:
        op = 1;
        break;
    case 2:
        op = 2;
        break;
    case 3:
        op = 3;
        break;
    case 4:
        op = 4;
        break;
    case 5:
        op = 5;
        break;
    default:
        break;
    }
    iCol = constr[1] - 'a' + 1;
}


AG3

    #HereDoc5
  makefile_part = <<-mkf

user_functions.o: user_functions.c search.h
        gcc -W -g -c user_functions.c

stl_to_sql.o: stl_to_sql.c stl_to_sql.h search.h
        gcc -g -c stl_to_sql.c

search.o: search.cpp search.h
        g++ -W -g -c search.cpp
mkf

    myfile = File.open("main_v2.template", "w") do |fw|
      fw.puts "\#include <stdlib.h>"
      fw.puts "\#include \"stl_to_sql.h\""
      fw.puts "\#include <pthread.h>"
      fw.puts "\#include <assert.h>"
      fw.puts @directives
      fw.puts auto_gen1
      w = 0
      while w < @ds_chars.length
        curr_ds = @ds_chars[w]
        # probably needs processing
        fw.puts "    queries[" + w.to_s + "] = \"" + curr_ds.gen_create_query() + "\";"
        fw.puts "    table_names[" + w.to_s + "] = \"" + curr_ds.name + "\";"
        w += 1
      end
      fw.puts auto_gen2
    end

    myfile = File.open("search.cpp", "w") do |fw|
      fw.puts "\#include \"search.h\""
      fw.puts "\#include <string>"
      fw.puts "\#include <assert.h>"
      fw.puts "\#include <stdio.h>"
      fw.puts "\#include <set>"
      fw.puts "\#include <map>"
      fw.puts @directives
      fw.puts
      fw.puts directives
      print_set_dependencies(fw)
      fw.puts "\n\n"
      print_copy_structs(fw)
      fw.puts "\n\n"
      print_set_carriers(fw)
      fw.puts "\n\n"
      print_realloc_carrier(fw)
      fw.puts "\n\n"
      print_fill_resultset(fw)
      fw.puts "\n\n"
      print_reset_nonNative(fw)
      fw.puts "\n\n"
      print_free_resultset(fw)
      fw.puts "\n\n"
      print_search_functions(fw)
      fw.puts "\n\n"
      print_retrieve_functions(fw)
    end
    myFile = File.open("makefile_v2", "w") do |fw|
      fw.print "test: main.o search.o stl_to_sql.o user_functions.o "
      tokenise_directive()
      print_directives(fw, 1)
      fw.print "\n    g++ -lswill -lsqlite3 -W -g main.o search.o stl_to_sql.o user_functions.o "
      print_directives(fw, 1)
      fw.puts "-o test\n\n"
      fw.print "main.o: main.cpp search.h "
      print_directives(fw, 3)
      fw.puts "\n\tg++ -W -g -c main.cpp"
      fw.puts makefile_part
      fw.puts
      print_directives(fw, 2)
    end
  end

# Processes each pair of parent and child structures and stores the 
# child's name to parent and the access statement for the child 
# table to child.
  def process_hierarchy()
    w = 0
    while w < @ds_chars.length
      parents = @ds_chars[w].parents
      p = 0
      while parents != nil && p < parents.length
        pr = 0
        while pr < @ds_chars.length && parents[p] != @ds_chars[pr].name
          pr += 1
        end
        if pr == @ds_chars.length
          puts "No such data structure recorded: " + parents[p] + 
            " parent of " + @ds_chars[w].name
          exit(1)
        end
        @ds_chars[pr].children[@ds_chars[pr].children.length] = Child.new
        access_stmt = @ds_chars[pr].search_fk(@ds_chars[w].name)
        if access_stmt == nil
          access_stmt = @ds_chars[w].search_fk(@ds_chars[pr].name)
        end
        if access_stmt == nil
          puts "Relationship between tables " + @ds_chars[pr].name + 
            " and " + @ds_chars[w].name + " not recorded correctly."
          exit(1)
        end
        @ds_chars[pr].children[@ds_chars[pr].children.length - 1].name = @ds_chars[w].name
        @ds_chars[pr].children[@ds_chars[pr].children.length - 1].access =  access_stmt
        puts "Child name: " + 
          @ds_chars[pr].children[@ds_chars[pr].children.length - 1].name + 
          " in parent: " + @ds_chars[pr].name + 
          " with access statement: " + 
          @ds_chars[pr].children[@ds_chars[pr].children.length - 1].access
        p += 1
      end
      if @ds_chars[w].signature.length > 0
        @ds_chars[w].nonNative = p
        puts "VT #{@ds_chars[w].name} nonNative of rate #{@ds_chars[w].nonNative}"
      else
        @ds_chars[w].nonNative = nil
        puts "VT #{@ds_chars[w].name} nonNative of rate nil"
      end
      w += 1
    end
  end


# Matches a description against a pattern and calls specific methods fow extracting characteristics out of the description.
  def match_pattern(w, columns)
  end


# User description first comes here. Description is cleaned from 
# surplus spaces and is split to extract directives to external 
# application and library files. 
# Each VT description is separated, matched against specific patterns
# and all elements are recorded including column specifications.
  def register_datastructures()
    @description.each { |x| puts x }
    @directives = @description[0]
    @description.delete_at(0)
    puts "Directives: " + @directives
    $elements = Array.new
    tables = Array.new
    views = Array.new
    w = 0
    while w < @description.length
      puts "\nDESCRIPTION No: " + w.to_s + "\n"
      des = @description[w].lstrip!
      case des
      when /^create element table/i
        $elements.push(Element.new).last.match_element(des)
      when /^create table/i
        tables.push(VirtualTable.new).last.match_table(des)
      when /^create view/i
        views.push(View.new).last.match_view(des)
      end
      w += 1
    end
    #    process_hierarchy()
  end
end

if __FILE__ == $0
  if !File.file?("input.txt")
    raise "File 'input.txt' does not exist.\n"
  end
  description = ""
  description = File.open("input.txt", "r") { |fw| fw.read }
  if description.match(/;/)
    token_description = description.split(/;/)
  else
    raise "Invalid description..delimeter ';' not used."
  end
  $s = "        "
  puts "description before whitespace squeeze "
#  token_description.each { |x| p x}
  token_description.each{ |x| x.squeeze(' ')}
  token_description.each{ |x| if x.length == 0 : description.delete(x) end }
  token_description.each{ |x| if x.match(/\n|\t|\r/) : x.gsub!(/\n|\t|\r/, "") end }
  puts "description after whitespace squeeze "
  ip = InputDescription.new(token_description)
  ip.register_datastructures()
#  ip.generate()
end
