# raise alarm if special characters are used in description

# what's the use of the class?
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
      	  @name=""
	  @signature=""	
	  @nested="" 
	  @template1_type="" 
	  @template2_type=""
      end
      attr_accessor(:name,:signature,:nested,
      		    :template1_type,:template2_type) 

end

class Input_Description

      def initialize(description="")
          @description=description
	  @signature=""
	  @column_traverse = ""
	  @jump = 0
	  @back = 0
	  @inheritance = 0
	  @action = Array.new
	  @follow_up = Array.new
	  @data_structures_array=Array.new
          @ds_nested_names = Array.new
	  @classnames=Hash.new
	  @container_type=""
	  @template_args=""
	  @filename="main.template"
	  @query=""
	  @queries=Array.new
	  @s = "        "
      end




# takes as input a template description and an array
# containing attributes of a class. template description is a Hash
# containing the class descriptions of a template instantiation.
# (Hash<string(class name),Array of strings(attributes)>)
# The method traverses all the class attributes.
# if they are of primitive type it concats them to form a query string.
# Otherwise there is either a nested class or an inheritance hierarchy
# to be taken into account. A recursive call is carried out to
# traverse the latter.

# assumption: first attribute records class name and type

      def recursive_traversal(tmpr_class, attributes)
        at = 0
	while at < attributes.length
	  ret_attribute = neat_attributes(attributes[at])
	  if ret_attribute.match(/jump_nested/)
	    split = ret_attribute.split(/ /)
	    puts "split " + split[1]
	    ret_attribute = recursive_traversal(tmpr_class,
				tmpr_class.fetch(split[1]))
	  elsif ret_attribute.match(/jump_inh/)
	    split = ret_attribute.split(/ /)
	    puts "split " + split[2]
	    ret_attribute = recursive_traversal(tmpr_class,
				tmpr_class.fetch(split[2]))
	  else
	    @query += ret_attribute + "," 
	    puts @query
	  end
	  at += 1
	end
      end


# to construct the string of the query to be passed
# to sqlite engine. some attributes are not of primitive type
# and signify special semantics as stored in
# @data_structures_array


      def neat_attributes(attribute)
        puts "neat_attributes"
        puts attribute
      	if attribute.include?("\s")
	  name_type = attribute.split(/ /)
	else
	  puts "error " + attribute
	end
	if name_type[1]=="ds" || name_type[1]=="ds_pointer"
	  ret_attribute = "id references " + name_type[0]
	elsif name_type[1]=="reference"
	  ret_attribute = "jump_nested " + name_type[0]
	elsif name_type[1].match(/inherits_from/)
	  ret_attribute = name_type[0] + " jump_inh " + name_type[2]
	else
	  ret_attribute = attribute
# primitive type	
  	end  
	return ret_attribute
      end

# produces a valid sql query for creating a virtual table according to
# the argument array columns

      def create_vt()

        puts "create_vt"
	puts ""
	q = 0
	tmpr_ds = Hash.new
	tmpr_chars = Data_structure_characteristics.new
	tmpr_keys=Array.new
	tmpr_template = Template.new
#	tmpr_classes1 = Hash.new	    
#	tmpr_classes2 = Hash.new
	tmpr_class = Hash.new
	template_class = Array.new
	attributes = Array.new

#	if !@classnames.empty?
#	  @classnames.clear
#	end


	while q < @data_structures_array.length
	  tmpr_ds = @data_structures_array[q]

# extract keys from original beasty hash
# contains only one key of type Data_structure_characteristics

	  tmpr_keys = tmpr_ds.keys
	  tmpr_chars = tmpr_keys[0]

          @query = "CREATE VIRTUAL TABLE " + tmpr_chars.name  + 
	  	 " USING stl(" + "pk integer primary key,"
