#include <stdio.h>
#include <string.h>
#include "stl_to_sql.h"

using namespace std;



void * thread_sqlite(void *data){
  char **queries;
  queries = (char **)sqlite3_malloc(sizeof(char *) * 1);
  int failure=0;
  queries[0] = "CREATE VIRTUAL TABLE employees USING stl(account_no TEXT,balance FLOAT)";
  failure = register_table("foo.db", 1, queries, data, enter 1 if table is to be created 0 if already created);
  printf("Thread sqlite returning..\n");
  sqlite3_free(queries);
  return (void *)failure;
}


int main(){
  int re_sqlite;
  void *data;

  // declare and fill datastructure;

  pthread_t sqlite_thread;
  re_sqlite = pthread_create(&sqlite_thread, NULL, thread_sqlite, data);
  pthread_join(sqlite_thread, NULL);
  printf("Thread sqlite returned %i\n", re_sqlite);
}
