#include "search.h"
#include "Account.h"
#include "Type.h"
#include <vector>
#include <iostream>

using namespace std;

// return size of datastructure
int get_data_structure_size(void *st){            
    stlTable *stl = (stlTable *)st;

// automatic code generation
    vector<Account> *accounts = (vector<Account> *)stl->data;
    return accounts->size();
}

// decode constraint and search the datastructure to find matches
void search(void *stc, char *constr, sqlite3_value *val){
    sqlite3_vtab_cursor *cur = (sqlite3_vtab_cursor *)stc;
    stlTable *stl = (stlTable *)cur->pVtab;  
    stlTableCursor *stcsr = (stlTableCursor *)stc;

// need to be extracted from description
    vector<Account> *accounts = (vector <Account> *)stl->data;    
    vector<Account>:: iterator iter;

    Type v;
    Type *value= &v;  

// empty where clause    
    if ( val==NULL ){         

// index all elements of data structure since all are eligible    
	for (int j=0; j<get_data_structure_size((void *)stl); j++){
	    stcsr->resultSet[j] = j;      
	    stcsr->size++;
	}
    }else{
	switch( sqlite3_value_type(val) ){
	case SQLITE_INTEGER: 
	    value->set_int(sqlite3_value_int(val)); 
	    value->set_type_of_value(0); 
	    break;
	case SQLITE_TEXT:    
	    value->set_text(sqlite3_value_text(val)); 
	    value->set_type_of_value(1); 
	    break;
	case SQLITE_FLOAT:   
	    value->set_doub(sqlite3_value_double(val)); 
	    value->set_type_of_value(2); 
	    break;
	case SQLITE_BLOB:    
	    value->set_blob(sqlite3_value_blob(val)); 
	    value->set_type_of_value(3); 
	    break;
	default:             
	    NULL;    
	    break;
	}
	
	int iCol;
	iCol = constr[1] - 'a' + 1;
// doxygen
	char *colName = stl->azColumn[iCol];     
	
// want the content of col_name and the actual op (not a string)
// so from this point on, code has to be generated automatically..
	
	/* switch ( constr[0] -'A' ){
	   case 0: 
	   traverse(colName, "<", value);
	   break;
	   case 1:
	   traverse(colName, "<=", value);
	   break;
	   case 2:
	   traverse(colName, "==", value);
	   break;
	   case 3:
	   traverse(colName, ">=", value);
	   break;
	   case 4:
	   traverse(colName, ">", value);
	   break;
	   
	   //    case SQLITE_INDEX_CONSTRAINT_MATCH: nidxStr[i]="F"; break;
	   }
	*/
	
// the idea hard-coded for now. 
	int count = 0;
	
	for(iter=accounts->begin(); iter!=accounts->end(); iter++){
	    if( !strcmp(iter->get_account_no(), 
			(const char *)value->get_text()) )
		stcsr->resultSet[count++] = iter - accounts->begin();
	}
	stcsr->size += count;
    }
}

// match datastructure entries against a given constraint. need cursor
void traverse(char *colName, char *op, void *value){   
// automatic code generation
    vector < Account >::iterator iter;
    Type *v=(Type *)value;
}

//
int retrieve(void *stc, int n, sqlite3_context* con){      
// automatic code generation
    sqlite3_vtab_cursor *svc = (sqlite3_vtab_cursor *)stc; 
    stlTable *stl = (stlTable *)svc->pVtab;
    stlTableCursor *stcsr = (stlTableCursor *)stc;

//case specific
    vector <Account> *accounts = (vector <Account> *)stl->data;  
    vector <Account>::iterator iter;

// has to be generated    
  char *colName = stl->azColumn[n];

  int index = stcsr->current;

//iterator implementation. serial traversing or hit?
  iter = accounts->begin() + stcsr->resultSet[index];

  int datatype;
  datatype = stl->colDataType[n];
  const char *pk = "PK";

// primary key  
  if ( (n==0) && (!strcmp(stl->azColumn[0], pk)) ){
    sqlite3_result_int(con, stcsr->resultSet[index]);
  }else{

// in automated code: "iter->get_" + col_name + "()" will work.safe?
    switch ( datatype ){
    case 0:
	sqlite3_result_int(con, iter->get_balance());
	break;
    case 1:
	sqlite3_result_text(con, iter->get_account_no(), -1, SQLITE_STATIC);
	break;
    case 2: 
	sqlite3_result_double(con, iter->get_balance());
	break;
    case 3:
	sqlite3_result_blob(con, stc, -1, SQLITE_STATIC);
	break;
    }
  }
  return SQLITE_OK;
}
