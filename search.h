#ifndef SEARCH_H
#define SEARCH_H

//#include "stl_to_sql.h"
#include "sqlite3.h"
#include "bridge.h"

/*class Account {

 private:
  char *account_no;
  float balance;

 public:
  Account(char *acc_no, float b);

  };*/

//extern "C" int register_table(char *db, char * query, void *data);
void search(int * resultset, void * data, char *constr, sqlite3_value *val);

#endif

