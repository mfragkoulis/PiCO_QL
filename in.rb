# raise alarm if special characters are used in description


class Input_Description

      def initialize(description="")
          @description=description
	  @classnames=Array.new
	  @filename="sql_calls.c"
	  @queries=Array.new
      end



# produces a valid sql query for creating a virtual table according to
# the argument array columns

      def create_vt(columns)

#         if columns[1].include?(".")
#	    no_extension=columns[1].split(/\./)
#	    columns[1]=no_extension[0]	    
#	 end

         query="CREATE VIRTUAL TABLE " + columns[2]  + " USING " + columns[0] + "("
	 i=3
	 while i<columns.length
	    query+=columns[i]
	    if i+1==columns.length
	       query+=")"	       	       
	    else 
	       query+=","
	    end
	    i+=1
	 end	 
	 puts query
	 @queries.push(query)
      end

# opens a new c source file and writes c code.
# there for each query a call to register_table is done
# to create the respective VT.
# includes action to be taken in case a call fails.
# all created VTs at that point are dropped and program exits.

      def write_to_file(db_name)
	myfile=File.open(@filename, "w") do |fw|
	  fw.puts "\#include <stdio.h>"
	  fw.puts "\#include <string.h>"
	  fw.puts "\#include \"stl_to_sql.h\""
	  fw.puts "\n\n\n"
	  fw.puts "int main() {"
	  fw.puts "  int failure=0, count=0;"
	  i=0
	  while i<@queries.length
	     fw.puts "  if (!failure) {"
	     fw.puts "    failure=register_table(\"" + db_name + "\",\"" + @queries[i] + "\");"
	     fw.puts "    if (!failure) count++;"
	     fw.puts "  }"
	     i+=1
	  end
	  fw.puts "  if (failure) printf(\"\\n\\nERROR STATE. CANCELLING COMPLETED OPERATIONS...\\n\\n\");"
	  i=0
	  while i<@classnames.length
	      fw.puts "  if (count>0) {"
	      fw.puts "    failure=register_table(\"" + db_name + "\",\"DROP TABLE " + @classnames[i] + "\");"
	      fw.puts "    count--;"
	      fw.puts "    if (failure) printf(\"\\n\\nFAILURE TO DROP TABLE\\n\\n\");"
	      fw.puts "  }"
	      i+=1
	  end
	  fw.puts "}"
	end
      end	       


