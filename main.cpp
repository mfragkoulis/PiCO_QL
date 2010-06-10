//#include "sqlite3.h"
#include <vector>
//#include "bridge.h"
#include "Account.h"

using namespace std;

int main(){
    Account acc1("10068", 500.0);
    Account acc2("10234", 394.28);
    vector<Account> accounts;
    accounts.push_back(acc1);
    accounts.push_back(acc2);
    void *data=(void *)&accounts;
    printf("data is in %x\n", data);
    char text[20];
    scanf("%s", text);
}
/*
//    register_table("foo.db", "stl", "CREATE VIRTUAL TABLE account USING stl( \
//INTEGER PRIMARY KEY AUTOINCREMENT,account_no TEXT,balance FLOAT)", data);   
//create the virtual table.done once



    sqlite3* db;
    sqlite3_stmt *stmt;
    int i , j;
    
    char *nDb = "foo.db";
    char *nModule = "stl";
//    char *query = "SELECT * FROM account,customer	\
//WHERE account.account_no==customer.account_id;";

    char *query = "SELECT * FROM account WHERE account_no='10068';";
    
    int re = sqlite3_open(nDb, &db);
    if( re ){
	printf("can't open database\n");
	sqlite3_close(db);
	exit(1);
    }
    sqlite3_module mod;
    fill_module(&mod);
    
    int output = sqlite3_create_module(db, nModule, &mod, data); 
    
    if( output==1 ) printf("Error while registering module\n");
    else if( output==0 ) printf("Module registered successfully\n");

    char text[20];
    printf("%x\n", db);
    scanf("%s", text);
    
    char ***pazResult = (char ***)sqlite3_malloc(sizeof(char***));
    int *nRows=(int *)sqlite3_malloc(sizeof(int));
    int *nCols=(int *)sqlite3_malloc(sizeof(int));
    char **errMsg=(char **)sqlite3_malloc(sizeof(char**));
  
    if( sqlite3_get_table(db, query, pazResult, nRows, nCols, errMsg)
	==SQLITE_OK){
	printf("\nquery is: %s\n", query);
	printf("\nQUERY SUCCESSFUL! \n\n");
	printf("result table:\n\n");
	for( i=0; i<(*nRows +1) * (*nCols); i++){
	    printf("%s ", pazResult[0][i]);
	    if( (i+(*nCols)+1) % (*nCols)==0 ) printf("\n");
	}
	printf("\n\n");
    }
    sqlite3_close(db);

    sqlite3_free_table(*pazResult);
    sqlite3_free(nRows);
    sqlite3_free(nCols);
    sqlite3_free(errMsg);
}
*/
