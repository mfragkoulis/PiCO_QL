#include <assert.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include "stl_search.h"
#include "user_functions.h"
#include "workers.h"
#include <map>
#include <string>
#include <vector>
#include <map>

#include "Truck.h"
#include "Customer.h"


using namespace std;

struct name_cmp {
    bool operator()(const char *a, const char *b) {
        return strcmp(a, b) < 0;
    }
};

static map<const char *, int, name_cmp> vt_directory;
static map<const char *, int, name_cmp>::iterator vtd_iter;


extern vector<Truck*>* vehicles;
extern map<int,Customer*> test;


void * thread_sqlite(void *data){
    const char **queries, **table_names;
    queries = (const char **)sqlite3_malloc(sizeof(char *) *
                   6);
    table_names = (const char **)sqlite3_malloc(sizeof(char *) *
                   6);
    int failure = 0;
    queries[0] = "CREATE VIRTUAL TABLE Trucks USING stl(truck_ptr INT)";
    table_names[0] = "Trucks";
    queries[1] = "CREATE VIRTUAL TABLE Truck USING stl(base INT,customers_ptr INT,cost DOUBLE,delcapacity INT)";
    table_names[1] = "Truck";
    queries[2] = "CREATE VIRTUAL TABLE Customers USING stl(base INT,customer_ptr INT)";
    table_names[2] = "Customers";
    queries[3] = "CREATE VIRTUAL TABLE Customer USING stl(base INT,position_ptr INT,demand INT,code TEXT,serviced INT)";
    table_names[3] = "Customer";
    queries[4] = "CREATE VIRTUAL TABLE Position USING stl(base INT,x_coord INT,y_coord INT)";
    table_names[4] = "Position";
    queries[5] = "CREATE VIRTUAL TABLE MapIndex USING stl(map_index INT,customer_ptr INT)";
    table_names[5] = "MapIndex";
    failure = register_table( "mydb" ,  6, queries, table_names, data);
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
    if ( !strcmp(stl->zName, "Trucks") ) {
        stl->data = (void *)vehicles;
        stl->embedded = 0;
    } else if ( !strcmp(stl->zName, "MapIndex") ) {
        stl->data = (void *)&test;
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
    if ( !strcmp(stl->zName, "Trucks") ) {
        vector<Truck*>* any_dstr = (vector<Truck*>*)stc->source;
        return (int)any_dstr->size();
    } else if ( !strcmp(stl->zName, "Customers") ) {
        vector<Customer*>* any_dstr = (vector<Customer*>*)stc->source;
        return (int)any_dstr->size();
    } else if ( !strcmp(stl->zName, "MapIndex") ) {
        map<int,Customer*>* any_dstr = (map<int,Customer*>*)stc->source;
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
int Trucks_search(sqlite3_vtab_cursor *cur, char *constr, sqlite3_value *val){
    stlTable *stl = (stlTable *)cur->pVtab;
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    vector<Truck*>* any_dstr = (vector<Truck*>*)stcsr->source;
    vector<Truck*>:: iterator iter;
    int op, iCol, i = 0, count = 0, re = 0;
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
            printf("Restricted area. Searching VT Trucks column truck_ptr...makes no sense.\n");
            return SQLITE_MISUSE;
        }
        if ( (re = compare_res(count, stcsr, temp_res)) != 0 )
            return re;
        sqlite3_free(temp_res);
    }
    return SQLITE_OK;
}


int Truck_search(sqlite3_vtab_cursor *cur, char *constr, sqlite3_value *val){
    stlTable *stl = (stlTable *)cur->pVtab;
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    Truck* any_dstr = (Truck*)stcsr->source;
    int op, iCol, i = 0, count = 0, re = 0;
    int size;
    if ( stl->zErr ) {
        sqlite3_free(stl->zErr);
        return SQLITE_MISUSE;
    }
    if ( val==NULL ){
        printf("Searching VT Truck with no BASE constraint...makes no sense.\n");
        return SQLITE_MISUSE;
    } else {
        check_alloc((const char *)constr, op, iCol);
        if ( equals_base(stl->azColumn[iCol]) ) {
            vtd_iter = vt_directory.find(stl->zName);
            if ( (vtd_iter == vt_directory.end()) || (vtd_iter->second == 0) ) {
                printf("Invalid cast to %s\n", stl->zName);
                return SQLITE_MISUSE;
            }
            vt_directory[stl->zName] = 0;
            stcsr->source = (void *)sqlite3_value_int64(val);
            any_dstr = (Truck*)stcsr->source;
        }
        size = get_datastructure_size(cur);
        int *temp_res;
	temp_res = (int *)sqlite3_malloc(sizeof(int)  * stcsr->max_size);
        if ( !temp_res ){
            printf("Error in allocating memory\n");
            return SQLITE_NOMEM;
        }
        switch( iCol ){
        case 0:
            temp_res[count++] = i;
            assert(count <= stcsr->max_size);
            break;
        case 1:
            printf("Restricted area. Searching VT Truck column customers_ptr...makes no sense.\n");
            return SQLITE_MISUSE;
        case 2:
            if (compare(any_dstr->get_cost(), op, sqlite3_value_double(val)) )
                temp_res[count++] = i;
            assert(count <= stcsr->max_size);
            break;
        case 3:
            if (compare(any_dstr->get_delcapacity(), op, sqlite3_value_int(val)) )
                temp_res[count++] = i;
            assert(count <= stcsr->max_size);
            break;
        }
        if ( (re = compare_res(count, stcsr, temp_res)) != 0 )
            return re;
        sqlite3_free(temp_res);
    }
    return SQLITE_OK;
}


int Customers_search(sqlite3_vtab_cursor *cur, char *constr, sqlite3_value *val){
    stlTable *stl = (stlTable *)cur->pVtab;
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    vector<Customer*>* any_dstr = (vector<Customer*>*)stcsr->source;
    vector<Customer*>:: iterator iter;
    int op, iCol, i = 0, count = 0, re = 0;
    int size;
    if ( stl->zErr ) {
        sqlite3_free(stl->zErr);
        return SQLITE_MISUSE;
    }
    if ( val==NULL ){
        printf("Searching VT Customers with no BASE constraint...makes no sense.\n");
        return SQLITE_MISUSE;
    } else {
        check_alloc((const char *)constr, op, iCol);
        if ( equals_base(stl->azColumn[iCol]) ) {
            vtd_iter = vt_directory.find(stl->zName);
            if ( (vtd_iter == vt_directory.end()) || (vtd_iter->second == 0) ) {
                printf("Invalid cast to %s\n", stl->zName);
                return SQLITE_MISUSE;
            }
            vt_directory[stl->zName] = 0;
            stcsr->source = (void *)sqlite3_value_int64(val);
            any_dstr = (vector<Customer*>*)stcsr->source;
            realloc_resultset(cur);
        }
        size = get_datastructure_size(cur);
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
            printf("Restricted area. Searching VT Customers column customer_ptr...makes no sense.\n");
            return SQLITE_MISUSE;
        }
        if ( (re = compare_res(count, stcsr, temp_res)) != 0 )
            return re;
        sqlite3_free(temp_res);
    }
    return SQLITE_OK;
}


