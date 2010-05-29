#include "search.h"
#include "Account.h"
#include <vector>

using namespace std;


void search(int *resultset, void *data, char *constr, sqlite3_value *val){



}



int main() {


  Account acc1("10068", 500.0);
  vector<Account> accounts;
  accounts.push_back(acc1);
  void *d;
  register_table("foo.db", "CREATE VIRTUAL TABLE account USING mod(INTEGER PRIMARY KEY AUTOINCREMENT,account_no TEXT,balance FLOAT)", d);



}
