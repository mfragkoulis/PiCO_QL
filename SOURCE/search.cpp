#include "search.h"
#include <string>
#include <assert.h>
#include <stdio.h>
#include <string>
#include <vector>

#include "Truck.h"
#include "Customer.h"

using namespace std;

int set_dependencies(void *st, void *ds, const char *table_name) {
    stlTable *stl = (stlTable *)st;
    dsCarrier **ddsC = (dsCarrier **)ds;
    dsCarrier *dsC = *ddsC;
    int c = 0, ch, dc;
    int dsC_size = dsC->size;
    while (c < dsC_size) {
        if ( !strcmp(table_name, dsC->dsNames[c]) )
            break;
        c++;
    }
    if( !strcmp(table_name, "Trucks") ) {
        data *d = (data *)sqlite3_malloc(sizeof(data) + sizeof(dsCarrier) + sizeof(long int *) * 1 + sizeof(const char *) * 1);
        d->children = (dsCarrier *)&d[1];
        d->children->memories = (long int **)&d->children[1];
        d->children->dsNames = (const char **)&d->children->memories[1];
        d->children->size = 1;
        ch = 0;
        dc = 0;
        while (dc < dsC_size) {
            if ( !strcmp(dsC->dsNames[dc], "Truck") )
                break;
            dc++;
        }
        dsC->memories[dc] = (long int *)&d->children->memories[ch];
        d->children->dsNames[ch] = dsC->dsNames[dc];
        ch++;
        assert (ch == d->children->size);
        d->mem = dsC->memories[c];
        stl->data = (void *)d;
    } else if( !strcmp(table_name, "Truck") ) {
        data *d = (data *)sqlite3_malloc(sizeof(data) + sizeof(dsCarrier) + sizeof(long int *) * 1 + sizeof(const char *) * 1);
        d->children = (dsCarrier *)&d[1];
        d->children->memories = (long int **)&d->children[1];
        d->children->dsNames = (const char **)&d->children->memories[1];
        d->children->size = 1;
        ch = 0;
        dc = 0;
        while (dc < dsC_size) {
            if ( !strcmp(dsC->dsNames[dc], "Customers") )
                break;
            dc++;
        }
        dsC->memories[dc] = (long int *)&d->children->memories[ch];
        d->children->dsNames[ch] = dsC->dsNames[dc];
        ch++;
        assert (ch == d->children->size);
        d->mem = dsC->memories[c];
        stl->data = (void *)d;
    } else if( !strcmp(table_name, "Customers") ) {
        data *d = (data *)sqlite3_malloc(sizeof(data) + sizeof(dsCarrier) + sizeof(long int *) * 1 + sizeof(const char *) * 1);
        d->children = (dsCarrier *)&d[1];
        d->children->memories = (long int **)&d->children[1];
        d->children->dsNames = (const char **)&d->children->memories[1];
        d->children->size = 1;
        ch = 0;
        dc = 0;
        while (dc < dsC_size) {
            if ( !strcmp(dsC->dsNames[dc], "Customer") )
                break;
            dc++;
        }
        dsC->memories[dc] = (long int *)&d->children->memories[ch];
        d->children->dsNames[ch] = dsC->dsNames[dc];
        ch++;
        assert (ch == d->children->size);
        d->mem = dsC->memories[c];
        stl->data = (void *)d;
    } else if( !strcmp(table_name, "Customer") ) {
        data *d = (data *)sqlite3_malloc(sizeof(data) + sizeof(dsCarrier) + sizeof(long int *) * 1 + sizeof(const char *) * 1);
        d->children = (dsCarrier *)&d[1];
        d->children->memories = (long int **)&d->children[1];
        d->children->dsNames = (const char **)&d->children->memories[1];
        d->children->size = 1;
        ch = 0;
        dc = 0;
        while (dc < dsC_size) {
            if ( !strcmp(dsC->dsNames[dc], "Position") )
                break;
            dc++;
        }
        dsC->memories[dc] = (long int *)&d->children->memories[ch];
        d->children->dsNames[ch] = dsC->dsNames[dc];
        ch++;
        assert (ch == d->children->size);
        d->mem = dsC->memories[c];
        stl->data = (void *)d;
    } else if( !strcmp(table_name, "Position") ) {
        data *d = (data *)sqlite3_malloc(sizeof(data));
        d->children = NULL;
        d->mem = dsC->memories[c];
        stl->data = (void *)d;
    }
}