# get template arguments from signature

	  cleared = tmpr_chars.signature.split(/</)
	  tml_arg = cleared[1].chomp(">")
	  puts "tml_arg " + tml_arg
	  puts ""
	  if tml_arg.match(/,/)
	    cleared = tml_arg.split(/,/)	  
	    template_class.push(cleared[0])
	    template_class.push(cleared[1])
	  else
	    template_class.push("none")
	    template_class.push(tml_arg)
	  end
	  puts "template_class1 " + template_class[0]
	  puts "template_class2 " + template_class[1]
	  puts ""
	  tmpr_template = tmpr_ds.fetch(tmpr_chars)

# tmpr_keys length should be one
#	  tmpr_keys = tmpr_template.keys
#	  tmpr_classes1 = tmpr_keys[0]
#	  tmpr_classes2 = tmpr_template.fetch(tmpr_keys[0])

# extract keys from Hash template
# contains only one key of type Array (template description)


	  tmpr_class = tmpr_template.template1
	  if tmpr_class.has_key?("none") 
	    puts "empty template"
	  else
	    recursive_traversal(tmpr_class,
			tmpr_class.fetch(template_class[0]))
	    puts "query " + @query
	  end
	  tmpr_class = tmpr_template.template2
	  recursive_traversal(tmpr_class,
			tmpr_class.fetch(template_class[1]))
	  @query = @query.chomp(",")
	  @query += ")"
	  puts "query final " + @query
	  @queries.push(@query)
	  template_class.clear
	  q += 1
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
                  if operation == "retrieve"
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
                  if operation == "retrieve"
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
                  if operation == "retrieve"
		    @column_traverse += ", -1, SQLITE_STATIC"
                    @column_traverse = "(const char *)" +
        	  		   @column_traverse
		  else
		    @column_traverse = "(const unsigned char *)" +
        	  		   @column_traverse
		  end
	elsif user_datatype=="references"
# needs taken care of
		  ret = "fk_column"
		  @back = 1
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
                        stcsr->resultSet[count++] = i;
                    iter++;
                }
                stcsr->size += count;
                break;
rslt

#	puts "gen_col"
        at = 0
        while at < attributes.length
	  if @back > 0 || @back < 0
	    @back = 0
	  end
          class_name = template.index(attributes)

#	  if @classnames.has_key?(class_name) && 
#	       @classnames[class_name].match(/inherits_from/)
#	    split = @classnames[class_name].split(/inherits_from/)
#	    @action[@action.length] = "inheritance"
#	    puts "INHERITANCE"
#	    ret_attribute = "inheritance"
#	    @inheritance += 1
#	    at -= 1

          ret_attribute = neat_attributes(attributes[at])
          split = ret_attribute.split(/ /)
#	  end
	  if ret_attribute.match(/jump_inh/)
	    @action[@action.length] = "inheritance"
	    puts "INHERITANCE"
	    @inheritance += 1
#	    at -= 1
	  end
# datatypes?length 3?
	  if at <= 1 && split.length == 2
            @follow_up.insert(@follow_up.length,"get_")	      
            if @classnames[class_name] == "class_pointer"
	      class_type = "->"
            else
	      class_type = "."
            end
#	    puts "RECURSIVE"
	  end
          if ret_attribute.match(/jump_nested/)
	    @action[@action.length] = "nested_structure"
	    @jump += 1
            @follow_up[@follow_up.length - 1] += split[1] + "()" + class_type
	    puts "follow_up is " + @follow_up[@follow_up.length - 1]
            split = ret_attribute.split(/ /)
            puts "split " + split[1]
	    if operation == "check"
              gen_col(template, template.fetch(split[1]),
                                    template_no, tmpr_chars, fw, "check")
	    else
              gen_col(template, template.fetch(split[1]),
                                    template_no, tmpr_chars, fw, "retrieve")
	    end
	  elsif ret_attribute.match(/jump_inh/)
            puts "split " + split[2]
	    if operation == "check"
              gen_col(template, template.fetch(split[2]),
                                    template_no, tmpr_chars, fw, "check")
	    else
              gen_col(template, template.fetch(split[2]),
                                    template_no, tmpr_chars, fw, "retrieve")
	    end	    
          else
            @counter += 1
            name_type = ret_attribute.split(/ /)
            @column_traverse = "iter->"
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
	    if operation == "check"
              @column_traverse += ", op, sqlite3_value_" + datatype + "(val)"
              @column_traverse = "if( traverse(" + @column_traverse + ") )"
	    else
	      @column_traverse = "sqlite3_result_" +
			  datatype + "(con, "  + @column_traverse + ");"
	    end
          end
