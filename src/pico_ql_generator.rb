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
    @data_type_class = ""     # Compound data type. Used 
                              # for string and UNION so far.
    @related_to = ""          # Reference to other VT's name (FK).
    @fk_col_type = ""         # Reference to other VT's type (FK).
    @fk_method_ret = 0        # True if FK access path is a method
    		   	      # return.
                              # Required for managing temporary variables.
    @saved_results_index = -1 # Required for naming the particular 
                              # saved results instance.
    @pre_access_path = ""     # Pre-access path statements in code block format.
			      # No DSL keywords are allowed (for now).
    @access_path = ""         # The access statement for the column value.
    @post_access_path = ""    # Post-access path statements in code block format.
			      # No DSL keywords are allowed (for now).
    @tokenized_access_path = Array.new # Access path tokens to check for 
                                       # NULLs.
    @lock = ""                # Custom lock to use for each column.
                              # A uniform abstraction is defined.
    @lock_class               # Reference to Lock template class.
    @lock_argument = ""       # Actual lock to hold/release, if any.
    @col_type = "object"      # Record type (pointer or reference) for 
                              # special columns, the ones that refer to 
                              # other VT or UNIONS.
    @case = ucase             # switch case for union view fields
    @@int_data_types        = ["int", "integer", "tinyint", 
                               "smallint", "mediumint", "int2",
                               "bool", "boolean", "numeric"]
    @@bigint_data_types     = ["bigint", "unsigned big int",
                               "int8"]
    @@double_data_types     = ["float", "double", "double precision", "real"]
    @@text_data_types       = ["text", "date", "datetime", "clob", "string"]
    @@text_match_data_types = [/character/i, /varchar/i, /nvarchar/i, 
                               /varying character/i, /native character/i,
                               /nchar/i]
  end
  attr_accessor(:name,:line,:data_type,
		:data_type_class,:related_to,
		:fk_col_type,:fk_method_ret,
		:saved_results_index,
		:pre_access_path,:access_path,
		:post_access_path,:col_type,
		:case,:tokenized_access_path,
		:lock,:lock_class,:lock_argument)

# Used to clone a Column object. Ruby does not support deep copies.
  def construct(name, data_type, data_type_class, 
                related_to, fk_method_ret, 
      		fk_col_type, saved_results_index,
                pre_access_path, access_path,
		post_access_path,
		type, line, ucase)
    @name = name
    @data_type = data_type
    @data_type_class = data_type_class
    @related_to = related_to
    @fk_method_ret = fk_method_ret
    @fk_col_type = fk_col_type
    @saved_results_index = saved_results_index
    @pre_access_path = pre_access_path
    @access_path = access_path
    process_access_path() 
    @post_access_path = post_access_path
    @col_type = type
    @line = line
    @case = ucase
  end

# If argument contains address of,
# subtract it and dereference.
# Used for memory management (MEM_MGT).
  def deref_address_of(access_path)
    if access_path.match(/^&/)
      access_path.gsub!(/^&/ , "")
    else
      access_path = "*#{access_path}"
    end
  end

# Array of strings: deep copy
  def array_string_copy_deep(array, copyArray)
    array.each { |el| copyArray.push(el.clone) }
  end

# Configure tokens to be standalone access paths.
  def configure_token_access_checks(token_ac_p, iden)
    root = String.new(iden)
    root.gsub!(/^&/, "")
    token_ac_p.each_index { |tap|
      if $argD == "DEBUG"
        puts "tap is #{tap.to_s}, token to check is: #{token_ac_p[tap]}"
      end
      if tap > 0
        token_ac_p[tap].insert(0, "#{token_ac_p[tap-1]}->")
      else
        token_ac_p[0].insert(0, root)
      end
      if $argD == "DEBUG"
        puts "After tapping token is: #{token_ac_p[tap]}"
      end
    }
    return token_ac_p
  end

# Print line directive
  def print_line_directive(fw)
    if @line != nil
      if $argD == "DEBUG"
        fw.puts "#line #{@line + 1} \"#{$argF}\""
      end
    end
  end

# Display NULL checks
  def display_null_check(token_ac_p, iden, action, fw, space)
    tuple_iter_inside = false
    iden2 = ""
    token_ac_p.each {|tap|
      if tap.match(/\(tuple_iter|,(\s*)tuple_iter|tuple_iter\)/)
        if $argD == "DEBUG"
          puts "Matched tuple_iter in #{tap}. iden is #{iden}."
        end
# Make this a separate function.
        if iden.empty?
          useless = token_ac_p[0].split("->")
          iden2 = useless[0]
          token_ac_p[0].gsub!(/^(.+?)->/, "")
        else
          iden2 = "#{iden}"
          iden2.chomp!("->")
        end
        tap.gsub!(/tuple_iter/, "#{iden2}")
        if $argD == "DEBUG"
          puts "After iden substitution: #{tap}"
        end
        tuple_iter_inside = true
        break
      end
    }
    if tuple_iter_inside == true
      root = ""
    else
      root = String.new(iden)
      root.gsub!(/^&/, "")
    end
    token_ac_p.each_index {|tap|
      if token_ac_p.length == 1
        fw.print "#{space}if (#{root}#{token_ac_p[0]} == NULL) "
      elsif token_ac_p.length > 1 && tap == 0
        fw.print "#{space}if ((#{root}#{token_ac_p[0]} == NULL) "
      elsif token_ac_p.length > 1 && tap > 0 && tap < token_ac_p.length - 1
        fw.print "|| (#{root}#{token_ac_p[tap]} == NULL) "
      elsif token_ac_p.length > 1 && tap == token_ac_p.length - 1
        fw.print "|| (#{root}#{token_ac_p[tap]} == NULL)) "
      end
    }
    if !token_ac_p.empty?
      fw.puts "\n#{space}  #{action}"
    end
  end

# Substitute special keywords in access paths.
# For pre, post access paths we don't want to substitute
# accessors. In access paths we do.
# Refactor to include 'tuple_iter' substitutions in here too.
# For PiCO QL Valgrind module: commenting out the base substitution block;
# it's not used in any DSL description. Are there cases that it would be useful?
  def sub_keywords(pre_post_ap, access_path, iter)
    if access_path
      if iter != nil &&
         access_path.match(/tuple_iter->|tuple_iter\.|tuple_iter,|tuple_iter\)/)
        access_path.gsub!(/tuple_iter/, "#{iter}") 
      end
      #if access_path.match(/base->|base\.|base,|base\)/)
      #  access_path.gsub!(/base(\.|->)/, "any_dstr->")
      #  access_path.gsub!(/base/, "any_dstr") 
      #end
    elsif pre_post_ap
      if iter != nil &&
         pre_post_ap.match(/tuple_iter->|tuple_iter\.|tuple_iter,|tuple_iter\)/)
        pre_post_ap.gsub!(/tuple_iter/, "#{iter}")
      end
      if pre_post_ap.match(/base->|base\.|base,|base\)/)
        pre_post_ap.gsub!(/base/, "any_dstr")
      end
    end
  end

# Generate code for column of type UNION in retrieve function.
  def retrieve_union(fw, vt, iden, token_access_checks, space)
    u = UnionView.new
    $union_views.each { |uv| 
      if uv.name == @name
        u = uv
      end 
    }
    union_access_path = String.new(@access_path)
    if u.switch.match(/tuple_iter/)
      switch = String.new(u.switch)
      switch.gsub!(/tuple_iter/, "#{iden}")
    else
      switch = String.new("#{iden}#{u.switch}")
      if !@access_path.end_with?(".") && !@access_path.end_with?("->")
        union_access_path.concat(".")
      end
    end
    fw.puts "#{space}    switch (#{switch}) {"
# Will not work for union in struct1 in struct2 (user responsibility?)
# if struct1.struct2.union then access path would be 
# (any_dstr->)struct2.union . reasonable to await struct2.union_mode
# from user?
    u.columns.each_index { |col|
      c = u.columns[col]
      fw.puts "#{space}    case #{c.case}:"
      fw.puts "#{space}      {"
      space.concat("    ")
      col_class, sqlite3_type,
      sqlite3_parameters,
      column_cast, 
      column_cast_back = c.bind_datatypes("retrieve")
      if $argD == "DEBUG"
        puts "sqlite3_type: #{sqlite3_type}"
        puts "column_cast: #{column_cast}"
        puts "sqlite3_parameters: #{sqlite3_parameters}"
        puts "column_cast_back: #{column_cast_back}"
        puts "union_access_path: #{union_access_path}"
        puts "access_path: #{c.access_path}"
        if col_class == "union"
          puts "Union: #{c.name}"
        end
        if c.line != nil
          puts "line: #{(c.line + 1).to_s}"
        else 
          puts "line: nil"
        end
      end
      case col_class
      when "fk"
        union_ap = "#{union_access_path}#{c.access_path}"
        c.retrieve_fk(fw, vt, iden, union_ap, 
		      token_access_checks, sqlite3_type,
                      sqlite3_parameters, column_cast, 
                      col, space)
      when "data"
        union_ap = "#{union_access_path}#{c.access_path}"
        c.retrieve_data(fw, iden, union_ap, 
			token_access_checks, sqlite3_type, 
                        sqlite3_parameters, 
                        column_cast, column_cast_back, 
                        space)
      when "union"
        iden.concat(union_access_path)     # iden will hold state;concat+concat..
        c.retrieve_union(fw, vt, iden, token_access_checks, space)
      end
      space.chomp!("    ")
      fw.puts "#{space}      }"     # close case
    }
    fw.puts "#{space}    }"
    fw.puts "#{space}    break;"
  end

# Generate code for data column in retrieve function.
  def retrieve_data(fw, iden, access_path,
		    token_access_checks, sqlite3_type,
		    sqlite3_parameters,
                    column_cast, column_cast_back, 
                    space)
    ap_copy = String.new(access_path)
    if !access_path.start_with?("tuple_iter") && access_path.match(/tuple_iter/)
      if access_path.match(/tuple_iter\.|tuple_iter->/)
        ap_copy.gsub!(/tuple_iter\.|tuple_iter->/, "#{iden}")
      end
      if access_path.match(/\(tuple_iter,|\(tuple_iter\)|tuple_iter\)/)
        if iden.end_with?(".")
          ap_copy.gsub!(/\(tuple_iter,/, "(#{iden.chomp(".")},")
          ap_copy.gsub!(/\(tuple_iter\)/, "(#{iden.chomp(".")})")
          ap_copy.gsub!(/tuple_iter\)/, "#{iden.chomp(".")})")
        elsif iden.end_with?("->")
          ap_copy.gsub!(/\(tuple_iter,/, "(#{iden.chomp("->")},")
          ap_copy.gsub!(/\(tuple_iter\)/, "(#{iden.chomp("->")})")
          ap_copy.gsub!(/tuple_iter\)/, "#{iden.chomp("->")})")
        end
      end
      if $argD == "DEBUG"
        puts "retrieve_data: substituting \"tuple_iter\" in access path:"
        puts "  #{iden}, #{ap_copy}"
      end
    elsif !access_path.match(/tuple_iter/)
      ap_copy = "#{iden}#{ap_copy}"
    end
    iden_block = String.new(iden) # Used for code block access path {pre, post}
    iden_block.chomp!("->")       # Chomping accessor because it is configured
    iden_block.chomp!(".")        # from user in code block.
    sub_keywords(nil, ap_copy, iden_block)
    pre_ap = String.new(@pre_access_path)
    sub_keywords(pre_ap, nil, iden_block)
    post_ap = String.new(@post_access_path)
    sub_keywords(post_ap, nil, iden_block)
    null_check_action = "{\n#{space}        sqlite3_result_null(con);\n#{space}        break;\n#{space}      }"
    display_null_check(token_access_checks,
                       iden,
                       null_check_action,
                       fw, "#{space}    ")
    if sqlite3_type == "text"
      if column_cast_back == ".c_str()"
        string_construct_cast = ""
      else
        string_construct_cast = "(const char *)"
      end
      if $argLB == "CPP"
        fw.puts "#ifdef PICO_QL_HANDLE_TEXT_ARRAY"
        if !pre_ap.empty? 
          fw.puts "#{space}    #{pre_ap}"
        end
        fw.puts "#{space}    textVector.push_back(#{string_construct_cast}#{ap_copy});"
        if !post_ap.empty?
          fw.puts "#{space}    #{post_ap}"
        end
        print_line_directive(fw)
        fw.puts "#{space}    sqlite3_result_text(con, (const char *)textVector.back().c_str()#{sqlite3_parameters});"
        fw.puts "#else"
      end
    end
    if !pre_ap.empty? 
      fw.puts "#{space}    #{pre_ap}"
    end
    fw.puts "#{space}    sqlite3_result_#{sqlite3_type}(con, #{column_cast}#{ap_copy}#{column_cast_back}#{sqlite3_parameters});"
    if !post_ap.empty?
      fw.puts "#{space}    #{post_ap}"
    end
    print_line_directive(fw)
    if sqlite3_type == "text" && $argLB == "CPP"
      fw.puts "#endif"
    end
    fw.puts "#{space}    break;"
  end