int Customer_search(sqlite3_vtab_cursor *cur, char *constr, sqlite3_value *val){
    stlTable *stl = (stlTable *)cur->pVtab;
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    Customer* any_dstr = (Customer*)stcsr->source;
    int op, iCol, i = 0, count = 0, re = 0;
    int size;
    if ( stl->zErr ) {
        sqlite3_free(stl->zErr);
        return SQLITE_MISUSE;
    }
    if ( val==NULL ){
        printf("Searching VT Customer with no BASE constraint...makes no sense.\n");
        return SQLITE_MISUSE;
    } else {
        check_alloc((const char *)constr, op, iCol);
        if ( equals_base(stl->azColumn[iCol]) ) {
            vtd_iter = vt_directory.find(stl->zName);
            if ( (vtd_iter == vt_directory.end()) || (vtd_iter->second == 0) ) {
                printf("Invalid cast to %s\n", stl->zName);
                return SQLITE_MISUSE;
            }
            vt_directory[stl->zName] = 0;
            stcsr->source = (void *)sqlite3_value_int64(val);
            any_dstr = (Customer*)stcsr->source;
        }
        size = get_datastructure_size(cur);
        int *temp_res;
	temp_res = (int *)sqlite3_malloc(sizeof(int)  * stcsr->max_size);
        if ( !temp_res ){
            printf("Error in allocating memory\n");
            return SQLITE_NOMEM;
        }
        switch( iCol ){
        case 0:
            temp_res[count++] = i;
            assert(count <= stcsr->max_size);
            break;
        case 1:
            printf("Restricted area. Searching VT Customer column position_ptr...makes no sense.\n");
            return SQLITE_MISUSE;
        case 2:
            if (compare(any_dstr->get_demand(), op, sqlite3_value_int(val)) )
                temp_res[count++] = i;
            assert(count <= stcsr->max_size);
            break;
        case 3:
            if (compare((const unsigned char *)any_dstr->get_code().c_str(), op, sqlite3_value_text(val)) )
                temp_res[count++] = i;
            assert(count <= stcsr->max_size);
            break;
        case 4:
            if (compare(any_dstr->get_serviced(), op, sqlite3_value_int(val)) )
                temp_res[count++] = i;
            assert(count <= stcsr->max_size);
            break;
        }
        if ( (re = compare_res(count, stcsr, temp_res)) != 0 )
            return re;
        sqlite3_free(temp_res);
    }
    return SQLITE_OK;
}