# avoid duplicating statement
	  if @back == 0
	    puts @column_traverse
            fw.puts @s + "    case " + @counter.to_s + ":"
	    if operation == "check"
              fw.puts @s + "        iter=any_dstr->begin();"
              fw.puts @s + "        for(int i=0;i<(int)any_dstr->size();i++){"
	    end
            fw.puts @s + "            " + @column_traverse
	    if operation == "check"
	      fw.puts gather_results
	    else 
	      fw.puts @s + "            break;"
	    end
	  end
          at += 1
        end
        @follow_up.delete_at(@follow_up.length - 1)
	if @inheritance > 0 && @action[@action.length - 1] == "inheritance"
	  @back = -1
	  @inheritance -= 1
	  @action.delete_at(@action.length - 1)
	else 
	  if @jump > 0 && @action == "nested_structure"
	    if @follow_up[@follow_up.length - 1].match(class_name)
	      reduce = @follow_up[@follow_up.length - 1].split(class_name)
	      @follow_up[@follow_up.length - 1] = reduce[0]
	      puts "deletion. now " + @follow_up.length.to_s + " records"
	    end
	    @back = 1
	    @jump -= 1
	    @action.delete_at(@action.length -1)
	  end
	end
      end


# opens a new c source file and writes c code.
# Specifically, it generates the main.template, the search.cpp
# and the makefile.
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
  int re_sqlite;
  void *data;

  // declare and fill datastructure;


  dsCarrier dsC;
  dsC.memories = mem;
  dsC.dsNames = names;

  dsC.size = // input the number of data structures you are registering ;

  data = (void *)&dsC;

  pthread_t sqlite_thread;
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




AG3

	  auto_gen4 = <<-AG4 
//    Type value;
    int op, count = 0;
// val==NULL then constr==NULL also
    if ( val==NULL ){
        for (int j=0; j<get_datastructure_size((void *)stl); j++){
            stcsr->resultSet[j] = j;
            stcsr->size++;
        }
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

        int iCol;
        iCol = constr[1] - 'a' + 1;
        char *colName = stl->azColumn[iCol];
// FK search
        const char *fk = "fk";
        if(!strcmp(colName, fk)){
            iter=any_dstr->begin();
            for(int i=0; i<(int)any_dstr->size(); i++){
                if( traverse((long int)&(*iter), op, sqlite3_value_int(val)) )
                    stcsr->resultSet[count++] = i;
                iter++;
            }
            stcsr->size += count;
        }else{
// handle colName
            switch( iCol ){
// i=0. search using PK?memory location?or no PK?
// no can't do.PK will be memory location. PK in every table
// PK search
            case 0: 
                iter=any_dstr->begin();
                for(int i=0; i<(int)any_dstr->size(); i++){
                    if( traverse((long int)&(*iter), op, sqlite3_value_int(val)) )
                        stcsr->resultSet[count++] = i;
                    iter++;
                }
                stcsr->size += count;
                break;
AG4

	
    #HereDoc5


	cls_search_opn_retrieve = <<-cls_opn
// more datatypes and ops exist
            }
        }
    }
}


cls_opn


    #HereDoc6

	auto_gen5 = <<-AG5
    char *colName = stl->azColumn[n];
    int index = stcsr->current;
// iterator implementation. serial traversing or hit?
    iter = any_dstr->begin();