# Generate code for FK column in retrieve function.
  def retrieve_fk(fw, vt, iden, access_path,        # access_path argument: for UNION 
                  token_access_checks, sqlite3_type,
                  sqlite3_parameters, column_cast, 
                  col, space)
    record_type = ""
    p_type = ""
    if access_path.empty?    # Access with (*iter) .
      if vt.type.match(/\*/)
        record_type = "*"
        p_type = ""
      else
        record_type = ""
        p_type = "&"
      end
    else                        # Access with (*iter)[.|->]access .
      if @col_type == "pointer" 
        record_type = "*" 
        p_type = ""
      else 
        record_type = ""
        p_type = "&"
      end
    end
    r_ap_copy = String.new(access_path)
    p_ap_copy = String.new(access_path)
    if access_path.match(/tuple_iter\.|tuple_iter->|\(tuple_iter,|\(tuple_iter\)|tuple_iter\)/)  # Configured access path ?
      if access_path.match(/tuple_iter\.|tuple_iter->/)
        r_ap_copy.gsub!(/tuple_iter\.|tuple_iter->/, "#{record_type}#{iden}")
        p_ap_copy.gsub!(/tuple_iter\.|tuple_iter->/, "#{p_type}#{iden}")
      end
      if access_path.match(/\(tuple_iter,|\(tuple_iter\)|tuple_iter\)/)
        if iden.end_with?(".")
          r_ap_copy.gsub!(/\(tuple_iter,/, "(#{record_type}#{iden.chomp(".")},")
          r_ap_copy.gsub!(/\(tuple_iter\)/, "(#{record_type}#{iden.chomp(".")})")
          r_ap_copy.gsub!(/tuple_iter\)/, "#{record_type}#{iden.chomp(".")})")
          p_ap_copy.gsub!(/\(tuple_iter,/, "(#{p_type}#{iden.chomp(".")},")
          p_ap_copy.gsub!(/\(tuple_iter\)/, "(#{p_type}#{iden.chomp(".")})")
          p_ap_copy.gsub!(/tuple_iter\)/, "#{p_type}#{iden.chomp(".")})")
        elsif iden.end_with?("->")
          r_ap_copy.gsub!(/\(tuple_iter,/, "(#{record_type}#{iden.chomp("->")},")
          r_ap_copy.gsub!(/\(tuple_iter\)/, "(#{record_type}#{iden.chomp("->")})")
          r_ap_copy.gsub!(/tuple_iter\)/, "#{record_type}#{iden.chomp("->")})")
          p_ap_copy.gsub!(/\(tuple_iter,/, "(#{p_type}#{iden.chomp("->")},")
          p_ap_copy.gsub!(/\(tuple_iter\)/, "(#{p_type}#{iden.chomp("->")})")
          p_ap_copy.gsub!(/tuple_iter\)/, "#{p_type}#{iden.chomp("->")})")
        end
      end
      if $argD == "DEBUG"
        puts "retrieve_fk: substituting \"tuple_iter\" in access path:"
        puts "  #{record_type}, #{iden}, #{r_ap_copy}"
        puts "  #{p_type}, #{iden}, #{p_ap_copy}"
      end
      r_access_path = "#{r_ap_copy}"
      p_access_path = "#{p_ap_copy}"
    else
      r_access_path = "#{record_type}#{iden}#{r_ap_copy}"
      p_access_path = "#{p_type}#{iden}#{p_ap_copy}"
    end
    iden_block = String.new(iden) # Used for code block access path {pre, post}
    iden_block.chomp!("->")       # Chomping accessor because it is configured
    iden_block.chomp!(".")        # from user in code block.
    sub_keywords(nil, r_access_path, iden_block)
    sub_keywords(nil, p_access_path, iden_block)
    pre_ap = String.new(@pre_access_path)
    sub_keywords(pre_ap, nil, iden_block)
    post_ap = String.new(@post_access_path)
    sub_keywords(post_ap, nil, iden_block)
    fw.puts "#{space}      long base_prov = 0;"
    if $argLB == "CPP"
      fw.puts "#ifdef PICO_QL_HANDLE_POLYMORPHISM"
      def_nop = "*"
      if @fk_col_type.match(/(.+)\*/)
        def_nop = ""
      end
      if !pre_ap.empty? 
        fw.puts "#{space}    #{pre_ap}"
      end
      fw.puts "#{space}      #{@fk_col_type}#{def_nop} cast = dynamic_cast<#{@fk_col_type}#{def_nop}>(#{p_access_path});"
      if !post_ap.empty?
        fw.puts "#{space}    #{post_ap}"
      end
      if $argM == "MEM_MGT" && @fk_method_ret == 1
        fw.puts "#{space}      if (cast != NULL) {"
        fw.puts "#{space}        saved_results_#{@saved_results_index}.push_back(*cast);"
        print_line_directive(fw)
        fw.puts "#ifdef ENVIRONMENT64"
        fw.puts "#{space}        sqlite3_result_#{sqlite3_type}(con, (base_prov = #{column_cast}&(saved_results_#{@saved_results_index}.back())));"
        fw.puts "#else"
        fw.puts "#{space}        sqlite3_result_#{sqlite3_parameters}(con, (base_prov = #{column_cast}&(saved_results_#{@saved_results_index}.back())));"
        fw.puts "#endif"
        fw.puts "#{space}      } else {"
        fw.puts "#ifdef ENVIRONMENT64"
        fw.puts "#{space}        sqlite3_result_#{sqlite3_type}(con, #{column_cast}(0));"
        fw.puts "#else"
        fw.puts "#{space}        sqlite3_result_#{sqlite3_parameters}(con, #{column_cast}(0));"
        fw.puts "#endif"
        fw.puts "#{space}      }"
      else
        fw.puts "#ifdef ENVIRONMENT64"
        fw.puts "#{space}      sqlite3_result_#{sqlite3_type}(con, (base_prov = #{column_cast}cast));"
        print_line_directive(fw)
        fw.puts "#else"
        fw.puts "#{space}      sqlite3_result_#{sqlite3_parameters}(con, (base_prov = #{column_cast}cast));"
        print_line_directive(fw)
        fw.puts "#endif"
      end
      fw.puts "#else"
    else # if $argLB == "C"
      fw.puts "#{space}      int j = 0;"
      fw.puts "#{space}      struct Vtbl *chargeVT#{col};"
    end
    null_check_action = "{\n#{space}          sqlite3_result_null(con);\n#{space}          break;\n#{space}        }"
    display_null_check(token_access_checks,
                       iden,
                       null_check_action,
                       fw, "#{space}      ")
    if $argM == "MEM_MGT" && @fk_method_ret == 1
      if !pre_ap.empty? 
        fw.puts "#{space}    #{pre_ap}"
      end
      fw.puts "#{space}      saved_results_#{@saved_results_index}.push_back(#{r_access_path});"
      if !post_ap.empty?
        fw.puts "#{space}    #{post_ap}"
      end
      print_line_directive(fw)
      fw.puts "#ifdef ENVIRONMENT64"
      fw.puts "#{space}      sqlite3_result_#{sqlite3_type}(con, (base_prov = #{column_cast}&(saved_results_#{@saved_results_index}.back())));"
      fw.puts "#else"
      fw.puts "#{space}      sqlite3_result_#{sqlite3_parameters}(con, (base_prov = #{column_cast}&(saved_results_#{@saved_results_index}.back())));"
      fw.puts "#endif"
    else
      fw.puts "#ifdef ENVIRONMENT64"
      if !pre_ap.empty? 
        fw.puts "#{space}    #{pre_ap}"
      end
      fw.puts "#{space}      sqlite3_result_#{sqlite3_type}(con, #{column_cast}#{p_access_path});"
      fw.puts "#{space}      base_prov = #{column_cast}#{p_access_path};"
      if !post_ap.empty?
        fw.puts "#{space}    #{post_ap}"
      end
      print_line_directive(fw)
      fw.puts "#else"
      if !pre_ap.empty? 
        fw.puts "#{space}    #{pre_ap}"
      end
      fw.puts "#{space}      sqlite3_result_#{sqlite3_parameters}(con, #{column_cast}#{p_access_path});"
      fw.puts "#{space}      base_prov = #{column_cast}#{p_access_path};"
      if !post_ap.empty?
        fw.puts "#{space}    #{post_ap}"
      end
      print_line_directive(fw)
      fw.puts "#endif"
      fw.puts "#ifdef PICO_QL_DEBUG"
      fw.puts "#{space}      printf(\"Sending base_prov %lx.\\n\", base_prov);"
      fw.puts "#endif"
    end
    if $argLB == "CPP"
      fw.puts "#endif"
      fw.puts "#{space}      VtblImpl *chargeVT#{col} = selector_vt[\"#{@related_to}\"];"
      if vt.base_var.empty?
        fw.puts "#{space}      return (*chargeVT#{col})(&c_map[cur], 1, &charged, base_prov);"
      else
        fw.puts "#{space}      map<Cursor *, bool> *map#{vt.name}#{col};"
        fw.puts "#{space}      map#{vt.name}#{col} = NULL;"
        fw.puts "#{space}      return (*chargeVT#{col})(&c_map[cur], 1, map#{vt.name}#{col}, base_prov);"
      end
    else
      fw.puts "#{space}      while ((j < (int)vtAll.size) && (strcmp(vtAll.instanceNames[j], \"#{@related_to}\"))) {j++;}"
      fw.puts "#{space}      if (j == (int)vtAll.size) {"
      fw.puts "#{space}        printf(\"In #{vt.name}_vt_retrieve(): VT #{@related_to} not registered.\\nExiting now.\\n\");"
      fw.puts "#{space}        return SQLITE_ERROR;"
      fw.puts "#{space}      }"
      fw.puts "#{space}      chargeVT#{col} = vtAll.instances[j];"
      if vt.base_var.empty?
        fw.puts "#{space}      return chargeVT#{col}->report_charge(cursors[cr], 1, &((#{vt.name}_vt *)vtbl)->charged, &((#{vt.name}_vt *)vtbl)->chargedSize, base_prov, chargeVT#{col});"
      else
        fw.puts "#{space}      return chargeVT#{col}->report_charge(cursors[cr], 1, NULL, NULL, base_prov, chargeVT#{col});"
      end
    end
    fw.puts "#{space}      break;"
  end

# Generate code for evaluating {F}irst constraint
# and any {N}ext constraint for data column
# in search function.
  def search_data_constr(fw, container_class, 
		         access_path, 
                         tokenized_access_path,
			 sqlite3_type,
                         column_cast, column_cast_back, 
                         add_to_result_set,
                         iteration, 
                         notC, space)
    null_check_action = "break;"
    if !container_class.empty?
      null_check_action = "continue;"
      if !iteration.empty?
        fw.puts "#{iteration.gsub(/<space>/, "#{space}")}"
        space.concat("  ")
      end
# not for union
    end
    sub_keywords(nil, access_path, nil)
    pre_ap = String.new(@pre_access_path)
    sub_keywords(pre_ap, nil, nil)
    post_ap = String.new(@post_access_path)
    sub_keywords(post_ap, nil, nil)
    display_null_check(tokenized_access_path, "",
                       null_check_action,
                       fw, space)
    if !pre_ap.empty?
      fw.puts "#{space}#{pre_ap}"
    end
    fw.puts "#{space}if (#{notC}compare_#{sqlite3_type}(#{column_cast}#{access_path}#{column_cast_back}, op, sqlite3_value_#{sqlite3_type}(val))) {"
    print_line_directive(fw)
    if !container_class.empty?
      space.chomp!("  ")
    end
    if !post_ap.empty?
      fw.puts "#{add_to_result_set.gsub(/\}\Z/,  # Generate post-access path within loop
                 "  #{post_ap}\n<space>}").gsub("<space>", "#{space}")}"
    else
      fw.puts "#{add_to_result_set.gsub("<space>", "#{space}")}"
    end
  end


# Generate code for data column in search function.
  def search_data(fw, vt, sqlite3_type,
                  column_cast, column_cast_back)
    notC = ""
    space = "#{$s}  "
    access_pathF, access_pathN, 
    idenF, idenN = vt.configure_search("data", @access_path, "", @data_type)
    add_to_result_setF, add_to_result_setN = vt.configure_result_set()
    iterationF, iterationN = vt.configure_iteration(@access_path)
    fw.puts "        if (first_constr == 1) {"
    token_ac_p = Array.new
    array_string_copy_deep(@tokenized_access_path, token_ac_p)
    configure_token_access_checks(token_ac_p, idenF)
    if !vt.try_optimize(fw, access_pathF, sqlite3_type)
      search_data_constr(fw, vt.container_class, 
		         access_pathF,
                         token_ac_p,
		         sqlite3_type,
                         column_cast, column_cast_back, 
                         add_to_result_setF, 
                         iterationF, 
                         notC, space)
    end
    if !vt.container_class.empty?
      fw.puts "        } else {"
    else
      fw.puts "        } else if (stcsr->size == 1) {"
    end
    notC = "!"
    token_ac_p.clear
    array_string_copy_deep(@tokenized_access_path, token_ac_p)
    configure_token_access_checks(token_ac_p, idenN)
    search_data_constr(fw, vt.container_class,
		       access_pathN, 
                       token_ac_p,
		       sqlite3_type,
                       column_cast, column_cast_back, 
                       add_to_result_setN, 
                       iterationN, 
                       notC, space)
    fw.puts "        }"
    fw.puts "        break;"    
  end

