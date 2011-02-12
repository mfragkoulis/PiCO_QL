
# raise alarm if special characters are used in description

# what's the use of the class?holds template instantiation inside Register
 class Template
      
      def initialize
        @template1 = Hash.new
	@template2 = Hash.new
      end
      attr_accessor(:template1,:template2)

 end
      
# used in Register to store the iam's of the data structure
# as shown below
class Data_structure_characteristics

      def initialize
      	  @name = ""
	  @level = ""
	  @used_create = 0
	  @used_gen_size = 0
	  @used_gen_def_sch = 0
	  @used_gen_def_rtv = 0
	  @used_check = 0
	  @used_retrieve = 0
	  @iter_var = ""
	  @index_naked = ""
# content: map<string,Truck*>
	  @signature = ""	
# content: string,Truck
	  @pure_signature = ""
	  @stl_class = ""
	  @children = Hash.new
	  @classnames = Hash.new 
	  @template1_type = "" 
	  @template1_name = ""
	  @template2_type = ""
	  @template2_name = ""
      end
      
      attr_accessor(:name,:level,:used_create,:used_gen_size,
      :used_gen_def_sch,:used_gen_def_rtv,:used_check,:used_retrieve,
      :iter_var,:index_naked,:signature,:pure_signature,:stl_class,:children,
      :classnames,:template1_type,:template1_name,
      :template2_type,:template2_name)

end

class Input_Description

      def initialize(description="")
          @description = description
	  @column_traverse = ""
	  @jump = 0
	  @back = 0
# temp
	  @was_inheritance = 0
	  @inheritance = 0
	  @action = Array.new
	  @follow_up = Array.new
	  @embedded_loop = Array.new
	  @embedded_index = Array.new
	  @format = "\"%d."
	  @index_vars = " i, "
	  @def_children = ""
	  @finish_string = ""


# keep iter_var of parent data structure to refer to from embedded
	  @parent_iter_var = ""

# key: data_structure_name, value: position in @...array
	  @data_structures_names=Hash.new
	  @data_structures_array=Array.new
          @ds_nested_names = Array.new

# key: class name, value: class type
	  @classnames=Hash.new
	  @classes_inserted = Array.new
	  @container_type=""
	  @template_args=""
	  @query=""
	  @queries=Array.new
	  @s = "        "
      end


      def ds_unravel(index, operation)
        puts "ds_unravel"
	tmpr_ds = Hash.new
	tmpr_chars = Data_structure_characteristics.new
	tmpr_keys = Array.new
	tmpr_template = Template.new
	tmpr_class1 = Hash.new
	tmpr_class2 = Hash.new

	puts "ds_un index: " + index.to_s
	tmpr_ds = @data_structures_array[index]

# extract keys from original beasty hash
# should contain only one key of type Data_structure_characteristics
	tmpr_keys = tmpr_ds.keys

# tmpr_keys length should be one
	tmpr_chars = tmpr_keys[0]

# return nil when 
# create & 1
# create & embedded
# create_embed & 1

# so match-create & 1
# =create & embedded & 0

	if operation.match(/create/) 
	  if tmpr_chars.used_create == 1 || 
	       (operation == "create" && tmpr_chars.level == "embedded")
	    return nil
	  else
	    tmpr_chars.used_create = 1
	  end
	elsif operation.match(/gen_size/) 
# no generate_nested. we don't want to generate size information for
# children data structures
	  if tmpr_chars.used_gen_size == 1 || 
	       (tmpr_chars.level == "embedded")
	    return nil
	  else
	    tmpr_chars.used_gen_size = 1
	  end
	elsif operation.match(/gen_def_sch/) 
	  if tmpr_chars.used_gen_def_sch == 1 || 
	       (tmpr_chars.level == "top")
	    return nil
	  else
	    tmpr_chars.used_gen_def_sch = 1
	  end
	elsif operation.match(/gen_def_rtv/) 
	  if tmpr_chars.used_gen_def_rtv == 1 || 
	       (tmpr_chars.level == "top")
	    return nil
	  else
	    tmpr_chars.used_gen_def_rtv = 1
	  end
	elsif operation.match(/check/) 
	  if tmpr_chars.used_check == 1 || 
	       (operation == "check" && tmpr_chars.level == "embedded")
	    return nil
	  else
	    tmpr_chars.used_check = 1
	  end
	elsif operation.match(/retrieve/) 
	  if tmpr_chars.used_retrieve == 1 || 
	       (operation == "retrieve" && tmpr_chars.level == "embedded")
	    return nil
	  else
	    tmpr_chars.used_retrieve = 1
	  end
	end

	tmpr_template = tmpr_ds.fetch(tmpr_chars)
        if tmpr_chars.template1_type != "none"
	  tmpr_class1 = tmpr_template.template1
	else
	  puts "ds_unravel: NO template 1"
	  tmpr_class1 = nil
        end        
	tmpr_class2 = tmpr_template.template2
	return tmpr_chars, tmpr_class1, tmpr_class2
      end



# takes as input a template description and an array
# containing attributes of a class. template description is a Hash
# containing the class descriptions of a template instantiation.
# (Hash<string(class name),Array of strings(attributes)>)
# The method traverses all the class attributes.
# if they are of primitive type it concats them to form a query string.
# Otherwise there is either a nested class or an inheritance hierarchy
# or a nested data structure to be taken into account. 
# A recursive call is carried out to
# traverse the latter.

# assumption: first attribute records class name and type

      def recursive_traversal(tmpr_class, attributes)
        at = 0
	while at < attributes.length
	  ret_attribute = neat_attributes(attributes[at])
	  if ret_attribute.match(/jump_nested_class/)
	    split = ret_attribute.split(/ /)
	    puts "split " + split[1]
	    recursive_traversal(tmpr_class,
				tmpr_class.fetch(split[1]))
	  elsif ret_attribute.match(/jump_inh/)
	    split = ret_attribute.split(/ /)
	    puts "split " + split[2]
	    recursive_traversal(tmpr_class,
				tmpr_class.fetch(split[2]))
	  elsif ret_attribute.match(/jump_nested_ds/)
	    split = ret_attribute.split(/ /)
	    puts "split " + split[1]
	    tmpr_chars = Data_structure_characteristics.new
	    tmpr_class1 = Hash.new
	    tmpr_class2 = Hash.new
	    ds_index = @data_structures_names[split[1]]
	    tmpr_chars, tmpr_class1, tmpr_class2 = 
	    		ds_unravel(ds_index, "create_nested")
	    if tmpr_chars == nil
	      at += 1
	      next
	    end
	    if tmpr_chars.template1_type != "none" 
	      recursive_traversal(tmpr_class1,
			tmpr_class1.fetch(tmpr_chars.template1_name))
	    else
	      puts "recursive traversal(rt): Empty_template"
	    end
	    puts "rt: query " + @query
	    recursive_traversal(tmpr_class2,
			tmpr_class2.fetch(tmpr_chars.template2_name))	    
	    puts "rt: Back from recursion"
	  else
	    @query += ret_attribute + "," 
	    puts "rt: " + @query
	  end
	  at += 1
	end
      end


# to construct the string of the query to be passed
# to sqlite engine. some attributes are not of primitive type
# and signify special semantics as stored in
# @data_structures_array


      def neat_attributes(attribute)
        puts "neat_attributes(nt)"
        puts attribute
      	if attribute.include?("\s")
	  name_type = attribute.split(/ /)
	else
	  puts "nt: Error " + attribute
	end
	if name_type[1].match(/ds/)
	  ret_attribute = "jump_nested_ds " + name_type[0]
	elsif name_type[1]=="reference"
	  ret_attribute = "jump_nested_class " + name_type[0]
	elsif name_type[1].match(/inherits_from/)
	  ret_attribute = name_type[0] + " jump_inh " + name_type[2]
	else
# primitive type	
	  ret_attribute = attribute
  	end
	return ret_attribute
      end


# visit a class attributes and fill in the data structure
# characteristics children Hash with:
# key: child data structure name "," index position in @...array
# value: child data structure signature
      
      def traverse_attributes(tmpr_chars, attributes)
        at = 0
        while at < attributes.length
	  if attributes[at].match(/ ds/)
	    split = attributes[at].split(/ /)
	    index = @data_structures_names[split[0]]	    
	    tmpr_chars_ch, t1, t2 = ds_unravel(index, "setup")
	    tmpr_chars.children[split[0] + "," + index.to_s] = 
	    				 tmpr_chars_ch.signature	    
	  end
	  at += 1
	end
      end

# fill in the iterator variable in Data structure characteristics
# value depends on the type of the data structure (object or pointer)
# and its level (top or embedded)

      def fill_iter_var(tmpr_chars, template_class, template_no, index)
      	if template_no == 1
	  template_type = tmpr_chars.template1_type
	else
	  template_type = tmpr_chars.template2_type	
	end  
        if template_type != "primitive" && 
	   		tmpr_chars.classnames[template_class].match(/pointer/)
	  if tmpr_chars.level == "top"
	    tmpr_chars.iter_var = "(*iter)"
	  else
	    tmpr_chars.iter_var = "(*iter" + index.to_s + ")"	        
	    tmpr_chars.index_naked = index.to_s
	  end
	else
	  if tmpr_chars.level == "top"
	    tmpr_chars.iter_var = "iter"
	  else
	    tmpr_chars.iter_var = "iter" + index.to_s
	    tmpr_chars.index_naked = index.to_s
	  end
	end
      end


      def setup_ds
	q = 0
	tmpr_ds = Hash.new
	tmpr_chars = Data_structure_characteristics.new
	tmpr_keys=Array.new
	tmpr_class1 = Hash.new
	tmpr_class2 = Hash.new
	attributes1 = Array.new
	attributes2 = Array.new


	while q < @data_structures_array.length
	  tmpr_chars, tmpr_class1, tmpr_class2 = ds_unravel(q, "setup")
	  if tmpr_chars.used_create==1 || tmpr_chars.used_check==1 || 
	     			       tmpr_chars.used_retrieve==1 
	    puts "FATAL ERROR: corrupt used attributes"
	    exit(1)
	  end

# get template arguments from signature
	  cleared = tmpr_chars.signature.split(/</)
	  tml_arg = cleared[1].chomp(">")

# purify template classes from pointer identifier
	  if tml_arg.match(/\*/)
	    tml_arg.gsub!(/\*/,"")
	  end

	  puts "tml_arg " + tml_arg
	  tmpr_chars.pure_signature = tml_arg
	  if tml_arg.match(/,/)
	    cleared = tml_arg.split(/,/)	  
	    tmpr_chars.template1_name = cleared[0]
	    tmpr_chars.template2_name = cleared[1]
	  else
	    tmpr_chars.template1_name = "none"
	    tmpr_chars.template2_name = tml_arg
	  end
	  puts "template_class1 " + tmpr_chars.template1_name
	  puts "template_class2 " + tmpr_chars.template2_name
	  puts ""

  	  c_type=tmpr_chars.signature.split(/</)
	  if c_type[0].match(/\imap/)
	    tmpr_chars.stl_class="map"
	  elsif c_type[0].match(/\iset/)
	    tmpr_chars.stl_class="set"
	  elsif (c_type[0].match(/\ihash/) && (c_type[0].match(/\set/)))
	    tmpr_chars.stl_class="hash_set"
	  elsif (c_type[0].match(/\ihash/) && (c_type[0].match(/\map/)))
	    tmpr_chars.stl_class="hash_map"
	  elsif c_type[0]=="hash"
	    tmpr_chars.stl_class="hash_set"
	  else 
	    tmpr_chars.stl_class = c_type[0]
	  end
	  
          if tmpr_chars.template1_type != "none"
            sep_classes = tmpr_chars.pure_signature.split(/,/)
	    fill_iter_var(tmpr_chars, sep_classes[0], 1, q)
	    fill_iter_var(tmpr_chars, sep_classes[1], 2, q)
	  else
	    fill_iter_var(tmpr_chars, tmpr_chars.pure_signature, 2, q)
	  end
	  puts "ds_setup(d_s): ITER VAR = " + tmpr_chars.iter_var

	  if tmpr_class1 != nil
	    attributes1 = tmpr_class1.fetch(tmpr_chars.template1_name)
	    traverse_attributes(tmpr_chars, attributes1)
	  end
	  attributes2 = tmpr_class2.fetch(tmpr_chars.template2_name)
	  traverse_attributes(tmpr_chars, attributes2)
	  q += 1
	end
      end