int Position_search(sqlite3_vtab_cursor *cur, char *constr, sqlite3_value *val){
    stlTable *stl = (stlTable *)cur->pVtab;
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    Position* any_dstr = (Position*)stcsr->source;
    int op, iCol, i = 0, count = 0, re = 0;
    int size;
    if ( stl->zErr ) {
        sqlite3_free(stl->zErr);
        return SQLITE_MISUSE;
    }
    if ( val==NULL ){
        printf("Searching VT Position with no BASE constraint...makes no sense.\n");
        return SQLITE_MISUSE;
    } else {
        check_alloc((const char *)constr, op, iCol);
        if ( equals_base(stl->azColumn[iCol]) ) {
            vtd_iter = vt_directory.find(stl->zName);
            if ( (vtd_iter == vt_directory.end()) || (vtd_iter->second == 0) ) {
                printf("Invalid cast to %s\n", stl->zName);
                return SQLITE_MISUSE;
            }
            vt_directory[stl->zName] = 0;
            stcsr->source = (void *)sqlite3_value_int64(val);
            any_dstr = (Position*)stcsr->source;
        }
        size = get_datastructure_size(cur);
        int *temp_res;
	temp_res = (int *)sqlite3_malloc(sizeof(int)  * stcsr->max_size);
        if ( !temp_res ){
            printf("Error in allocating memory\n");
            return SQLITE_NOMEM;
        }
        switch( iCol ){
        case 0:
            temp_res[count++] = i;
            assert(count <= stcsr->max_size);
            break;
        case 1:
            if (compare(any_dstr->get_x(), op, sqlite3_value_int(val)) )
                temp_res[count++] = i;
            assert(count <= stcsr->max_size);
            break;
        case 2:
            if (compare(any_dstr->get_y(), op, sqlite3_value_int(val)) )
                temp_res[count++] = i;
            assert(count <= stcsr->max_size);
            break;
        }
        if ( (re = compare_res(count, stcsr, temp_res)) != 0 )
            return re;
        sqlite3_free(temp_res);
    }
    return SQLITE_OK;
}


int MapIndex_search(sqlite3_vtab_cursor *cur, char *constr, sqlite3_value *val){
    stlTable *stl = (stlTable *)cur->pVtab;
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    map<int,Customer*>* any_dstr = (map<int,Customer*>*)stcsr->source;
    map<int,Customer*>:: iterator iter;
    int op, iCol, i = 0, count = 0, re = 0;
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
                if (compare((*iter).first, op, sqlite3_value_int(val)) )
                    temp_res[count++] = i;
                iter++;
            }
            assert(count <= stcsr->max_size);
            break;
        case 1:
            printf("Restricted area. Searching VT MapIndex column customer_ptr...makes no sense.\n");
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
    if( !strcmp(stl->zName, "Trucks") )
        return Trucks_search(cur, constr, val);
    if( !strcmp(stl->zName, "Truck") )
        return Truck_search(cur, constr, val);
    if( !strcmp(stl->zName, "Customers") )
        return Customers_search(cur, constr, val);
    if( !strcmp(stl->zName, "Customer") )
        return Customer_search(cur, constr, val);
    if( !strcmp(stl->zName, "Position") )
        return Position_search(cur, constr, val);
    if( !strcmp(stl->zName, "MapIndex") )
        return MapIndex_search(cur, constr, val);
}


