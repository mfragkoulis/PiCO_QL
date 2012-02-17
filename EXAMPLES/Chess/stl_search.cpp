#include <assert.h>
#include <stdio.h>
#include <string>
#include "stl_search.h"
#include "user_functions.h"
#include "workers.h"
#include <map>
#include "ChessPiece.h"
#include <vector>


using namespace std;

struct name_cmp {
    bool operator()(const char *a, const char *b) {
        return strcmp(a, b) < 0;
    }
};

static map<const char *, int, name_cmp> vt_directory;
static map<const char *, int, name_cmp>::iterator vtd_iter;


extern vector<vector<ChessPiece> >* chessBoard;


void * thread_sqlite(void *data){
    const char **queries, **table_names;
    queries = (const char **)sqlite3_malloc(sizeof(char *) *
                   2);
    table_names = (const char **)sqlite3_malloc(sizeof(char *) *
                   2);
    int failure = 0;
    queries[0] = "CREATE VIRTUAL TABLE ChessRow USING stl(base INT,name TEXT,color TEXT)";
    table_names[0] = "ChessRow";
    queries[1] = "CREATE VIRTUAL TABLE ChessBoard USING stl(row_id INT)";
    table_names[1] = "ChessBoard";
    failure = register_table( "ChessDB" ,  2, queries, table_names, data);
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
    if ( !strcmp(stl->zName, "ChessBoard") ) {
        stl->data = (void *)chessBoard;
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
    if ( !strcmp(stl->zName, "ChessRow") ) {
        vector<ChessPiece>* any_dstr = (vector<ChessPiece>*)stc->source;
        return (int)any_dstr->size();
    } else if ( !strcmp(stl->zName, "ChessBoard") ) {
        vector<vector<ChessPiece> >* any_dstr = (vector<vector<ChessPiece> >*)stc->source;
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
int ChessRow_search(sqlite3_vtab_cursor *cur, char *constr, sqlite3_value *val){
    stlTable *stl = (stlTable *)cur->pVtab;
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    vector<ChessPiece>* any_dstr;
    vector<ChessPiece>:: iterator iter;
    int op, iCol, count = 0, i = 0, re = 0;
    int size;
    if ( stl->zErr ) {
        sqlite3_free(stl->zErr);
        return SQLITE_MISUSE;
    }
    if ( val==NULL ){
        printf("Searching VT ChessRow with no BASE constraint...makes no sense.\n");
        return SQLITE_MISUSE;
    } else {
        check_alloc((const char *)constr, op, iCol);
        if ( equals_base(stl->azColumn[iCol]) ) {
            stcsr->source = (void *)sqlite3_value_int64(val);
            any_dstr = (vector<ChessPiece>*)stcsr->source;
            size = get_datastructure_size(cur);
            realloc_resultset(cur);
        }
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
                temp_res[count++] = i;
                iter++;
            }
            assert(count <= stcsr->max_size);
            break;
        case 1:
            iter = any_dstr->begin();
            for(int i=0; i<size;i++){
                if (compare((const unsigned char *)(*iter).get_name().c_str(), op, sqlite3_value_text(val)) )
                    temp_res[count++] = i;
                iter++;
            }
            assert(count <= stcsr->max_size);
            break;
        case 2:
            iter = any_dstr->begin();
            for(int i=0; i<size;i++){
                if (compare((const unsigned char *)(*iter).get_color().c_str(), op, sqlite3_value_text(val)) )
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


int ChessBoard_search(sqlite3_vtab_cursor *cur, char *constr, sqlite3_value *val){
    stlTable *stl = (stlTable *)cur->pVtab;
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    vector<vector<ChessPiece> >* any_dstr = (vector<vector<ChessPiece> >*)stcsr->source;
    vector<vector<ChessPiece> >:: iterator iter;
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
            printf("Restricted area. Searching VT ChessBoard column row_id...makes no sense.\n");
            return SQLITE_MISUSE;
        }
        if ( (re = compare_res(count, stcsr, temp_res)) != 0 )
            return re;
        sqlite3_free(temp_res);
    }
    return SQLITE_OK;
}


int search(sqlite3_vtab_cursor* cur, char *constr, sqlite3_value *val){
    stlTable *stl = (stlTable *)cur->pVtab;
    if( !strcmp(stl->zName, "ChessRow") )
        return ChessRow_search(cur, constr, val);
    if( !strcmp(stl->zName, "ChessBoard") )
        return ChessBoard_search(cur, constr, val);
}


int ChessRow_retrieve(sqlite3_vtab_cursor *cur, int n, sqlite3_context *con){
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    vector<ChessPiece>* any_dstr = (vector<ChessPiece>*)stcsr->source;
    vector<ChessPiece>:: iterator iter;
    int index = stcsr->current;
    iter = any_dstr->begin();
    for(int i=0; i<stcsr->resultSet[index]; i++){
        iter++;
    }
    switch( n ){
    case 0:
        sqlite3_result_int64(con, (long int)any_dstr);
        break;
    case 1:
        sqlite3_result_text(con, (const char *)(*iter).get_name().c_str(), -1, SQLITE_STATIC);
        break;
    case 2:
        sqlite3_result_text(con, (const char *)(*iter).get_color().c_str(), -1, SQLITE_STATIC);
        break;
    }
    return SQLITE_OK;
}


int ChessBoard_retrieve(sqlite3_vtab_cursor *cur, int n, sqlite3_context *con){
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    vector<vector<ChessPiece> >* any_dstr = (vector<vector<ChessPiece> >*)stcsr->source;
    vector<vector<ChessPiece> >:: iterator iter;
    int index = stcsr->current;
    iter = any_dstr->begin();
    for(int i=0; i<stcsr->resultSet[index]; i++){
        iter++;
    }
    switch( n ){
    case 0:
        if ( (vtd_iter = vt_directory.find("ChessRow")) != vt_directory.end() )
            vtd_iter->second = 1;
        sqlite3_result_int64(con, (long int)&*iter);
        break;
    }
    return SQLITE_OK;
}


int retrieve(sqlite3_vtab_cursor *cur, int n, sqlite3_context *con){
    stlTable *stl = (stlTable *)cur->pVtab;
    if( !strcmp(stl->zName, "ChessRow") )
        return ChessRow_retrieve(cur, n, con);
    if( !strcmp(stl->zName, "ChessBoard") )
        return ChessBoard_retrieve(cur, n, con);
}