int realloc_carrier(void *st, void *ds, const char *tablename) {
    dsCarrier **ddsC = (dsCarrier **)ds;
    dsCarrier *dsC = *ddsC;
    dsCarrier *tmp_dsC;
    int x_size, nByte, c;
    char *c_temp;
#ifdef DEBUGGING
    printf("dsC->size: %i\n", dsC->size);
#endif
    if (dsC->size != 5) {
        x_size = dsC->size;
        nByte = sizeof(dsCarrier) + sizeof(long int *) * 5 + sizeof(const char *) * 5 + 41;
        tmp_dsC = (dsCarrier *)sqlite3_malloc(nByte);
        if (tmp_dsC != NULL) {
            tmp_dsC->size = 5;
            tmp_dsC->dsNames = (const char **)&tmp_dsC[1];
            tmp_dsC->memories = (long int **)&tmp_dsC->dsNames[5];
            c_temp = (char *)&tmp_dsC->memories[5];
            c = 0;
            int len;
            while (c < x_size) {
                tmp_dsC->dsNames[c] = c_temp;
                len = (int)strlen(dsC->dsNames[c]) + 1;
                memcpy(c_temp, dsC->dsNames[c], len);
                c_temp += len;
                tmp_dsC->memories[c] = dsC->memories[c];
                c++;
            }
            tmp_dsC->dsNames[c] = c_temp;
            len = (int)strlen("Truck") + 1;
            memcpy(c_temp, "Truck", len);
            c_temp += len;
            c++;
            tmp_dsC->dsNames[c] = c_temp;
            len = (int)strlen("Customers") + 1;
            memcpy(c_temp, "Customers", len);
            c_temp += len;
            c++;
            tmp_dsC->dsNames[c] = c_temp;
            len = (int)strlen("Customer") + 1;
            memcpy(c_temp, "Customer", len);
            c_temp += len;
            c++;
            tmp_dsC->dsNames[c] = c_temp;
            len = (int)strlen("Position") + 1;
            memcpy(c_temp, "Position", len);
            c_temp += len;
            c++;
            assert(c == 5);
#ifdef DEBUGGING
            printf("c_temp: %lx <= tmp_dsC: %lx \n", c_temp, &((char *)tmp_dsC)[nByte]);
#endif
            assert(c_temp <= &((char *)tmp_dsC)[nByte]);
            sqlite3_free(dsC);
            *ddsC = tmp_dsC;
#ifdef DEBUGGING
            printf("\nReallocating carrier..now size %i \n\n", tmp_dsC->size);
#endif
        }else{
            free(tmp_dsC);
            printf("Error (re)allocating memory\n");
            exit(1);
        }
    }
    set_dependencies(st, ds, tablename);
}


int update_structures(void *cur) {
    sqlite3_vtab_cursor *stc = (sqlite3_vtab_cursor *)cur;
    stlTable *stl = (stlTable *)stc->pVtab;
    const char *table_name = stl->zName;
    data *d = (data *)stl->data;
    stlTableCursor *stcsr = (stlTableCursor *)stc;
    const char **names;
    int dc, index, no_child;
    if( !strcmp(table_name, "Trucks") ) {
        vector<Truck*> *any_dstr = (vector<Truck*> *)d->mem;
        vector<Truck*>::iterator iter;
        iter = any_dstr->begin();
        index = stcsr->current;
        for(int i=0; i<stcsr->resultSet[index]; i++){
            iter++;
        }
        dc = 0;
        names = d->children->dsNames;
        no_child = d->children->size;
        while (dc < no_child) {
            if ( !strcmp(names[dc], "Truck") ) {
                d->children->memories[dc] = (long int *)(*iter);
                break;
            }
            dc++;
        }
    } else if( !strcmp(table_name, "Truck") ) {
        Truck *any_dstr = (Truck *)*d->mem;
        dc = 0;
        names = d->children->dsNames;
        no_child = d->children->size;
        while (dc < no_child) {
            if ( !strcmp(names[dc], "Customers") ) {
                d->children->memories[dc] = (long int *)any_dstr->get_Customers();
                break;
            }
            dc++;
        }
    } else if( !strcmp(table_name, "Customers") ) {
        vector<Customer*> *any_dstr = (vector<Customer*> *)*d->mem;
        vector<Customer*>::iterator iter;
        iter = any_dstr->begin();
        index = stcsr->current;
        for(int i=0; i<stcsr->resultSet[index]; i++){
            iter++;
        }
        dc = 0;
        names = d->children->dsNames;
        no_child = d->children->size;
        while (dc < no_child) {
            if ( !strcmp(names[dc], "Customer") ) {
                d->children->memories[dc] = (long int *)(*iter);
                break;
            }
            dc++;
        }
    } else if( !strcmp(table_name, "Customer") ) {
        Customer *any_dstr = (Customer *)*d->mem;
        dc = 0;
        names = d->children->dsNames;
        no_child = d->children->size;
        while (dc < no_child) {
            if ( !strcmp(names[dc], "Position") ) {
                d->children->memories[dc] = (long int *)any_dstr->get_pos();
                break;
            }
            dc++;
        }
    } else if( !strcmp(table_name, "Position") ) {
        ;
    }
}


