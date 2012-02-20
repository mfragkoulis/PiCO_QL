#include <assert.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include "stl_search.h"
#include "user_functions.h"
#include "workers.h"
#include <map>
#include "SuperAccount.h"
#include "Account.h"
#include "SpecialAccount.h"
#include <map>
#include <list>
#include <vector>


using namespace std;

struct name_cmp {
    bool operator()(const char *a, const char *b) {
        return strcmp(a, b) < 0;
    }
};

static map<const char *, int, name_cmp> vt_directory;
static map<const char *, int, name_cmp>::iterator vtd_iter;


extern map<string,SuperAccount> superaccounts;
extern vector<Account> accounts;
extern list<SpecialAccount> specialaccounts;


void * thread_sqlite(void *data){
    const char **queries, **table_names;
    queries = (const char **)sqlite3_malloc(sizeof(char *) *
                   3);
    table_names = (const char **)sqlite3_malloc(sizeof(char *) *
                   3);
    int failure = 0;
    queries[0] = "CREATE VIRTUAL TABLE SuperAccounts USING stl(id TEXT,balance DOUBLE,account_no TEXT,rate DOUBLE,isbn INT)";
    table_names[0] = "SuperAccounts";
    queries[1] = "CREATE VIRTUAL TABLE Accounts USING stl(balance DOUBLE,account_no TEXT,rate DOUBLE,isbn INT,type TEXT)";
    table_names[1] = "Accounts";
    queries[2] = "CREATE VIRTUAL TABLE Specialaccounts USING stl(balance DOUBLE,account_no TEXT,rate DOUBLE,isbn INT,bonus DOUBLE)";
    table_names[2] = "Specialaccounts";
    failure = register_table( "Bank" ,  3, queries, table_names, data);
    printf("Thread sqlite returning..\n");
    sqlite3_free(queries);
    sqlite3_free(table_names);
    return (void *)failure;
}


int call_sqtl() {
    pthread_t sqlite_thread;
    int re_sqlite = pthread_create(&sqlite_thread, NULL, thread_sqlite, NULL);
    pthread_join(sqlite_thread, NULL);
    return re_sqlite;
}



void register_vt(stlTable *stl) {
    if ( !strcmp(stl->zName, "SuperAccounts") ) {
        stl->data = (void *)&superaccounts;
        stl->embedded = 0;
    } else if ( !strcmp(stl->zName, "Accounts") ) {
        stl->data = (void *)&accounts;
        stl->embedded = 0;
    } else if ( !strcmp(stl->zName, "Specialaccounts") ) {
        stl->data = (void *)&specialaccounts;
        stl->embedded = 0;
    } else {
        stl->data = NULL;
        stl->embedded = 1;
    }
    vt_directory[stl->zName] = 0;
}



int get_datastructure_size(sqlite3_vtab_cursor *cur){
    stlTableCursor *stc = (stlTableCursor *)cur;
    stlTable *stl = (stlTable *)cur->pVtab;
    if ( !strcmp(stl->zName, "SuperAccounts") ) {
        map<string,SuperAccount>* any_dstr = (map<string,SuperAccount>*)stc->source;
        return (int)any_dstr->size();
    } else if ( !strcmp(stl->zName, "Accounts") ) {
        vector<Account>* any_dstr = (vector<Account>*)stc->source;
        return (int)any_dstr->size();
    } else if ( !strcmp(stl->zName, "Specialaccounts") ) {
        list<SpecialAccount>* any_dstr = (list<SpecialAccount>*)stc->source;
        return (int)any_dstr->size();
    }
    return 1;
}