# Generate code for evaluating {F}irst constraint
# and any {N}ext constraint for FK column
# in search function.
  def search_fk_constr(fw, container_class, 
		       access_path, 
                       tokenized_access_path,
                       sqlite3_type, 
                       sqlite3_parameters, 
                       column_cast, 
                       add_to_result_set, 
                       iteration,
		       notC, space)
    null_check_action = "break;"
    if !container_class.empty?
      null_check_action = "continue;"
    end
    sub_keywords(nil, access_path, nil)
    pre_ap = String.new(@pre_access_path)
    sub_keywords(pre_ap, nil, nil)
    post_ap = String.new(@post_access_path)
    sub_keywords(post_ap, nil, nil)
    if !container_class.empty?
      if !iteration.empty?
        fw.puts "#{iteration.gsub("<space>", "#{space}")}"
        space.concat("  ")
      end
    end
    fw.puts "#ifdef ENVIRONMENT64"
    display_null_check(tokenized_access_path, "",
                       null_check_action,
                       fw, space) 
    if $argM == "MEM_MGT" && @fk_method_ret == 1    # Returning from a method.
      deref_address_of(access_path) 		    # Called once; it suffices.
      if !pre_ap.empty?
        fw.puts "#{space}#{pre_ap}"
      end
      fw.puts "#{space}typeof(#{access_path}) t = #{access_path};"
      fw.puts "#{space}if (#{notC}compare_#{sqlite3_type}(#{column_cast}&t, op, #{column_cast.chomp('&')}sqlite3_value_#{sqlite3_type}(val))) {"
    else
      if !pre_ap.empty?
        fw.puts "#{space}#{pre_ap}"
      end
      fw.puts "#{space}if (#{notC}compare_#{sqlite3_type}(#{column_cast}#{access_path}, op, #{column_cast.chomp('&')}sqlite3_value_#{sqlite3_type}(val))) {"
    end
    print_line_directive(fw)
    fw.puts "#else"
    display_null_check(tokenized_access_path, "",
                       null_check_action,
                       fw, space) 
    if $argM == "MEM_MGT" && @fk_method_ret == 1
      if !pre_ap.empty?
        fw.puts "#{space}#{pre_ap}"
      end
      fw.puts "#{space}typeof(#{access_path}) t = #{access_path};"
      fw.puts "#{space}if (#{notC}compare_#{sqlite3_parameters}(#{column_cast}&t, op, #{column_cast.chomp('&')}sqlite3_value_#{sqlite3_parameters}(val))) {"
    else
      if !pre_ap.empty?
        fw.puts "#{space}#{pre_ap}"
      end
      fw.puts "#{space}if (#{notC}compare_#{sqlite3_parameters}(#{column_cast}#{access_path}, op, #{column_cast.chomp('&')}sqlite3_value_#{sqlite3_parameters}(val))) {"
    end
    if !container_class.empty?
      space.chomp!("  ")
    end
    print_line_directive(fw)
    fw.puts "#endif"
    if !post_ap.empty?
      fw.puts "#{add_to_result_set.gsub(/\}\Z/,  # Generate post-access path within loop
                 "  #{post_ap}\n<space>}").gsub("<space>", "#{space}")}"
    else
      fw.puts "#{add_to_result_set.gsub("<space>", "#{space}")}"
    end
    if !container_class.empty?
      if iteration.length > 0
        space.chomp!("  ")
      end
    end
    if container_class.empty?
      space.chomp!("  ")
    end
  end

# Generate code for FK column in search function.
  def search_fk(fw, vt, 
		sqlite3_type, 
                sqlite3_parameters, 
                column_cast)
    access_pathF, access_pathN, idenF, idenN = vt.configure_search("fk", @access_path, @col_type, "")
    add_to_result_setF, add_to_result_setN = vt.configure_result_set()
    iterationF, iterationN = vt.configure_iteration(@access_path)
    fw.puts "        if (first_constr) {"
    space = "#{$s}  "
    notC = ""
    token_ac_p = Array.new
    array_string_copy_deep(@tokenized_access_path, token_ac_p)
    configure_token_access_checks(token_ac_p, idenF)
    search_fk_constr(fw, vt.container_class, 
                      access_pathF, 
                      token_ac_p,
                      sqlite3_type, 
                      sqlite3_parameters, 
                      column_cast, 
                      add_to_result_setF, 
                      iterationF,
		      notC, space)
    if !vt.container_class.empty?
      fw.puts "        } else {"
    else
      fw.puts "        } else if (stcsr->size == 1) {"
    end
    space.concat("  ")
    notC = "!"
    token_ac_p.clear
    array_string_copy_deep(@tokenized_access_path, token_ac_p)
    configure_token_access_checks(token_ac_p, idenN)
    search_fk_constr(fw, vt.container_class, 
                      access_pathN, 
                      token_ac_p,
                      sqlite3_type, 
                      sqlite3_parameters, 
                      column_cast, 
                      add_to_result_setN, 
                      iterationN,
		      notC, space)
    fw.puts "        }"
    fw.puts "        break;"
  end

# Performs case analysis with respect to the column data type
# and operation.
  def bind_datatypes(op) 
    tmp_text_array = Array.new      # Do not process the original array.
    tmp_text_array.replace(@@text_match_data_types)
    if !@related_to.empty?          # column refers to other VT.
      return "fk", "int64", "int",  # "int":for 32-bit architectures.used in retrieve and search.
             "(long int)", nil 
    elsif @name == "base"              # 'base' column. refactor: elsif perhaps?
      return "base", "int64", "int",
             "(long int)", nil
    elsif @name == "rownum"           # 'rownum'column
      return "rownum", "int", nil,
             nil, nil
    elsif @data_type_class == "union"
      return "union", nil, nil,
	     nil, nil 
    else
      sqlite3_type = ""
      sqlite3_parameters = ""
      column_cast = ""
      column_cast_back = ""
      dt = @data_type.downcase         # Normal data column.
      if @@bigint_data_types.include?(dt)
        sqlite3_type = "int64"
      elsif @@int_data_types.include?(dt)
        sqlite3_type = "int"
      elsif (dt == "blob")
        sqlite3_type = "blob"
        sqlite3_parameters = ", -1, SQLITE_STATIC"
        column_cast = "(const void *)"
      elsif @@double_data_types.include?(dt) ||
          /decimal/i.match(dt)
        sqlite3_type = "double"
      elsif @@text_data_types.include?(dt) || 
        tmp_text_array.reject! { |rgx| rgx.match(dt) != nil } != nil
        # If match against RegExp, reject.
                                    # Then, if array changed, enter.
        case op
        when "search"
          column_cast = "(const unsigned char *)"
        when "retrieve" 
          column_cast = "(const char *)"
        end
        sqlite3_type = "text"
        if @data_type_class == "string"
          column_cast_back = ".c_str()" 
        end
        sqlite3_parameters = ", -1, SQLITE_STATIC"
      end
      return "data", sqlite3_type,
	     sqlite3_parameters,
	     column_cast, column_cast_back
    end
  end

# Generate code for evaluating {F}irst constraint
# and any {N}ext constraint for column of type UNION
# in search function.
  def search_union_constr(fw, container_class,
                          root_access_path, 
                          union_access_path, 
                          tokenized_access_path,
                          add_to_result_set, 
                          iteration, notC)
    null_check_action = "break;"
    if !container_class.empty?
      null_check_action = "continue;"
    end
    u = UnionView.new
    $union_views.each { |uv| 
      if uv.name == @name
         u = uv
      end 
    }
    space = "#{$s}  "
    if !container_class.empty?
      space.concat("  ")
    end
    display_null_check(tokenized_access_path, "",
                       null_check_action,
                       fw, space)
# Imitating Column::process_access_path(). Not able to access from here. 
    if u.switch.match(/->/)
      tokenized_switch_path = u.switch.split(/->/)
      tokenized_switch_path.pop
      if $argD == "DEBUG"
        tokenized_switch_path.each { |t| p t}
      end
      configure_token_access_checks(tokenized_switch_path, 
                                    root_access_path)
      display_null_check(tokenized_switch_path, "",
                         null_check_action,
                         fw, space)
    end
    if u.switch.match(/tuple_iter/)
      switch = String.new(u.switch)
      switch.gsub!(/tuple_iter/, "#{root_access_path}")
    else
      switch = String.new("#{root_access_path}#{u.switch}")
    end
    fw.puts "#{space}switch (#{switch}) {"
    u.columns.each { |col|
      fw.puts "#{space}case #{col.case}:"
      fw.puts "#{space}  {"
      space.concat("    ")
      col_class, sqlite3_type,
      sqlite3_parameters,
      column_cast,
      column_cast_back = col.bind_datatypes("search") 
      if col.access_path.match(/tuple_iter/)   # generic enough or hack?
        total_access_path = "#{col.access_path}"
        total_access_path.gsub!(/tuple_iter/, "#{root_access_path}")
      else
        total_access_path = "#{union_access_path}#{col.access_path}"
      end
      if col_class == "fk" && col.col_type == "object"
        total_access_path = "&#{total_access_path}"
      elsif col_class == "union"   # What cause does this serve?
        if col.col_type == "pointer"
          total_access_path.concat("->")
        else
          total_access_path.concat(".")
        end
      end
      un_col_ac_t = Array.new
      array_string_copy_deep(col.tokenized_access_path, un_col_ac_t)
      configure_token_access_checks(un_col_ac_t, total_access_path)
      if $argD == "DEBUG"
        puts "sqlite3_type: #{sqlite3_type}"
        puts "column_cast: #{column_cast}"
        puts "sqlite3_parameters: #{sqlite3_parameters}"
        puts "column_cast_back: #{column_cast_back}"
        puts "union_access_path: #{total_access_path}"
        puts "col_access_path: #{col.access_path}"
        puts "col_type: #{col.col_type}"
        if col_class == "union"
          puts "Union: #{col.name}"
        end
        if col.line != nil
          puts "line: #{(col.line + 1).to_s}"
        else 
          puts "line: nil"
        end
      end
      case col_class
      when "fk"
        col.search_fk_constr(fw, container_class, 
                             total_access_path, 
                             un_col_ac_t,
                             sqlite3_type, 
                             sqlite3_parameters, 
                             column_cast, 
                             add_to_result_set,
			     iteration, 
                             notC, space)
      when "data"
        col.search_data_constr(fw, container_class, 
                               total_access_path,
                               un_col_ac_t,
		               sqlite3_type, 
                               column_cast, column_cast_back, 
                               add_to_result_set, 
                               iteration, 
                               notC, space)
      when "union"
# Not tested. In fact, it would probably make sense
# to call search_union and not search_union_constr in
# such a case.
        col.search_union_constr(fw, container_class,
                                union_access_path, 
                                total_access_path, 
                                un_col_ac_t,
                                add_to_result_set, 
                                iteration, notC)
      end
      if !container_class.empty?
        space.concat("  ")
      end
      fw.puts "#{space}break;"
      space.chomp!("  ")
      fw.puts "#{space}}"   # close case
      space.chomp!("  ")
    }
    fw.puts "#{space}}"
  end

# Generate code for column of type UNION in search function.
  def search_union(fw, vt) 
    full_union_access_pathF, 
    full_union_access_pathN, 
    idenF, idenN = vt.configure_search("union", @access_path, @col_type, "")
    add_to_result_setF, add_to_result_setN = vt.configure_result_set()
    iterationF, iterationN = vt.configure_iteration("")
    fw.puts "        if (first_constr == 1) {"
    space = "#{$s}  "
    if !vt.container_class.empty?
      add_to_result_setF.chomp!("\n<space>}")
      add_to_result_setN.chomp!("\n<space>}")
      fw.puts "#{iterationF.gsub(/<space>/, "#{space}")}"
#      space.concat("  ")
    end
    notC = ""
    iteration = ""
    un_ac_t = Array.new
    array_string_copy_deep(@tokenized_access_path, un_ac_t)
    configure_token_access_checks(un_ac_t, idenF)
    search_union_constr(fw, vt.container_class, idenF,
                        full_union_access_pathF, 
                        un_ac_t,
                        add_to_result_setF, 
                        iteration, notC)
    if !vt.container_class.empty?
      fw.puts "#{$s}  }"
      fw.puts "        } else {"
      fw.puts "#{iterationN.gsub(/<space>/, "#{space}")}"
