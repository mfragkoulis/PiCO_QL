#include "sqlite3.h"
#include <vector>
#include "bridge.h"
#include "Account.h"

using namespace std;

int main() {
  Account acc1("10068", 500.0);
  Account acc2("10234", 394.28);
  vector<Account> accounts;
  accounts.push_back(acc1);
  accounts.push_back(acc2);
  void *data=(void *)&accounts;
  register_table("foo.db", "stl", "CREATE VIRTUAL TABLE account USING stl(INTEGER PRIMARY KEY AUTOINCREMENT,\
account_no TEXT,balance FLOAT, address TEXT, another_int INTEGER)", data);  //create the virtual table.done once                                                                                                                                                                                                     
  //  query_table("foo.db", "stl", data, "SELECT * FROM account WHERE balance>400;");


  sqlite3* db;
  sqlite3_stmt *stmt;
  int i , j;

  char *ndb="foo.db";
  char *nmodule="stl";
  char *query="SELECT * FROM account WHERE balance>400;";

  //  open_register(ndb, &db, nmodule, data);                                                                                                                                                                    
  int re=sqlite3_open(ndb, &db);
  if (re) {
    printf("can't open database\n");
    sqlite3_close(db);
    exit(1);
  }
  sqlite3_module mod;
  fill_module(&mod);

  int output=sqlite3_create_module(db, nmodule, &mod, data);              // hard-coded                                                                                                                          
  if (output==1) printf("Error while registering module\n");
  else if (output==0) printf("Module registered successfully\n");


  char ***pazResult=(char ***)sqlite3_malloc(sizeof(char***));
  int *nRows=(int *)sqlite3_malloc(sizeof(int));
  int *nCols=(int *)sqlite3_malloc(sizeof(int));
  char **errMsg=(char **)sqlite3_malloc(sizeof(char**));

  if (sqlite3_get_table(db, query, pazResult, nRows, nCols, errMsg)==SQLITE_OK) {
    printf("\nQUERY SUCCESSFUL! \n\n");
    printf("result table:\n\n");
    for (i=0; i< (*nRows +1)*(*nCols); i++) {
      printf("%s ", pazResult[0][i]);
      if ( (i+(*nCols) +1) % (*nCols)==0) printf("\n");
    }
    printf("\n\n");
  }
  sqlite3_close(db);

  sqlite3_free(pazResult);
  sqlite3_free(nRows);
  sqlite3_free(nCols);
  sqlite3_free(errMsg);
}