# produces a valid sql query for creating a virtual table according to
# the argument array columns

      def create_vt()

        puts "create_vt(cv)"
	q = 0
	tmpr_chars = Data_structure_characteristics.new
	tmpr_class1 = Hash.new
	tmpr_class2 = Hash.new

	while q < @data_structures_array.length
	  tmpr_chars, tmpr_class1, tmpr_class2=ds_unravel(q, "create")
	  if tmpr_chars == nil
	    puts "cv: NIL " + q.to_s
	    q += 1
	    next
	  end

          @query = "CREATE VIRTUAL TABLE " + tmpr_chars.name  + 
	  	 " USING stl(" + "pk integer primary key,"

	  if tmpr_chars.template1_type != "none"
	    recursive_traversal(tmpr_class1,
			tmpr_class1.fetch(tmpr_chars.template1_name))
	  else
	    puts "empty template"
	  end
	  puts "query " + @query
	  recursive_traversal(tmpr_class2,
			tmpr_class2.fetch(tmpr_chars.template2_name))
	  @query = @query.chomp(",")
	  @query += ")"
	  puts "cv: query final " + @query
	  @queries.push(@query)
	  q += 1
        end
      end


# not used. see line 1364 for explanation
# traverses children data structures and prints size information

=begin
      def generate_size_children(fw, tmpr_chars)
	children = tmpr_chars.children
	ch = 0
	key_array = children.keys
	while ch < children.length
	  split_key = key_array[ch].split(/,/)	      
	  if ch == 0
	    type_if = "if"
	  else
	    type_if = "elsif"
	  end
	  fw.puts @s + type_if + "( !strcmp(stl->children[" + ch.to_s +
	      	      "], data_structure_name) ){"
	  fw.puts @s + "    " + children.fetch(key_array[ch]) +
            	    " *emb_dstr" + split_key[1] + 
		    " = (" + children.fetch(key_array[ch]) + " *) " +
		    tmpr_chars.iter_var + "->get_" + split_key[0] + "();"
	  tmpr_chars_child, t1, t2 = 
	  		    ds_unravel(@data_structures_names[split_key[0]], 
			    "generate_nested")
	  if tmpr_chars_child == nil
	    ch += 1
	    next
	  end
	  if tmpr_chars_child.children.length > 0
	    generate_size_children(fw, tmpr_chars_child)
#	    fw.puts @s + "{else"
	  end
	  fw.puts @s + "    return ((int)emb_dstr" + split_key[1] + 
	      	    "->size());"
	  fw.puts @s + "}"
	  ch += 1
	end
      end
=end


      def gen_emb_def(fw, iter_var, children, operation, 
      	  		  	    	      	index_vars, emb_def_array)
        ch = 0
	key_array = children.keys
	while ch < children.length
	  puts "ged: SPLIT_KEY whole is: " + key_array[ch] 
	  split_key = key_array[ch].split(/,/)	      
	  tmpr_chars, tmpr_class1, tmpr_class2 = 
	  			ds_unravel(split_key[1].to_i, operation)
	  if tmpr_chars == nil
	    q += 1
	    next
	  end
	  fw.puts "    " + children.fetch(key_array[ch]) +
            	  " *emb_dstr" + split_key[1] + 
		  ";"

# = (" + children.fetch(key_array[ch]) + " *) " +
#		    tmpr_chars.iter_var + "->get_" + split_key[0] + "();"

	  fw.puts "    " + children.fetch(key_array[ch]) + 
	    	  ":: iterator iter" + split_key[1] + ";"
	  fw.puts "    int index" + split_key[1] + ";"
          if operation.match(/rtv/)
	    fw.puts "    char * emb_index" + split_key[1] +
	    	    " = strtok( NULL, \".\");"
#	    fw.puts "    index" + split_key[1] + ";"
	    fw.puts "    if (emb_index" + split_key[1] + " != NULL)"
	    fw.puts "        sscanf(emb_index" + split_key[1] + 
	      	    ", \"%d\",&index" +
	            split_key[1] + ");"
	    fw.puts "    else"
	    fw.puts "        printf(\"FAULT detokenising\\n\");"
	    fw.puts "    "
	    fw.puts "    emb_dstr" + split_key[1] +
		    " = " + iter_var + "->" +
		    "get_" + split_key[0] +
		    "();\n"  + "    iter" + 
		    split_key[1] +
		    " = emb_dstr" + split_key[1] +
		    "->begin();\n" + 
		    "    for(int i" + split_key[1] +
	    	    "=0; i" + split_key[1] + "<index" +
	    	    split_key[1] + "; i" + split_key[1] + 
		    "++){\n" + @s +
		    "iter" + split_key[1] + "++;\n    }"
	  else
	    emb_def_array[emb_def_array.length] = 
	    	    @s + "    emb_dstr" + split_key[1] +
		    " = " + iter_var + "->" +
		    "get_" + split_key[0] +
		    "();\n" + @s  + "    index" + 
		    split_key[1] +
		    " = emb_dstr" + split_key[1] +
		    "->size();\n" + @s + 
		    "    for(int i" + split_key[1] +
	    	    "=0; i" + split_key[1] + "<index" +
	    	    split_key[1] + "; i" + split_key[1] + 
		    "++){\n"
	    index_vars[index_vars.length] = split_key[1]
	  end
	  if tmpr_chars.children.length > 0
	    gen_emb_def(fw, tmpr_chars.iter_var, tmpr_chars.children, 
	    		    operation, index_vars, emb_def_array)
	  end
	  ch += 1
	end
      end


      def traverse_children(tmpr_chars)
        puts "Travesre Children(tc)"
	children = tmpr_chars.children
	ch = 0
	key_array = children.keys
	while ch < children.length
	  split_key = key_array[ch].split(/,/) 
	  tmpr_chars_child, t1, t2 = ds_unravel(split_key[1].to_i, "child")
	  if tmpr_chars_child == nil
	    puts "tc: nil"
	  end
	  @def_children += "\n" + @s + @s +
	  		 "    emb_dstr" + split_key[1] +
		    " = " + tmpr_chars.iter_var + "->" +
		    "get_" + split_key[0] +
		    "();\n" + @s + @s + "    index" + 
		    split_key[1] +
		    " = emb_dstr" + split_key[1] +
		    "->size();\n" + @s + @s + 
		    "    for(int i" + split_key[1] +
	    	    "=0; i" + split_key[1] + "<index" +
	    	    split_key[1] + "; i" + split_key[1] +
 		    "++){"

	  @finish_string = @s + @s + "    }\n" + @s + @s +
	  		  "    iter" + split_key[1] +
	  	        "++;\n" 

	  @format += "%d."
	  @index_vars += "i" + split_key[1] + ", "
	  if tmpr_chars_child.children.length > 0
	    traverse_children(tmpr_chars_child)
	  end
	  ch += 1
	end
      end


# checks if a template is of type primitive


      def primitive(template_no, tmpr_chars)
        if (template_no == 1 && tmpr_chars.template1_type == "primitive") ||
           (template_no == 2 && tmpr_chars.template2_type == "primitive")
              return true
        else
              return false
        end
      end


# returns the sqlite compatible data type for a given
# data type given by user. also takes care of any type cast
# necessary to conform to sqlite requirements

      def which_datatype(user_datatype, operation)
        if user_datatype=="int" || user_datatype=="integer" ||
                user_datatype=="tinyint" ||
                user_datatype=="smallint"||
                user_datatype=="mediumint" ||
                user_datatype=="bigint" ||
                user_datatype=="unsigned bigint" ||
                user_datatype=="int2" ||
                user_datatype=="bool" || user_datatype=="boolean" ||
                user_datatype=="int8" || user_datatype=="numeric"
                  ret = "int"
        elsif user_datatype=="blob"
	          ret = "blob"
                  if operation.match(/retrieve/)
		    @column_traverse += ", -1, SQLITE_STATIC"
		  end
		  @column_traverse = "(const void *)" +
		          @column_traverse
        elsif user_datatype=="float" ||
              user_datatype=="double"  ||
              user_datatype.match(/\idecimal/) ||
              user_datatype=="double precision" ||
              user_datatype=="real"
                  ret = "double"
        elsif user_datatype=="text" || user_datatype=="date" ||
                user_datatype=="datetime" ||
                user_datatype.match(/\icharacter/) ||
                user_datatype.match(/\ivarchar/) ||
                user_datatype.match(/\invarchar/) ||
                user_datatype.match(/\ivarying character/) ||
                user_datatype.match(/\inative character/) ||
                user_datatype=="clob" ||
                user_datatype.match(/\inchar/)
                  ret = "text"
                  if operation.match(/retrieve/)
		    @column_traverse += ", -1, SQLITE_STATIC"
                    @column_traverse = "(const char *)" +
        	  		   @column_traverse
		  else
		    @column_traverse = "(const unsigned char *)" +
        	  		   @column_traverse
		  end
        elsif user_datatype=="string"
		  ret = "text"
		  @column_traverse = @column_traverse.chomp(".")
                  @column_traverse += ".c_str()"
                  if operation.match(/retrieve/)
		    @column_traverse += ", -1, SQLITE_STATIC"
                    @column_traverse = "(const char *)" +
        	  		   @column_traverse
		  else
		    @column_traverse = "(const unsigned char *)" +
        	  		   @column_traverse
		  end
	elsif user_datatype=="references"
# needs taken care of
		  ret = "int"
		  @column_traverse = "(long int)" +
        	  		   @column_traverse
        end
	return ret
      end


# returns the identifier matching the data structure class used

      def tmpl_complexity(template_no, tmpr_chars, class_type)
        ret = ""
        if tmpr_chars.template1_type != "none"
          if template_no == 1
            ret = "first" + class_type
          else
            ret = "second" + class_type
          end
        end
	return ret
      end


# takes as input a template description
# (Hash<string(class name),Array of strings(attributes)>),
# attributes of a class (Array), template_no(1 or 2),
# tmpr_chars(internally used class containing the data structure
# characteristics. fw is the opened stream which writes to the c file.
# operation is either check or retrieve to signify which family of
# methods is generated (search or retrieve).
# Different statements are printed respectively.




      def gen_col(template, attributes, template_no, tmpr_chars,
      	  			  fw, operation)

#HereDoc

	gather_results = <<-rslt
		}
                iter++;
            }
            assert(count <= stcsr->max_size);
            break;
rslt


	gather_results_embedded = <<-rslt_em
		    }
rslt_em

	close_embedded_loop = <<-cel
	        iter++;
            }
            assert(count <= stcsr->max_size);
            break;