#      space.concat("  ")
    else
      fw.puts "        } else if (stcsr->size == 1) {"
    end
    notC = "!"
    un_ac_t.clear
    array_string_copy_deep(@tokenized_access_path, un_ac_t)
    configure_token_access_checks(un_ac_t, idenN)
    search_union_constr(fw, vt.container_class, idenN, 
                        full_union_access_pathN, 
                        un_ac_t,
                        add_to_result_setN, 
                        iteration, notC)
    if !vt.container_class.empty?
      fw.puts "#{$s}  }"
    end
    fw.puts "        }"
    fw.puts "        break;"
  end

# Generate code for [vt]'s column [col] in search function.
  def search(fw, vt, col)
    fw.puts "    case #{col}:"
    fw.puts "      {"
    space = "      "
    col_class, sqlite3_type,
    sqlite3_parameters, column_cast,
    column_cast_back = bind_datatypes("search") 
    if $argD == "DEBUG"
      puts "sqlite3_type: #{sqlite3_type}"
      puts "column_cast: #{column_cast}"
      puts "sqlite3_parameters: #{sqlite3_parameters}"
      puts "column_cast_back: #{column_cast_back}"
      puts "access_path: #{access_path}"
      if col_class == "union"
        puts "Union: #{@name}"
      end
      if line != nil
        puts "line: #{(line + 1).to_s}"
      else 
        puts "line: nil"
      end
    end
    case col_class
    when "fk"
      search_fk(fw, vt,
               sqlite3_type, sqlite3_parameters, 
               column_cast)
    when "data"
      search_data(fw, vt, sqlite3_type,
		  column_cast, column_cast_back)
    when "union"
      search_union(fw, vt) 
    when "base"
      vt.search_base(fw)
    when "rownum"
      vt.search_rownum(fw)
    end
    fw.puts "      }"   # close case 
  end

# Generate code for a [vt]'s column [col] in retrieve function.
  def retrieve(fw, vt, col)
    fw.puts "  case #{col}:"
    fw.puts "    {"
    col_class, sqlite3_type,
    sqlite3_parameters, column_cast,
    column_cast_back = bind_datatypes("retrieve") 
    iden = vt.configure_retrieve(@access_path, col_class, @data_type)
    token_ac_p = Array.new
    if col_class == "fk" || col_class == "data" ||
       col_class == "union"
      array_string_copy_deep(@tokenized_access_path, token_ac_p)
      configure_token_access_checks(token_ac_p, "")
    end
    case col_class
      when "base"
        fw.puts "#ifdef ENVIRONMENT64"
        # sqlite3_type = "int64" always in this case.
        fw.puts "      sqlite3_result_#{sqlite3_type}(con, #{column_cast}any_dstr);"
        fw.puts "#else"
        # sqlite3_parameters = "int" always in this case.
        fw.puts "      sqlite3_result_#{sqlite3_parameters}(con, #{column_cast}any_dstr);"
        fw.puts "#endif"
        fw.puts "      break;"
      when "rownum"
        fw.puts "      sqlite3_result_#{sqlite3_type}(con, rs->offset);"
        fw.puts "      break;"
      when "fk"
        retrieve_fk(fw, vt, iden, @access_path, 
		    token_ac_p, sqlite3_type,
                    sqlite3_parameters, column_cast, 
                    col, "")
      when "data"
        retrieve_data(fw, iden, @access_path,
 		      token_ac_p, sqlite3_type,
                      sqlite3_parameters, 
                      column_cast, column_cast_back, 
                      "  ")
      when "union"
        retrieve_union(fw, vt, iden, token_ac_p, "  ")
      end
      fw.puts "    }"  # close case
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

# Process and tokenize access paths in code block format.
# Output:
# 	 pre_access_path: C statements prior to "this" C statement
# 	     access_path: "this" C statement; the actual access path
# 	post_access_path: C statements after "this" statement
  def process_access_path_code_block()
    @access_path.lstrip!
    @access_path.rstrip!
    @access_path.delete!("{")  # Required for code block format
    @access_path.chomp!("}")   # ditto
    @access_path.chomp!(";")   # ditto - maybe
    if @access_path.match(/;/)
      statements = @access_path.split(/;/)
      bare_access_path = ""
      statements.each{ |s|
        s.lstrip!
        s.rstrip!
        if !s.match(/tuple_iter/)  # {pre,post} shouldn't contain "tuple_iter"..for now;
          bare_access_path.empty? ? @pre_access_path << "#{s};" : @post_access_path << "#{s};"
        else                       # if use cases dictate we will redesign.
          bare_access_path << s
        end
      }
      @access_path.replace(bare_access_path)
      if $argD == "DEBUG"
        puts "In process_access_path_code_block:"
        puts "  @pre_access_path: #{@pre_access_path}"
        puts "  @access_path: #{@access_path}"
        puts "  @post_access_path: #{@post_access_path}"
      end
    end
  end

# Checks if NULL checks for access path
# have to be performed 
  def process_access_path()
    if @access_path.match(/\{(.+)\}/)
      process_access_path_code_block()
    else
      @access_path.chomp!(";") # In case users end an access path in C fashion
    end
    if $argD == "DEBUG"
      puts "Access path to process is #{@access_path}."
    end
    if @access_path.match(/->/)
      if @access_path.match(/tuple_iter->/) ||
         @access_path.match(/tuple_iter\./)
        if @access_path.match(/tuple_iter->(.+?),(.+)\)/) ||
           @access_path.match(/tuple_iter->(.+?)\)/) || 
           @access_path.match(/tuple_iter->(.+)/) ||  # Introduced with code block
           @access_path.match(/tuple_iter\.(.+)->(.+),/) ||
           @access_path.match(/tuple_iter\.(.+)->(.+)\)/) ||
           @access_path.match(/tuple_iter\.(.+)->(.+)/) # Configured access path
          case @access_path
          when /tuple_iter->(.+?),(.+)\)/    # Skipping checks here; not good.
            matchdata = @access_path.match(/tuple_iter->(.+?),(.+)\)/)
          when /tuple_iter->(.+)\)/
            matchdata = @access_path.match(/tuple_iter->(.+)\)/)
          when /tuple_iter->(.+)/			# Introduced with code block
            matchdata = @access_path.match(/tuple_iter->(.+)/)
          when /tuple_iter\.(.+)->(.+),/
            matchdata = @access_path.match(/tuple_iter\.(.+),/)
          when /tuple_iter\.(.+)->(.+)\)/
            matchdata = @access_path.match(/tuple_iter\.(.+)\)/)
          when /tuple_iter\.(.+)->(.+)/
            matchdata = @access_path.match(/tuple_iter\.(.+)/)
          end
          if $argD == "DEBUG"
            puts "Access path with 'tuple_iter' included, path for checking is #{matchdata[1]}."
          end
          if matchdata[1].match(/->/)
            @tokenized_access_path = matchdata[1].split(/->/)
            if @data_type != "TEXT" # Place a check for the char * too.
              @tokenized_access_path.pop
            end
          end
        else 
          puts "WARNING: Dereference in access path: #{@access_path} not part of data structure registered with PiCO QL.\n"
        end
      else
        @tokenized_access_path = @access_path.split(/->/)
        if @data_type != "TEXT"
          @tokenized_access_path.pop
        end
      end
      if $argD == "DEBUG"
        puts "Access path: #{@access_path}, processed tokens for NULL checking:"
        @tokenized_access_path.each { |tap| p tap }
      end
    end
  end