// serial traversing. simple and generic. visitor pattern is next step.
    for(int i=0; i<stcsr->resultSet[index]; i++){
        iter++;
    }
// int datatype;
// datatype = stl->colDataType[n];
    const char *pk = "pk";
    const char *fk = "fk";
    if ( (n==0) && (!strcmp(stl->azColumn[0], pk)) ){
// attention!
        sqlite3_result_int(con, (long int)&(*iter));
        printf(\"memory location of PK: %x\\n\", &(*iter));
    }else if( !strncmp(stl->azColumn[n], fk, 2) ){
        sqlite3_result_int(con, (long int)&(*iter));
    }else{
// in automated code: \"iter->get_\" + col_name + \"()\" will work.safe?
// no.doxygen.
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

# END OF HereDocs

        myfile=File.open(@filename, "w") do |fw|
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

# extract keys from original beasty hash
# contains only one key of type Data_structure_characteristics

            tmpr_keys=tmpr_ds.keys
            tmpr_chars=tmpr_keys[0]
	    c_type=tmpr_chars.signature.split(/</)
	    if c_type[0].match(/\imap/)
	      c_type[0]="map"
	    elsif c_type[0].match(/\iset/)
	      c_type[0]="set"
	    elsif (c_type[0].match(/\ihash/) && (c_type[0].match(/\set/)))
	      c_type[0]="hash_set"
	    elsif (c_type[0].match(/\ihash/) && (c_type[0].match(/\map/)))
	      c_type[0]="hash_map"
	    elsif c_type[0]=="hash"
	      c_type[0]="hash_set"
# defined also in hash_map
  	    end
            fw.puts "\#include <" + c_type[0] + ">"
	    q += 1
	  end
	  @classnames.each {|key,value| fw.puts "\#include \"#{key}.h\""}

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

	myfile=File.open("search.cpp", "w") do |fw|
          fw.puts "\#include \"search.h\""
          fw.puts "\#include <string>"
	  q = 0
          while q < @data_structures_array.length
            tmpr_ds=@data_structures_array[q]

# extract keys from original beasty hash
# contains only one key of type Data_structure_characteristics

            tmpr_keys=tmpr_ds.keys
            tmpr_chars=tmpr_keys[0]
	    c_type=tmpr_chars.signature.split(/</)
	    if c_type[0].match(/\imap/)
	      c_type[0]="map"
	    elsif c_type[0].match(/\iset/)
	      c_type[0]="set"
	    elsif (c_type[0].match(/\ihash/) && (c_type[0].match(/\set/)))
	      c_type[0]="hash_set"
	    elsif (c_type[0].match(/\ihash/) && (c_type[0].match(/\map/)))
	      c_type[0]="hash_map"
	    elsif c_type[0]=="hash"
	      c_type[0]="hash_set"
# defined also in hash_map
  	    end
            fw.puts "\#include <" + c_type[0] + ">"
	    q += 1
	  end
	  @classnames.each{|key,value| fw.puts "\#include \"#{key}.h\""}

	  fw.puts
	  fw.puts "using namespace std;\n\n"

	  


	  fw.puts "int get_datastructure_size(void *st){"
	  fw.puts "    stlTable *stl = (stlTable *)st;"
	  q = 0
          while q < @data_structures_array.length
            tmpr_ds=@data_structures_array[q]

# extract keys from original beasty hash
# contains only one key of type Data_structure_characteristics

            tmpr_keys=tmpr_ds.keys
            tmpr_chars=tmpr_keys[0]
	    fw.puts "    if( !strcmp(stl->zName, \"" + tmpr_chars.name + "\") ){"
	    fw.puts "        " + tmpr_chars.signature + " *any_dstr = (" +
              tmpr_chars.signature + " *)stl->data;"
	    fw.puts "        return ((int)any_dstr->size());"
	    fw.puts "    }"
	  q += 1
	  end
	  fw.puts "}"


# call HereDoc3	  
	  fw.puts auto_gen3
          q = 0
#          tmpr_ds = Hash.new
#          tmpr_chars=Data_structure_characteristics.new
#          tmpr_keys=Array.new
# created above
          tmpr_template = Template.new
#          tmpr_classes1 = Hash.new
#          tmpr_classes2 = Hash.new
          tmpr_class = Hash.new

          while q < @data_structures_array.length
            tmpr_ds=@data_structures_array[q]

# extract keys from original beasty hash
# contains only one key of type Data_structure_characteristics

            tmpr_keys=tmpr_ds.keys
            tmpr_chars=tmpr_keys[0]
            tmpr_template = tmpr_ds.fetch(tmpr_chars)
# tmpr keys length should be one
#            tmpr_keys = tmpr_template.keys
#            tmpr_classes1 = tmpr_keys[0]
#            tmpr_classes2 = tmpr_template.fetch(tmpr_keys[0])


	    fw.puts "void " + tmpr_chars.name +
            	    "_search(void *stc, char *constr, sqlite3_value *val){"
	    fw.puts "    sqlite3_vtab_cursor *cur = (sqlite3_vtab_cursor *)stc;"
	    fw.puts "    stlTable *stl = (stlTable *)cur->pVtab;"
	    fw.puts "    stlTableCursor *stcsr = (stlTableCursor *)stc;"
	    fw.puts "    " + tmpr_chars.signature +
            	    " *any_dstr = (" + tmpr_chars.signature + " *)stl->data;"
	    fw.puts "    " + tmpr_chars.signature + ":: iterator iter;"
	    fw.puts auto_gen4

#          i=1
# bottom-up
            spl = tmpr_chars.signature.split(/</)
            tmpl_classes = spl[1].chomp(">")
            @counter = 0
            if tmpr_chars.template1_type != "none"
              template_no = 1
              sep_classes = tmpl_classes.split(/,/)
              tmpr_class = tmpr_template.template1
              gen_col(tmpr_class, tmpr_class.fetch(sep_classes[0]),
                                     template_no, tmpr_chars, fw, "check")
              template_no = 2
              tmpr_class = tmpr_template.template2
              gen_col(tmpr_class, tmpr_class.fetch(sep_classes[1]),
                                    template_no, tmpr_chars, fw, "check")
            else
              template_no = 2
              tmpr_class = tmpr_template.template2
              gen_col(tmpr_class, tmpr_class.fetch(tmpl_classes),
                                     template_no, tmpr_chars, fw, "check")
            end
# call HereDoc29
            q += 1
            fw.puts cls_search_opn_retrieve
          end
	  
	  fw.puts "\n\n"

	  q = 0
          while q < @data_structures_array.length
            tmpr_ds=@data_structures_array[q]

# extract keys from original beasty hash
# contains only one key of type Data_structure_characteristics

            tmpr_keys=tmpr_ds.keys
            tmpr_chars=tmpr_keys[0]
            tmpr_template = tmpr_ds.fetch(tmpr_chars)
# tmpr keys length should be one
#            tmpr_keys = tmpr_template.keys
#            tmpr_classes1 = tmpr_keys[0]
#            tmpr_classes2 = tmpr_template.fetch(tmpr_keys[0])

	    fw.puts "int " + tmpr_chars.name +
            	    "_retrieve(void *stc, int n, sqlite3_context *con){"
	    fw.puts "    sqlite3_vtab_cursor *cur = (sqlite3_vtab_cursor *)stc;"
	    fw.puts "    stlTable *stl = (stlTable *)cur->pVtab;"
	    fw.puts "    stlTableCursor *stcsr = (stlTableCursor *)stc;"
	    fw.puts "    " + tmpr_chars.signature +
            	    " *any_dstr = (" + tmpr_chars.signature + " *)stl->data;"
	    fw.puts "    " + tmpr_chars.signature + ":: iterator iter;"
	    fw.puts auto_gen5

            fw.puts "        switch ( n ){"


#          i=1
# bottom-up
            spl = tmpr_chars.signature.split(/</)
            tmpl_classes = spl[1].chomp(">")
            @counter = 0
            if tmpr_chars.template1_type != "none"
              template_no = 1
              sep_classes = tmpl_classes.split(/,/)
              tmpr_class = tmpr_template.template1
              gen_col(tmpr_class, tmpr_class.fetch(sep_classes[0]),
                                     template_no, tmpr_chars, fw, "retrieve")
              template_no = 2
              tmpr_class = tmpr_template.template2
              gen_col(tmpr_class, tmpr_class.fetch(sep_classes[1]),
                                    template_no, tmpr_chars, fw, "retrieve")
            else
              template_no = 2
              tmpr_class = tmpr_template.template2
              gen_col(tmpr_class, tmpr_class.fetch(tmpl_classes),
                                     template_no, tmpr_chars, fw, "retrieve")
           end
# call HereDoc29
            q += 1
          fw.puts "        }"
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

# extract keys from original beasty hash
# contains only one key of type Data_structure_characteristics

          tmpr_keys=tmpr_ds.keys
          tmpr_chars=tmpr_keys[0]
	  fw.puts "    if( !strcmp(stl->zName, \"" + tmpr_chars.name + "\") )"
	  fw.puts "        " + tmpr_chars.name +
	    	    "_search(stc, constr, val);"
	  q += 1
	end

	fw.puts "}\n\n"


	fw.puts "int retrieve(void* stc, int n, sqlite3_context *con){"
	fw.puts "    sqlite3_vtab_cursor *cur = (sqlite3_vtab_cursor *)stc;"
	fw.puts "    stlTable *stl = (stlTable *)cur->pVtab;"
	q = 0
        while q < @data_structures_array.length
          tmpr_ds=@data_structures_array[q]

# extract keys from original beasty hash
# contains only one key of type Data_structure_characteristics

          tmpr_keys=tmpr_ds.keys
          tmpr_chars=tmpr_keys[0]
	  fw.puts "    if( !strcmp(stl->zName, \"" + tmpr_chars.name + "\") )"
	  fw.puts "        " + tmpr_chars.name +
	    	    "_retrieve(stc, n, con);"
	  q += 1
	end

	fw.puts "}\n\n"

      end

      myfile = File.open("makefile.template","w") do |fw|
        fw.print "test: main.o search.o stl_to_sql.o user_functions.o "
	@classnames.each{|key,value| fw.print "#{key}.o "}
	fw.puts
        fw.print "	g++ -lswill -lsqlite3 -W -g main.o search.o stl_to_sql.o user_functions.o "
	@classnames.each{|key,value| fw.print "#{key}.o "}
	fw.puts "-o test"
	fw.puts makefile_part	
	fw.puts
	@classnames.each{|key,value| fw.puts "#{key}.o: #{key}.cpp #{key}.h \n" + "	g++ -W -g -c #{key}.cpp \n\n"}
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
#	argv.push("stl")
#	argv.push(my_array[0])
#	argv.push(my_array[2])
# PK for all tables (top level too)
#	argv.push("id INTEGER PRIMARY KEY AUTOINCREMENT")
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
# top level tables won't go in this condition only intermediate ones.
# table name is set as default so that it works for top level.
# intermediate tables override default with respective class name.
#	      argv.delete_at(2)
#	      argv.insert(2,name_type[0])
#	      argv.push("INTEGER PRIMARY KEY AUTOINCREMENT")
	    elsif name_type[1].downcase=="reference"
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
#	template = Hash.new
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
#	template = description
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
we aaume that for each attribute, let's call it x, there is a get_x()
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
#	template1 = Array.new
#	template2 = Array.new

	w = - 1 + ds.length
	l = - 1 + ds.length
	while w > 0

	  puts "\nDATA STRUCTURE DESCRIPTION No: " + w.to_s + "\n"

	  data_structure[l-w] = Hash.new
          templates_representation[l-w] = Template.new
	  ds_chars[l-w] = Data_structure_characteristics.new
#	  template1[l-w] = Hash.new
#	  template2[l-w] = Hash.new

	  my_array = ds[w].split(/;/)
# data structure name
	  ds_chars[l-w].name=my_array[0]
	  @ds_nested_names.push(my_array[0])

# @classnames is used to keep track of classes contained
# in a datastructure for avoidance of duplication
# and to help recognizing nested classes
# cleared to be reused

	  if !@classnames.empty?
	    @classnames.clear
	  end

	  if my_array[1].include?("<") && my_array[1].include?(">")
	    container_split=my_array[1].split(/</)
	    container_class=container_split[0]
	  else
	    raise ArgumentError.new(class_sign + my_array[1] + 
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
	       raise ArgumentError.new(class_sign + my_array[1] + 
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

	  @signature=my_array[1]
	  ds_chars[l-w].signature=my_array[1]
	  puts "container signature is: " + @signature
          puts "no of template args is: " + @template_args
	  puts "container type is: " + @container_type

	  i=0
	  while i<my_array.length
	    puts my_array[i]
	    i+=1
	  end 

	  if my_array.length==3
	    unless @template_args=="single"
	      puts $err_state
	      raise ArgumentError.new(no_args)
	    end
	    ds_chars[l-w].template1_type="none"
	    
	    templates_representation[l-w].template2 = 
	    			register_class(ds_chars[l-w], my_array, 2)
	    templates_representation[l-w].template1["none"] = nil
#	    templates_representation[l-w].store(template1[l-w], 
#	    						template2[l-w])
	  elsif my_array.length==4
	    unless @template_args=="double"
	      puts $err_state
	      raise ArgumentError.new(col_args)
	    end
	    templates_representation[l-w].template1 = 
	    			register_class(ds_chars[l-w], my_array, 2)
	    templates_representation[l-w].template2 = 
	    			register_class(ds_chars[l-w], my_array, 3)
#	    templates_representation[l-w].store(template1[l-w], 
#	    						template2[l-w])
	  else
	    puts $err_state
	    raise ArgumentError.new(nargs)
	  end
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

# extract keys from original beasty hash
# contains only one key of type Data_structure_characteristics

	  tmpr_keys=tmpr_ds.keys
	  tmpr_chars=tmpr_keys[0]

	  puts tmpr_chars.name
	  puts tmpr_chars.signature
	  puts tmpr_chars.template1_type
	  puts tmpr_chars.template2_type

	  tmpr_template = tmpr_ds.fetch(tmpr_chars)

# tmpr_keys length should be one
#	  tmpr_keys = tmpr_template.keys
#	  tmpr_classes1 = tmpr_keys[0]
#	  tmpr_classes2 = tmpr_template.fetch(tmpr_keys[0])

# extract keys from Hash template
# contains only one key of type Array (template description)

  	  

	  tc = 0
#	  tmpr_class = tmpr_template.template1
	  if tmpr_class.has_key?("none") 
	    puts "empty template"
	  else
	    tmpr_class.each_pair{|class_name, attribute_array|
	    attribute_array.each{|attribute|
	    puts "#{class_name}, #{attribute}"}}
	  end
	  tmpr_class = tmpr_template.template2
	  if tmpr_class.has_key?("none")
	    puts "empty template"
	  else
	    tmpr_class.each_pair{|class_name, attribute_array|
	     attribute_array.each{|attribute|
	     puts "#{class_name}, #{attribute}"}}
	  end
	  q += 1
	end
	create_vt
	write_to_file(ds[0])
	puts "CONGRATS?"
	puts generated_files
    end

#=end

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
  "foo.db!account;map<string,Account>;nick_name,string;Account,class
  inherits_from
  SuperAccount-account_no,text-balance,float-isbn,integer:SuperAccount,
  class-iba,text"
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