// hard-coded
int equals_base(const char *zCol) {
    int length = (int)strlen(zCol) + 1;
    char copy[length], *token;
    memcpy(copy, zCol, length);
    token = strtok(copy, " ");
    if ( token != NULL ) {
        if ( !strcmp(token, "base") )
            return true;
        else
            return false;
    } else
        return SQLITE_NOMEM;
}
int SuperAccounts_search(sqlite3_vtab_cursor *cur, char *constr, sqlite3_value *val){
    stlTable *stl = (stlTable *)cur->pVtab;
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    map<string,SuperAccount>* any_dstr = (map<string,SuperAccount>*)stcsr->source;
    map<string,SuperAccount>:: iterator iter;
    int op, iCol, count = 0, i = 0, re = 0;
    int size = get_datastructure_size(cur);
    if ( val==NULL ){
        for (int j=0; j<size; j++){
            stcsr->resultSet[j] = j;
            stcsr->size++;
	}
        assert(stcsr->size <= stcsr->max_size);
        assert(&stcsr->resultSet[stcsr->size] <= &stcsr->resultSet[stcsr->max_size]);
    } else {
        check_alloc((const char *)constr, op, iCol);
        int *temp_res;
	temp_res = (int *)sqlite3_malloc(sizeof(int)  * stcsr->max_size);
        if ( !temp_res ){
            printf("Error in allocating memory\n");
            return SQLITE_NOMEM;
        }
        switch( iCol ){
        case 0:
            iter = any_dstr->begin();
            for(int i=0; i<size;i++){
                if (compare((const unsigned char *)(*iter).first.c_str(), op, sqlite3_value_text(val)) )
                    temp_res[count++] = i;
                iter++;
            }
            assert(count <= stcsr->max_size);
            break;
        case 1:
            iter = any_dstr->begin();
            for(int i=0; i<size;i++){
                if (compare((*iter).second.get_balance(), op, sqlite3_value_double(val)) )
                    temp_res[count++] = i;
                iter++;
            }
            assert(count <= stcsr->max_size);
            break;
        case 2:
            iter = any_dstr->begin();
            for(int i=0; i<size;i++){
                if (compare((const unsigned char *)(*iter).second.get_account_no(), op, sqlite3_value_text(val)) )
                    temp_res[count++] = i;
                iter++;
            }
            assert(count <= stcsr->max_size);
            break;
        case 3:
            iter = any_dstr->begin();
            for(int i=0; i<size;i++){
                if (compare((*iter).second.get_rate(), op, sqlite3_value_double(val)) )
                    temp_res[count++] = i;
                iter++;
            }
            assert(count <= stcsr->max_size);
            break;
        case 4:
            iter = any_dstr->begin();
            for(int i=0; i<size;i++){
                if (compare((*iter).second.get_isbn(), op, sqlite3_value_int(val)) )
                    temp_res[count++] = i;
                iter++;
            }
            assert(count <= stcsr->max_size);
            break;
        }
        if ( (re = compare_res(count, stcsr, temp_res)) != 0 )
            return re;
        sqlite3_free(temp_res);
    }
    return SQLITE_OK;
}


