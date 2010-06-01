#include "search.h"
#include "Account.h"
#include "Type.h"
#include <vector>
#include <iostream>

using namespace std;

int get_data_structure_size(void *st) {             //automatic generation needed
  stl_table *stl=(stl_table *)st;
  vector < Account > *accounts=(vector < Account > *)stl->data;
  return accounts->size();

}

void search(void *stc, int *initial, void *st, char *constr, sqlite3_value *val){
  stl_table *stl=(stl_table *)st;
  stl_table_cursor *stcsr=(stl_table_cursor *)stc;
  vector < Account > *accounts=(vector < Account > *)stl->data;     // need to be extracted from description
  vector < Account >:: iterator iter;
  Type v;
  Type *value=&v;           // why dynamic??
  //  int results[accounts->size()];

  if (val==NULL) {          // empty where clause
    for (int j=0; j<stcsr->size ;j++) {
      stcsr->resultset[j]=j;       //index all elements of data structure since all are eligible      
    }
  } else {
    switch( sqlite3_value_type(val) ){
    case SQLITE_INTEGER: value->set_int(sqlite3_value_int(val)); value->set_type_of_value(0); break;
    case SQLITE_TEXT:    value->set_text(sqlite3_value_text(val)); value->set_type_of_value(1); break;
    case SQLITE_FLOAT:   value->set_doub(sqlite3_value_double(val)); value->set_type_of_value(2); break;
    case SQLITE_BLOB:    value->set_blob(sqlite3_value_blob(val)); value->set_type_of_value(3); break;
    default:             NULL;    break;
    }
    int iCol;
    iCol = constr[1] - 'a' +1;    // uncertain
    char *col_name=stl->azColumn[iCol];
    
    // want the content of col_name and the actual op (not a string)
    // so from this point on, code has to be generated automatically..
    
    /*  switch (constr[0] - 'A') {
	case 0: traverse(col_name, "<", (void *)value); break;
	case 1: traverse(col_name, "<=", (void *)value); break;
	case 2: traverse(col_name, "=", (void *)value); break;
	case 3: traverse(col_name, ">=", (void *)value); break;
	case 4: traverse(col_name, ">", (void *)value); break;
	
	//    case SQLITE_INDEX_CONSTRAINT_MATCH: nidxStr[i]="F"; break;                                                                                                                                           
	}
    */
    // the idea hard-coded for now. 
    int count=0;
    if (stcsr->size==0) {  // not enough. can't differentiate initial state, from a possible final empty resultset or a possible intermediate empty resultset 
      if (*initial) {      // now ok with additional check
	for (iter=accounts->begin(); iter!=accounts->end(); iter++) {
	  if (iter->get_balance()==value->get_doub()) stcsr->resultset[count++]=iter - accounts->begin();
	}
	stcsr->size=count-1;
	//      stcsr->resultset=results;
      } else *initial=-1;
    } else { // should be generic. next constraint to be checked
      iter=accounts->begin();
      for (int i=0; i< stcsr->size; i++) {
	if (iter->get_account_no()==value->get_text()) stcsr->resultset[i]=-1;    // would like to delete.realloc??
	iter++;
      }
      stcsr->size=count-1;
      // stcsr->resultset=results;
    }
  }
}

void traverse(char *col_name, char *op, void *value) {   // automatic code generation
  vector < Account >::iterator iter;    //case-specific
  Type *v=(Type *)value;

}

int retrieve(void *stc, int n, sqlite3_context* con) {       // code generation needed
  sqlite3_vtab_cursor *svc=(sqlite3_vtab_cursor *)stc; 
  stl_table *stl=(stl_table *)svc->pVtab;
  stl_table_cursor *stcsr=(stl_table_cursor *)stc;
  vector < Account > *accounts=(vector < Account > *)stl->data;   // case-specific
  vector < Account >::iterator iter;                              // case-specific
  char *col_name=stl->azColumn[n];                               // has to be generated
  int index=stcsr->current;
  iter=accounts->begin() + stcsr->resultset[index];
  int datatype=2;                                     //hard-coded
  float r=iter->get_balance();      //to make sure it works
  if (n==0) {
    sqlite3_result_int(con, stcsr->resultset[index]);                         //primary key
  } else {
    switch (datatype) {                                 // in automated code: "iter->" + col_name will work.
    case 0: sqlite3_result_int(con, iter->get_balance()); break;                       // ignore
    case 1: sqlite3_result_text(con, iter->get_account_no(), -1, SQLITE_STATIC); break; //ignore
    case 2: sqlite3_result_double(con, r); break;
    case 3: sqlite3_result_blob(con, stc, -1, SQLITE_STATIC); break;    //ignore
    }
  }
}



int main() {


  Account acc1("10068", 500.0);
  vector<Account> accounts;
  accounts.push_back(acc1);
  void *data=(void *)&accounts;
  //  register_table("foo.db", "CREATE VIRTUAL TABLE account USING stl(INTEGER PRIMARY KEY AUTOINCREMENT,account_no TEXT,balance FLOAT)", data);  //create the virtual table.done once
  sqlite3* db;
  sqlite3_stmt *stmt;
  int re;

  re=sqlite3_open("foo.db", &db);

  if (re) {
    cout << "can't open database" << endl;
    sqlite3_close(db);
    exit(1);
  }

  sqlite3_module stl;
  fill_module(&stl);

  int output=sqlite3_create_module(db, "stl", &stl, data);              // hard-coded. register the module every time because it is not included in the source code
 
  if (output==1) printf("Error while registering module\n");
  else if (output==0) printf("Module registered successfully\n");

  prep_exec(db, "SELECT balance FROM account;");
  sqlite3_close(db);
}


//"SELECT account_no FROM account WHERE balance=500.0
