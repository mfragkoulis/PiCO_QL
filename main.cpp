#include "sqlite3.h"
#include <vector>
#include "bridge.h"
#include "Account.h"

using namespace std;


void * thread_sqlite(void *data){
    printf("Thread sqlite beginning..\n");
    int re;
    re = register_table("foo.db", "stl", "CREATE VIRTUAL TABLE account USING stl( \
INTEGER PRIMARY KEY AUTOINCREMENT,account_no TEXT,balance FLOAT)", data, 0);   
    printf("thread sqlite returning..\n");
    return (void *)re;
}


int main(){
    
    int re_sqlite;
    void *data;

    Account acc1("10068", 500.0);
    Account acc2("10234", 394.28);
    vector<Account> accounts;
    accounts.push_back(acc1);
    accounts.push_back(acc2);
    data = (void *)&accounts;

    printf("\ndata is in: %x\n", data);

    pthread_t sqlite_thread;
    re_sqlite = pthread_create(&sqlite_thread, NULL, thread_sqlite, data);

    pthread_join(sqlite_thread, NULL);

    printf("thread sqlite returned: %i\n", re_sqlite);
}

