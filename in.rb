# raise alarm if special characters are used in description


class Input_Description

      def initialize(description="")
          @description=description
	  @signature=""
	  @table_columns=Array.new
# make twodimensional and cancel columns array?
	  @classnames=Array.new
	  @container_type=""
	  @template_args=""
	  @key_class_type=0
# key class type is used to declare what kind of key an associative 
# datastructure has 
# 0=plain, 1=user-defined, 2=user-defined nested
	  @key_class_attributes=1
# cancel filename?
	  @filename="main.cpp"
	  @queries=Array.new
      end


      def two_dms_array(height, width)
      	  a = Array.new(height)
	  a.map!{Array.new(width)}
	  return a
      end

# produces a valid sql query for creating a virtual table according to
# the argument array columns

      def create_vt(columns)

#         if columns[1].include?(".")
#	    no_extension=columns[1].split(/\./)
#	    columns[1]=no_extension[0]	    
#	 end

         query="CREATE VIRTUAL TABLE " + columns[2]  + " USING " + 
	 	       columns[0] + "("
	 i=3
	 while i<columns.length
	    @table_columns.push(columns[i])
# one-level-table
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

  pthread_t sqlite_thread;
  re_sqlite = pthread_create(&sqlite_thread, NULL, thread_sqlite, data);
  pthread_join(sqlite_thread, NULL);
  printf(\"Thread sqlite returned %i\\n\", re_sqlite);
}


AG2



    #HereDoc3


	auto_gen3 = <<-AG3

using namespace std;