# Validates a column data type.
# The following data types are the ones accepted by sqlite.
  def verify_data_type()
    dt = @data_type.downcase
    tmp_text_array = Array.new
    tmp_text_array.replace(@@text_match_data_types)
    begin
      if dt == "string"
        @data_type_class = "#{dt}"
        @data_type = "TEXT"
        return 1
      elsif @@text_data_types.include?(dt) || 
          tmp_text_array.reject! { |rgx| rgx.match(dt) != nil } != nil
        return 1
      elsif @@bigint_data_types.include?(dt) || 
            @@int_data_types.include?(dt) || 
            @@double_data_types.include?(dt) || 
          /decimal/i.match(dt) != nil
        return 0
      elsif dt == "union"
        @data_type_class = "#{dt}"
        @data_type = "NUMERIC"
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
          if coln.access_path.match(/tuple_iter\.|tuple_iter->/)
            access_path = coln.access_path.gsub(/tuple_iter\.|tuple_iter->/, '\0<accessor>')
            access_path.gsub!("<accessor>", "#{matchdata[2]}#{access_type_link}")
          else
            access_path = "#{matchdata[2]}#{access_type_link}#{coln.access_path}"
          end
          name = "#{matchdata[1]}#{coln.name}"
          this_columns.last.construct(name,
                                      coln.data_type.clone,
                                      coln.data_type_class.clone,
                                      coln.related_to.clone, 
                                      coln.fk_method_ret,
                                      coln.fk_col_type.clone,
                                      coln.saved_results_index,
                                      coln.pre_access_path.clone, 
                                      access_path, 
                                      coln.post_access_path.clone, 
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
    column_ptn1a = /includes struct view (\w+) from (.+) pointer/im
    column_ptn1b = /includes struct view (\w+) from (.+)/im
    column_ptn2 = /includes struct view (\w+)/im
    column_ptn3 = /foreign key(\s*)\((\s*)(\w+)(\s*)\) from (.+) references (\w+)(\s*)(\w*)/im
    column_ptn4 = /(\w+) (.+) from (.+) pointer/im # for UNION column
    column_ptn5 = /(\w+) (.+) from (.+) using lock (.+)/im
    column_ptn6 = /(\w+) (.+) from (.+)/im
    case column
    when column_ptn1a
      matchdata = column_ptn1a.match(column)
      col_type_text = manage_inclusion(matchdata, "->")
      return col_type_text
    when column_ptn1b
      matchdata = column_ptn1b.match(column)
      col_type_text = manage_inclusion(matchdata, ".")
      return col_type_text
    when column_ptn2         # Include a struct_view 
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
      @data_type = "UNSIGNED BIG INT"
      @related_to = matchdata[6]
      @access_path = matchdata[5]
      begin
        !@related_to.empty?
        if @access_path.match(/(.+)\)/)    # Returning from a method.
          @fk_method_ret = 1
        end
        if matchdata[8].empty?
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
      @lock = matchdata[4]
      @lock, @lock_class, @lock_argument = Lock.process_lock(@lock)
    when column_ptn6
      matchdata = column_ptn6.match(column)
      @name = matchdata[1]
      @data_type = matchdata[2]
      @access_path = matchdata[3]
    end
    register_line()
    col_type_text = verify_data_type()
    if @access_path == "tuple_iter"
      @access_path.gsub!(/tuple_iter/,"")
    end
    if @access_path.match(/#/)     # Substituting back "," in place of "#" 
      @access_path.gsub!(/#/,",")  # for code block formats.
    end
    process_access_path()
    if $argD == "DEBUG"
      puts "Column name is: #{@name}"
      puts "Column data type is: #{@data_type}"
      puts "Column related to: #{@related_to}"
      puts "Column fk_col_type: #{@fk_col_type}"
      puts "Column fk_method_ret: #{@fk_method_ret.to_s}"
      puts "Column saved_results_index: #{@saved_results_index.to_s}"
      puts "Column access path is: #{@access_path}"
      puts "Column type is: #{@col_type}"
      puts "Column is of text type: #{col_type_text.to_s}"
      puts "Column case: #{@case}"
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
    @base_var = ""        # Base variable registration id provided 
			  # at DSL description and at application code.
    @struct_view          # Reference to the respective 
                          # struct_view definition.
    @signature = ""       # The C/C++ signature of the (base) struct.
    @assignable_signature = ""  # The C/C++assignable signature of the (base) struct.
                                # Different only for multi-dimensional C arrays.
    @signature_pointer = "" # Signature is of type pointer? ("*")
    @container_class = "" # A C/C++ container instance. 
    @type = ""            # The record type for the VT. 
                          # Use @type for management. It is active for 
                          # both container and object.
    @pointer = ""         # Is the record type pointer? ("*").
#    @iter_type = ""       # The type of the iterator, which is used for iterating
                          # over a container's elements
    @loop = ""            # Custom loop to use for iterating custom containers;
                          # A uniform abstraction is defined.
    @nloops = 0           # Counts nested loops
    @loop_root = ""       # Holds starting address of C array for NULL 
                          # checking C containers (linked lists, arrays etc)
                          # A uniform abstraction is defined.
    @lock = ""            # Custom lock to use for iterating C containers
                          # (linked lists, arrays, etc.)
                          # A uniform abstraction is defined.
    @lock_class		  # Reference to Lock template class.
    @lock_argument = ""   # Actual lock to hold/release, if any.
    @lock_root = ""       # Holds starting address of C array for locking 
                          # C containers (linked lists, arrays etc)
                          # A uniform abstraction is defined.
    @record_lock = ""     # Custom lock to use for each record.
                          # A uniform abstraction is defined.
    @record_lock_class    # Reference to Lock template class.
    @record_lock_argument = ""   # Actual lock to hold/release, if any.
    @object_class = ""    # If an object instance.
    @columns = Array.new  # References to the VT columns.
			  # A struct view may be used by many VTs.
			  # A VT's set of columns includes its respective
			  # struct view's set of columns but the sets are
			  # not necessarily same (e.g. Process_VT and
			  # EProcess_VT).
    @include_text_col = 0 # True if VirtualTable includes column
    		      	  # of text data type. Required for
			  # generating code for
			  # PICO_QL_HANDLE_TEXT_ARRAY C++ flag.
    @@C_container_types = ["c_container"]
              # Maintained for backward compatibility.
  end
  attr_accessor(:name,:base_var_line,
                :signature_line,:base_var,
                :struct_view,
                :signature,:signature_pointer,
                :assignable_signature,
                :container_class,:type,
                :pointer,
                :object_class,:columns,
                :include_text_col,
                :C_container_types, :loop, :nloops,
                :loop_root, :lock, :lock_class,
                :lock_argument)

# Getter for static member C_container_types at class level
  def self.C_container_types
    @@C_container_types
  end

# Get the context of code evaluation.
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
  def configure_retrieve(access_path, op, data_type)
    iden = ""
    type_check = ""
    if !access_path.start_with?("tuple_iter")
      if !@container_class.empty?
        if @type.end_with?("iterator")  # iteration-based abstraction
          access_path.empty? ? iden = "**(rs->resIter)" : iden = "(**(rs->resIter))."
        else    # pointer-based abstraction 
          if $argLB == "CPP"
            access_path.empty? ? iden = "*(rs->resIter)" : iden = "(*(rs->resIter))."
          else
            access_path.empty? ? iden = "((#{@name}ResultSetImpl *)rs)->res[rs->offset]" : iden = "((#{@name}ResultSetImpl *)rs)->res[rs->offset]."
          end
        end
      else
        access_path.empty? ? iden = "any_dstr" : iden = "any_dstr->"
      end
    elsif access_path.start_with?("tuple_iter.")  # won't work for object
      iden = "(*(rs->resIter))."
      return iden
    end
    case op
    when /data|union/
      if !@container_class.empty?
        if access_path.empty?   # only for data: see erroClassV UNION in Valgrind-mod 
          if op == "data" && @pointer.end_with?("*") && data_type != "TEXT"
            iden = "*#{iden}"
          end
        else
          if (access_path == "first" && 
              @pointer.match(/\*,/)) ||
              (access_path == "second" &&
               @pointer.end_with?("*"))
            iden = "*#{iden}"
          elsif !access_path.match(/first/) &&
                 !access_path.match(/second/) &&
                 @pointer.end_with?("*") &&
                 iden.end_with?(".")
            iden.chomp!(".")
            iden.concat("->")
          end
        end
      end
    when "fk"
      # is object after transformations
      if !@container_class.empty?
        if !access_path.empty? &&
           !access_path.match(/first/) &&
           !access_path.match(/second/) &&
           @pointer.end_with?("*") &&
           iden.end_with?(".")
          iden.chomp!(".")
          iden.concat("->")
        end
      end
    end
    return iden
  end

# Generates code to retrieve each VT struct.
# Each retrieve case matches a specific column of the VT.
  def retrieve_columns(fw)
    fw.puts "  switch (nCol) {"
    @columns.each_index { |col| @columns[col].retrieve(fw, self, col) }
    fw.puts "  }"
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
    @pointer.match(/\*/) ? typep = "" : typep = "*"
    if !@container_class.empty?
      if @@C_container_types.include?(@container_class)
        if $argLB == "CPP"
          add_to_result_setF = "<space>    rs->res.push_back(tuple_iter);\n<space>    rs->resBts.push_back(1);\n<space>  } else {\n<space>    rs->resBts.push_back(0);\n<space>  }\n<space>}"            
        else
          add_to_result_setF = "<space>    rs->size++;\n<space>    rs->actualSize++;\n<space>    if (rs->size == rs->malloced) {\n<space>      rs->malloced *= 2;\n<space>      ((#{@name}ResultSetImpl *)rs)->res = (#{@type}*)sqlite3_realloc(((#{@name}ResultSetImpl *)rs)->res, sizeof(#{@type}) * rs->malloced);\n<space>      if (((#{@name}ResultSetImpl *)rs)->res == NULL)\n<space>        return SQLITE_NOMEM;\n<space>      rs->resBts = (int *)sqlite3_realloc(rs->resBts, sizeof(unsigned int) * (rs->malloced / WORDBITS + 1));\n<space>      if (rs->resBts == NULL)\n<space>        return SQLITE_NOMEM;\n<space>    }\n<space>    ((#{@name}ResultSetImpl *)rs)->res[rs->size - 1] = tuple_iter;\n<space>    rs->resBts[(rs->size - 1) / WORDBITS] |= (1 << ((rs->size - 1) % WORDBITS));\n<space>  }\n<space>}"
        end
      else
        add_to_result_setF = "<space>    rs->res.push_back(tuple_iter);\n<space>    rs->resBts.set(rsIndex, 1);\n<space>  }\n<space>  rsIndex++;\n<space>}"
      end
      if $argLB == "CPP"
        add_to_result_setN = "<space>    resIterC = rs->res.erase(resIterC);\n<space>    rs->resBts.reset(rsIndex);\n<space>  } else\n<space>    resIterC++;\n<space>  rsIndex = rs->resBts.find_next(rsIndex);\n<space>}"
      else
        add_to_result_setN = "<space>    rs->actualSize--;\n<space>    rs->resBts[rsIndex / WORDBITS] &= (~(1 << (rsIndex % WORDBITS)));\n<space>  }\n<space>  rsIndex++;\n<space>  while ((rsIndex < rs->size) && !(rs->resBts[rsIndex / WORDBITS] & (1 << (rsIndex % WORDBITS)))) {rsIndex++;}\n<space>}"
      end
      for i in 0..@nloops-1        # Generate closing curly braces for nested loops
        add_to_result_setF.concat("\n<space>}")
      end
    else
      add_to_result_setF = "<space>  stcsr->size = 1;\n<space>}"
      add_to_result_setN = "<space>  stcsr->size = 0;\n<space>}"
    end
    return add_to_result_setF, add_to_result_setN
  end

  def configure_search(op, access_path, 
                       fk_type, data_type)
    idenF = ""
    idenN = ""
    access_pathF = ""
    access_pathN = ""
    ap = String.new(access_path)
    if !@container_class.empty?
      if @type.end_with?("iterator")  # iteration-based abstraction
        ap.empty? ? idenF = "(*tuple_iter)" : idenF = "(*tuple_iter)."
        ap.empty? ? idenN = "(**resIterC)" : idenN = "(**resIterC)."
      else    # pointer-based abstraction 
        ap.empty? ? idenF = "tuple_iter" : idenF = "tuple_iter."
        if $argLB == "CPP"
          ap.empty? ? idenN = "(*resIterC)" : idenN = "(*resIterC)."
        else
          ap.empty? ? idenN = "((#{@name}ResultSetImpl *)rs)->res[rsIndex]" : idenN = "((#{@name}ResultSetImpl *)rs)->res[rsIndex]."
        end
      end
    else
      ap.empty? ? idenF = "any_dstr" : idenF = "any_dstr->"
      ap.empty? ? idenN = "any_dstr" : idenN = "any_dstr->"
    end
    case op
    when "union"
      if !ap.empty? 
        if (@type.gsub(/ /,"").match(/(\w+)\*,/) &&
            ap.match(/first/)) || 
            (@type.gsub(/ /,"").match(/,(\w+)\*/) &&
            ap.match(/second/)) ||
            @type.gsub(/ /,"").match(/(\w+)\*/)
          if idenF.end_with?(".") && !@pointer.empty?
            idenF.chomp!(".")
            idenF.concat("->")
          end
          if idenN.end_with?(".") && !@pointer.empty?
            idenN.chomp!(".")
            idenN.concat("->")
          end
        end
      end
      if fk_type == "pointer"
        ap.concat("->")
      else
        ap.concat(".")
      end
    when "data"
      if !@container_class.empty?
        if ap.empty?
# Dereference what is there for all containers but not char *.
          if @pointer.end_with?("*") && data_type != "TEXT"
            idenF = "*#{idenF}"
            idenN = "*#{idenN}"
          end
        else
          if (ap == "first" && 
              @pointer.match(/\*,/)) ||
              (ap == "second" &&
               @pointer.end_with?("*"))
            idenF = "*#{idenF}"
            idenN = "*#{idenN}"
          elsif !ap.match(/first|key()/) &&
              !ap.match(/second|value()/) &&
              @pointer.end_with?("*")
            if idenF.end_with?(".")
              idenF.chomp!(".")
              idenF.concat("->")
            end
            if idenN.end_with?(".")
              idenN.chomp!(".")
              idenN.concat("->")
            end
          end
        end
      end
    when "fk"
# is object after transformations
      if !@container_class.empty?
        if !ap.empty? &&
            !ap.match(/first/) &&
            !ap.match(/second/) &&
            @pointer.end_with?("*")
          if idenF.end_with?(".")
            idenF.chomp!(".")
            idenF.concat("->")
          end
          if idenN.end_with?(".")
            idenN.chomp!(".")
            idenN.concat("->")
          end
        end
      end
      if (ap.empty? && 
          !@pointer.end_with?("*")) ||
          (!ap.empty? && 
           fk_type == "object")
        idenF = "&#{idenF}"
        idenN = "&#{idenN}"
      end
    end
    ap_copyF = String.new(ap)
    ap_copyN = String.new(ap)
    if !access_path.start_with?("tuple_iter") && access_path.match(/tuple_iter/)  # Substitute
      if ap.match(/tuple_iter\.|tuple_iter->/)
        ap_copyF.gsub!(/tuple_iter\.|tuple_iter->/, "#{idenF}")
        ap_copyN.gsub!(/tuple_iter\.|tuple_iter->/, "#{idenN}")
      end
      if ap.match(/\(tuple_iter,|\(tuple_iter\)|tuple_iter\)/)
        if idenF.end_with?(".")
          ap_copyF.gsub!(/\(tuple_iter,/, "(#{idenF.chomp(".")},")
          ap_copyF.gsub!(/\(tuple_iter\)/, "(#{idenF.chomp(".")})")
          ap_copyF.gsub!(/tuple_iter\)/, "#{idenF.chomp(".")})")
        elsif idenF.end_with?("->")
          ap_copyF.gsub!(/\(tuple_iter,/, "(#{idenF.chomp("->")},")
          ap_copyF.gsub!(/\(tuple_iter\)/, "(#{idenF.chomp("->")})")
          ap_copyF.gsub!(/tuple_iter\)/, "#{idenF.chomp("->")})")
        end
        if idenN.end_with?(".")
          ap_copyN.gsub!(/\(tuple_iter,/, "(#{idenN.chomp(".")},")
          ap_copyN.gsub!(/\(tuple_iter\)/, "(#{idenN.chomp(".")})")
          ap_copyN.gsub!(/tuple_iter\)/, "#{idenN.chomp(".")})")
        elsif idenN.end_with?("->")
          ap_copyN.gsub!(/\(tuple_iter,/, "(#{idenN.chomp("->")},")
          ap_copyN.gsub!(/\(tuple_iter\)/, "(#{idenN.chomp("->")})")
          ap_copyN.gsub!(/tuple_iter\)/, "#{idenN.chomp("->")})")
        end
      end
      if $argD == "DEBUG"
        puts "configure_search: substituting \"tuple_iter\" in access path:"
        puts "  F-#{idenF}, #{ap_copyF}"
        puts "  N-#{idenN}, #{ap_copyN}"
      end
      access_pathF = "#{ap_copyF}"
      access_pathN = "#{ap_copyN}"
    elsif !access_path.match(/tuple_iter/) # tuple_iter omitted; the usual case.
      access_pathF = "#{idenF}#{ap_copyF}"
      access_pathN = "#{idenN}#{ap_copyN}"
    elsif access_path.start_with?("tuple_iter.") # access path configured by user. expandable.
      idenF = "tuple_iter."
      idenN = "(*resIterC)."
      access_pathF = "#{ap_copyF}"
      access_pathN = "#{ap_copyN}"
    end
    return access_pathF, access_pathN, idenF, idenN
  end

# Configure loop variant for display.
  def display_loop(base)
    loop = String.new(@loop)
    if loop.match(/base/)
      loop.gsub!(/base\.|base->/, "#{base}->")
      loop.gsub!(/base/, "#{base}")
    else
      if $argD == "DEBUG"
        puts "Attention: not matched 'base' in #{@loop}."
      end
    end
    if $argD == "DEBUG"
      puts "Loop to display is #{loop}."
    end
    return loop
  end

# Configure iteration variant for display.
  def configure_iteration(access_path)
    if !@container_class.empty?
      if !@loop.empty?
# for C containers resBts has size 1 to differ from error conditions.
# Since we don't know size before hand we push_back as in result set.
# We need to start pushing from scratch.
# C container handling for both bindings (CPP, C}.
# Passing base as argument to display_loop:
# base is always "any_dstr".
        loop = display_loop("any_dstr")
        if @@C_container_types.include?(@container_class) && $argLB == "CPP"
          iterationF = "<space>rs->resBts.clear();\n<space>#{loop} {"
        else
          iterationF = "<space>#{loop} {"
        end
        # Hard-coded NULL check for container elements.
        if !@pointer.empty? && !@pointer.match(/,/)
          if @type.end_with?("iterator")  # C++ iterator over pointer type
            iterationF.concat("\n<space>  if (*tuple_iter == NULL) continue;")
          else                            # iterator is a pointer to the contained elements
            iterationF.concat("\n<space>  if (tuple_iter == NULL) continue;")
          end
        end
      else
        iterationF = "<space>for (tuple_iter = any_dstr->begin(); tuple_iter != any_dstr->end(); tuple_iter++) {"
        if !@pointer.empty? && !@pointer.match(/,/)
          # Hard-coded NULL check for container elements.
          # If '.' matches accessors [first,second] will
          # be checked as part of the access path. Nothing
          # to do here. 
          iterationF.concat("\n<space>  if (*tuple_iter == NULL) continue;")
        end
      end
      if $argLB == "CPP"
        iterationN = "<space>rsIndex = rs->resBts.find_first();\n<space>resIterC = rs->res.begin();\n<space>while (resIterC != rs->res.end()) {"
      else
        iterationN = "<space>rsIndex = 0;\n<space>while (!(rs->resBts[rsIndex / WORDBITS] & (1 << (rsIndex % WORDBITS)))) {rsIndex++;}\n<space>while (rsIndex < (int)rs->size) {"
      end
      return iterationF, iterationN
    end
    return "", ""
  end

# For specific container classes, those that offer
# associative or random access, generate code
# that leverages programming language algorithms
# for optimized query execution.
  def try_optimize(fw, access_path, sqlite3_type)
    space = "#{$s}"
    optimize = false
    if @container_class.match(/map$/i) && @type.end_with?("::iterator") &&  # unordered?
       access_path.match(/first|key\(\)/)
      if @container_class == "QMap" || @container_class == "QMultiMap"
        upBoundF = "upperBound"
        loBoundF = "lowerBound"
      else
        upBoundF = "upper_bound"
        loBoundF = "lower_bound"
      end
      if @signature.match(/(\w*)string,/i)  # also the various types of char * ?
        matchdata = @signature.match(/(\w*)string,/i)
        fw.puts "#{space}  #{matchdata[0].chomp(",")} s((const char *)sqlite3_value_text(val));"
        fw.puts "#ifdef PICO_QL_DEBUG"
        fw.puts "#{space}  printf(\"string key: %s\\n\", s.c_str());"
        fw.puts "#endif"
        rhs = "s"
      else
        rhs = "sqlite3_value_#{sqlite3_type}(val)"
      end
      case @container_class
      when /unordered_map/i
      when /unordered_multimap/i
      when /multimap/i    # ordered
        fw.puts "#{space}  int i;"
        fw.puts "#{space}  switch (op) {"
        fw.puts "#{space}  case 0:"
        fw.puts "#{space}    tuple_iter = any_dstr->#{upBoundF}(#{rhs});"
        fw.puts "#{space}    i = 0;"
        fw.puts "#{space}    for (#{@type} it = any_dstr->begin(); it != tuple_iter; it++) {"
        fw.puts "#{space}      rs->res.push_back(it);"
        fw.puts "#{space}      rs->resBts.set(i, 1);"
        fw.puts "#{space}      i++;"
        fw.puts "#{space}    }"
        fw.puts "#{space}    break;"
        fw.puts "#{space}  case 1:"
        fw.puts "#{space}    tuple_iter = any_dstr->#{loBoundF}(#{rhs});"
        fw.puts "#{space}    i = 0;"
        fw.puts "#{space}    for (#{@type} it = any_dstr->begin(); it != tuple_iter; it++) {"
        fw.puts "#{space}      rs->res.push_back(it);"
        fw.puts "#{space}      rs->resBts.set(i, 1);"
        fw.puts "#{space}      i++;"
        fw.puts "#{space}    }"
        fw.puts "#{space}    break;"
        fw.puts "#{space}  case 2:"
        fw.puts "#{space}  {"
        fw.puts "#{space}    pair<#{@type}, #{@type}> ret = any_dstr->equal_range(#{rhs});"
        fw.puts "#{space}    for (tuple_iter = ret.first; tuple_iter != ret.second; tuple_iter++) {"
        fw.puts "#{space}      rs->res.push_back(tuple_iter);"
        fw.puts "#{space}      i = 0;"
        fw.puts "#{space}      for (#{@type} it = any_dstr->begin(); it != tuple_iter; it++) i++;"
        fw.puts "#{space}      rs->resBts.set(i, 1);"
        fw.puts "#{space}    }"
        fw.puts "#{space}    break;"
        fw.puts "#{space}  }"
        fw.puts "#{space}  case 3:"
        fw.puts "#{space}    tuple_iter = any_dstr->#{loBoundF}(#{rhs});"
        fw.puts "#{space}    i = 0;"
        fw.puts "#{space}    for (#{@type} it = tuple_iter; it != any_dstr->end(); it++) {"
        fw.puts "#{space}      rs->res.push_back(it);"
        fw.puts "#{space}      rs->resBts.set(i, 1);"
        fw.puts "#{space}      i++;"
        fw.puts "#{space}    }"
        fw.puts "#{space}    break;"
        fw.puts "#{space}  case 4:"
        fw.puts "#{space}    tuple_iter = any_dstr->#{upBoundF}(#{rhs});"
        fw.puts "#{space}    i = 0;"
        fw.puts "#{space}    for (#{@type} it = tuple_iter; it != any_dstr->end(); it++) {"
        fw.puts "#{space}      rs->res.push_back(it);"
        fw.puts "#{space}      rs->resBts.set(i, 1);"
        fw.puts "#{space}      i++;"
        fw.puts "#{space}    }"
        fw.puts "#{space}    break;"
        fw.puts "#{space}  }"
      when /map/i         # ordered
        fw.puts "#{space}  int i;"
        fw.puts "#{space}  switch (op) {"
        fw.puts "#{space}  case 0:"
        fw.puts "#{space}    tuple_iter = any_dstr->#{upBoundF}(#{rhs});"
        fw.puts "#{space}    i = 0;"
        fw.puts "#{space}    for (#{@type} it = any_dstr->begin(); it != tuple_iter; it++) {"
        fw.puts "#{space}      rs->res.push_back(it);"
        fw.puts "#{space}      rs->resBts.set(i, 1);"
        fw.puts "#{space}      i++;"
        fw.puts "#{space}    }"
        fw.puts "#{space}    break;"
        fw.puts "#{space}  case 1:"
        fw.puts "#{space}    tuple_iter = any_dstr->#{loBoundF}(#{rhs});"
        fw.puts "#{space}    i = 0;"
        fw.puts "#{space}    for (#{@type} it = any_dstr->begin(); it != tuple_iter; it++) {"
        fw.puts "#{space}      rs->res.push_back(it);"
        fw.puts "#{space}      rs->resBts.set(i, 1);"
        fw.puts "#{space}      i++;"
        fw.puts "#{space}    }"
        fw.puts "#{space}    break;"
        fw.puts "#{space}  case 2:"
        fw.puts "#{space}    tuple_iter = any_dstr->find(#{rhs});"
        fw.puts "#{space}    if (tuple_iter != any_dstr->end()) {"
        fw.puts "#{space}      rs->res.push_back(tuple_iter);"
        fw.puts "#{space}      i = 0;"
        fw.puts "#{space}      for (#{@type} it = any_dstr->begin(); it != tuple_iter; it++) i++;"
        fw.puts "#{space}      rs->resBts.set(i, 1);"
        fw.puts "#{space}    }"
        fw.puts "#{space}    break;"
        fw.puts "#{space}  case 3:"
        fw.puts "#{space}    tuple_iter = any_dstr->#{loBoundF}(#{rhs});"
        fw.puts "#{space}    i = 0;"
        fw.puts "#{space}    for (#{@type} it = tuple_iter; it != any_dstr->end(); it++) {"
        fw.puts "#{space}      rs->res.push_back(it);"
        fw.puts "#{space}      rs->resBts.set(i, 1);"
        fw.puts "#{space}      i++;"
        fw.puts "#{space}    }"
        fw.puts "#{space}    break;"
        fw.puts "#{space}  case 4:"
        fw.puts "#{space}    tuple_iter = any_dstr->#{upBoundF}(#{rhs});"
        fw.puts "#{space}    i = 0;"
        fw.puts "#{space}    for (#{@type} it = tuple_iter; it != any_dstr->end(); it++) {"
        fw.puts "#{space}      rs->res.push_back(it);"
        fw.puts "#{space}      rs->resBts.set(i, 1);"
        fw.puts "#{space}      i++;"
        fw.puts "#{space}    }"
        fw.puts "#{space}    break;"
        fw.puts "#{space}  }"
      end
      optimize = true
    elsif @container_class.match(/vector$/i) && access_path == "rownum"  # works only for ==
      fw.puts "        tuple_iter = any_dstr->begin() + rowNum;"
      optimize = true
    end
    return optimize
  end

# Generate code for rownum column in search function.
# Because rownum column is generated for CPP binding only
# this code will only be generated for CPP binding.
# Works only for == [TODO - extend]
  def search_rownum(fw)
    fw.puts "        rowNum = sqlite3_value_int(val);"
    if !@@C_container_types.include?(@container_class) && @loop.empty?
      fw.puts "        if (rowNum > (int)rs->resBts.size()) {"
    end
    if !@loop.empty?
      fw.puts "        #{display_loop("any_dstr")} {"
      fw.puts "#{$s}  if (rowNum == i) {"
      if $argLB == "CPP"
        fw.puts "#{$s}    found = true;"
      else
        fw.puts "#{$s}    found = 1;"
      end
      fw.puts "#{$s}    break;"
      fw.puts "#{$s}  }"
      fw.puts "#{$s}  i++;"
      fw.puts "        }"
      for i in 0..@nloops-1    # Generate closing curly braces for nested loops
        fw.puts "        }"
      end
      fw.puts "        if (!found) {"
    end
    fw.puts "          rs->res.clear();"
    fw.puts "          rs->resBts.clear();"
    fw.puts "          return SQLITE_OK;" 
    fw.puts "        }"
    if !@@C_container_types.include?(@container_class) && @loop.empty?
      if !try_optimize(fw, "rownum", nil)
        fw.puts "        tuple_iter = any_dstr->begin();"
        fw.puts "        for (int i = 0; i < rowNum; i++)"
        fw.puts "          tuple_iter++;"
      end
    end
    fw.puts "        if (first_constr == 1) {"
    if @@C_container_types.include?(@container_class)
      fw.puts "#{$s}  rs->resBts.resize(rowNum + 1, 0);"
    end
    fw.puts "#{$s}  rs->res.push_back(tuple_iter);"
    fw.puts "#{$s}  rs->resBts.set(rowNum, 1);"
    fw.puts "        } else {"
    fw.puts "#{$s}  if (rs->resBts.test(rowNum)) {"
    fw.puts "#{$s}    rs->resBts.reset();"
    fw.puts "#{$s}    rs->resBts.set(rowNum, 1);"
    fw.puts "#{$s}    rs->res.clear();"
    fw.puts "#{$s}    rs->res.push_back(tuple_iter);"
    fw.puts "#{$s}    rs->resIter = rs->res.begin();"
    fw.puts "#{$s}  } else {"
    fw.puts "#{$s}    rs->resBts.clear();"
    fw.puts "#{$s}    rs->res.clear();"
    fw.puts "#{$s}  }"
    fw.puts "        }"
    fw.puts "        break;"
  end

# Generate code for base column in search function.
  def search_base(fw)
    fw.puts "        if (first_constr == 1) {"
    iterationF, useless = configure_iteration("")
    add_to_result_setF, useless = configure_result_set()
    if !@container_class.empty?
      fw.puts "#{iterationF.gsub(/<space>/, "#{$s}  ")}"
    end
    # Customizing..each gsub targets a single case so safe.
    add_to_result_setF.gsub!(/\n<space>  \} else \{\n<space>    rs->resBts.push_back\(0\);\n<space>  \}/, 
                             "") 		       # CPP, C_containers
    if !@@C_container_types.include?(@container_class) # All cases except CPP, C containers: 
      add_to_result_setF.gsub!(/\n<space>\}/, "")      # no compare to close if in base.
    else 					       # CPP, C containers: configure spacing
      add_to_result_setF.gsub!(/\n<space>  \}\n<space>\}/, "\n<space>    }\n<space>  }")
      if $argLB == "C" 				       # C, C containers : remove extra '}'
        add_to_result_setF.gsub!(/\n<space>    \}\n<space>  \}/, "\n<space>  }")
      end
    end
    add_to_result_setF.gsub!(/\n<space>  rsIndex\+\+;/, "") # CPP, CPP_containers
    add_to_result_setF.gsub!(/rsIndex/, "rsIndex++")
    fw.puts "#{add_to_result_setF.gsub("<space>", "        ")}"
    fw.puts "        } else {"
    fw.puts "#{$s}  printf(\"Constraint for BASE column on embedded data structure has not been placed first. Exiting now.\\n\");"
    fw.puts "#{$s}  return SQLITE_MISUSE;"
    fw.puts "        }"
    fw.puts "        break;"
  end
  
