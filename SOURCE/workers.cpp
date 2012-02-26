#include <assert.h>
#include <cstdlib>
#include <stdio.h>
#include <string>
#include <string.h>
#include "workers.h"

using namespace std;

int realloc_resultset(sqlite3_vtab_cursor *cur) {
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    int arraySize;
    int *res;
    arraySize = get_datastructure_size(cur);
    if ( arraySize != stcsr->max_size ){
        res = (int *)sqlite3_realloc(stcsr->resultSet, sizeof(int) * arraySize);
        if (res!=NULL){
            stcsr->resultSet = res;
            memset(stcsr->resultSet, -1,
                   sizeof(int) * arraySize);
            stcsr->max_size = arraySize;
#ifdef DEBUG
            printf("\\nReallocating resultSet..now max size %i \\n\\n", stcsr->max_size);
#endif
        }else{
            sqlite3_free(res);
            printf("Error (re)allocating memory\\n");
            return SQLITE_NOMEM;
        }
    }
    return SQLITE_OK;
}


int compare(int dstr_value, int op, int value){
    switch( op ){
    case 0:
        return dstr_value<value;
    case 1:
        return dstr_value<=value;
    case 2:
        return dstr_value==value;
    case 3:
        return dstr_value>=value;
    case 4:
        return dstr_value>value;
    }
}


int compare(long int dstr_value, int op, long int value){
    switch( op ){
    case 0:
        return dstr_value<value;
    case 1:
        return dstr_value<=value;
    case 2:
        return dstr_value==value;
    case 3:
        return dstr_value>=value;
    case 4:
        return dstr_value>value;
    }
}


int compare(double dstr_value, int op, double value){
    switch( op ){
    case 0:
        return dstr_value<value;
    case 1:
        return dstr_value<=value;
    case 2:
        return dstr_value==value;
    case 3:
        return dstr_value>=value;
    case 4:
        return dstr_value>value;
    }
}


int compare(const void *dstr_value, int op, const void *value){
    switch( op ){
    case 0:
        return dstr_value<value;
    case 1:
        return dstr_value<=value;
    case 2:
        return dstr_value==value;
    case 3:
        return dstr_value>=value;
    case 4:
        return dstr_value>value;
    }
}


int compare(const unsigned char *dstr_value, int op,
	    const unsigned char *value){
    switch( op ){
    case 0:
        return strcmp((const char *)dstr_value,(const char *)value)<0;
    case 1:
        return strcmp((const char *)dstr_value,(const char *)value)<=0;
    case 2:
        return strcmp((const char *)dstr_value,(const char *)value)==0;
    case 3:
        return strcmp((const char *)dstr_value,(const char *)value)>=0;
    case 4:
        return strcmp((const char *)dstr_value,(const char *)value)>0;
    }
}


int compare_res(int count, stlTableCursor *stcsr, int *temp_res) {
    int ia, ib;
    int *i_res;
    int i_count = 0;
    if ( (stcsr->size == 0) && (stcsr->first_constr == 1) ){
        memcpy(stcsr->resultSet, temp_res, sizeof(int) *
               stcsr->max_size);
        stcsr->size = count;
        stcsr->first_constr = 0;
    }else if (stcsr->size > 0){
        i_res = (int *)sqlite3_malloc(sizeof(int) *
                                      stcsr->max_size);
        if ( i_res == NULL ) {
            sqlite3_free(i_res);
            printf("Error (re)allocating memory\\n");
            return SQLITE_NOMEM;
        }
        for(int a=0; a<stcsr->size; a++){
            for(int b=0; b<count; b++){
                ia = stcsr->resultSet[a];
                ib = temp_res[b];
                if( ia==ib ){
                    i_res[i_count++] = ia;
                }else if( ia < ib )
                    b = count;
            }
        }
        assert( i_count <= stcsr->max_size );
        memcpy(stcsr->resultSet, i_res, sizeof(int) *
               i_count);
        stcsr->size = i_count;
        sqlite3_free(i_res);
    }
    return SQLITE_OK;
}


void check_alloc(const char *constr, int &op, int &iCol) {
    switch( constr[0] - 'A' ){
    case 0:
        op = 0;
        break;
    case 1:
        op = 1;
        break;
    case 2:
        op = 2;
        break;
    case 3:
        op = 3;
        break;
    case 4:
        op = 4;
        break;
    case 5:
        op = 5;
        break;
    default:
        break;
    }
    iCol = constr[1] - 'a' + 1;
}