int get_datastructure_size(void *st){
    stlTable *stl = (stlTable *)st;
    #{@signature} *any_dstr = (#{@signature} *)stl->data;
    return ((int)any_dstr->size());
}


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

	  
void search(void *stc, char *constr, sqlite3_value *val){
    sqlite3_vtab_cursor *cur = (sqlite3_vtab_cursor *)stc;
    stlTable *stl = (stlTable *)cur->pVtab;
    stlTableCursor *stcsr = (stlTableCursor *)stc;
    #{@signature} *any_dstr = (#{@signature} *)stl->data;
    #{@signature}:: iterator iter;
    Type value;
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
        const char *fk = "FK";
        if(!strcmp(colName, fk)){
            iter=any_dstr->begin();
            for(int i=0; i<(int)any_dstr->size(); i++){
                if( traverse((int)&(*iter), op, sqlite3_value_int(val)) )
                    stcsr->resultSet[count++] = i;
                iter++;
            }
            stcsr->size += count;
        }else{


// handle colName\n\n

            switch( iCol ){

// i=0. search using PK?memory location?or no PK?
// no can't do.PK will be memory location. PK in every table
// PK search
            case 0: 
                iter=any_dstr->begin();
                for(int i=0; i<(int)any_dstr->size(); i++){
                    if( traverse((int)&(*iter), op, sqlite3_value_int(val)) )
                        stcsr->resultSet[count++] = i;
                    iter++;
                }
                stcsr->size += count;
                break;
AG3

	
    #HereDoc4


	trv111 = <<-T111
                    if( traverse(iter->first.get_
T111

	
    #HereDoc5


	rtrv111 = <<-RT111
(), op, sqlite3_value_int(val)) )
RT111

	
    #HereDoc6


	trv112 = <<-T112
                   if( traverse(iter->first, op, sqlite3_value_int(val)) )
T112

	
    #HereDoc7


	trv113 = <<-T113
                    if( traverse(iter->second.get_
T113

	
    #HereDoc8


	trv114 = <<-T114
                    if( traverse(iter->get_
T114

	
    #HereDoc9


	trv121 = <<-T121
                    if( traverse((const void*)iter->first.get_
T121

	
    #HereDoc10


	rtrv121 = <<-RT121
(), op, sqlite3_value_blob(val)) )
RT121

	
    #HereDoc11


	trv122 = <<-T122
                    if( traverse((const void*)iter->first, op, 
		    	sqlite3_value_blob(val)) )
T122

	
    #HereDoc12


	trv123 = <<-T123
                    if( traverse((const void*)iter->second.get_
T123

	
    #HereDoc13


	trv124 = <<-T124
                    if( traverse((const void*)iter->get_
T124

	
    #HereDoc14


	trv131 = <<-T131
                    if( traverse(iter->first.get_
T131

	
    #HereDoc15


	rtrv131 = <<-RT131
(), op, sqlite3_value_double(val)) )
RT131

	
    #HereDoc16


	trv132 = <<-T132
                    if( traverse(iter->first, op, sqlite3_value_double(val)) ) 
T132

	
    #HereDoc17


	trv133 = <<-T133
                    if( traverse(iter->second.get_
T133

	
    #HereDoc18


	trv134 = <<-T134
                    if( traverse(iter->get_
T134

	
    #HereDoc19


	trv141 = <<-T141
                    if( traverse((const unsigned char
                    *)iter->first.get_
T141

	
    #HereDoc20


	rtrv141 = <<-RT141
(), op, sqlite3_value_text(val)) )
RT141

	
    #HereDoc21


	trv142 = <<-T142
                    if( traverse((const unsigned char *)iter->first, op, 
		    	sqlite3_value_text(val)) )
T142

	
    #HereDoc22


	trv143 = <<-T143
                    if( traverse((const unsigned char *)iter->second.get_
T143

	
    #HereDoc23


	trv144 = <<-T144
                    if( traverse((const unsigned char *)iter->get_
T144

	
    #HereDoc24


	trv151 = <<-T151
                    if( traverse((const unsigned char *)iter->first.get_
T151

	
    #HereDoc25


	rtrv151 = <<-RT151
().c_str(), op, sqlite3_value_text(val)) )
RT151

	
    #HereDoc26


	trv152 = <<-T152
                    if( traverse((const unsigned char *)iter->first.c_str(), 
		    	op, sqlite3_value_text(val)) )
T152

	
    #HereDoc27


	trv153 = <<-T153
                    if( traverse((const unsigned char *)iter->second.get_
T153

		
    #HereDoc28


	trv154 = <<-T154
                    if( traverse((const unsigned char *)iter->get_
T154

	
    #HereDoc29


	gather_results = <<-rslt
                        stcsr->resultSet[count++] = i;
                    iter++;
                }
                stcsr->size += count;
                break;
rslt

#	      i+=1
#	  end

	
    #HereDoc30


	cls_search_opn_retrieve = <<-cls_opn
// more datatypes and ops exist
            }
        }
    }
}



int retrieve(void *stc, int n, sqlite3_context* con){
    sqlite3_vtab_cursor *svc = (sqlite3_vtab_cursor *)stc;
    stlTable *stl = (stlTable *)svc->pVtab;
    stlTableCursor *stcsr = (stlTableCursor *)stc;
    #{@signature} *any_dstr = (#{@signature} *)stl->data;
    #{@signature}:: iterator iter;
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
    const char *pk = "id";
    const char *fk = "FK";
    if ( (n==0) && (!strcmp(stl->azColumn[0], pk)) ){
// attention!
        sqlite3_result_int(con, (int)&(*iter));
        printf(\"memory location of PK: %x\\n\", &(*iter));
    }else if( !strncmp(stl->azColumn[n], fk, 2) ){
        sqlite3_result_int(con, (int)&(*iter));
// need work
    }else{
// in automated code: \"iter->get_\" + col_name + \"()\" will work.safe?
// no.doxygen.
cls_opn

#	  i=1
#          fw.puts "        switch ( n ){"
#	  fw.puts "// why necessarily iter->second in associative?"

	
    #HereDoc31


	trv211 = <<-T211
            sqlite3_result_int(con, iter->first.get_
T211

	
    #HereDoc32


	trv212 = <<-T212
            sqlite3_result_int(con, iter->first);
T212


    #HereDoc33


	trv213 = <<-T213
            sqlite3_result_int(con, iter->second.get_
T213


    #HereDoc34


	trv214 = <<-T214
            sqlite3_result_int(con, iter->get_
T214


    #HereDoc35

    
	trv221 = <<-T221
          sqlite3_result_blob(con, (const void *)iter->first.get_
T221


    #HereDoc36

    
	rtrv221 = <<-RT221
(),-1,SQLITE_STATIC);
RT221


    #HereDoc37

    
	trv222 = <<-T222
            sqlite3_result_blob(con, (const void *)iter->first,-1,
	    			     SQLITE_STATIC);
T222


    #HereDoc38

    
	trv223 = <<-T223
            sqlite3_result_blob(con, (const void *)iter->second.get_
T223


    #HereDoc39

    
	trv224 = <<-T224
            sqlite3_result_blob(con, (const void *)iter->get_
T224


    #HereDoc40

    
	trv231 = <<-T231
            sqlite3_result_double(con, iter->first.get_
T231


    #HereDoc41

    
	trv232 = <<-T232
            sqlite3_result_double(con, iter->first);
T232


    #HereDoc42

    
	trv233 = <<-T233
            sqlite3_result_double(con, iter->second.get_
T233


    #HereDoc43

    
	trv234 = <<-T234
            sqlite3_result_double(con, iter->get_
T234


    #HereDoc44

    
	trv241 = <<-T241
            sqlite3_result_text(con, (const char *)iter->first.get_
T241


    #HereDoc45

    
	trv242 = <<-T242
            sqlite3_result_text(con, (const char *)iter->first,-1,
	    			     SQLITE_STATIC);
T242


    #HereDoc46

    
	trv243 = <<-T243
            sqlite3_result_text(con, (const char *)iter->second.get_
T243


    #HereDoc47

    
	trv244 = <<-T244
            sqlite3_result_text(con, (const char *)iter->get_
T244


    #HereDoc48

    
	trv251 = <<-T251
            sqlite3_result_text(con, (const char *)iter->first.get_
T251


    #HereDoc49

    
	rtrv251 = <<-RT251
().c_str(),-1,SQLITE_STATIC);
RT251


    #HereDoc50

    
	trv252 = <<-T252
            sqlite3_result_text(con, (const char *)iter->first.c_str(),-1,
	    			     SQLITE_STATIC);
T252


    #HereDoc51

    
	trv253 = <<-T253
            sqlite3_result_text(con, (const char *)iter->second.get_
T253

    #HereDoc52

    
	trv254 = <<-T254
            sqlite3_result_text(con, (const char *)iter->get_
T254

# END OF HereDocs

        myfile=File.open(@filename, "w") do |fw|
          fw.puts "\#include <stdio.h>"
          fw.puts "\#include <string>"
          fw.puts "\#include \"stl_to_sql.h\""
          fw.puts "\#include <pthread.h>"
	  c_type=@signature.split(/</)
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
	  k=0
	  while (k<@classnames.length-1)
            fw.puts "\#include \"" + @classnames[k] + ".h\""
	    k+=1
	  end

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
	  c_type=@signature.split(/</)
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
          fw.puts "\#include \"search.h\""
          fw.puts "\#include <string>"
          fw.puts "\#include \"Type.h\""
	  k=0
	  while (k<@classnames.length-1)
            fw.puts "\#include \"" + @classnames[k] + ".h\""
	    k+=1
	  end

# call HereDoc3	  
	  fw.puts auto_gen3
	  i=1
	  while( i<@table_columns.length )
	    split_column = @table_columns[i].split(/ /)
	    fw.puts "            case " + i.to_s + ":" 
	    fw.puts "// why necessarily iter->second in associative?"
	    fw.puts "// if non pointer then second. else second->"
	    fw.puts "                iter=any_dstr->begin();"
	    fw.puts "                for(int i=0; i<(int)any_dstr->size(); i++){"
	      split_column[1]=split_column[1].downcase
              if split_column[1]=="int" || split_column[1]=="integer" ||
		split_column[1]=="tinyint" || split_column[1]=="smallint"|| 
		split_column[1]=="mediumint" || split_column[1]=="bigint" ||
		split_column[1]=="unsigned bigint" ||
		split_column[1]=="int2" ||
                split_column[1]=="bool" || split_column[1]=="boolean" ||
		split_column[1]=="int8" || split_column[1]=="numeric" 
		  if @template_args=="double"
		    if( (@key_class_type==1)&&(i<=@key_class_attributes) )
		      fw.puts trv111.chomp + split_column[0] + rtrv111
		    elsif( (@key_class_type==0)&&(i==1) )
		      fw.puts trv112
		    elsif( i>@key_class_attributes)
		      fw.print trv113.chomp + split_column[0] + rtrv111
	 	    end
		  else
		    fw.puts trv114.chomp + split_column[0] + rtrv111
		  end
	      elsif split_column[1]=="blob"
	        if @template_args=="double"
		  if( (@key_class_type==1)&&(i<=@key_class_attributes) )
		    fw.puts trv121.chomp + split_column[0] + rtrv121
		  elsif( (@key_class_type==0)&&(i==1) )
		    fw.puts trv122
		  elsif( i>@key_class_attributes )
		    fw.puts trv123.chomp + split_column[0] + rtrv121
		  end
	      else
		fw.puts trv124.chomp + split_column[0] + rtrv121
	      end
              elsif split_column[1]=="float" ||	split_column[1]=="double"  ||
	        split_column[1].match(/\idecimal/) ||
      	        split_column[1]=="double precision" || split_column[1]=="real"
	          if @template_args=="double"
		    if( (@key_class_type==1)&&(i<=@key_class_attributes) )
		      fw.puts trv131.chomp + split_column[0] + rtrv131
		    elsif( (@key_class_type==0)&&(i==1) )
		      fw.puts trv132
		    elsif( i>@key_class_attributes)
		      fw.puts trv133.chomp + split_column[0] + rtrv131
		    end
		  else
		    fw.puts trv134.chomp + split_column[0] + rtrv131
		  end
	      elsif split_column[1]=="text" || split_column[1]=="date" ||
		split_column[1]=="datetime" ||
                split_column[1].match(/\icharacter/) || 
		split_column[1].match(/\ivarchar/) ||
		split_column[1].match(/\invarchar/) || 
		split_column[1].match(/\ivarying character/) ||
		split_column[1].match(/\inative character/) || 
		split_column[1]=="clob" ||
		split_column[1].match(/\inchar/)
		  if @template_args=="double"
		    if( (@key_class_type==1)&&(i<=@key_class_attributes) )
		      fw.puts trv141.chomp + split_column[0] + rtrv141
		    elsif( (@key_class_type==0)&&(i==1) )
		      fw.puts trv142
		    elsif( i>@key_class_attributes)
		      fw.puts trv143.chomp + split_column[0] + rtrv141
		    end
		  else
		    fw.puts trv144.chomp + split_column[0] + rtrv141
	 	  end
	      elsif split_column[1]=="string"
# need to use c_str() to convert string to char *
	        if @template_args=="double"
		  if( (@key_class_type==1)&&(i<=@key_class_attributes) )
		    fw.puts trv151.chomp + split_column[0] + rtrv151
		  elsif( (@key_class_type==0)&&(i==1) )
		    fw.puts trv152
		  elsif( i>@key_class_attributes )
		    fw.puts trv153.chomp + split_column[0] + rtrv151
		  end
		else
		  fw.puts trv154.chomp + split_column[0] + rtrv1
		end
	      end

# call HereDoc29
	      fw.puts gather_results
	      i+=1
	  end

# call HereDoc30
	  fw.puts cls_search_opn_retrieve

	  i=1
          fw.puts "        switch ( n ){"
	  fw.puts "// why necessarily iter->second in associative?"
# only for one-level description!!!
	  while( i<@table_columns.length )
	    split_column = @table_columns[i].split(/ /)
	    split_column[1]=split_column[1].downcase
	    fw.puts "        case " + i.to_s + ":" 
            if split_column[1]=="int" || split_column[1]=="integer" ||
	      split_column[1]=="tinyint" || split_column[1]=="smallint" || 
	      split_column[1]=="mediumint" || split_column[1]=="bigint" ||
	      split_column[1]=="unsigned bigint" || split_column[1]=="int2" ||
              split_column[1]=="bool" || split_column[1]=="boolean" ||
	      split_column[1]=="int8" || split_column[1]=="numeric" 
		if @template_args=="double"
		  if( (@key_class_type==1)&&(i<=@key_class_attributes) )
          	    fw.puts trv211.chomp + split_column[0] + "());"
		  elsif( (@key_class_type==0)&&(i==1) )
          	    fw.puts trv212
		  elsif( i>@key_class_attributes )
          	    fw.puts trv213.chomp + split_column[0] + "());"
		  end
		else
          	  fw.puts trv214.chomp + split_column[0] + "());"
		end
		fw.puts "            break;"
	    elsif split_column[1]=="blob"
	      if @template_args=="double"
	        if( (@key_class_type==1)&&(i<=@key_class_attributes) )
          	  fw.puts trv221.chomp + split_column[0] + rtrv221
		elsif( (@key_class_type==0)&&(i==1) )
          	  fw.puts trv222
		elsif( i>@key_class_attributes)
          	  fw.puts trv223.chomp + split_column[0] + rtrv221
		end
	      else
      	        fw.puts trv224.chomp + split_column[0] + rtrv221
	      end
	      fw.puts "            break;"
              elsif split_column[1]=="float" ||	split_column[1]=="double"  ||
	      	split_column[1].match(/\idecimal/) ||
      	  	split_column[1]=="double precision" || 
		split_column[1]=="real"
		  if @template_args=="double"
		    if( (@key_class_type==1)&&(i<=@key_class_attributes) )
          	      fw.puts trv231.chomp + split_column[0] + "());"
		    elsif( (@key_class_type==0)&&(i==1) )
          	      fw.puts trv232
		    elsif( i>@key_class_attributes )
          	      fw.puts trv233.chomp + split_column[0] + "());"
		    end
		  else
          	    fw.puts trv234.chomp + split_column[0] + "());"
		  end
		  fw.puts "            break;"
	      elsif split_column[1]=="text" || split_column[1]=="date" ||
		  split_column[1]=="datetime" ||
                  split_column[1].match(/\icharacter/) || 
		  split_column[1].match(/\ivarchar/) ||
		  split_column[1].match(/\invarchar/) || 
		  split_column[1].match(/\ivarying character/) ||
		  split_column[1].match(/\inative character/) || 
		  split_column[1]=="clob" ||
		  split_column[1].match(/\inchar/)
		    if @template_args=="double"
		      if( (@key_class_type==1)&&(i<=@key_class_attributes) )
          		fw.puts trv241.chomp + split_column[0] + rtrv221
# @key_class_attributes==1 instead?
		      elsif( (@key_class_type==0)&&(i==1) )
          		fw.puts trv242
		      elsif( i>@key_class_attributes)
          	        fw.puts trv243.chomp + split_column[0] + rtrv221
		      end
		    else
          	      fw.puts trv244.chomp + split_column[0] + rtrv221
		    end
		    fw.puts "            break;"
	      elsif split_column[1]=="string"
# need to use c_str() to convert string to char *
	        if @template_args=="double"
		  if( (@key_class_type==1)&&(i<=@key_class_attributes) )
          	    fw.puts trv251.chomp + split_column[0] + rtrv251
		  elsif( (@key_class_type==0)&&(i==1) )
          	    fw.puts trv252
		  elsif( i>@key_class_attributes)
          	    fw.puts trv253.chomp + split_column[0] + rtrv251
		  end
		else
          	  fw.puts trv254.chomp + split_column[0] + rtrv251
		end
		fw.puts "            break;"
	      end
	      i+=1
	  end

          fw.puts "        }"
          fw.puts "    }"
          fw.puts "    return SQLITE_OK;"
          fw.puts "}"

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

      def transform(my_array, attributes)


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
	argv.push("stl")
	argv.push(my_array[0])
	argv.push(my_array[1])
# PK for all tables (top level too)
	argv.push("id INTEGER PRIMARY KEY AUTOINCREMENT")
	i=0
	while i< attributes.length
	  if attributes[i].include?(",")
	    name_type=attributes[i].split(/,/)
	    if name_type.length!=2
	      puts $err_state
	      raise ArgumentError.new("expected pair name,type got " + 
	      	    attributes[i] + "\n\n NOW EXITING. \n") 
	    end
	    name_type[1]=name_type[1].downcase
            if name_type[1]=="class"
	      k=0
	      while k<@classnames.length
		if @classnames[k]==name_type[0]
		  puts $err_state
 		  raise ArgumentError.new("Attempt to create virtual table" +
	 	  	" twice \n\n NOW EXITING. \n")
		end
		k+=1
	      end
	      @classnames.push(name_type[0])
              puts "table_name is " + name_type[0]
# top level tables won't go in this condition only intermediate ones.
# table name is set as default so that it works for top level.
# intermediate tables override default with respective class name.
	      argv.delete_at(2)
	      argv.insert(2,name_type[0])
#	      argv.push("INTEGER PRIMARY KEY AUTOINCREMENT")
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
	        puts $err_state
		raise ArgumentError.new(no_bind.chomp + name_type[0] + 
		      "\n\n NOW EXITING. \n")
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
	      name_type[1]=="datetime" || 
	      name_type[1].match(/\idecimal/) ||
	      name_type[1]=="text" || name_type[1]=="clob" ||
              name_type[1].match(/\icharacter/) || 
	      name_type[1].match(/\ivarchar/) ||
	      name_type[1].match(/\invarchar/) || 
	      name_type[1].match(/\ivarying character/) ||
	      name_type[1].match(/\inative character/) || 
	      name_type[1].match(/\inchar/)
		argv.push(name_type[0] + " " + name_type[1].upcase)
	    elsif name_type[1]=="string"
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



#method overloading
# splits a template argument into the classes definitions that it
# contains (: delimeter). For each class it calls transform and then
# create_vt() to produce a valid sql query that will result in the
# respective VT creation. the version that takes two arguments
# concerns the top class(es) of the template argument(s). 

      def register_class(*args)


# HERE DOCUMENTS FOR METHOD


  # HereDoc1

     tmpl_abuse = <<-TA
template arguments abuse class definition. 
multiple class name-type assignments

NOW EXITING

TA

  # HereDoc2

     method_args = <<-MA
internal: method register_class accepts either two or three arguments


NOW EXITING.

MA

# END OF HEREDOCS

        columns=Array.new
        if args.size==3
	  my_array=args[0]
	  index=args[1]
	  if my_array[index].include?(":")
	    classes=my_array[index].split(/:/)
	    i= -1 + classes.length

# why third parameter(it is fixed.)?seperate versions

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
	  elsif my_array[index].include?("-") && my_array.length==4        
# datastructure is of type collection
	    return my_array[index]
#	    attributes=my_array[index].split(/-/)
#	    columns=transform(my_array, attributes)	    
#	    create_vt(columns)       
#need for return. register_class(my_array,3,1)
#	    puts $query
	  else
#why?
	    attributes=Array.new(1,my_array[index])
	    return my_array[index]
	  end
# function:transform classes elements into valid sqlite3 column 
# parameter format
# create a virtual table for each corresponding class description, 
# don't forget PK, FK 
# complex vs class

# why?
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
# intervention
	      keep_class=attributes[i].split(/,/)
	      @classnames.push(keep_class[0])
	      count+=1
	      attributes.delete_at(i)
	    end
	  i+=1
	  end
	  if count>2
	    puts $err_state
	    raise ArgumentError.new(tmpl_abuse)
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
	  puts $err_state
	  raise ArgumentError.new(method_args)
	end
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

# END OF HEREDOCS

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
          raise TypeError.new("no such container class: " + container_class +
 	  			  "\n\n NOW EXITING. \n")
        end

# comparison function for user defined types ruins the check. but c.f
# shouldn't be reported so go ahead

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

	@signature=my_array[2]
	puts "container signature is: " + @signature
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
	  top_class=register_class(my_array,3,1)
	  puts "top class is: " + top_class
	  register_class(my_array,top_class)
	elsif my_array.length==5
	  unless @template_args=="double"
	    puts $err_state
	    raise ArgumentError.new(col_args)
	  end
	  top_class1=register_class(my_array, 3, 1)
	  top_class2=register_class(my_array, 4, 1)
	  puts "top_class1 :" + top_class1
	  puts "top_class2 :" + top_class2	     
# needed for method search in order to be able to address both
# template arguments's columns. however, attributes stored anywhere?
	  if( my_array[3].include?("-") ) 
	    @key_class_type = 1
	    no_attributes = my_array[3].split(/-/)
	    @key_class_attributes = no_attributes.length
	  else 
	    @key_class_type = 0
	  end
	  register_class(my_array, top_class1 + "-" + top_class2)
	else
	  puts $err_state
	  raise ArgumentError.new(nargs)
	end
#why?use as top table name
	@classnames.push(my_array[1])	
	write_to_file(my_array[0])
	puts "CONGRATS?"
      end

#=end

end

# test cases

if __FILE__==$0
=begin
    input=Input_Description.new("foo .db;account;
    vector<Account>;Account,class-a ccount_no,text-balance,FLoat")  

#=begin
    input=Input_Description.new("foo .db;account;	map<string,Account>;nick_name,string;Account,class-a ccount_no,text-balance,FLoat-isbn,integer")
#=end
    input=Input_Description.new("foo .db;account;
    deque<Account>;Account,class-a ccount_no,text-balance,FLoat-isbn,integer") 
#=end
    input=Input_Description.new("foo .db;account;
    set<Account>;Account,class-a ccount_no,text-balance,FLoat") 
#=end
    input=Input_Description.new("foo .db;account;
    multiset<Account>;Account,class-a ccount_no,text-balance,FLoat-isbn,integer") 
=end
    input=Input_Description.new("foo .db;account;	multimap<string,Account>;nick_name,string;Account,class-a ccount_no,text-balance,FLoat-isbn,integer")
=begin
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