cel


	puts "gen_col(gc): " + operation
        at = 0
        while at < attributes.length
	  puts "gc at: " + at.to_s
	  if @back > 0
	    @back = 0
	  elsif @back < 0 
	    @back = 0
	    @was_inheritance = 1
	  end
	  class_name = template.index(attributes)
          @classes_inserted[@classes_inserted.length] = class_name

          ret_attribute = neat_attributes(attributes[at])
          split = ret_attribute.split(/ /)
	  puts "gc: ret_attribute = " + ret_attribute
	  if @action.length == 0
	    puts "gc: action empty"
	  else
	    puts "gc: action: " + @action[@action.length - 1]
	  end
	  if @action.length > 0 && 
	     		    @action[@action.length - 1].match(/nested_ds/)
	    ds_name = @action[@action.length - 1].split(/ /)	    
	    puts "gc: " + ds_name[0]
	  end
	  if ret_attribute.match(/jump_inh/)
	    @action[@action.length] = "inheritance"
	    puts "gc: INHERITANCE"
	    @inheritance += 1
	  end
# datatypes?length 3?
	  if at == 0 && !ret_attribute.match(/jump_inh/) || 
	  at == 1 && @was_inheritance == 1
	    if @was_inheritance == 1 
	      @was_inheritance = 0
	    end
            @follow_up.insert(@follow_up.length,"get_")	      
	    puts @follow_up.length.to_s
            if tmpr_chars.classnames[class_name] == "class_pointer"
	      class_type = "->"
            else
	      class_type = "."
            end
	  end
          if ret_attribute.match(/jump_nested_class/)
	    @action[@action.length] = "nested_class"
	    @jump += 1
            @follow_up[@follow_up.length - 1] += split[1] + "()" + class_type
	    puts "gc_nc: follow_up is " + @follow_up[@follow_up.length - 1]
            split = ret_attribute.split(/ /)
            puts "gc_nc: split " + split[1]
	    if operation.match(/check/)
              gen_col(template, template.fetch(split[1]),
                                    template_no, tmpr_chars, fw, 
				    "check_nested")
	    else
              gen_col(template, template.fetch(split[1]),
                                    template_no, tmpr_chars, fw, 
				    "retrieve_nested")
	    end
	  elsif ret_attribute.match(/jump_nested_ds/)
	    @jump += 1
	    split = ret_attribute.split(/ /)	    
	    puts "gc_nds: split " + split[1]
	    @action[@action.length] = "nested_ds " + split[1]
	    ds_index = @data_structures_names[split[1]]
	    puts "gc_nds: ds_index = "+ ds_index.to_s
            @follow_up[@follow_up.length - 1] += split[1] + "()" + class_type
	    puts "gc_nds: follow_up is " + @follow_up[@follow_up.length - 1]

	    @parent_iter_var = tmpr_chars.iter_var

	    tmpr_chars = Data_structure_characteristics.new
	    tmpr_class1 = Hash.new
	    tmpr_class2 = Hash.new

# patch: because we do not know what operation (nested or top) we are
# currently in we check and make it a nested if it isn't.
	    if !operation.match(/nested/)
	      operation = operation + "nested"
	    end
	    tmpr_chars, tmpr_class1, tmpr_class2 = 
	    		ds_unravel(ds_index,operation)
	    if tmpr_chars == nil
	      puts "gc_nds: UNRAVEL NIL"
	      at += 1
	      next
	    end
	    
	    index_now = tmpr_chars.index_naked
	    @embedded_index[@embedded_index.length] = index_now
	    if operation.match(/check/)
	      @embedded_loop[@embedded_loop.length] = 
	    			 "emb_dstr" + index_now +
				 " = " + 
				 @parent_iter_var + "->" + 
			@follow_up[@follow_up.length - 1].chomp("->") + 
				 ";\n" + @s + @s + "iter" + 
				 index_now +
				  " = emb_dstr" + index_now +
				  "->begin();\n" + @s + @s +  
              			"for(int i" + index_now + 
				"=0;i" + index_now + "<(int)emb_dstr" +
				index_now +
				"->size();i" + index_now + "++){\n"
	    else 
	      @embedded_loop[@embedded_loop.length] = 
	    			 "emb_dstr" + index_now +
				 " = " + 
				 @parent_iter_var + "->" +
			@follow_up[@follow_up.length - 1].chomp("->") + 
				 ";\n" + @s  + "iter" + 
				 index_now +
				  " = emb_dstr" + index_now +
				  "->begin();\n"
	    end


            if tmpr_chars.template1_type != "none"
	      if operation.match(/check/)
                gen_col(tmpr_class1, 
			tmpr_class1.fetch(tmpr_chars.template1_name),
                            1, tmpr_chars, fw, "check_nested")
	      else
                gen_col(tmpr_class1,
		        tmpr_class.fetch(tmpr_chars.template1_name),
                         1, tmpr_chars, fw, "retrieve_nested")
	      end
            else
	      puts "gc_nds: " + tmpr_chars.name
	      puts "gc_nds: " + tmpr_chars.pure_signature
	      puts "gc_nds: " + tmpr_chars.classnames.inspect
	      if operation.match(/check/)
                gen_col(tmpr_class2,
		        tmpr_class2.fetch(tmpr_chars.template2_name),
                        2, tmpr_chars, fw, "check_nested")
	      else
                gen_col(tmpr_class2,
		        tmpr_class2.fetch(tmpr_chars.template2_name),
                        2, tmpr_chars, fw, "retrieve_nested")
              end
	    end
	  elsif ret_attribute.match(/jump_inh/)
            puts "gc_inh: split " + split[2]

# check correctness please
	    if @classes_inserted.include?(split[2])
	      puts "Attempt to define same class\n"
	    elsif operation.match(/check/)
              gen_col(template, template.fetch(split[2]),
                                    template_no, tmpr_chars, fw, 
				    "check_nested")
	    else
              gen_col(template, template.fetch(split[2]),
                                    template_no, tmpr_chars, fw, 
				    "retrieve_nested")
	    end	    
          else
            @counter += 1
            name_type = ret_attribute.split(/ /)

# check correctness please
# name_type[0]->name, name_type[1]->type but after preparing column
#          for a fk they come the other way around so:
	    if ret_attribute.match(/_id references/)
#	      temp = name_type[2]
	      name_type[0] = name_type[2]
	      puts name_type[0]
	      puts name_type[1]
	    end
            @column_traverse = tmpr_chars.iter_var  + "->"
            compl = tmpl_complexity(template_no, tmpr_chars, class_type)
	    @column_traverse += compl	    
            if primitive(template_no, tmpr_chars)
            else
              f_ups = 0
              while f_ups < @follow_up.length
	        puts "follow_up : " + @follow_up[f_ups]
                @column_traverse += @follow_up[f_ups]
                f_ups += 1
              end
              @column_traverse += name_type[0] + "()"
            end
            datatype = which_datatype(name_type[1].downcase, operation)
	    if tmpr_chars.level == "embedded"
	      split_string = "->get_" + name_type[0]
#	      fw.puts split_string
	      if @column_traverse.match(split_string)
	      	emb_ds = @column_traverse.split(split_string)
	      else
		puts "gc_gencf: FATAL ERROR: undefined column data"
		exit(1)
	      end
#		fw.puts emb_ds[0]
# two mistakes, one correct. we require the parent iter var
# but we have the embedded one's instead (l. 630)
# we will correct it in time (l. 664)
	      clean_ds = emb_ds[0].split(tmpr_chars.iter_var)
	      if clean_ds.length == 0
		puts "gc_gencf: FATAL ERROR: undefined column data"
		exit(1)
	      end
	    end
	    if operation.match(/check/)
	      if tmpr_chars.level == "embedded"