int Accounts_search(sqlite3_vtab_cursor *cur, char *constr, sqlite3_value *val){
    stlTable *stl = (stlTable *)cur->pVtab;
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    vector<Account>* any_dstr = (vector<Account>*)stcsr->source;
    vector<Account>:: iterator iter;
    int op, iCol, count = 0, i = 0, re = 0;
    int size = get_datastructure_size(cur);
    if ( val==NULL ){
        for (int j=0; j<size; j++){
            stcsr->resultSet[j] = j;
            stcsr->size++;
	}
        assert(stcsr->size <= stcsr->max_size);
        assert(&stcsr->resultSet[stcsr->size] <= &stcsr->resultSet[stcsr->max_size]);
    } else {
        check_alloc((const char *)constr, op, iCol);
        int *temp_res;
	temp_res = (int *)sqlite3_malloc(sizeof(int)  * stcsr->max_size);
        if ( !temp_res ){
            printf("Error in allocating memory\n");
            return SQLITE_NOMEM;
        }
        switch( iCol ){
        case 0:
            iter = any_dstr->begin();
            for(int i=0; i<size;i++){
                if (compare((*iter).get_balance(), op, sqlite3_value_double(val)) )
                    temp_res[count++] = i;
                iter++;
            }
            assert(count <= stcsr->max_size);
            break;
        case 1:
            iter = any_dstr->begin();
            for(int i=0; i<size;i++){
                if (compare((const unsigned char *)(*iter).get_account_no(), op, sqlite3_value_text(val)) )
                    temp_res[count++] = i;
                iter++;
            }
            assert(count <= stcsr->max_size);
            break;
        case 2:
            iter = any_dstr->begin();
            for(int i=0; i<size;i++){
                if (compare((*iter).get_rate(), op, sqlite3_value_double(val)) )
                    temp_res[count++] = i;
                iter++;
            }
            assert(count <= stcsr->max_size);
            break;
        case 3:
            iter = any_dstr->begin();
            for(int i=0; i<size;i++){
                if (compare((*iter).get_isbn(), op, sqlite3_value_int(val)) )
                    temp_res[count++] = i;
                iter++;
            }
            assert(count <= stcsr->max_size);
            break;
        case 4:
            iter = any_dstr->begin();
            for(int i=0; i<size;i++){
                if (compare((const unsigned char *)(*iter).type, op, sqlite3_value_text(val)) )
                    temp_res[count++] = i;
                iter++;
            }
            assert(count <= stcsr->max_size);
            break;
        }
        if ( (re = compare_res(count, stcsr, temp_res)) != 0 )
            return re;
        sqlite3_free(temp_res);
    }
    return SQLITE_OK;
}


int Specialaccounts_search(sqlite3_vtab_cursor *cur, char *constr, sqlite3_value *val){
    stlTable *stl = (stlTable *)cur->pVtab;
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    list<SpecialAccount>* any_dstr = (list<SpecialAccount>*)stcsr->source;
    list<SpecialAccount>:: iterator iter;
    int op, iCol, count = 0, i = 0, re = 0;
    int size = get_datastructure_size(cur);
    if ( val==NULL ){
        for (int j=0; j<size; j++){
            stcsr->resultSet[j] = j;
            stcsr->size++;
	}
        assert(stcsr->size <= stcsr->max_size);
        assert(&stcsr->resultSet[stcsr->size] <= &stcsr->resultSet[stcsr->max_size]);
    } else {
        check_alloc((const char *)constr, op, iCol);
        int *temp_res;
	temp_res = (int *)sqlite3_malloc(sizeof(int)  * stcsr->max_size);
        if ( !temp_res ){
            printf("Error in allocating memory\n");
            return SQLITE_NOMEM;
        }
        switch( iCol ){
        case 0:
            iter = any_dstr->begin();
            for(int i=0; i<size;i++){
                if (compare((*iter).get_balance(), op, sqlite3_value_double(val)) )
                    temp_res[count++] = i;
                iter++;
            }
            assert(count <= stcsr->max_size);
            break;
        case 1:
            iter = any_dstr->begin();
            for(int i=0; i<size;i++){
                if (compare((const unsigned char *)(*iter).get_account_no(), op, sqlite3_value_text(val)) )
                    temp_res[count++] = i;
                iter++;
            }
            assert(count <= stcsr->max_size);
            break;
        case 2:
            iter = any_dstr->begin();
            for(int i=0; i<size;i++){
                if (compare((*iter).get_rate(), op, sqlite3_value_double(val)) )
                    temp_res[count++] = i;
                iter++;
            }
            assert(count <= stcsr->max_size);
            break;
        case 3:
            iter = any_dstr->begin();
            for(int i=0; i<size;i++){
                if (compare((*iter).get_isbn(), op, sqlite3_value_int(val)) )
                    temp_res[count++] = i;
                iter++;
            }
            assert(count <= stcsr->max_size);
            break;
        case 4:
            iter = any_dstr->begin();
            for(int i=0; i<size;i++){
                if (compare((*iter).bonus, op, sqlite3_value_double(val)) )
                    temp_res[count++] = i;
                iter++;
            }
            assert(count <= stcsr->max_size);
            break;
        }
        if ( (re = compare_res(count, stcsr, temp_res)) != 0 )
            return re;
        sqlite3_free(temp_res);
    }
    return SQLITE_OK;
}


