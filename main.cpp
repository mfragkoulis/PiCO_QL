#include <stdio.h>
#include <string>
#include "stl_to_sql.h"
#include <pthread.h>
#include <vector>
#include <map>
#include "Account.h"

using namespace std;



void * thread_sqlite(void *data){
  const char **queries;
  queries = (const char **)sqlite3_malloc(sizeof(char *) * 
  	    	   1);
  int failure = 0;
  queries[0] = "CREATE VIRTUAL TABLE account USING stl(pk integer primary key,nick_name STRING,account_no TEXT,balance FLOAT,isbn INTEGER)";
  failure = register_table( "foo.db" ,  1, queries,
  	   data, 1);
  printf("Thread sqlite returning..\n");  
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
  int **mem;
  char **names;
  mem = (int **)sqlite3_malloc(sizeof(int*));
  names = (char **)sqlite3_malloc(sizeof(char *));

  Account acc1("10068", 500.0, 478923743);
  Account acc2("10234", 394.28, 692346729);
  map<string,Account> accounts;
  accounts.insert(make_pair("0", acc1));
  accounts.insert(make_pair("1", acc2));


  mem[0] = (int *)&accounts;
  names[0] = "account";

  dsCarrier dsC;
  dsC.memories = mem;
  dsC.dsNames = names;
  dsC.size = 1;
//  dsCarrier dsC((const int *)mem, (const char **)names, 1);


  data = (void *)&dsC;


  pthread_t sqlite_thread;
  re_sqlite = pthread_create(&sqlite_thread, NULL, thread_sqlite, data);
  pthread_join(sqlite_thread, NULL);
  sqlite3_free(mem);
  sqlite3_free(names);
  printf("Thread sqlite returned %i\n", re_sqlite);
}