# Generates code to search each VT struct.
# Each search case matches a specific column of the VT.
  def search_columns(fw)
    fw.puts "    switch (nCol) {"
    @columns.each_index { |col| @columns[col].search(fw, self, col) }
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
          puts "LINE is #{$lined_description[line]}"
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
      when /(\w+)<(.+)>(\**):(.+)/m       # Negative lookahead does not work for ':(?!:). 
        @signature.gsub!(/:{3}/, "[DDD]") # So hacking it non-elegantly. Seek alternative.'
        @signature.gsub!(/:{2}/, "[DD]")
        if @signature.match(/(.+):(.+)/)  # USING LOOP implied
          matchdata = @signature.match(/(.+):(.+)/)
          @signature = matchdata[1]
          @type = matchdata[2]
          @signature.gsub!(/\[DDD\]/, ":::")
          @signature.gsub!(/\[DD\]/, "::")
          matchdata1 = @signature.match(/(\w+)<(.+)>/)
          @container_class = matchdata1[1]
          @type.gsub!(/\[DDD\]/, ":::")
          @type.gsub!(/\[DD\]/, "::")
          @signature.rstrip!
          @type.rstrip!
          if !@signature.end_with?("*")
            @signature.concat("*")
          end
          @signature_pointer = "*"
          if @type.end_with?("::iterator")
            if matchdata1[2].match(/,/)
              type_array = matchdata1[2].split(",")
              if type_array[0].end_with?("*")
                @pointer.concat("*,")
              else  # Don't like this
                @pointer.concat(",")
              end
              if type_array[1].end_with?("*")
                @pointer.concat("*")
              end
            elsif matchdata1[2].end_with?("*")
              @pointer = "*"
            end
          elsif @type.end_with?("*")
              @pointer = "*"
          end
        end
      when /(\w+)<(.+)>(\**)/m
        matchdata = /(\w+)<(.+)>(\**)/m.match(@signature)
        @type = @type.replace(@signature).chomp("*").concat("::iterator")
        if matchdata[3].empty?
          @signature.concat("*")
        end
        @signature_pointer = "*"
        @container_class = matchdata[1]
        if @type.match(/,/)
          type_array = matchdata[2].split(",")
          if type_array[0].end_with?("*")
            @pointer.concat("*,")
          else  # Don't like this
            @pointer.concat(",")
          end
          if type_array[1].end_with?("*")
            @pointer.concat("*")
          end
        elsif matchdata[2].end_with?("*") # We care about the record type, not the iterator type
          @pointer = "*"
        end
        if $argD == "DEBUG"
          puts "Virtual table container class name is: #{@container_class}."
          puts "Virtual table record is of type: #{@type}"
          puts "Virtual table type is of type pointer: #{@pointer}"
        end
      when /(\w+)\*|(\w+)/
        if @signature.match(/(.+):(.+)/) && !@signature.match(/::/)
          matchdata = @signature.match(/(.+):(.+)/) # C container.
          @signature = matchdata[1]
          @type = matchdata[2]
          @signature.rstrip!
          @type.rstrip!
          if !@signature.end_with?("*")     # How sound is this block?
            if !@signature.end_with?("]")   # Not an array.
              @signature.concat("*")
            else
              if !@signature.match(/\(\*\)(\s*)\[/)
                @signature.sub!(/\[/, "(*)[")
              end
              @assignable_signature = @signature.gsub(/\(\*\)(.+)/, '(*<variable_name>)\1')
            end
          end
          @signature_pointer = "*"
          if @type.end_with?("*")
            @pointer = "*"
          end
        else
          if !@signature.end_with?("*")
            @signature.concat("*")
          end
          @signature_pointer = "*"
          @type = @signature
          @pointer = @signature_pointer
        end
        if !@loop.empty?                # C style container.
          @container_class = "c_container"
        else
          @object_class = @signature
        end
      when /(.+)/
        raise "Template instantiation faulty: #{@signature}.\n"
      end
      if @assignable_signature.empty?
        @assignable_signature = @signature
      end
      if $argD == "DEBUG"
        puts "Table object class name : #{@object_class}"
        puts "Table container class name : #{@container_class}"
        puts "Table base is of type : #{@signature}"
        puts "Table assignable base is of type : #{@assignable_signature}"
        puts "Table base is pointer: #{@signature_pointer}"
        puts "Table record is of type: #{@type}"
        puts "Table type is of type pointer: #{@pointer}"
      end
    rescue
      puts "Template instantiation faulty: #{@signature}.\n"
      exit(1)
    end
  end

# Process lock directive.
  def process_lock()
    if @lock.match(/record lock (.+)/i)
      matchdata = @lock.split(/record lock (.+)/i)
      @record_lock = "#{matchdata[1]}"
      @lock.gsub!(/record lock (.+)/i, "")
    end
    @lock, @lock_class, @lock_argument = Lock.process_lock(@lock)
    if !@record_lock.empty? 
      @record_lock, @record_lock_class, @record_lock_argument = Lock.process_lock(@record_lock)
    end
  end

# Isolate root address of C container for NULL checking.
  def process_loop()
    matched = 0
    @loop.chomp!("{")
    @nloops = @loop.count("{")
    if @nloops > 0
      @loop.gsub!(/\{ /, "{\n<space>") # Configure spacing for nested loop.
    end 			       # Opening curly braces required.
    if !@signature.match(/<|>/) &&     # C container.
       @signature.match(":")           # C array possibly.
      matchdata = @loop.split(/,|;/)   # Too loose (wide) a match
      matchdata.each { |m|             # perhaps?
        if $argD == "DEBUG"
          puts "Matching #{m}"
        end
        case m
        when /base\.(.+)\./
          matchdata2 = m.match(/base\.(.+)\./)
        when /base->(.+)\./
          matchdata2 = m.match(/base->(.+)\./)
        when /base\.(.+)->/
          matchdata2 = m.match(/base(.+)->/)
        when /base->(.+)->/
          matchdata2 = m.match(/base->(.+)->/)
        when /base\.(.+)/
          matchdata2 = m.match(/base\.(.+)/)
        when /base->(.+)/
          matchdata2 = m.match(/base->(.+)/)
        when /base/
          matchdata2 = m.match(/base/)  # see for example skb_queue_walk_safe 
        end			        # for Linux kernel.
        if matchdata2
          matched = 1 			# We are insterested to catch the C 
          if matchdata2[1] 		# container's start address. Most probably 
            @loop_root = "&any_dstr->#{matchdata2[1]}" # it will be an array. Therefore, 
            if $argD == "DEBUG" 	# we hard-code '&'. For arrays of primitive types,
              puts "@loop = #{@loop}" 	# '&' will probably preceded 'base'. It is discarded
              puts "@nloops = #{@nloops.to_s}"   # in the patterns and refit here. Otherwise, 
              puts "@loop_root = #{@loop_root}"  # it will probably not be there (in order to
              puts "matchdata[0] = #{matchdata2[0]}"   # talk about the array's first element)
              puts "matchdata[1] = #{matchdata2[1]}"   # but we want it there.
            end
          end
          break
        end
      }
      if !matched
        if $argD == "DEBUG" 	# we hard-code '&'. For arrays of primitive types,
          puts "Attention: not matched 'base' in #{@loop}."
        end
      end
    end
  end

# Matches VT definitions against prototype patterns.
  def match_table(table_description)
    table_ptn1 = /^create virtual table (\w+) using struct view (\w+) with registered c name (.+) with registered c type (.+) using loop (.+) using lock (.+)/im
    table_ptn2 = /^create virtual table (\w+) using struct view (\w+) with registered c type (.+) using loop (.+) using lock (.+)/im
    table_ptn3 = /^create virtual table (\w+) using struct view (\w+) with registered c type (.+) using lock (.+)/im
    table_ptn4 = /^create virtual table (\w+) using struct view (\w+) with registered c name (.+) with registered c type (.+) using loop (.+)/im
    table_ptn5 = /^create virtual table (\w+) using struct view (\w+) with registered c type (.+) using loop (.+)/im
    table_ptn6 = /^create virtual table (\w+) using struct view (\w+) with registered c name (.+) with registered c type (.+)/im
    table_ptn7 = /^create virtual table (\w+) using struct view (\w+) with registered c type (.+)/im
    if $argD == "DEBUG"
      puts "Table description is: #{table_description}"
    end
    struct_view_name = ""
    case table_description
    when table_ptn1
      matchdata = table_ptn1.match(table_description)
      @name = matchdata[1]
      struct_view_name = matchdata[2]
      @base_var = matchdata[3]
      @signature = matchdata[4]
      @loop = matchdata[5]
      process_loop()
      @lock = matchdata[6]
      process_lock()
    when table_ptn2
      matchdata = table_ptn2.match(table_description)
      @name = matchdata[1]
      struct_view_name = matchdata[2]
      @signature = matchdata[3]
      @loop = matchdata[4]
      process_loop()
      @lock = matchdata[5]
      process_lock()
    when table_ptn3
      matchdata = table_ptn3.match(table_description)
      @name = matchdata[1]
      struct_view_name = matchdata[2]
      @signature = matchdata[3]
      @lock = matchdata[4]
      process_lock()
    when table_ptn4
      matchdata = table_ptn4.match(table_description)
      @name = matchdata[1]
      struct_view_name = matchdata[2]
      @base_var = matchdata[3]
      @signature = matchdata[4]
      @loop = matchdata[5]
      process_loop()
    when table_ptn5
      matchdata = table_ptn5.match(table_description)
      @name = matchdata[1]
      struct_view_name = matchdata[2]
      @signature = matchdata[3]
      @loop = matchdata[4]
      process_loop()
    when table_ptn6
      matchdata = table_ptn6.match(table_description)
      @name = matchdata[1]
      struct_view_name = matchdata[2]
      @base_var = matchdata[3]
      @signature = matchdata[4]
    when table_ptn7
      matchdata = table_ptn7.match(table_description)
      @name = matchdata[1]
      struct_view_name = matchdata[2]
      @signature = matchdata[3]
    end
    verify_signature()
    $struct_views.each { |sv| 
      if sv.name == struct_view_name 
        @struct_view = sv 
      end 
    }
    begin
      if @struct_view == nil
        raise "Cannot match struct_view for table #{@name}.\\n"
      end
    rescue
      if @name.empty? 
        @name = "<empty_name>" 
        puts "Perhaps you have forgotten to type the database name, e.g. CREATE VIRTUAL TABLE DB_NAME.TABLE_NAME"
      end
      puts "Cannot match struct_view for table #{@name}."
      exit(1)
    end
    $table_index[@name] = @signature
    if @base_var.empty?             # base column for embedded structs.
      @columns.push(Column.new("")).last.set("base INT FROM base")
    end  			    # access path is just a placeholder; never used
    if !@container_class.empty? && $argLB == "CPP"
      @columns.push(Column.new("")).last.set("rownum INT FROM rownum") # ditto
    end
    @include_text_col = @struct_view.include_text_col
    @columns = @columns | @struct_view.columns  # A struct view may be used by many VTs
    register_line()
    if $argD == "DEBUG"
      puts "Table name is: #{@name}"
      puts "Table's C NAME defined at line #{@base_var_line + 1} of #{$argF}"
      puts "Table's C TYPE defined at line #{@signature_line + 1} of #{$argF}"
      puts "Table base variable name is: #{@base_var}"
      puts "Table signature name is: #{@signature}"
      puts "Table follows struct_view: #{@struct_view.name}"
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
    if matchdata                     # First record of table_data contains the whole 
      @name = matchdata[1]           # description of the structview.
      all_columns = String.new(matchdata[3]) 
      columns_str = Array.new
      if all_columns.match(/\{(.+?)\}/)       # For access paths in code block format
        all_columns.gsub!(/\{(.+?)\}/) { |m|  # we need to substitute "," in the code
          m.gsub!(/,/, "#")                   # block with "#" to allow splitting the
          "#{m}"                              # column descriptions using the SQL-reminiscent
        }                                     # "," delimeter. Seek alternative.
      end
      if all_columns.match(/,/)
        columns_str = all_columns.split(/,/)
      else
        columns_str[0] = all_columns
      end
    end
    begin
      columns_str.each { |c| @include_text_col += @columns.push(Column.new("")).last.set(c) }
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

# Extract the relational view's name.
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

# Match union view definition to DSL specification.
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
      puts "Union view switch statement is: #{@switch}."
      puts "Union view includes #{@include_text_col} columns of type text."
      puts "Columns follow:"
      @columns.each { |x| p x }
    end
  end

end


class Lock
  def initialize
    @name = ""
    @lock_function = ""
    @unlock_function = ""
  end
  attr_accessor(:name, :lock_function, 
                :unlock_function)

  def self.process_lock(lock_description)
    lock = lock_description
    lock_argument = ""
    if lock.match(/\(/)
      matchdata = lock.split(/\(|\)/)
      lock_name = matchdata[0]
      if matchdata[1]
        lock_argument = matchdata[1]
      end
      if !lock_argument.empty?
         if lock_argument.match(/base\.|base->/)
           lock.gsub!(/base\.|base->/, "any_dstr->")
           lock_argument.gsub!(/base\.|base->/, "any_dstr->")
         elsif lock_argument.match(/base/)
      #puts "Lock is: #{lock}."
      #puts "Lock argument is: #{lock_argument}."
           lock.gsub!(/base/, "any_dstr")
           lock_argument.gsub!(/base/, "any_dstr")
         end
      end
    else
      lock_name = lock
    end
    lock_class = nil
    $locks.each { |l|
      if l.name == "#{lock_name}"
        lock_class = l 
        break
      end
    }
    if lock_class == nil
      puts "Lock class for lock #{lock} not found."
      puts "Exiting now."
      exit(1)
    end
    if $argD == "DEBUG"
      puts "Processed lock is #{lock}"
      puts "Processed lock name is #{lock_class.name}"
      puts "Processed lock argument is #{lock_argument}"
    end
    return lock, lock_class, lock_argument
#TODO: NULL checking
#NULL checking as implemented below might need improvement.
#Hint:pointer vs instance
  end

  def match_lock(lock_description)
    lock_ptn = /create lock (.+) hold with (.+) release with (.+)/im
    case lock_description
    when lock_ptn
      matchdata = lock_ptn.match(lock_description)
      lock_match = matchdata[1].match(/\((.+)\)/)
      if lock_match
        @lock_function = matchdata[2].gsub(lock_match[1], "<selector>")
        @unlock_function = matchdata[3].gsub(lock_match[1], "<selector>")
        @name = matchdata[1].gsub(/\((.+)\)/, "")  # Remove potential argument
      else
        @name = matchdata[1]
        @lock_function = matchdata[2]
        @unlock_function = matchdata[3]
      end
    end
    if $argD == "DEBUG"
      puts "Lock description: #{lock_description}"
      puts "Lock class name: #{@name}"
      puts "Lock function: #{@lock_function}"
      puts "Unlock function: #{@unlock_function}"
      puts "Lock class #{@name} registered."
    end
  end
  
end


# Models the input description.
class InputDescription
  def initialize(description)
    # original description tokenised in an Array using '$' delimeter
    @description = description
    # array with entries the identity of each virtual table
    @tables = Array.new
    @views = Array.new
    @directives = ""
  end
  attr_accessor(:description,:tables,:views,:directives)

# Get the context of code evaluation.
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
# functions in pico_ql_search.{c, cpp}
  def print_register_serve(fw)
    file = File.open("pico_ql_erb_templates/pico_ql_register_serve_#{$argLB.downcase}.erb").read
    app_interface = ERB.new(file, 0, '>')
    fw.puts app_interface.result(get_binding)
  end

# Generates the LICENSE copyright notice and application interface 
# functions in pico_ql_search.h
  def print_search_h(fw)
    file = File.open("pico_ql_erb_templates/pico_ql_search_h.erb").read
    app_interface = ERB.new(file, 0, '>')
    fw.puts app_interface.result(get_binding)
  end

# Generates application-specific code to complement the PiCO QL library.
# There is a call to each of the above generative functions.
  def generate()
    myfile = File.open("pico_ql_search.h", "w") do |fw|
      print_search_h(fw)
    end
    puts "Created/updated pico_ql_search.h"
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
    if $argK != "KERNEL" && $argVLG != "VALGRIND"
      myFile = File.open("pico_ql_makefile.append", "w") do |fw|
        file = File.open("pico_ql_erb_templates/pico_ql_makefile_#{$argLB.downcase}.erb").read
        makefile = ERB.new(file, 0, '>')
        fw.puts makefile.result(get_binding)
      end
      puts "Created/updated pico_ql_makefile.append ."
    end
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
    if token_d[0].match(/#include|#ifndef|#define/) # Acknowledge boilerplate C
      @directives = token_d[0]			     # code section in the upper
      token_d.delete_at(0)			     # part of the DSL description.
      if $argD == "DEBUG"
        line = 0        	      # Put compiler line directives in include directives.
        if @directives.match(/\n/)
          @directives.gsub!(/\n/){ |nl|
            "\n#line #{(line += 1).to_s} \"#{$argF}\"" + nl
          }
        end
        puts "Directives: #{@directives}"
      end
    end
    token_d.each { |x|            
      if /\/\*(.+?)\*\//m.match(x)  		 # Clean multi-line comments
        x.gsub!(/\/\*(.+?)\*\//m, "") 		 # from the body of the DSL description.
      end
      if /\n|\t|\r|\f/.match(x)   		# Clean white space.
        x.gsub!(/\n|\t|\r|\f/, " ") 
      end
      x.lstrip!
      x.rstrip!
      x.squeeze!(" ")
      if / ,|, /.match(x)
        x.gsub!(/ ,|, /, ",") 
      end
    }
    @description = token_d.select{ |x| x =~ /(\S+)/ }
    if $argD == "DEBUG"
      puts "Description after whitespace cleanup: "
      @description.each { |x| p x }
    end
    $struct_views = Array.new
    $union_views = Array.new
    $locks = Array.new
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
      when /^create lock/im
        $locks.push(Lock.new).last.match_lock(stmt)
      end
      w += 1
    }
    if $argD == "DEBUG"
      puts "Table index entries:"
      $table_index.each_pair { |k,v| p "#{k}-#{v}"}
    end
  end
  
end


# Compare kernel version numbers
def cmp_v(condition, lhs, rhs)
  if $argD == "DEBUG"
    puts "Comparing kernel versions: #{lhs} #{condition} #{rhs}\n"
  end
  case condition
  when "<"
    return lhs < rhs
  when "<="
    return lhs <= rhs
  when "="
    return lhs == rhs
  when "=="
    return lhs == rhs
  when ">="
    return lhs >= rhs
  when ">"
    return lhs > rhs
  end
end

# Reform kernel version no part to comparable size.
def form(version_part, cmp_size)
  version_part.lstrip!
  version_part.rstrip!
  if $argD == "DEBUG"
    puts "version_part initially is: #{version_part} and has length #{version_part.size.to_s}"
  end
  while version_part.size < cmp_size
    version_part.insert(0, '0')
  end
  if $argD == "DEBUG"
    puts "version_part now is: #{version_part}"
  end
  return version_part
end

# Check DSL-given kernel version against 
# this machine's kernel version.
def kernel_version_match(condition, version, subv, ssv)
  machine_kernel_version = `uname -r`
  if $argD == "DEBUG"
    puts "Machine kernel version: #{machine_kernel_version}"
    puts "Compare if #{condition} than kernel version: #{version}.#{subv}.#{ssv}"
  end
  matchdata = machine_kernel_version.match(/(\d)\.(\d{1,2})(\.(\d{1,3})?)/) 
  if $argD == "DEBUG"
    puts "machine kernel version tokenized: #{matchdata.inspect}"
  end
  version << form(subv, 2) << form(ssv, 3)
  version_no_form = version.to_i
  machine_kernel_version_tokenized = String.new(matchdata[1])
  machine_kernel_version_tokenized << form(matchdata[2], 2)
  if matchdata[4] != nil
    machine_kernel_version_tokenized << form(matchdata[4], 3)
  end
  machine_kernel_version_no_form = machine_kernel_version_tokenized.to_i
  return cmp_v(condition, machine_kernel_version_no_form, version_no_form) 
end

# Compare multiple kernel versions to the underlying one
# in order to branch correctly to the if part or the else part.
def compare_many_versions(versions, action_true, action_false)
  cmp_versions = String.new(versions)
  if $argD == "DEBUG"
    puts "Kernel versions for comparison are: #{cmp_versions}."
  end
  cmp_versions.lstrip!
  cmp_versions.rstrip!
  if $argD == "DEBUG"
    puts "(After cleaning up spaces) kernel versions for comparison are: #{cmp_versions}."
  end
  if cmp_versions.match(/\d\s?(<|<=|=|==|>=|>)/)    # #if KERNEL_VERSION < 3.2.0 >= 3.14.4
    cmp_versions.gsub!(/(<|<=|=|==|>=|>)(\s?)(\d)/, '\1\3')
    if $argD == "DEBUG"
      puts "(After removing unneeded spaces) kernel versions for comparison are: #{cmp_versions}."
    end
    version = cmp_versions.split(" ")
    condition = true
    # && between the various conditions is implied
    version.each { |v|
      mv = v.match(/(<|<=|=|==|>=|>)(\d)\.(\d{1,2})(\.(\d{1,3})?)/)
      if !kernel_version_match($1, $2, $3, $5)
        condition = false
        action = action_false
      end
    }
    if condition == true
      action = action_true
    end
    return true, action
  else
    return false, nil
  end
end
   

# Take cases on command-line arguments.
def take_cases(argv)
  case argv
  when /debug/i
    $argD = "DEBUG"
  when /no_mem_mgt/i
    $argM = "NO_MEM_MGT"
  when /C/i
    $argLB = "C"
  when /kernel/i
    $argK = "KERNEL"
    $argLB = "C"
  when /valgrind/i
    $argVLG = "VALGRIND"
    $argLB = "C"
  when /concept_check/i
    $argC = "CONCEPT_CHECK"
  end
end


# The main method.
if __FILE__ == $0
  $argF = ARGV[0]
  $argM = "MEM_MGT"
  $argLB = "CPP"
  $argK = ""
  $argVLG = ""
  $argC = ""
  $argCNST = ""
  ARGV.each_index { |arg| 
    if arg > 0
      take_cases(ARGV[arg]) 
    end 
  }
  begin
    $lined_description = File.open($argF, "r") { |fw| 
      fw.readlines.each{ |line|   # Drop single-line comments, but
        if line.match(/\/\/(.+)/) # multi-line comments retained in upper part.
          line.gsub!(/\/\/(.+)/, "")
        end
      }
    }
#    $lined_description = $lined_description.select { |line| !line.empty? }
  rescue Exception => e
    puts e.message
    exit(1)
  end
  $lined_description.each { |line|  # Strip white-space.
    line.squeeze!(" ")
    if / ,|, /.match(line)
      line.gsub!(/ ,|, /, ",") 
    end
    #      if / \(/.match(line) : line.gsub!(/ \(/, "(") end
    if /\( /.match(line)
      line.gsub!(/\( /, "(") 
    end
    #      if /\) /.match(line) : line.gsub!(/\) /, ")") end
    if / \)/.match(line) 
      line.gsub!(/ \)/, ")") 
    end
  }
  description = $lined_description.join
  if $argK == "KERNEL"
    # A further (\.(\d{1,3})?) for case e.g. 3.2.4.245 breaks the pattern
    description.gsub!(/^#if KERNEL_VERSION((\s?(<|<=|=|==|>=|>)\s?(\d)\.(\d{1,2})(\.(\d{1,3})?))+)(\s+)(.+?)\n#((else(\s+)(.+?)\n#)*)endif/im) { |m|
      if $argD == "DEBUG"
        puts "m initially is: #{m}\n1:#{$1}\n2:#{$2}\n3:#{$3}\n4:#{$4}\n5:#{$5}\n6:#{$6}\n7:#{$7}\n8:#{$8}\n9:#{$9}\n10:#{$10}\n11:#{$11}\n12:#{$12}\n13:#{$13}\n14:#{$14}\n15:#{$15}\n16:#{$16}"
      end
      valid, m = compare_many_versions($1, $9, $13)
      if valid == false
        if kernel_version_match($3, $4, $5, $7)
          m = $9
        else
          m = $13
        end
      end
      if $argD == "DEBUG"
        puts "Result of condition is: #{m}\n"
      end
      "#{m}"   # The actual substitution.
    }
    kvm_is_running = system("lsmod | grep 'kvm'")
    if $argD == "DEBUG"
      puts "kvm is running? #{kvm_is_running.to_s}\n"
    end
    description.gsub!(/^#if KVM_RUNNING\n(.+?)\n#endif KVM_RUNNING/m) { |m|
      kvm_is_running ? m = $1 : m = ""
      "#{m}"   # The actual substitution.
    }
  end
  begin
    token_description = description.split(/\$/)
  rescue
    puts "Invalid description..delimeter '$' not used."
    exit(1)
  end
  $s = "        "    # Global variable for space abbreviation.
  ip = InputDescription.new(token_description)
  ip.register_datastructures()
  ip.generate()
end