# produces the array argv which contains all the necessary arguments for a well-formed "CREATE VIRTUAL TABLE" query
# argument attributes is an array containing a class description (pairs of name,type aka attributes)
# maps types to sqlite3 data_types and pushes pairs to argv

      def transform(my_array, attributes)
      	  argv=Array.new
	  argv.push("mod")
	  argv.push(my_array[0])
	  argv.push(my_array[1])
	  i=0
	  while i< attributes.length
	     if attributes[i].include?(",")
	        name_type=attributes[i].split(/,/)
		if name_type.length!=2
		    puts "\nERROR STATE. CANCELLING COMPLETED OPERATIONS:\n"
		    puts "\nPRINTING ERROR INFO:\n"
		    raise ArgumentError.new("expected pair name,type got " + attributes[i] + "\n\n NOW EXITING. \n") 
		end
		name_type[1]=name_type[1].downcase
                if name_type[1]=="class"
		    k=0
		    while k<@classnames.length
		       if @classnames[k]==name_type[0]
		          puts "\nERROR STATE. CANCELLING COMPLETED OPERATIONS:\n"
			  puts "\nPRINTING ERROR INFO:\n"
 		          raise ArgumentError.new("Attempt to create virtual table twice" + "\n\n NOW EXITING. \n")
		       end
		       k+=1
		    end
		    @classnames.push(name_type[0])
                    puts "table_name is " + name_type[0]
		    argv.delete_at(2)
		    argv.insert(2,name_type[0])
		    argv.push("INTEGER PRIMARY KEY AUTOINCREMENT")
		elsif name_type[1]=="reference"
		    k=0
		    while k<@classnames.length
		       if @classnames[k]==name_type[0]
		          pushed=true
		       end
		       k+=1
             	    end
		    if pushed
		       argv.push(name_type[0] + "_id references " + name_type[0])
		    else 
		       puts "\nERROR STATE. CANCELLING COMPLETED OPERATIONS:\n"
		       puts "\nPRINTING ERROR INFO:\n"
		       raise ArgumentError.new("no binding between class and reference keywords. trying to refer to non-defined class: " + name_type[0] + "\n\n NOW EXITING. \n")
		    end
                elsif name_type[1]=="int" || name_type[1]=="integer" ||
		  name_type[1]=="tinyint" || name_type[1]=="smallint" || 
		  name_type[1]=="mediumint" || name_type[1]=="bigint" ||
		  name_type[1]=="unsigned bigint" || name_type[1]=="int2" ||
		  name_type[1]=="int8" || name_type[1]=="blob" ||
                  name_type[1]=="float" || name_type[1]=="double"  ||
      	  	  name_type[1]=="double precision" || name_type[1]=="real" ||
		  name_type[1]=="numeric" || name_type[1]=="date" ||
                  name_type[1]=="bool" || name_type[1]=="boolean" ||
		  name_type[1]=="datetime" || name_type[1].match(/\idecimal/) ||
		  name_type[1]=="text" || name_type[1]=="clob" ||
                  name_type[1].match(/\icharacter/) || name_type[1].match(/\ivarchar/) ||
		  name_type[1].match(/\invarchar/) || name_type[1].match(/\ivarying character/) ||
		  name_type[1].match(/\inative character/) || name_type[1].match(/\inchar/)
		    argv.push(name_type[0] + " " + name_type[1].upcase)
		elsif name_type[1]=="string"
		    argv.push(name_type[0] + " TEXT")
                else
	           puts "\nERROR STATE. CANCELLING COMPLETED OPERATIONS:\n"
		   puts "\nPRINTING ERROR INFO:\n"
                   raise TypeError.new("no such data type " + name_type[1].upcase + ".\nplease select one from following list:
		   int or integer, tinyint, smallint, mediumint,bigint, unsigned bigint, int2, int8, 
		   blob(no type.values stored exactly as input), float, double, double precision, real, 
      	  	   numeric, date, datetime, bool or boolean, decimal(10,5), 
      	  	   text, clob(type text), character(20), varchar(255), varying 
      	  	   character(255), nchar(55), native character(70), 
      	  	   nvarchar(100), string" + "\n\n NOW EXITING. \n") 
		end
	     else
		puts "\nERROR STATE. CANCELLING COMPLETED OPERATIONS:\n"
		puts "\nPRINTING ERROR INFO:\n"
	        raise ArgumentError.new("error in input format. expected name,type got " + attributes[i] + "\n\n NOW EXITING. \n")
	     end
	  i+=1
	  end
	  return argv
      end



#method overloading
# splits a template argument into the classes definitions that it
# contains (: delimeter). For each class it calls transform and then
# create_vt() to produce a valid sql query that will result in the
# respective VT creation. the version that takes two arguments
# concerns the top class(es) of the template argument(s). 

      def register_class(*args)
        columns=Array.new
        if args.size==3
	  my_array=args[0]
	  index=args[1]
	  if my_array[index].include?(":")
	    classes=my_array[index].split(/:/)
	    i= -1 + classes.length
	    while i>=args[2]
	       puts classes[i]
	       attributes=classes[i].split(/-/)
	       k=0
	       while k<attributes.length
	  	 puts attributes[k]
		 k+=1
	       end
	       i-=1
	       columns=transform(my_array, attributes)
	       t=0
	       while t<columns.length
	       	  puts columns[t]
	       	  t+=1
	       end
	       create_vt(columns)
#	       puts $query
	    end
	    return classes[0]
	  elsif my_array[index].include?("-") && my_array.length==4        # datastructure is of type collection
	    return my_array[index]
#	    attributes=my_array[index].split(/-/)
#	    columns=transform(my_array, attributes)	    
#	    create_vt(columns)       #need for return. register_class(my_array,3,1)
#	    puts $query
	  else
	    attributes=Array.new(1,my_array[index])
	    return my_array[index]
	  end
# function:transform classes elements into valid sqlite3 column parameter format
# create a virtual table for each corresponding class description, don't forget PK, FK 
# complex vs class
  	  if classes
	     return classes[0]
	  end
	elsif args.size==2
	  my_array=args[0]
	  template_arg=args[1]
          attributes=template_arg.split(/-/)
	  i=0
	  count=0
	  while i<attributes.length 
#    don't really care about the class names. the table
#    name has been given seperately
	    if attributes[i].match(/,class/)
	      count+=1
	      attributes.delete_at(i)
	    end
	  i+=1
	  end
	  if count>2
	    puts "\nERROR STATE. CANCELLING COMPLETED OPERATIONS:\n"
	    puts "\nPRINTING ERROR INFO:\n"
	    raise ArgumentError.new("template arguments abuse class definition. multiple class name-type assignments" + "\n\n NOW EXITING. \n")
	  end
	  columns=transform(my_array, attributes)
	  t=0
	  while t<columns.length
	     puts columns[t]
	     t+=1
	  end
	  create_vt(columns)
#	  puts $query
	else
          puts "\nERROR STATE. CANCELLING COMPLETED OPERATIONS:\n"
	  puts "\nPRINTING ERROR INFO:\n"
	  raise ArgumentError.new("internal: method register_class accepts either two or three arguments" + "\n\n NOW EXITING. \n")
	end
      end

# split the string description into arguments (; delimeter) to extract
# the required info and then
# call register_class for each template argument. 

      def register_datastructure
          puts "description before whitespace cleanup " + @description
          @description.gsub!(/\s/,"")
          puts "description after whitespace cleanup " + @description

	  my_array=@description.split(/;/)
=begin
	  if my_array[0].include?(".")
	     no_extension=my_array[0].split(/\./)
	     my_array[0]=no_extension[0]
# make foo.db -> foo
	  end
=end
	  container_class=my_array[2]


          if container_class=="list" || container_class=="deque"  || container_class=="vector" || container_class=="slist" ||
            container_class=="set" || container_class=="multiset" ||
            container_class=="hash_set" || container_class=="hash_multiset"
                     container_type="collection"
          elsif container_class=="map" ||
            container_class=="multimap" || container_class=="hash_map" || container_class=="hash_multimap"
                     container_type="associative"
          else     
	       puts "\nERROR STATE. CANCELLING COMPLETED OPERATIONS:\n"
	       puts "\nPRINTING ERROR INFO:\n"
               raise TypeError.new("no such container class: " + container_class + "\n\n NOW EXITING. \n")
          end

          puts container_type

	  i=0
	  while i<my_array.length
	  	puts my_array[i]
		i+=1
	  end 
	  if my_array.length==4
	     unless container_type=="collection"
	        puts "\nERROR STATE. CANCELLING COMPLETED OPERATIONS:\n"
		puts "\nPRINTING ERROR INFO:\n"
	     	raise ArgumentError.new("wrong number of arguments for associative datastructure" + "\n\n NOW EXITING. \n")
	     end
	     top_class=register_class(my_array,3,1)
	     puts "top class is: " + top_class
	     register_class(my_array,top_class)
	  elsif my_array.length==5
	     unless container_type=="associative"
		puts "\nERROR STATE. CANCELLING COMPLETED OPERATIONS:\n"
		puts "\nPRINTING ERROR INFO:\n"
	        raise ArgumentError.new("wrong number of arguments for datastructure of type collection" + "\n\n NOW EXITING. \n")
	     end
	     top_class1=register_class(my_array, 3, 1)
	     top_class2=register_class(my_array, 4, 1)
	     puts "top_class1 :" + top_class1
	     puts "top_class2 :" + top_class2	     
	     register_class(my_array, top_class1 + "-" + top_class2)
	  else
	     puts "\nERROR STATE. CANCELLING COMPLETED OPERATIONS:\n"
	     puts "\nPRINTING ERROR INFO:\n"
	     raise ArgumentError.new("wrong number of arguments. check input description" + "\n\n NOW EXITING. \n")
	  end
	  @classnames.push(my_array[1])	
	  write_to_file(my_array[0])
	  puts "CONGRATS?"
      end

end

# test cases

if __FILE__==$0
=begin
    input=Input_Description.new("foo .db;emplo	yees;	multimap;nick_name,string;employee,class-name,	string-salary,int-account
    ,rEferEnce:account,class-a ccount_no,
string-balance,FLoat")                                  

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

=end

    input=Input_Description.new("foo .db;test;
    multimap;nick_name,class-nick_name,string;class,class-att1,int-att2,string-att3,double-classA,reference-classB,reference:classA,class-att1,int-att2,string-att3,double-classC,
    reference-classD,reference:classC,class-att1,int-att2,string-att3,double:classD,class-att1,int-att2,string-att3,double-classG,reference:classG,class-att1,int-att2,string-att3,double:
    classB,class-att1,int-att2,string-att3,double-classE,
    reference-classF,reference:classE,class-att1,int-att2,string-att3,double:classF,class-att1,int-att2,string-att3,double-classH,reference:
    classH,class-att1,int-att2,string-att3,double")        


    input.register_datastructure

end