int get_datastructure_size(void *st){
    stlTable *stl = (stlTable *)st;
    if( !strcmp(stl->zName, "Trucks") ){
        data *d = (data *)stl->data;
        vector<Truck*> *any_dstr = (vector<Truck*> *)d->mem;
        return ((int)any_dstr->size());
    }
    if( !strcmp(stl->zName, "Customers") ){
        data *d = (data *)stl->data;
        vector<Customer*> *any_dstr = (vector<Customer*> *)*d->mem;
        return ((int)any_dstr->size());
    }
    return 1;
}


void realloc_resultset(void *stc) {
    sqlite3_vtab_cursor *cur = (sqlite3_vtab_cursor *)stc;
    stlTable *stl = (stlTable *)cur->pVtab;
    stlTableCursor *stcsr = (stlTableCursor *)stc;
    int arraySize;
    int *res;
    arraySize = get_datastructure_size((void *)stl);
    if ( arraySize != stcsr->max_size ){
        res = (int *)sqlite3_realloc(stcsr->resultSet, sizeof(int) * arraySize);
        if (res!=NULL){
            stcsr->resultSet = res;
            memset(stcsr->resultSet, -1,
                   sizeof(int) * arraySize);
            stcsr->max_size = arraySize;
#ifdef DEBUGGING
            printf("\nReallocating resultSet..now max size %i \n\n", stcsr->max_size);
#endif
        }else{
            free(res);
            printf("Error (re)allocating memory\n");
            exit(1);
        }
    }
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


void Trucks_search(void *stc, char *constr, sqlite3_value *val){
    sqlite3_vtab_cursor *cur = (sqlite3_vtab_cursor *)stc;
    stlTable *stl = (stlTable *)cur->pVtab;
    stlTableCursor *stcsr = (stlTableCursor *)stc;
    data *d = (data *)stl->data;
    vector<Truck*> *any_dstr = (vector<Truck*> *)d->mem;
    vector<Truck*>:: iterator iter;
    int op, iCol, count = 0, i = 0;
    if ( val==NULL ){
        for (int j=0; j<get_datastructure_size((void *)stl); j++){
            stcsr->resultSet[j] = j;
            stcsr->size++;
	}
        assert(stcsr->size <= stcsr->max_size);
        assert(&stcsr->resultSet[stcsr->size] <= &stcsr->resultSet[stcsr->max_size]);
    }else{
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
        int *temp_res;
	temp_res = (int *)sqlite3_malloc(sizeof(int)  * stcsr->max_size);
        if ( !temp_res ){
            printf("Error in allocating memory\n");
            exit(1);
        }
        switch( iCol ){
        case 0:
            iter = any_dstr->begin();
            for(int i=0; i<(int)any_dstr->size();i++){
                temp_res[count++] = i;
                iter++;
            }
            assert(count <= stcsr->max_size);
            break;
        }
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
        sqlite3_free(temp_res);
    }
}


void Truck_search(void *stc, char *constr, sqlite3_value *val){
    sqlite3_vtab_cursor *cur = (sqlite3_vtab_cursor *)stc;
    stlTable *stl = (stlTable *)cur->pVtab;
    stlTableCursor *stcsr = (stlTableCursor *)stc;
    data *d = (data *)stl->data;
    Truck *any_dstr = (Truck *)*d->mem;
    int op, iCol, count = 0, i = 0;
    if ( val==NULL ){
        stcsr->resultSet[count++] = i;
        stcsr->size++;
    }else{
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
        int *temp_res;
	temp_res = (int *)sqlite3_malloc(sizeof(int)  * stcsr->max_size);
        if ( !temp_res ){
            printf("Error in allocating memory\n");
            exit(1);
        }
        switch( iCol ){
        case 0:
            temp_res[count++] = i;
            assert(count <= stcsr->max_size);
            break;
        case 1:
            if (compare(any_dstr->get_cost(), op, sqlite3_value_double(val)) )
                temp_res[count++] = i;
            assert(count <= stcsr->max_size);
            break;
        case 2:
            if (compare(any_dstr->get_delcapacity(), op, sqlite3_value_int(val)) )
                temp_res[count++] = i;
            assert(count <= stcsr->max_size);
            break;
        case 3:
            if (compare(any_dstr->get_pickcapacity(), op, sqlite3_value_int(val)) )
                temp_res[count++] = i;
            assert(count <= stcsr->max_size);
            break;
        case 4:
            if (compare(any_dstr->get_rlpoint(), op, sqlite3_value_int(val)) )
                temp_res[count++] = i;
            assert(count <= stcsr->max_size);
            break;
        }
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
        sqlite3_free(temp_res);
    }
}


void Customers_search(void *stc, char *constr, sqlite3_value *val){
    sqlite3_vtab_cursor *cur = (sqlite3_vtab_cursor *)stc;
    stlTable *stl = (stlTable *)cur->pVtab;
    stlTableCursor *stcsr = (stlTableCursor *)stc;
    data *d = (data *)stl->data;
    vector<Customer*> *any_dstr = (vector<Customer*> *)*d->mem;
    vector<Customer*>:: iterator iter;
    int op, iCol, count = 0, i = 0;
    realloc_resultset(stc);
    if ( val==NULL ){
        for (int j=0; j<get_datastructure_size((void *)stl); j++){
            stcsr->resultSet[j] = j;
            stcsr->size++;
	}
        assert(stcsr->size <= stcsr->max_size);
        assert(&stcsr->resultSet[stcsr->size] <= &stcsr->resultSet[stcsr->max_size]);
    }else{
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
        int *temp_res;
	temp_res = (int *)sqlite3_malloc(sizeof(int)  * stcsr->max_size);
        if ( !temp_res ){
            printf("Error in allocating memory\n");
            exit(1);
        }
        switch( iCol ){
        case 0:
            iter = any_dstr->begin();
            for(int i=0; i<(int)any_dstr->size();i++){
                temp_res[count++] = i;
                iter++;
            }
            assert(count <= stcsr->max_size);
            break;
        case 1:
            iter = any_dstr->begin();
            for(int i=0; i<(int)any_dstr->size();i++){
                temp_res[count++] = i;
                iter++;
            }
            assert(count <= stcsr->max_size);
            break;
        }
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
        sqlite3_free(temp_res);
    }
}


void Customer_search(void *stc, char *constr, sqlite3_value *val){
    sqlite3_vtab_cursor *cur = (sqlite3_vtab_cursor *)stc;
    stlTable *stl = (stlTable *)cur->pVtab;
    stlTableCursor *stcsr = (stlTableCursor *)stc;
    data *d = (data *)stl->data;
    Customer *any_dstr = (Customer *)*d->mem;
    int op, iCol, count = 0, i = 0;
    if ( val==NULL ){
        stcsr->resultSet[count++] = i;
        stcsr->size++;
    }else{
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
        int *temp_res;
	temp_res = (int *)sqlite3_malloc(sizeof(int)  * stcsr->max_size);
        if ( !temp_res ){
            printf("Error in allocating memory\n");
            exit(1);
        }
        switch( iCol ){
        case 0:
            temp_res[count++] = i;
            assert(count <= stcsr->max_size);
            break;
        case 1:
            if (compare(any_dstr->get_demand(), op, sqlite3_value_int(val)) )
                temp_res[count++] = i;
            assert(count <= stcsr->max_size);
            break;
        case 2:
            if (compare((const unsigned char *)any_dstr->get_code().c_str(), op, sqlite3_value_text(val)) )
                temp_res[count++] = i;
            assert(count <= stcsr->max_size);
            break;
        case 3:
            if (compare(any_dstr->get_serviced(), op, sqlite3_value_int(val)) )
                temp_res[count++] = i;
            assert(count <= stcsr->max_size);
            break;
        case 4:
            if (compare(any_dstr->get_pickdemand(), op, sqlite3_value_int(val)) )
                temp_res[count++] = i;
            assert(count <= stcsr->max_size);
            break;
        case 5:
            if (compare(any_dstr->get_starttime(), op, sqlite3_value_int(val)) )
                temp_res[count++] = i;
            assert(count <= stcsr->max_size);
            break;
        case 6:
            if (compare(any_dstr->get_servicetime(), op, sqlite3_value_int(val)) )
                temp_res[count++] = i;
            assert(count <= stcsr->max_size);
            break;
        case 7:
            if (compare(any_dstr->get_finishtime(), op, sqlite3_value_int(val)) )
                temp_res[count++] = i;
            assert(count <= stcsr->max_size);
            break;
        case 8:
            if (compare(any_dstr->get_revenue(), op, sqlite3_value_int(val)) )
                temp_res[count++] = i;
            assert(count <= stcsr->max_size);
            break;
        case 9:
            temp_res[count++] = i;
            assert(count <= stcsr->max_size);
            break;
        }
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
        sqlite3_free(temp_res);
    }
}


void Position_search(void *stc, char *constr, sqlite3_value *val){
    sqlite3_vtab_cursor *cur = (sqlite3_vtab_cursor *)stc;
    stlTable *stl = (stlTable *)cur->pVtab;
    stlTableCursor *stcsr = (stlTableCursor *)stc;
    data *d = (data *)stl->data;
    Position *any_dstr = (Position *)*d->mem;
    int op, iCol, count = 0, i = 0;
    if ( val==NULL ){
        stcsr->resultSet[count++] = i;
        stcsr->size++;
    }else{
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
        int *temp_res;
	temp_res = (int *)sqlite3_malloc(sizeof(int)  * stcsr->max_size);
        if ( !temp_res ){
            printf("Error in allocating memory\n");
            exit(1);
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
        sqlite3_free(temp_res);
    }
}


void search(void* stc, char *constr, sqlite3_value *val){
    sqlite3_vtab_cursor *cur = (sqlite3_vtab_cursor *)stc;
    stlTable *stl = (stlTable *)cur->pVtab;
    if( !strcmp(stl->zName, "Trucks") )
        Trucks_search(stc, constr, val);
    if( !strcmp(stl->zName, "Truck") )
        Truck_search(stc, constr, val);
    if( !strcmp(stl->zName, "Customers") )
        Customers_search(stc, constr, val);
    if( !strcmp(stl->zName, "Customer") )
        Customer_search(stc, constr, val);
    if( !strcmp(stl->zName, "Position") )
        Position_search(stc, constr, val);
}


int Trucks_retrieve(void *stc, int n, sqlite3_context *con){
    sqlite3_vtab_cursor *cur = (sqlite3_vtab_cursor *)stc;
    stlTable *stl = (stlTable *)cur->pVtab;
    stlTableCursor *stcsr = (stlTableCursor *)stc;
    data *d = (data *)stl->data;
    vector<Truck*> *any_dstr = (vector<Truck*> *)d->mem;
    vector<Truck*>:: iterator iter;
    int index = stcsr->current;
    iter = any_dstr->begin();
    for(int i=0; i<stcsr->resultSet[index]; i++){
        iter++;
    }
    char *colName = stl->azColumn[n];
    switch( n ){
    case 0:
        sqlite3_result_text(con, "N/A", -1, SQLITE_STATIC);
        break;
    }
    return SQLITE_OK;
}


int Truck_retrieve(void *stc, int n, sqlite3_context *con){
    sqlite3_vtab_cursor *cur = (sqlite3_vtab_cursor *)stc;
    stlTable *stl = (stlTable *)cur->pVtab;
    stlTableCursor *stcsr = (stlTableCursor *)stc;
    data *d = (data *)stl->data;
    Truck *any_dstr = (Truck *)*d->mem;
    char *colName = stl->azColumn[n];
    switch( n ){
    case 0:
        sqlite3_result_text(con, "N/A", -1, SQLITE_STATIC);
        break;
    case 1:
        sqlite3_result_double(con, any_dstr->get_cost());
        break;
    case 2:
        sqlite3_result_int(con, any_dstr->get_delcapacity());
        break;
    case 3:
        sqlite3_result_int(con, any_dstr->get_pickcapacity());
        break;
    case 4:
        sqlite3_result_int(con, any_dstr->get_rlpoint());
        break;
    }
    return SQLITE_OK;
}


int Customers_retrieve(void *stc, int n, sqlite3_context *con){
    sqlite3_vtab_cursor *cur = (sqlite3_vtab_cursor *)stc;
    stlTable *stl = (stlTable *)cur->pVtab;
    stlTableCursor *stcsr = (stlTableCursor *)stc;
    data *d = (data *)stl->data;
    vector<Customer*> *any_dstr = (vector<Customer*> *)*d->mem;
    vector<Customer*>:: iterator iter;
    int index = stcsr->current;
    iter = any_dstr->begin();
    for(int i=0; i<stcsr->resultSet[index]; i++){
        iter++;
    }
    char *colName = stl->azColumn[n];
    switch( n ){
    case 0:
        sqlite3_result_text(con, "N/A", -1, SQLITE_STATIC);
        break;
    case 1:
        sqlite3_result_text(con, "N/A", -1, SQLITE_STATIC);
        break;
    }
    return SQLITE_OK;
}


int Customer_retrieve(void *stc, int n, sqlite3_context *con){
    sqlite3_vtab_cursor *cur = (sqlite3_vtab_cursor *)stc;
    stlTable *stl = (stlTable *)cur->pVtab;
    stlTableCursor *stcsr = (stlTableCursor *)stc;
    data *d = (data *)stl->data;
    Customer *any_dstr = (Customer *)*d->mem;
    char *colName = stl->azColumn[n];
    switch( n ){
    case 0:
        sqlite3_result_text(con, "N/A", -1, SQLITE_STATIC);
        break;
    case 1:
        sqlite3_result_int(con, any_dstr->get_demand());
        break;
    case 2:
        sqlite3_result_text(con, (const char *)any_dstr->get_code().c_str(), -1, SQLITE_STATIC);
        break;
    case 3:
        sqlite3_result_int(con, any_dstr->get_serviced());
        break;
    case 4:
        sqlite3_result_int(con, any_dstr->get_pickdemand());
        break;
    case 5:
        sqlite3_result_int(con, any_dstr->get_starttime());
        break;
    case 6:
        sqlite3_result_int(con, any_dstr->get_servicetime());
        break;
    case 7:
        sqlite3_result_int(con, any_dstr->get_finishtime());
        break;
    case 8:
        sqlite3_result_int(con, any_dstr->get_revenue());
        break;
    case 9:
        sqlite3_result_text(con, "N/A", -1, SQLITE_STATIC);
        break;
    }
    return SQLITE_OK;
}


int Position_retrieve(void *stc, int n, sqlite3_context *con){
    sqlite3_vtab_cursor *cur = (sqlite3_vtab_cursor *)stc;
    stlTable *stl = (stlTable *)cur->pVtab;
    stlTableCursor *stcsr = (stlTableCursor *)stc;
    data *d = (data *)stl->data;
    Position *any_dstr = (Position *)*d->mem;
    char *colName = stl->azColumn[n];
    switch( n ){
    case 0:
        sqlite3_result_text(con, "N/A", -1, SQLITE_STATIC);
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


int retrieve(void* stc, int n, sqlite3_context *con){
    sqlite3_vtab_cursor *cur = (sqlite3_vtab_cursor *)stc;
    stlTable *stl = (stlTable *)cur->pVtab;
    if( !strcmp(stl->zName, "Trucks") )
        return Trucks_retrieve(stc, n, con);
    if( !strcmp(stl->zName, "Truck") )
        return Truck_retrieve(stc, n, con);
    if( !strcmp(stl->zName, "Customers") )
        return Customers_retrieve(stc, n, con);
    if( !strcmp(stl->zName, "Customer") )
        return Customer_retrieve(stc, n, con);
    if( !strcmp(stl->zName, "Position") )
        return Position_retrieve(stc, n, con);
}