=begin
printing definition of embedded data structure and signature
now is outside cases with alternative design (children data structure
in parent to enclose necessary info for the definitions
tmpr_chars.signature + " *emb_dstr" + tmpr_chars.index_naked + "  = " + 
				 @parent_iter_var + clean_ds[1] + 
				 ";\n" + @s + @s +  
				 tmpr_chars.signature + 
				 ":: iterator iter" + tmpr_chars.index_naked + 
				 ";\n"

	        @column_traverse = "emb_dstr" + tmpr_chars.index_naked +
				 " = " + 
				 @parent_iter_var + clean_ds[1] + 
				 ";\n" + @s + @s + "iter" + 
				 tmpr_chars.index_naked +
				  " = emb_dstr" + tmpr_chars.index_naked +
				  "->begin();\n" + @s + @s +  
              			"for(int k=0;k<(int)emb_dstr" +
				tmpr_chars.index_naked +
				"->size();k++){\n"
=end
		el = 0
		embedded_loop_total = ""
		resultset = "%d."
		emb_index = ", i, "
		while el < @embedded_loop.length 
		  embedded_loop_total += @embedded_loop[el]
		  emb_index += "i" + @embedded_index[el] + ", "
		  resultset += "%d."
		  el += 1
		end
		@column_traverse = embedded_loop_total + @s + @s + 
				 "    if (traverse(" + clean_ds[0] + 
				tmpr_chars.iter_var + 
				split_string + emb_ds[1] +
				", op, sqlite3_value_" + datatype + 
				 "(val))){" + "\n" + @s + @s + @s + 
				 "sprintf(temp_res[count++],\"" + 
				 resultset.chomp(".") + 
				 "\"" + emb_index.chomp(", ") + 
				 ");"

	      else
                @column_traverse = "if( traverse(" + @column_traverse + 
				 ", op, sqlite3_value_" + datatype + 
				 "(val))){"
		if tmpr_chars.children.length > 0
		  traverse_children(tmpr_chars)
		  @column_traverse += @def_children
		end
	      end
	    else
	      if tmpr_chars.level == "embedded"
		el = 0
		embedded_loop_total = ""
		while el < @embedded_loop.length 
		  embedded_loop_total += @embedded_loop[el]
		  el += 1
		end
#	        @column_traverse = embedded_loop_total + @s +  
	        @column_traverse =  
				  "sqlite3_result_" + datatype +
				"(con, " + clean_ds[0] + 
				tmpr_chars.iter_var + 
				split_string + emb_ds[1] + ");"
	      else
	        @column_traverse = "sqlite3_result_" +
			  datatype + "(con, "  + @column_traverse + ");"
	      end
	    end
          end
# avoid duplicating statement
	  if @back == 0
	    puts @column_traverse
	    if operation.match(/check/)
              fw.puts @s + "case " + @counter.to_s + ":"
              fw.puts @s + "    iter=any_dstr->begin();"
              fw.puts @s + "    for(int i=0;i<(int)any_dstr->size();i++){"
              fw.puts @s + "        " + @column_traverse
	    else
              fw.puts "    case " + @counter.to_s + ":"
              fw.puts @s + @column_traverse
	    end
	    if operation.match(/check/)
	      if tmpr_chars.level == "embedded"
	        fw.puts gather_results_embedded
		ei = 0
		while ei < @embedded_index.length
		  fw.puts @s + @s + "    iter" + 
		  	  @embedded_index[ei] + "++;\n"
		  fw.puts @s + @s + "}"
		  ei += 1
		end
		fw.puts close_embedded_loop
	      else
		fw.puts @s + @s + @s + 
			"sprintf(temp_res[count++], " +
      			@format.chomp(".") + "\", " + 
			@index_vars.chomp(", ") + ");" 
		fw.puts @finish_string
	        fw.puts gather_results
		
	      end
	    else 
	      fw.puts @s + "break;"
	    end
	  end
          at += 1
	  @def_children = ""
	  @finish_string = ""
	  @format = "\"%d."
	  @index_vars = " i, "
        end
        @follow_up.delete_at(@follow_up.length - 1)
	if @inheritance > 0 && @action[@action.length - 1] == "inheritance"
	  @back = -1
	  @inheritance -= 1
	  @action.delete_at(@action.length - 1)
	else 
	  if @jump > 0 
	    if @action[@action.length - 1] == "nested_class"
	      structure = class_name
	    elsif @action[@action.length - 1].match(/nested_ds/)
	      structure = ds_name[1]
	      @embedded_loop.delete_at(@embedded_loop.length - 1)
	      @embedded_index.delete_at(@embedded_index.length - 1)
	    end
	    if @follow_up[@follow_up.length - 1].match(structure)
	      reduce = @follow_up[@follow_up.length - 1].split(structure)
	      @follow_up[@follow_up.length - 1] = reduce[0]
	      puts "deletion. now " + @follow_up.length.to_s + " records"
	      puts @follow_up[@follow_up.length - 1]
	    end
	    @back = 1
	    @jump -= 1
	    @action.delete_at(@action.length - 1)
	  end
	end
      end


# opens a new c source file and writes c code.
# Specifically, it generates the main.template, the search.cpp
# and the makefile.template.
# All information about data structures to be registered to
# sqlite is used (resides at Register) to generate the necessary
# methods.
# there for each query a call to register_table is done
# to create the respective VT.


      def write_to_file(db_name)



# HERE DOCUMENTS FOR METHOD


  # HereDoc1

      auto_gen1 = <<-AG1

using namespace std;



void * thread_sqlite(void *data){
  const char **queries;
  queries = (const char **)sqlite3_malloc(sizeof(char *) * 
  	    	   #{@queries.length.to_s});
  int failure = 0;
AG1



   #HereDoc2

      auto_gen2 = <<-AG2
  failure = register_table( "#{db_name}" ,  #{@queries.length.to_s}, queries,
  	   data, enter 1 if table is to be created 0 if already created);
  printf(\"Thread sqlite returning..\\n\");  
  sqlite3_free(queries);
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
  char *helper;

  //names of data structures to be registered
  const char *name1 = "to be filled_in";
  int n_name1 = (int)strlen(name1) + 1;
  // etc for subsequent data structures. eg:
  // const char *name2 = "to be filled_in";
  // int n_name2 = (int)strlen(name2) + 1;
  // length of data structures names

  dsCarrier *dsC;
  int nByte = sizeof(dsCarrier) + sizeof(long int *) * 2 +
    sizeof(const char *) * 2 + n_name1;
  // etc for subsequent data structures. eg: + n_name2;
  dsC = (dsCarrier *)sqlite3_malloc(nByte);
  memset(dsC, 0, nByte);
  pthread_t sqlite_thread;

// assignment of data structure characteristics to dsC
  // number of data structures to register
  dsC->size = to be filled_in;
  dsC->dsNames = (const char **)&dsC[1];
  dsC->memories = (long int **)&dsC->dsNames[dsC->size];
  helper = (char *)&dsC->memories[dsC->size];

  dsC->memories[0] = (long int *) to be filled in with memory address;
  // etc for subsequent data structures. eg:
  // dsC->memories[1] = (long int *) to be filled in with memory address;


  dsC->dsNames[0] = helper;
  memcpy(helper, name1, n_name1);
  helper += n_name1;
  // etc for subsequent data structures
  // dsC->dsNames[1] = helper;
  // memcpy(helper, name2, n_name2);
  // helper += n_name2;

  assert(helper <= &((char *)dsC)[nByte]);

  data = (void *)dsC;

  re_sqlite = pthread_create(&sqlite_thread, NULL, thread_sqlite, data);
  pthread_join(sqlite_thread, NULL);
  printf(\"Thread sqlite returned %i\\n\", re_sqlite);
}


AG2



    #HereDoc3


	auto_gen3 = <<-AG3


int traverse(int dstr_value, int op, int value){
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


int traverse(double dstr_value, int op, double value){
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

// compare addresses???
int traverse(const void *dstr_value, int op, const void *value){
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


int traverse(const unsigned char *dstr_value, int op, 
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


int cmp_int(char *tok_tr, char *tok_res){
    int index_tr, index_res;
    sscanf( tok_tr, "%d", &index_tr);
    sscanf( tok_res, "%d", &index_res);
    if ( index_tr < index_res )
        return -1;
    else if (index_tr > index_res)
    	return 1;
    else
	return 0;
}


int str_token(char *source, char *token, char dlm){
    char * match;
    char * copy = (char *)sqlite3_malloc(sizeof(char) * 20);
    char * init_address;
    int position = 0, k = 0;
    strcpy(copy, source);
    match = strchr( copy, dlm);
    if ( match != NULL ){
        position = match - copy;
	strcpy(token, copy);
	token[position] = '\\0';
	printf("token: %s\\n", token);
	init_address = copy;
        while (k <= position){
	    copy++;
	    k += 1;
	}
	strcpy( source, copy);
	printf("source: %s\\n", source);
	sqlite3_free(init_address);
	return 1;
    }else{
	sqlite3_free(copy);
        return 0;
    }
}


int cmp_str(char *str_tr, char *str_res){
    int ci = 0, c_tr, c_res;
    char copy_tr[20], copy_res[20], tok_tr[20], tok_res[20];
    strcpy(copy_tr, str_tr);
    strcpy(copy_res, str_res);
    c_tr = str_token(copy_tr, tok_tr, '.');
    c_res = str_token(copy_res, tok_res, '.');
    if ( (!c_tr) && (!c_res) )
        return cmp_int(copy_tr, copy_res);
    else{
	while ( (c_tr) && (c_res) && (ci == 0 ) ){
	    ci = cmp_int(tok_tr, tok_res);
    	    c_tr = str_token(copy_tr, tok_tr, '.');
    	    c_res = str_token(copy_res, tok_res, '.');
	}
    	if ( (!c_tr) && (!c_res) )
            ci = cmp_int(copy_tr, copy_res);
	return ci;
    }
}


AG3

	  auto_gen35 = <<-AG35 
    }else{
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
            NULL;
            break;
        }

        iCol = constr[1] - 'a' + 1;
AG35
#	int *temp_res;

	embedded_level1 = <<-eml1
        int arraySize;
        int *res;
        if (iCol == 0){
            stl->data = (void *)sqlite3_value_int64(val);
eml1


	    embedded_level2 = <<-eml2
	}

        arraySize=get_datastructure_size(stl);

        if ( arraySize != stcsr->max_size ){
            res = (int *)sqlite3_realloc(stcsr->resultSet, sizeof(int) * arraySize);
            if (res!=NULL){
                stcsr->resultSet = res;
                memset(stcsr->resultSet, -1,
                       sizeof(int) * arraySize);
                stcsr->max_size = arraySize;
                printf("\\nReallocating resultSet..now max size %i \\n\\n", stcsr->max_size);
            }else{
                free(res);
                printf("Error (re)allocating memory\\n");
                exit(1);
            }
        }
eml2


#	  auto_gen4 = <<-AG4
#	temp_res = (int *)sqlite3_malloc(sizeof(int)  * stcsr->max_size);
#        if ( !temp_res ){
#            printf("Error in allocating memory\\n");
#            exit(1);
#        }

	  auto_gen4 = <<-AG4
        switch( iCol ){
// i=0. search using PK?memory location?or no PK?
// no can't do.PK will be memory location. PK in every table
// PK search
        case 0: 
            iter=any_dstr->begin();
            for(int i=0; i<(int)any_dstr->size(); i++){
	        sprintf(temp_res[count++], "%d", i);
            }
            assert(count <= stcsr->max_size);
            break;
AG4

	
    #HereDoc5


=begin	
cls_search = <<-cls
// more datatypes and ops exist
        }
        int ia, ib;
        int *i_res;
        int i_count = 0;
        if (stcsr->size == 0){
            memcpy(stcsr->resultSet, temp_res, sizeof(int) *
	    			     stcsr->max_size);
            stcsr->size = count;
        }else{
            i_res = (int *)sqlite3_malloc(sizeof(int) *
	    	    	 		stcsr->max_size);
            for(int a=0; a<stcsr->size; a++){
                for(int b=0; b<count; b++){
                    ia = stcsr->resultSet[a];
                    ib = temp_res[b];
                    if( ia==ib ){
                        i_res[i_count++] = ia;
                        b++;
                    }else if( ia < ib )
                        b = count;
                    else
                        b++;
                }
            }
            assert( i_count <= stcsr->max_size );
            memcpy(stcsr->resultSet, i_res, sizeof(int) *
	    			     i_count);
            stcsr->size = i_count;
            sqlite3_free(i_res);
        sqlite3_free(temp_res);
=end
	cls_search = <<-cls
	}
    }
    if ( stcsr->init_constr ){
	if ( count > 0 )
	    memcpy(*stcsr->resultSet, *temp_res, 
		sizeof(char) * 20 * total_size);
	stcsr->init_constr = 0;
	stcsr->size = count;
    }else{
	if ( count == 0 ){
	    memset(stcsr->resultSet, '\\0', memory_size);
	    stcsr->size = 0;
	}else{
	    char ** copy_res = (char **)sqlite3_malloc(memory_size); 
	    memset(copy_res, '\\0', memory_size); 
  	    *copy_res = (char *)&copy_res[total_size];
  	    int cr;
  	    for (cr=0; cr<total_size; cr++){
    		copy_res[cr+1] = &copy_res[cr][20];
  	    }
	    int success = 0, ci;
	    char  str_tr[20], str_res[20];
	    for(int w=0; w<count; w++){
		strcpy(str_tr, temp_res[w]);
	        for (int k=0; k<stcsr->size; k++){
		    strcpy(str_res, stcsr->resultSet[k]);
		    printf("tr: %s, set: %s\\n", str_tr, 
			   str_res);
		    ci = cmp_str(str_tr, str_res);
		    if (ci < 0)
		        break;
		    else if (ci == 0){
			strcpy(copy_res[success++], temp_res[w]);
			printf("Success\\n");
		    }
		}
	    }
	    memcpy(*stcsr->resultSet, *copy_res, memory_size);
	    stcsr->size = success;
	    sqlite3_free(copy_res);
	}
    }
    sqlite3_free(temp_res);
}


cls


    #HereDoc6
#    int index = stcsr->current;

	auto_gen5 = <<-AG5
    char *colName = stl->azColumn[n];
// iterator implementation. serial traversing or hit?
    iter = any_dstr->begin();
// serial traversing. simple and generic. visitor pattern is next step.
    for(int i=0; i<index; i++){
        iter++;
    }
AG5

  makefile_part = <<-mkf

main.o: main.cpp Account.h bridge.h
	g++ -W -g -c main.cpp

user_functions.o: user_functions.c bridge.h
	gcc -W -g -c user_functions.c

stl_to_sql.o: stl_to_sql.c stl_to_sql.h bridge.h
	gcc -g -c stl_to_sql.c

search.o: search.cpp bridge.h Account.h
	g++ -W -g -c search.cpp
mkf

  realloc = <<-rlc
    if (total_size > stcsr->max_size){
        printf("TOTAL_SIZE: %d, stcsr->max_size: %d", 
			    total_size, stcsr->max_size);
        char **res;
        int memory_size = (sizeof(char*) + sizeof(char) * 20) * total_size;
        res = (char **)sqlite3_realloc(stcsr->resultSet, memory_size);
        if (res!=NULL){
            stcsr->resultSet = res;
            memset(stcsr->resultSet, '\\0',
                       memory_size);
	    *stcsr->resultSet = (char *)&stcsr->resultSet[total_size];
	    int i;
	    for (i=0; i<total_size; i++){
	        stcsr->resultSet[i+1] = &stcsr->resultSet[i][20];
	    }
            stcsr->max_size = total_size;
            printf("\\nReallocating resultSet..now max size %i \\n\\n", 
				stcsr->max_size);
        }else{
            free(res);
            printf("Error (re)allocating memory\\n");
            exit(1);
        }
    }
rlc


  top_level_no_constr = <<-tplnc
	iter++;
        assert(count <= stcsr->max_size);
        assert(&stcsr->resultSet[count] <= &stcsr->resultSet[stcsr->max_size]);
    }
tplnc


  top_level = <<-tpl
	    iter++;
        }
        assert(count <= stcsr->max_size);
        assert(&stcsr->resultSet[count] <= &stcsr->resultSet[stcsr->max_size]);
tpl


  exit_search = <<-exs
        printf("embedded data structure cannot be requested constraint-less.must be joined");
        exit(1);
exs


# END OF HereDocs
#=begin
	puts "in write_to_file"
        myfile=File.open("main.template", "w") do |fw|
          fw.puts "\#include <stdio.h>"
          fw.puts "\#include <string>"
          fw.puts "\#include \"stl_to_sql.h\""
          fw.puts "\#include <pthread.h>"


	  q = 0
	  tmpr_ds = Hash.new
	  tmpr_keys = Array.new
	  tmpr_chars = Data_structure_characteristics.new

          while q < @data_structures_array.length
            tmpr_ds=@data_structures_array[q]
            tmpr_keys=tmpr_ds.keys
            tmpr_chars=tmpr_keys[0]
            fw.puts "\#include <" + tmpr_chars.stl_class + ">"
	    q += 1
	  end
	  tmpr_chars.classnames.each {|key,value| fw.puts "\#include \"#{key}.h\""}

# call HereDoc1
	  fw.puts auto_gen1

          i=0
          while i<@queries.length
             fw.puts "  queries[" + i.to_s + "] = \"" + @queries[i] + "\";"
             i+=1
          end

# call HereDoc2
	  fw.puts auto_gen2
        end
#=begin
	myfile=File.open("search.cpp", "w") do |fw|
          fw.puts "\#include \"search.h\""
          fw.puts "\#include <string>"
	  fw.puts "\#include \"assert.h\""
	  q = 0
          while q < @data_structures_array.length
            tmpr_ds=@data_structures_array[q]
            tmpr_keys=tmpr_ds.keys
            tmpr_chars=tmpr_keys[0]
            fw.puts "\#include <" + tmpr_chars.stl_class + ">"
	    q += 1
	  end
	  tmpr_chars.classnames.each{|key,value| fw.puts "\#include \"#{key}.h\""}

	  fw.puts
	  fw.puts "using namespace std;\n\n"

	  

	  fw.puts "int get_datastructure_size(void *st){"
	  fw.puts "    stlTable *stl = (stlTable *)st;"
	  q = 0
          while q < @data_structures_array.length
	    tmpr_chars, t1, t2 = ds_unravel(q, "gen_size")
	    if tmpr_chars == nil
	      q += 1
	      next
	    end

	    fw.puts "    if( !strcmp(stl->zName, \"" + 
	      	      tmpr_chars.name + "\") ){"
	    fw.puts "        " + tmpr_chars.signature + " *any_dstr = (" +
              	      tmpr_chars.signature + " *)stl->data;"

=begin
complex implementation in case we need to find the size of embedded
	  	  data structures via the function
	  	  get_datastructure_size.
	    children = tmpr_chars.children

	    if children.length > 0
	      generate_size_children(fw, tmpr_chars)
	    end
	    ch = 0
	    key_array = children.keys
	    while ch < children.length
	      split_key = key_array[ch].split(/,/)	      
	      fw.puts @s + "if( !strcmp(stl->children[" + ch.to_s +
	      	      "], \"" + split_key[0] + "\") ){"
	      fw.puts @s + "    " + children.fetch(key_array[ch]) +
            	    " *emb_dstr" + split_key[1] + 
		    " = (" + children.fetch(key_array[ch]) + " *) " +
		    tmpr_chars.iter_var + "->get_" + split_key[0] + "();"
	      fw.puts @s + "    return ((int)emb_dstr" + split_key[1] + 
	      	    "->size());"
	      ch += 1
	    end
=end
	    fw.puts "        return ((int)any_dstr->size());"
	    fw.puts "    }"

	    q += 1
	  end
	  fw.puts "}"

# call HereDoc3	  
	  fw.puts auto_gen3
          q = 0
          tmpr_class1 = Hash.new
	  tmpr_class2 = Hash.new
          while q < @data_structures_array.length
	    tmpr_chars,tmpr_class1,tmpr_class2=ds_unravel(q, "check")
	    if tmpr_chars == nil
	      q += 1
	      next
	    end

	    fw.puts "void " + tmpr_chars.name +
            	    "_search(void *stc, char *constr, sqlite3_value *val){"
	    fw.puts "    sqlite3_vtab_cursor *cur = (sqlite3_vtab_cursor *)stc;"
	    fw.puts "    stlTable *stl = (stlTable *)cur->pVtab;"
	    fw.puts "    stlTableCursor *stcsr = (stlTableCursor *)stc;"
	    fw.puts "    " + tmpr_chars.signature +
            	    " *any_dstr = (" + tmpr_chars.signature + " *)stl->data;"
	    fw.puts "    " + tmpr_chars.signature + ":: iterator iter;"

	    children = tmpr_chars.children
	    index_vars = Array.new
	    emb_def_array = Array.new
	    if children.length > 0
	      gen_emb_def(fw, tmpr_chars.iter_var, children, 
	      "gen_emb_def_sch", index_vars, emb_def_array)
	    end

=begin
	    ch = 0
	    key_array = children.keys
	    while ch < children.length
	      split_key = key_array[ch].split(/,/)	      
	      fw.puts "    " + children.fetch(key_array[ch]) +
            	    " *emb_dstr" + split_key[1] + 
		    ";"

# = (" + children.fetch(key_array[ch]) + " *) " +
#		    tmpr_chars.iter_var + "->get_" + split_key[0] + "();"

	      fw.puts "    " + children.fetch(key_array[ch]) + 
	    	    ":: iterator iter" + split_key[1] + ";"
	      ch += 1
	    end
=end
	    fw.puts "    int op, iCol, count = 0, total_size = 0;"
	    if children.length > 0
	      fw.puts "    iter = any_dstr->begin();"
              fw.puts "    for (int i=0; i<(int)any_dstr->size(); i++){"
	      closing_all = ""
	      ed = 0
	      while ed < emb_def_array.length - 1
	        fw.puts emb_def_array[ed]
# are you sure [ed]?maybe "}" first?
		closing_all += "   iter" + index_vars[ed] + "++;\n    }"
		ed += 1
	      end
	      tokenize = emb_def_array[ed].split(@s)
	      fw.puts "    " + tokenize[1] + "    " + tokenize[2]
	      fw.puts @s + "total_size += index" + index_vars[ed] + ";"
	      fw.puts closing_all
	      fw.puts top_level_no_constr
	    end
	    
	    fw.puts "    char **temp_res;"
	    fw.puts "    int memory_size = (sizeof(char *) + 
			 sizeof(char) * 20) * total_size;"
	    fw.puts "    temp_res = (char**)sqlite3_malloc(memory_size);" 
	    fw.puts "     memset(temp_res, '\\0', memory_size);" 
	    fw.puts "     *temp_res = (char *)&temp_res[total_size];"
	    fw.puts "     int tr;"
  	    fw.puts "	  for (tr=0; tr<total_size; tr++){"
	    fw.puts "         temp_res[tr+1] = &temp_res[tr][20];"
	    fw.puts "	  }"

	    fw.puts realloc

	    fw.puts "// val==NULL then constr==NULL also"
	    fw.puts "    if ( val==NULL ){"
	    fw.puts @s + "iter = any_dstr->begin();"
            fw.puts @s + "for (int i=0; i<(int)any_dstr->size(); i++){"

	    if children.length > 0
	      pl = 0
	      sprintf = "sprintf(temp_res[count++], "
	      format = "\"%d."
	      iv = "i, i"
	      closing_all = ""
	      while pl < index_vars.length
	        fw.puts emb_def_array[pl]
	        format += "%d."
		iv += index_vars[pl] + ", i"
# further check
		if ed > 0 
		   closing_all += "   iter" + index_vars[ed - 1] +
	      	   	       "++;\n" + @s + "}"
		end
		pl += 1
	      end
	      sprintf += format.chomp(".") + "\", " + iv.chomp(", i") + 
	      	      	 ");"
	      fw.puts @s + @s + sprintf + "\n" + @s + @s + closing_all + 
	      	      "\n" + @s + "    }" 
#	    else 
#	      fw.puts @s + "sprintf(temp_res[i], \"%d\", i);"
#	      fw.puts @s + "stcsr->size++;"
	    end

	    if tmpr_chars.level=="top"
	      fw.puts top_level
	    else 
	      fw.puts exit_search
	    end
	    fw.puts auto_gen35
=begin
	    if tmpr_chars.level=="embedded"
	      fw.puts embedded_level1
	      fw.puts @s + "    any_dstr = (" + tmpr_chars.signature + " *)stl->data;"
	      fw.puts embedded_level2
	    end	    
=end
	    fw.puts auto_gen4

#          i=1
# bottom-up
            @counter = 0
            if tmpr_chars.template1_type != "none"
              gen_col(tmpr_class1, 
	              tmpr_class1.fetch(tmpr_class1.template1_name),
                      1, tmpr_chars, fw, "check")
            end
	      puts tmpr_chars.name
	      puts tmpr_chars.pure_signature
	      puts tmpr_chars.classnames.inspect
              gen_col(tmpr_class2, 
	              tmpr_class2.fetch(tmpr_chars.template2_name),
                      2, tmpr_chars, fw, "check")
# call HereDoc29
            q += 1
            fw.puts cls_search
          end
	  
	  fw.puts "\n\n"

# empty helper structure to be reused in retrieve
# keeps registered class so as not to be reinserted in inheritance
# cases primarily
	  @classes_inserted.clear

	  q = 0
          while q < @data_structures_array.length
	    tmpr_chars,tmpr_class1,tmpr_class2=ds_unravel(q, "retrieve")
	    if tmpr_chars == nil
	      q += 1
	      next
	    end
	    fw.puts "int " + tmpr_chars.name +
            	    "_retrieve(void *stc, int n, sqlite3_context *con){"
	    fw.puts "    sqlite3_vtab_cursor *cur = (sqlite3_vtab_cursor *)stc;"
	    fw.puts "    stlTable *stl = (stlTable *)cur->pVtab;"
	    fw.puts "    stlTableCursor *stcsr = (stlTableCursor *)stc;"
	    fw.puts "    " + tmpr_chars.signature +
            	    " *any_dstr = (" + tmpr_chars.signature + " *)stl->data;"
	    fw.puts "    " + tmpr_chars.signature + ":: iterator iter;"
	    fw.puts "    char record[20];"
	    fw.puts "    strcpy(record, stcsr->resultSet[stcsr->current]);"
	    fw.puts "    char * top_index = strtok(record, \".\");"
	    fw.puts "    int index;"
	    fw.puts "    if (top_index != NULL)"
	    fw.puts "        sscanf( top_index, \"%d\", &index);"      
	    fw.puts "    else"
	    fw.puts "        sscanf( record, \"%d\", &index);"
	    fw.puts auto_gen5


	    if tmpr_chars.children.length > 0
	      gen_emb_def(fw, tmpr_chars.iter_var, tmpr_chars.children, 
	      		      "gen_emb_def_rtv", nil, nil)
	    end
	    
=begin
	    ch = 0
	    key_array = children.keys	    
	    while ch < children.length
	      split_key = key_array[ch].split(/,/)	      
	      fw.puts "    " + children.fetch(key_array[ch]) +
            	    " *emb_dstr" + split_key[1] + 
		    ";"

# = (" + children.fetch(key_array[ch]) + " *) " +
#		    tmpr_chars.iter_var + "->get_" + split_key[0] + "();"

	      fw.puts "    " + children.fetch(key_array[ch]) + 
	    	    ":: iterator iter" + split_key[1] + ";"
	      fw.puts "    char * emb_index" + split_key[1] +
	    	    " = strtok( NULL, \".\");"
	      fw.puts "    int index" + split_key[1] + ";"
	      fw.puts "    if (emb_index" + split_key[1] + " != NULL)"
	      fw.puts "        sscanf( emb_index" + split_key[1] + 
	      	      ", \"%d\",&index" +
	      split_key[1] + ");"
	      fw.puts "    else"
	      fw.puts "        printf(\"FAULT detokenising\\n\");"
	      fw.puts "    "
	      fw.puts "    emb_dstr" + split_key[1] +
				 " = " + 
				 tmpr_chars.iter_var + "->" +
				 "get_" + split_key[0] +
				 "();\n"  + "    iter" + 
				 split_key[1] +
				  " = emb_dstr" + split_key[1] +
				  "->begin();\n" + 
				  "    for(int i" + split_key[1] +
	    	    		  "=0; i" + split_key[1] + 
				  "<index" +
	    	    		  split_key[1] + "; i" + split_key[1] + 
		    		  "++){\n" + @s +
				  "iter" + split_key[1] + "++;\n    }"
				  
	      ch += 1
	    end
=end
            fw.puts "    switch( n ){"
	    fw.puts "    case 0:"
	    fw.puts "        sqlite3_result_text(con, \"(null)\", -1, SQLITE_STATIC);"
	    fw.puts "        break;"

#          i=1
# bottom-up
            @counter = 0
            if tmpr_chars.template1_type != "none"
              gen_col(tmpr_class1, 
	              tmpr_class1.fetch(tmpr_chars1.template1_name),
                      1, tmpr_chars, fw, "retrieve")
            end
              gen_col(tmpr_class2, 
	              tmpr_class2.fetch(tmpr_chars.template2_name),
                      2, tmpr_chars, fw, "retrieve")
# call HereDoc29
            q += 1
          fw.puts "    }"
          fw.puts "    return SQLITE_OK;"
          fw.puts "}\n\n\n"
        end
	fw.puts "void search(void* stc, char *constr, sqlite3_value *val){"
	fw.puts "    sqlite3_vtab_cursor *cur = (sqlite3_vtab_cursor *)stc;"
	fw.puts "    stlTable *stl = (stlTable *)cur->pVtab;"
	q = 0
        while q < @data_structures_array.length
          tmpr_ds=@data_structures_array[q]
          tmpr_keys=tmpr_ds.keys
          tmpr_chars=tmpr_keys[0]
	  if tmpr_chars.level == "top"
	    fw.puts "    if( !strcmp(stl->zName, \"" + tmpr_chars.name + 
	    	    "\") )"
	    fw.puts "        " + tmpr_chars.name +
	    	    "_search(stc, constr, val);"
	  end
	  q += 1
	end

	fw.puts "}\n\n"


	fw.puts "int retrieve(void* stc, int n, sqlite3_context *con){"
	fw.puts "    sqlite3_vtab_cursor *cur = (sqlite3_vtab_cursor *)stc;"
	fw.puts "    stlTable *stl = (stlTable *)cur->pVtab;"
	q = 0
        while q < @data_structures_array.length
          tmpr_ds=@data_structures_array[q]
          tmpr_keys=tmpr_ds.keys
          tmpr_chars=tmpr_keys[0]
	  if tmpr_chars.level == "top"
	    fw.puts "    if( !strcmp(stl->zName, \"" + tmpr_chars.name + 
	  	  "\") )"
	    fw.puts "        return " + tmpr_chars.name +
	    	    "_retrieve(stc, n, con);"
	  end
	  q += 1
	end

	fw.puts "}\n\n"
      end

      myfile = File.open("makefile.template","w") do |fw|
	q = 0
        while q < @data_structures_array.length
          tmpr_ds=@data_structures_array[q]
          tmpr_keys=tmpr_ds.keys
          tmpr_chars=tmpr_keys[0]
          fw.print "test: main.o search.o stl_to_sql.o user_functions.o "
	  tmpr_chars.classnames.each{|key,value| fw.print "#{key}.o "}
	  fw.puts
          fw.print "	g++ -lswill -lsqlite3 -W -g main.o search.o stl_to_sql.o user_functions.o "
	  tmpr_chars.classnames.each{|key,value| fw.print "#{key}.o "}
	  fw.puts "-o test"
	  fw.puts makefile_part	
	  fw.puts
	  tmpr_chars.classnames.each{|key,value| fw.puts "#{key}.o: #{key}.cpp #{key}.h \n" + "	g++ -W -g -c #{key}.cpp \n\n"}
	  q += 1
	end
      end
    end



# GLOBAL HERE DOCUMENTS

  # HereDoc1

      $err_state = <<-ERS

ERROR STATE. CANCELLING COMPLETED OPERATIONS:

PRINTING ERROR INFO:

ERS

# END OF GLOBAL HEREDOCS


# produces the array argv which contains all the necessary arguments for a 
# well-formed "CREATE VIRTUAL TABLE" query
# argument attributes is an array containing a class description 
# (pairs of name,type aka attributes)
# maps types to sqlite3 data_types and pushes pairs to argv


# my_array obsolete in tranform?

      def transform(attributes)


# HERE DOCUMENTS FOR METHOD

  # HereDoc1

      no_bind = <<-NB
no binding between class and reference keywords. 
trying to refer to non-defined class: 
NB


  # HereDoc2

      data_types = <<-DT
.
please select one from following list:
int or integer, tinyint, smallint, mediumint, 
bigint, unsigned bigint, int2, int8, 
blob(no type.values stored exactly as input), 
float, double, double precision, real, numeric, date, 
datetime, bool or boolean, decimal(10,5), text, 
clob(type text), character(20), varchar(255), varying 
character(255), nchar(55), native character(70), 
nvarchar(100), string

NOW EXITING.
DT

# END OF HEREDOCS

        argv=Array.new
	i=0
	while i< attributes.length
	  if attributes[i].include?(",")
	    name_type=attributes[i].split(/,/)
	    if name_type.length!=2
	      puts $err_state
	      raise ArgumentError.new("expected pair name,type got " + 
	      	    attributes[i] + "\n\n NOW EXITING. \n") 
	    end
#	    name_type[1]=name_type[1].downcase
            if name_type[1].downcase.match(/class/)
	      if @classnames.has_key?(name_type[0])
		puts $err_state
 		raise ArgumentError.new("Attempt to declare same class" +
	 	      " twice \n\n NOW EXITING. \n")
	      end
	      @classnames[name_type[0]]=name_type[1]
              puts "class_name is " + name_type[0]
              puts "class_type is " + name_type[1]
	      if name_type[1].downcase.match(/inherits_from/)
	        superclass = name_type[1].split(/inherits_from/)
	        argv.push(name_type[0] + " inherits_from " + superclass[1]) 
	      end
	    elsif name_type[1].downcase=="reference"
# why pushed and not straight argv.push...?
	      if @classnames.has_key?(name_type[0])
		pushed=true
	      end
	      if pushed
		argv.push(name_type[0] + " reference")
	      else 
	        puts $err_state
		raise ArgumentError.new(no_bind.chomp + name_type[0] + 
		      "\n\n NOW EXITING. \n")
	      end
	    elsif name_type[1].downcase.match(/ds/)
	      e=0
	      while e < @ds_nested_names.length
# same here as in L1203
	        if @ds_nested_names[e]==name_type[0]
		  bind=true
	        end
		e += 1
	      end
	      if bind
		argv.push(name_type[0] + " " + name_type[1])
	      else
	        puts $err_state
		raise ArgumentError.new(no_bind.chomp + name_type[0] + 
		      "\n\n NOW EXITING. \n")
	      end
            elsif name_type[1].downcase=="int" || 
	      name_type[1].downcase=="integer" ||
	      name_type[1].downcase=="tinyint" ||
	      name_type[1].downcase=="smallint" || 
	      name_type[1].downcase=="mediumint" || 
	      name_type[1].downcase=="bigint" ||
	      name_type[1].downcase=="unsigned bigint" || 
	      name_type[1].downcase=="int2" ||
	      name_type[1].downcase=="int8" || 
	      name_type[1].downcase=="blob" ||
              name_type[1].downcase=="float" || 
	      name_type[1].downcase=="double"  ||
      	      name_type[1].downcase=="double precision" || 
	      name_type[1].downcase=="real" ||
	      name_type[1].downcase=="numeric" || 
	      name_type[1].downcase=="date" ||
              name_type[1].downcase=="bool" || 
	      name_type[1].downcase=="boolean" ||
	      name_type[1].downcase=="datetime" || 
	      name_type[1].downcase.match(/\idecimal/) ||
	      name_type[1].downcase=="text" || 
	      name_type[1].downcase=="clob" ||
              name_type[1].downcase.match(/\icharacter/) || 
	      name_type[1].downcase.match(/\ivarchar/) ||
	      name_type[1].downcase.match(/\invarchar/) || 
	      name_type[1].downcase.match(/\ivarying character/) ||
	      name_type[1].downcase.match(/\inative character/) || 
	      name_type[1].downcase.match(/\inchar/)
		argv.push(name_type[0] + " " + name_type[1].upcase)
	    elsif name_type[1].downcase=="string"
	      argv.push(name_type[0] + " STRING")
# STRING: was TEXT
            else
	      puts $err_state
              raise TypeError.new("no such data type " + name_type[1].upcase +
 	      	    		      data_types) 
	    end
	  else
	    puts $err_state
	    raise ArgumentError.new("error in input format." + 
	    	  "expected name,type got " + attributes[i] + 
	    	  "\n\n NOW EXITING. \n")
	  end
	  i+=1
	end
	return argv
      end



# splits a template argument into the classes definitions that it
# contains (: delimeter). 

      def register_class(ds_chars_inst, my_array, index)


# HERE DOCUMENTS FOR METHOD


  # HereDoc1

     tmpl_abuse = <<-TA
template arguments abuse class definition. 
multiple class name-type assignments

NOW EXITING

TA


# END OF HEREDOCS

        columns=Array.new
	description = Hash.new
	if my_array[index].include?(":")
	  if ds_chars_inst.template1_type == "none"
	    ds_chars_inst.template2_type = "complex"
	  elsif ds_chars_inst.template1_type==""
	    ds_chars_inst.template1_type = "complex"
	  else
	    ds_chars_inst.template2_type = "complex"
	  end
	  classes=my_array[index].split(/:/)

# bottom up
	  i = -1 + classes.length
	  while i >= 0
	     puts classes[i]	
	     attributes = classes[i].split(/-/)
	     k = 0
	     while k<attributes.length
	       puts attributes[k]
	       if attributes[k].match(/,class/)
		 name = attributes[k].split(/,/)
	       end
	       k += 1
	     end
	     columns = transform(attributes)
	     description[name[0]] = columns
	     i -= 1
	  end
	elsif my_array[index].include?("-")   
# user defined class with attributes of primitive type
	  if ds_chars_inst.template1_type == "none"
	    ds_chars_inst.template2_type = "user_simple"
	  elsif ds_chars_inst.template1_type==""
	    ds_chars_inst.template1_type = "user_simple"
	  else
	    ds_chars_inst.template2_type = "user_simple"
	  end
	  attributes = my_array[index].split(/-/)
	  k = 0
	  while k<attributes.length
	    puts attributes[k]
	    if attributes[k].match(/,class/)
	      name = attributes[k].split(/,/)
	    end
	    k += 1
	  end
	  columns = transform(attributes)
	  description[name[0]] = columns
	else
	  if ds_chars_inst.template1_type == "none"
	    ds_chars_inst.template2_type = "primitive"
	  elsif ds_chars_inst.template1_type==""
	    ds_chars_inst.template1_type = "primitive"
	  else
	    ds_chars_inst.template2_type = "primitive"
	  end
	  attributes = Array.new
	  if my_array[index].include?(",")
	    attributes.push(my_array[index])
# for type validation
# need to ush to attributes and call transform?
	    columns = transform(attributes)
	    name = my_array[index].split(/,/)
# see to it: age INT
	    columns[0] = name[0] + " " + name[1].upcase
	    description[name[1]] = columns
	  else
	    "error in input format " + my_array[index]
	  end
	end
 	description.each_pair { |key, value_array| 
	   value_array.each {|value| puts "key is #{key} value is #{value}"}}  
	return description
      end


# split the string description into arguments (; delimeter) to extract
# the required info and then
# call register_class for each template argument. 

      def register_datastructure

# HERE DOCUMENTS FOR METHOD


    # HereDoc1

      class_sign = <<-CS
STL class signature not properly given:
template error in
CS

    # HereDoc2

      ass_args = <<-ASR
wrong number of arguments for associative datastructure

NOW EXITING.

ASR

    # HereDoc3

      col_args = <<-CLR
wrong number of arguments for datastructure of type collection

NOW EXITING.

CLR

    # HereDoc4

      nargs = <<-NAR
wrong number of arguments. check input description 


NOW EXITING.

NAR

  generated_files = <<-fg

--------------------------------------------------------------
EXECUTION SUCCESSFUL!

If you type "ls" you will notice three new files inside the 
current directory namely:

-> search.cpp
-> main.template
-> makefile.template

The .template files need modification from you.

Main.template expects from you one or more data structures to 
be defined and filled with data.
You will also need to fill in the number of data structures you have
in the respective field.
You can consult main.example for additional help.
After you have completed the modifications type in command line
"mv main.template main.cpp" to rename the file.

In order to have access to the fields of your user defined classes
we assume that for each attribute, let's call it x, there is a get_x()
function present.

Makefile.template needs to be modified too so that S[Q->T]L can be 
compiled together with your application code.
Modifications involve relationships betwen classes in your code.
The compile statements for each class file independently are 
there.
Also add the path to the sqlite3, swill libraries if you have them 
installed locally.
After you have completed the modifications type in command line
"mv makefile.template makefile" to rename the file.

Finally, type make to compile and then ./test to run the executable.

fg

# END OF HEREDOCS

        puts "description before whitespace cleanup " + @description
        @description.gsub!(/\s/,"")
        puts "description after whitespace cleanup " + @description

	
	ds = @description.split(/!/)

	data_structure = Array.new
        templates_representation = Array.new
	ds_chars = Array.new

	w = -1 + ds.length
	l = -1 + ds.length

	while w > 0

	  puts "\nDATA STRUCTURE DESCRIPTION No: " + w.to_s + "\n"

	  data_structure[l-w] = Hash.new
          templates_representation[l-w] = Template.new
	  ds_chars[l-w] = Data_structure_characteristics.new

	  my_array = ds[w].split(/;/)
# data structure name
	  ds_chars[l-w].name=my_array[0]
	  @ds_nested_names.push(my_array[0])
	  @data_structures_names[my_array[0]]=l-w
	  ds_chars[l-w].level=my_array[1]

# @classnames is used to keep track of classes contained
# in a datastructure for avoidance of duplication
# and to help recognizing nested classes
# cleared to be reused

	  if !@classnames.empty?
	    @classnames.clear
	  end

	  if my_array[2].include?("<") && my_array[2].include?(">")
	    container_split=my_array[2].split(/</)
	    container_class=container_split[0]
	  else
	    raise ArgumentError.new(class_sign + my_array[2] + 
	     	     "\n\n NOW EXITING. \n") 
	  end

	
          if container_class=="list" || container_class=="deque"  || 
	    container_class=="vector" || container_class=="slist" ||
            container_class=="set" || container_class=="multiset" ||
            container_class=="hash_set" || container_class=="hash_multiset" ||
	    container_class=="bitset"
                       @template_args="single"
          elsif container_class=="map" ||
            container_class=="multimap" || container_class=="hash_map" || 
 	    container_class=="hash_multimap"
                       @template_args="double"
          else     
	    puts $err_state
            raise TypeError.new("no such container class: " + container_class+
 	  			  "\n\n NOW EXITING. \n")
          end

	  if (@template_args=="single" && container_split[1].include?(",")) || 
	     (@template_args=="double" && !container_split[1].include?(","))
	       raise ArgumentError.new(class_sign + my_array[2] + 
	     	     "\n\n NOW EXITING. \n")
	  end


          if container_class=="list" || container_class=="deque"  || 
	    container_class=="vector" || container_class=="slist" ||
	    container_class=="bitset"
                       @container_type="sequence"
          elsif container_class=="map" ||
            container_class=="multimap" || container_class=="hash_map" || 
	    container_class=="hash_multimap" || 
            container_class=="set" || container_class=="multiset" ||
            container_class=="hash_set" || container_class=="hash_multiset"
                       @container_type="associative"
          elsif container_class=="bitset"
	  	       @container_type="bitset"
	  end

	  ds_chars[l-w].signature=my_array[2]
	  puts "container signature is: " + ds_chars[l-w].signature
          puts "no of template args is: " + @template_args
	  puts "container type is: " + @container_type

	  i=0
	  while i<my_array.length
	    puts my_array[i]
	    i+=1
	  end 

	  if my_array.length==4
	    unless @template_args=="single"
	      puts $err_state
	      raise ArgumentError.new(no_args)
	    end
	    ds_chars[l-w].template1_type="none"
	    
	    templates_representation[l-w].template2 = 
	    			register_class(ds_chars[l-w], my_array, 3)
	    templates_representation[l-w].template1["none"] = nil
	  elsif my_array.length==5
	    unless @template_args=="double"
	      puts $err_state
	      raise ArgumentError.new(col_args)
	    end
	    templates_representation[l-w].template1 = 
	    			register_class(ds_chars[l-w], my_array, 3)
	    templates_representation[l-w].template2 = 
	    			register_class(ds_chars[l-w], my_array, 4)
	  else
	    puts $err_state
	    raise ArgumentError.new(nargs)
	  end
	  ds_chars[l-w].classnames.replace(@classnames)
	  puts ds_chars[l-w].classnames.inspect
	  data_structure[l-w].store(ds_chars[l-w],
				templates_representation[l-w])
	  @data_structures_array.push(data_structure[l-w])
	  w -= 1
	end

	q = 0
	tmpr_ds=Hash.new
	tmpr_chars=Data_structure_characteristics.new
	tmpr_keys=Array.new
	tmpr_template = Template.new
#	tmpr_classes1 = Hash.new	    
#	tmpr_classes2 = Hash.new
	tmpr_class = Hash.new

	puts "\n Data structures stored: " + 
	     @data_structures_array.length.to_s + "\n"
	while q < @data_structures_array.length
	  tmpr_ds=@data_structures_array[q]
	  tmpr_keys=tmpr_ds.keys
	  tmpr_chars=tmpr_keys[0]

	  puts tmpr_chars.name
	  puts tmpr_chars.signature
	  puts tmpr_chars.template1_type
	  puts tmpr_chars.template2_type

	  tmpr_template = tmpr_ds.fetch(tmpr_chars)

	  tmpr_class = tmpr_template.template1
	  if tmpr_class.has_key?("none") 
	    puts "empty template"
	  else
	    tmpr_class.each_pair{|class_name, attribute_array|
	    attribute_array.each{|attribute|
	    puts "#{class_name}, #{attribute}"}}
	  end
	  tmpr_class = tmpr_template.template2
	  tmpr_class.each_pair{|class_name, attribute_array|
	  attribute_array.each{|attribute|
	  puts "#{class_name}, #{attribute}"}}
	  q += 1
	end
	setup_ds
	create_vt
	write_to_file(ds[0])
	puts "CONGRATS?"
	puts generated_files
    end


end



=begin

user input

automate makefile

welcome bla bla 
latin characters
database name->.db extension
useful information:
if multiple ds embedded in one another start with base
embedded ds will be input as an attribute ie its name followed by a
keyword. detailed instructions follow.
resetting the procedure will get you back to the immediate previous step. 
to reset type reset
ds loop:
  data structure name keywords dependency? note:vt will be named after it
  data structure signature with examples.template instantiation 
       identical to class name
  class loop:
    class name. keywords dependency. identical
    attribute loop:
      attribute name. kd. identical. get_ convention
      attribute type. show list. if not acceptable ask again and again
      input another attribute?
    end
    input another class?
  end
input another data structure?
end

help
reset
construct description

=end

if __FILE__==$0

# Main Heredocs

  welcome = <<-wel

Welcome to S[Q|T]L for C++ applications, an SQL query interface for in memory
objects structured using an STL container.

The way the interface works is by mapping the user defined data
structure to a virtual table using the respective API of the Sqlite3 
database engine.

For this to happen we need from you to provide us with a description
of the data structure content, namely the characteristics of the
inherent object.

We hope that the input handler will be of adequate help.
Please use only latin letters in your description.
Step by step instructions are provided.


wel

  ds_description = <<-dsd

Now we will guide you through the process of checking in the
descriprion of your data structures.
You can check in any number of data structure descriptions.

For additional information type "help".


dsd

  ds_help = <<-dsh

If a data structure contains another (as attribute of a class), start
 with the description of the base one.  
You can pinpoint the embedded one as an attribute and provide its
description as a separate data structure after finishing the first description.

Please ensure that exactly the same name is used in order to perform
the mapping between the data structure attribute and its description.


dsh

  init_datastr_des = <<-ind

If you desire to check in a data structure description please
   type "yes" and hit <return>: 

note: Any other answer will be interpreted as no.


ind

  init_class_des = <<-inc

For additional information type "help".

If you desire to check in a class description please
   type "yes" and hit <return>: 

note: Any other answer will be interpreted as no.


inc

  class_help = <<-clh
Please start with base class i.e. the one that appears in container
signature and input embedded objects as attributes. 
Specific keyword exists to pinpoint the special semantics.

After you have finished with the first description you can check in
the description for the embedded class.

Please ensure that the name of the embedded attribute is identical to
the name of the respective class desription.

You can repeat this process as many times as you need.
There is no restriction on the complexity of the class or classes
contained in the data structure.

They are used internally to direct operations towards the mapping to
virtual tables.


clh

  init_attr_des = <<-inat

For additional information type "help".

If you desire to check in an attribute description please
   type "yes" and hit <return>:

note: Any other answer will be interpreted as no. 


inat


  attr_help = <<-ath

If the attribute is an object then give as name the
exact name of the class and as type either "class" or "class_pointer"
depending on how it is provided in class definition.
After you finish this class description start a new one for the
embedded object.
Please remember that the class name should match.

If the attribute is a data structure, again, give as name the
exact name of the data structure and as type either "ds" or "ds_pointer"
depending on how it is provided in class definition.
After you finish this class description start a new one for the
embedded object.


Available primitive data types are:
int or integer, tinyint, smallint, mediumint, 
bigint, unsigned bigint, int2, int8, 
blob(no type.values stored exactly as input), 
float, double, double precision, real, numeric, date, 
datetime, bool or boolean, decimal(10,5), text, 
clob(type text), character(20), varchar(255), varying 
character(255), nchar(55), native character(70), 
nvarchar(100), string


ath

# Please refrain from using as class or attribute names the following
# words: "ds", "ds_pointer", "jump", "reference", "class", "inherits_from"

# what happens with ds or ds pointer

# multiple inheritance?e.g. Person, , Baker

# You will always have the opportunity to go one step back by typing "reset".

  description =
  "foo.db!Trucks;top;vector<Truck*>;Truck,class_pointer
  -cost,double-delcapacity,int-pickcapacity,int-rlpoint,int-Customers,ds_pointer!Customers;embedded;vector<Customer*>;Customer,class_pointer-demand,int-code,string-serviced,bool-pickdemand,int-starttime,int-servicetime,int-finishtime,int-revenue,int"

#  description = "foo.db!account;vector<Account>;Account,class
#  inherits_from
#  SpecialAccount-account_no,text-balance,float-isbn,integer:SpecialAccount,class
#  inherits_from
#  SuperAccount-special_no,text-special_balance,float-special_isbn,integer:SuperAccount\
#,
#  class-iba,integer"

#=end
=begin
   puts welcome
   puts "Please input the database name(no extension needed) and hit <return>:"
   puts "note:if a database file with that name does not exist in the 
   	directory a new one will be created."
   db_name = gets.chomp
   description += db_name + "!"
   puts ds_description
   puts init_datastr_des
   ds_des = gets.chomp     
   while ds_des == "help"	   
     puts ds_help
     puts init_datastr_des
     ds_des = gets.chomp
   end
#=begin
   ds_counter = 0
   while ds_des=="yes"
     if ds_counter != 0
       description += "!"
     end
     puts "Please input the data structure name: "
     puts "note: the Virtual Table will be named after it."
     ds_name = gets.chomp
     description += ds_name + ";"
     puts "Please input data structure signature: "
     puts "note: signature consists of container class 
     	  and template argument instantiation(s)"
     puts "e.g. vector<Account> , map<int,Account>"
     signature = gets.chomp
     description += signature + ";" 
     if signature.match(/,/)
       template_args = 2
     else 
       template_args = 1
     end
     tmpl_counter = 0     
     while tmpl_counter < template_args
       if tmpl_counter != 0
         description += ";"
       end
       if tmpl_counter==0 
         if template_args==2
           puts "Please check in first template argument description"
         else template_args==1
           puts "Please check in template argument description"       
	 end
       else
         puts "Please check in second template argument description"
       end
       puts "Is the template argument of primitive type?"
       puts "e.g. the key of an associative data structure, perhaps of
       	       type int or string?"
       puts "If this is the case please type \"yes\"."	 
       puts "Any other answer will be interpreted as no."
       prmtv = gets.chomp
       if prmtv == "yes"
         puts "Please input a name for the field:"
	 field_name = gets.chomp
	 puts "Please input its data type:"
	 puts "For information on data types type \"help\"."
	 d_type = gets.chomp
	 while d_type == "help"	   
             puts attr_help
	     puts "Please input its data type:"
     	     d_type = gets.chomp
         end
	 description += field_name + "," + d_type
       else
         puts init_class_des
         clas = gets.chomp
         while clas == "help"	   
           puts class_help
     	   puts init_class_des
     	   clas = gets.chomp
         end
         class_counter = 0
         while clas == "yes"
           if class_counter != 0
             description += ":"
           end
           puts "Please input class name: "
	   class_name = gets.chomp
	   puts "Please input the way objects of the class can be accessed: "
	   puts "Two possible answers expected: \"object\" or \"pointer\""
	   class_type = gets.chomp
	   if class_type == "object"
	     class_type = "class"
	   elsif class_type == "pointer"
	     class_type = "class_pointer"
	   else
	     raise TypeError.new("expected \"object\" or \"pointer\" got " +
   	   	   class_type + "\n")
	   end
	   puts "Does " + class_name + " class inherit from another class?"
	   puts "If this is the case please type \"yes\"."	 
	   puts "Any other answer will be interpreted as no."
	   inh = gets.chomp
	   if inh == "yes"
	     puts "Please type the exact name of the super class:"
	     super_class = gets.chomp
	     super_class = " inherits_from " + super_class
	   else
	     super_class = ""
	   end
	   description += class_name + "," + class_type + super_class
	   puts init_attr_des
       	   attribute = gets.chomp
       	   while attribute == "help"	   
             puts attr_help
     	     puts init_attr_des
     	     attribute = gets.chomp
           end
	   attr_counter = 0
	   while attribute == "yes"
             description += "-"
	     puts "Please type the attribute name: "
	     attribute_name = gets.chomp
	     puts "Please enter the attibute type: "
	     puts "For additional information type \"help\"."
	     attribute_type = gets.chomp
	     while attribute_type == "help"	   
	       puts attr_help
	       puts "Please enter the attibute type: "
	       puts "For additional information type \"help\"."
	       attribute_type = gets.chomp
	     end
	     description += attribute_name + "," + attribute_type
	     puts init_attr_des
       	     attribute = gets.chomp
       	     while attribute == "help"	   
               puts attr_help
     	       puts init_attr_des
     	       attribute = gets.chomp
             end
	     attr_counter += 1
	   end
           puts init_class_des
           clas = gets.chomp
           while clas == "help"	   
             puts class_help
     	     puts init_class_des
     	     clas = gets.chomp
           end
	   class_counter += 1
	 end
       end
       tmpl_counter += 1
     end
     puts init_datastr_des
     ds_des = gets.chomp
     while ds_des == "help"	   
       puts ds_help
       puts init_datastr_des
       ds_des = gets.chomp
     end
     ds_counter += 1
   end
=end
input=Input_Description.new(description)
=begin
    input=Input_Description.new("foo .db!account;
    vector<Account>;Account,class-a ccount_no,text-balance,FLoat")  

#=end
    input=Input_Description.new("foo .db!account;
    map< string,Account >;
    nick_name,string;Account,class-a ccount_no,text-balance,FLoat-isbn,
    integer")
# -persons,ds!persons;object;vector<Person>;Person,
# class_pointerinherits_fromAddress-name,string-age,int:
# Address,class-street,string-number,int-postcode,int")
#=begin
    input=Input_Description.new("foo .db;account;
    deque<Account>;Account,class-a ccount_no,text-balance,FLoat-isbn,integer") 
#=end
    input=Input_Description.new("foo .db;account;
    set<Account>;Account,class-a ccount_no,text-balance,FLoat") 
#=end
    input=Input_Description.new("foo .db;account;
    multiset<Account>;Account,class-a ccount_no,text-balance,FLoat-isbn,integer") 
#=end
    input=Input_Description.new("foo .db;account;	multimap<string,Account>;nick_name,string;Account,class-a ccount_no,text-balance,FLoat-isbn,integer")
#=begin
#=end
    input=Input_Description.new("foo .db;emplo	yees;
     vector;nick_name,class-nick_name,string;")         
#=end
    input=Input_Description.new("foo .db;emplo	yees;
    map;nick_name,class-nick_name,string;")             
#=end
    input=Input_Description.new("foo .db;emplo	yees;
    multimap;nick_name,string;employee,class-name,
    string-salary,int")                                
#=end
    input=Input_Description.new("foo .db;emplo	yees;	vector;nick_name,string;employee,class-name,	string-salary,int-account
    ,reference:account,class-a ccount_no,
 string-balance,float")                                 e
#=end
    input=Input_Description.new("foo .db;emplo	yees;	multimap;nick_name,class-nick_name,strin;employee,class-name,	string-salary,int-account
    ,reference-Car,reference:account,class-a ccount_no,
string-balance,float:car,class-model,string-km,double")    e
#=end
    input=Input_Description.new("foo .db;emplo	yees;	multimap;nick_name,class-nick_name,string;employee,class-name,	string-salary,int-account
    ,reference-Car,reference:Car,class-model,string-km,double:account,class-a ccount_no,
string-balance,float")                                     
#=end
    input=Input_Description.new("foo .db;emplo	yees;	multimap;nick_name,class-nick_name,string;employee,class-name,	string-salary,int-account
    ,reference:account,class-a ccount_no,
string-balance,float-Car,reference:Car,class-model,string-km,double")                                     
#=end
    input=Input_Description.new("foo .db;emplo	yees;	multimap;nick_name,class-nick_name,string;employee,class-name,	string-salary,int-account
    ,reference:account,class-a ccount_no,
string-balance,float-Bank,reference:Bank,class-address,string-head,string")                
#=end
    input=Input_Description.new("foo .db;emplo	yees;	multimap;nick_name,class-nick_name,string;employee,class-name,	string-salary,int-account
    ,reference:Bank,class-address,string-head,string:account,class-a ccount_no,
string-balance,float-Bank,reference")                                         e
#=end
    input=Input_Description.new("foo .db;emplo	yees;	multimap;nick_name,class-nick_name,string;employee,class-name,	string-salary,int-account
    ,reference:account,class-a ccount_no,
string-balance,float-Bank,reference")                                         e
#=end
    input=Input_Description.new("foo .db;emplo	yees;	multimap;nick_name,class-nick_name,string;employee,class-name,	string-salary,int-account
    ,reference-Car,reference:Car,class-model,string-km,double:account,class-a ccount_no,
string-balance,float")                       
#=end            

#breadth first

    input=Input_Description.new("foo .db;test;
    multimap;nick_name,class-nick_name,string;top,class-name,
    string-salary,int-classA
    ,reference-classB,reference:classA,class-model,string-km,double-classC
   ,reference-classD,reference:classB,class-a ccount_no,
   string-balance,float-classE,reference-classF,reference:classC,class-att1,int-att2,string-att3,double
   :classD,class-att1,int-att2,string-att3,double:classE,class-att1,int-att2,string-att3,double:classF,class-att1,int-att2,string-att3,double")     
#=end

#depth first

    input=Input_Description.new("foo .db;test;
    multimap;nick_name,class-nick_name,string;class,class-att1,int-att2,string-att3,double-classA,reference-classB,reference:classA,class-att1,int-att2,string-att3,double-classC,
    reference-classD,reference:classC,class-att1,int-att2,string-att3,double:classD,class-att1,int-att2,string-att3,double:classB,class-att1,int-att2,string-att3,double-classE,
    reference-classF,reference:classE,class-att1,int-att2,string-att3,double:classF,class-att1,int-att2,string-att3,double")        



    input=Input_Description.new("foo .db;test;
    multimap<string,Account>;nick_name,class-nick_name,string;class,class-att1,int-att2,string-att3,double-classA,reference-classB,reference:classA,class-att1,int-att2,string-att3,double-classC,
    reference-classD,reference:classC,class-att1,int-att2,string-att3,double:classD,class-att1,int-att2,string-att3,double-classG,reference:classG,class-att1,int-att2,string-att3,double:
    classB,class-att1,int-att2,string-att3,double-classE,
    reference-classF,reference:classE,class-att1,int-att2,string-att3,double:classF,class-att1,int-att2,string-att3,double-classH,reference:
    classH,class-att1,int-att2,string-att3,double")        

=end
    input.register_datastructure

end