int search(sqlite3_vtab_cursor* cur, char *constr, sqlite3_value *val){
    stlTable *stl = (stlTable *)cur->pVtab;
    if( !strcmp(stl->zName, "SuperAccounts") )
        return SuperAccounts_search(cur, constr, val);
    if( !strcmp(stl->zName, "Accounts") )
        return Accounts_search(cur, constr, val);
    if( !strcmp(stl->zName, "Specialaccounts") )
        return Specialaccounts_search(cur, constr, val);
}


int SuperAccounts_retrieve(sqlite3_vtab_cursor *cur, int n, sqlite3_context *con){
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    map<string,SuperAccount>* any_dstr = (map<string,SuperAccount>*)stcsr->source;
    map<string,SuperAccount>:: iterator iter;
    int index = stcsr->current;
    iter = any_dstr->begin();
    for(int i=0; i<stcsr->resultSet[index]; i++){
        iter++;
    }
    switch( n ){
    case 0:
        sqlite3_result_text(con, (const char *)(*iter).first.c_str(), -1, SQLITE_STATIC);
        break;
    case 1:
        sqlite3_result_double(con, (*iter).second.get_balance());
        break;
    case 2:
        sqlite3_result_text(con, (const char *)(*iter).second.get_account_no(), -1, SQLITE_STATIC);
        break;
    case 3:
        sqlite3_result_double(con, (*iter).second.get_rate());
        break;
    case 4:
        sqlite3_result_int(con, (*iter).second.get_isbn());
        break;
    }
    return SQLITE_OK;
}


int Accounts_retrieve(sqlite3_vtab_cursor *cur, int n, sqlite3_context *con){
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    vector<Account>* any_dstr = (vector<Account>*)stcsr->source;
    vector<Account>:: iterator iter;
    int index = stcsr->current;
    iter = any_dstr->begin();
    for(int i=0; i<stcsr->resultSet[index]; i++){
        iter++;
    }
    switch( n ){
    case 0:
        sqlite3_result_double(con, (*iter).get_balance());
        break;
    case 1:
        sqlite3_result_text(con, (const char *)(*iter).get_account_no(), -1, SQLITE_STATIC);
        break;
    case 2:
        sqlite3_result_double(con, (*iter).get_rate());
        break;
    case 3:
        sqlite3_result_int(con, (*iter).get_isbn());
        break;
    case 4:
        sqlite3_result_text(con, (const char *)(*iter).type, -1, SQLITE_STATIC);
        break;
    }
    return SQLITE_OK;
}


int Specialaccounts_retrieve(sqlite3_vtab_cursor *cur, int n, sqlite3_context *con){
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    list<SpecialAccount>* any_dstr = (list<SpecialAccount>*)stcsr->source;
    list<SpecialAccount>:: iterator iter;
    int index = stcsr->current;
    iter = any_dstr->begin();
    for(int i=0; i<stcsr->resultSet[index]; i++){
        iter++;
    }
    switch( n ){
    case 0:
        sqlite3_result_double(con, (*iter).get_balance());
        break;
    case 1:
        sqlite3_result_text(con, (const char *)(*iter).get_account_no(), -1, SQLITE_STATIC);
        break;
    case 2:
        sqlite3_result_double(con, (*iter).get_rate());
        break;
    case 3:
        sqlite3_result_int(con, (*iter).get_isbn());
        break;
    case 4:
        sqlite3_result_double(con, (*iter).bonus);
        break;
    }
    return SQLITE_OK;
}


int retrieve(sqlite3_vtab_cursor *cur, int n, sqlite3_context *con){
    stlTable *stl = (stlTable *)cur->pVtab;
    if( !strcmp(stl->zName, "SuperAccounts") )
        return SuperAccounts_retrieve(cur, n, con);
    if( !strcmp(stl->zName, "Accounts") )
        return Accounts_retrieve(cur, n, con);
    if( !strcmp(stl->zName, "Specialaccounts") )
        return Specialaccounts_retrieve(cur, n, con);
}