int Trucks_retrieve(sqlite3_vtab_cursor *cur, int n, sqlite3_context *con){
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    vector<Truck*>* any_dstr = (vector<Truck*>*)stcsr->source;
    vector<Truck*>:: iterator iter;
    int index = stcsr->current;
    iter = any_dstr->begin();
    for(int i=0; i<stcsr->resultSet[index]; i++){
        iter++;
    }
    switch( n ){
    case 0:
        if ( (vtd_iter = vt_directory.find("Truck")) != vt_directory.end() )
            vtd_iter->second = 1;
        sqlite3_result_int64(con, (long int)*iter);
        break;
    }
    return SQLITE_OK;
}


int Truck_retrieve(sqlite3_vtab_cursor *cur, int n, sqlite3_context *con){
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    Truck* any_dstr = (Truck*)stcsr->source;
    switch( n ){
    case 0:
        sqlite3_result_int64(con, (long int)any_dstr);
        break;
    case 1:
        if ( (vtd_iter = vt_directory.find("Customers")) != vt_directory.end() )
            vtd_iter->second = 1;
        sqlite3_result_int64(con, (long int)any_dstr->get_Customers());
        break;
    case 2:
        sqlite3_result_double(con, any_dstr->get_cost());
        break;
    case 3:
        sqlite3_result_int(con, any_dstr->get_delcapacity());
        break;
    }
    return SQLITE_OK;
}


int Customers_retrieve(sqlite3_vtab_cursor *cur, int n, sqlite3_context *con){
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    vector<Customer*>* any_dstr = (vector<Customer*>*)stcsr->source;
    vector<Customer*>:: iterator iter;
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
        if ( (vtd_iter = vt_directory.find("Customer")) != vt_directory.end() )
            vtd_iter->second = 1;
        sqlite3_result_int64(con, (long int)*iter);
        break;
    }
    return SQLITE_OK;
}


int Customer_retrieve(sqlite3_vtab_cursor *cur, int n, sqlite3_context *con){
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    Customer* any_dstr = (Customer*)stcsr->source;
    switch( n ){
    case 0:
        sqlite3_result_int64(con, (long int)any_dstr);
        break;
    case 1:
        if ( (vtd_iter = vt_directory.find("Position")) != vt_directory.end() )
            vtd_iter->second = 1;
        sqlite3_result_int64(con, (long int)any_dstr->get_pos());
        break;
    case 2:
        sqlite3_result_int(con, any_dstr->get_demand());
        break;
    case 3:
        sqlite3_result_text(con, (const char *)any_dstr->get_code().c_str(), -1, SQLITE_STATIC);
        break;
    case 4:
        sqlite3_result_int(con, any_dstr->get_serviced());
        break;
    }
    return SQLITE_OK;
}


int Position_retrieve(sqlite3_vtab_cursor *cur, int n, sqlite3_context *con){
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    Position* any_dstr = (Position*)stcsr->source;
    switch( n ){
    case 0:
        sqlite3_result_int64(con, (long int)any_dstr);
        break;
    case 1:
        sqlite3_result_int(con, any_dstr->get_x());
        break;
    case 2:
        sqlite3_result_int(con, any_dstr->get_y());
        break;
    }
    return SQLITE_OK;
}


int MapIndex_retrieve(sqlite3_vtab_cursor *cur, int n, sqlite3_context *con){
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    map<int,Customer*>* any_dstr = (map<int,Customer*>*)stcsr->source;
    map<int,Customer*>:: iterator iter;
    int index = stcsr->current;
    iter = any_dstr->begin();
    for(int i=0; i<stcsr->resultSet[index]; i++){
        iter++;
    }
    switch( n ){
    case 0:
        sqlite3_result_int(con, (*iter).first);
        break;
    case 1:
        if ( (vtd_iter = vt_directory.find("Customer")) != vt_directory.end() )
            vtd_iter->second = 1;
        sqlite3_result_int64(con, (long int)(*iter).second);
        break;
    }
    return SQLITE_OK;
}


int retrieve(sqlite3_vtab_cursor *cur, int n, sqlite3_context *con){
    stlTable *stl = (stlTable *)cur->pVtab;
    if( !strcmp(stl->zName, "Trucks") )
        return Trucks_retrieve(cur, n, con);
    if( !strcmp(stl->zName, "Truck") )
        return Truck_retrieve(cur, n, con);
    if( !strcmp(stl->zName, "Customers") )
        return Customers_retrieve(cur, n, con);
    if( !strcmp(stl->zName, "Customer") )
        return Customer_retrieve(cur, n, con);
    if( !strcmp(stl->zName, "Position") )
        return Position_retrieve(cur, n, con);
    if( !strcmp(stl->zName, "MapIndex") )
        return MapIndex_retrieve(cur, n, con);
}
