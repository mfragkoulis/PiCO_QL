#include "search.h"
#include "Account.h"
#include <vector>

using namespace std;


void search(int *size, int *resultset, void *st, char *constr, sqlite3_value *val){
  stl_table *stl=(stl_table *)st;
  vector < Account > *accounts=(vector < Account > *)stl->data;
  vector < Account >:: iterator iter;
  Type value;
  int results[accounts->size()];
  switch( sqlite3_value_type(val) ){
  case SQLITE_INTEGER: value.set_int(sqlite3_value_int(val)); break;
  case SQLITE_TEXT:    value.set_text(sqlite3_value_text(val));    break;
  case SQLITE_FLOAT:   value.set_doub(sqlite3_value_double(val));    break;
  case SQLITE_BLOB:    value.set_blob(sqlite3_value_blob(val));    break;
  default:             NULL;    break;
  }
  int iCol;
  iCol = constr[1] - 'a' +1;
  char *col_name=stl->azColumn[iCol];
  //from this point on, code has to be generated automatically..
  switch (constr[0] - 'A') {
  case 0: traverse(iter, col_name, "<", value); break;
  case 1: traverse(iter, col_name, "<=", value); break;
  case 2: traverse(iter, col_name, "=", value); break;
  case 3: traverse(iter, col_name, ">=", value); break;
  case 4: traverse(iter, col_name, ">", value); break;
    //    case SQLITE_INDEX_CONSTRAINT_MATCH: nidxStr[i]="F"; break;                                                                                                                                           
  }

  // the idea hard-coded for now. 
  int count=0;
  if (*size==0) {  // not enough. can't differentiate initial state, from a possible final empty resultset or a possible intermediate empty resultset 
    for (iter=accounts->begin(); iter!=accounts->end(); iter++) {
      if (iter->get_balance()==value.get_doub()) results[count++]=iter - accounts->begin();
    }
    *size=count-1;
    resultset=results;
  } else { // should be generic. next constraint to be checked
    iter=accounts->begin();
    for (int i=0; i< *size; i++) {
      if (iter->get_balance()==value.get_doub()) results[count++]=resultset[i];
      iter++;
    }
    *size=count-1;
    resultset=results;
  }
}

void traverse(vector <Account>::iterator iter, char *col_name, char *op, Type value) {


}



int main() {


  Account acc1("10068", 500.0);
  vector<Account> accounts;
  accounts.push_back(acc1);
  void *d;
  register_table("foo.db", "CREATE VIRTUAL TABLE account USING mod(INTEGER PRIMARY KEY AUTOINCREMENT,account_no TEXT,balance FLOAT)", d);



}
