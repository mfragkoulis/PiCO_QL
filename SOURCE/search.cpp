#include "search.h"
#include <string>
#include <assert.h>
#include <stdio.h>
#include <string>
#include <vector>

#include "Truck.h"
#include "Customer.h"

using namespace std;


/*
#define DEBUGGING
*/

int set_dependencies(sqlite3_vtab *pVtab, dsArray *dsC, const char *table_name, char **pzErr) {
    stlTable *stl = (stlTable *)pVtab;
    dsData **tmp_data = dsC->ds, *curr_data;
    attrCarrier **children, *curr_child, *curr_attr;
    int c = 0, ch, dc, ch_size, *index;
    int dsC_size = dsC->ds_size;
    while (c < dsC_size) {
        if ( !strcmp(table_name, tmp_data[c]->attr->dsName) )
            break;
        c++;
    }
    curr_data = tmp_data[c];
    curr_attr = curr_data->attr;
    if( !strcmp(table_name, "Trucks") ) {
        stl->data = curr_data;
        curr_data->children = (attrCarrier **)sqlite3_malloc((sizeof(attrCarrier *) + sizeof(int)) * 1);
        children = curr_data->children;
        curr_data->children_size = 1;
        ch_size = curr_data->children_size;
        index = (int *)&children[ch_size];
        ch = 0;
        dc = 0;
        while (dc < dsC_size) {
            if ( !strcmp(tmp_data[dc]->attr->dsName, "Truck") )
                break;
            dc++;
        }
        children[ch] = tmp_data[dc]->attr;
        curr_child = children[ch];
        curr_child->set_memory = (int *)&index[ch];
        ch++;
        assert (ch == ch_size);
    } else if( !strcmp(table_name, "Truck") ) {
        stl->data = curr_data;
        curr_data->children = (attrCarrier **)sqlite3_malloc((sizeof(attrCarrier *) + sizeof(int)) * 1);
        children = curr_data->children;
        curr_data->children_size = 1;
        ch_size = curr_data->children_size;
        index = (int *)&children[ch_size];
        ch = 0;
        dc = 0;
        while (dc < dsC_size) {
            if ( !strcmp(tmp_data[dc]->attr->dsName, "Customers") )
                break;
            dc++;
        }
        children[ch] = tmp_data[dc]->attr;
        curr_child = children[ch];
        curr_child->set_memory = (int *)&index[ch];
        ch++;
        assert (ch == ch_size);
    } else if( !strcmp(table_name, "Customers") ) {
        stl->data = curr_data;
        curr_data->children = (attrCarrier **)sqlite3_malloc((sizeof(attrCarrier *) + sizeof(int)) * 1);
        children = curr_data->children;
        curr_data->children_size = 1;
        ch_size = curr_data->children_size;
        index = (int *)&children[ch_size];
        ch = 0;
        dc = 0;
        while (dc < dsC_size) {
            if ( !strcmp(tmp_data[dc]->attr->dsName, "Customer") )
                break;
            dc++;
        }
        children[ch] = tmp_data[dc]->attr;
        curr_child = children[ch];
        curr_child->set_memory = (int *)&index[ch];
        ch++;
        assert (ch == ch_size);
    } else if( !strcmp(table_name, "Customer") ) {
        stl->data = curr_data;
        curr_data->children = (attrCarrier **)sqlite3_malloc((sizeof(attrCarrier *) + sizeof(int)) * 1);
        children = curr_data->children;
        curr_data->children_size = 1;
        ch_size = curr_data->children_size;
        index = (int *)&children[ch_size];
        ch = 0;
        dc = 0;
        while (dc < dsC_size) {
            if ( !strcmp(tmp_data[dc]->attr->dsName, "Position") )
                break;
            dc++;
        }
        children[ch] = tmp_data[dc]->attr;
        curr_child = children[ch];
        curr_child->set_memory = (int *)&index[ch];
        ch++;
        assert (ch == ch_size);
    } else if( !strcmp(table_name, "Position") ) {
        stl->data = curr_data;
        curr_data->children = NULL;
    } else if( !strcmp(table_name, "MapIndex") ) {
        stl->data = curr_data;
        curr_data->children = (attrCarrier **)sqlite3_malloc((sizeof(attrCarrier *) + sizeof(int)) * 1);
        children = curr_data->children;
        curr_data->children_size = 1;
        ch_size = curr_data->children_size;
        index = (int *)&children[ch_size];
        ch = 0;
        dc = 0;
        while (dc < dsC_size) {
            if ( !strcmp(tmp_data[dc]->attr->dsName, "Customer") )
                break;
            dc++;
        }
        children[ch] = tmp_data[dc]->attr;
        curr_child = children[ch];
        curr_child->set_memory = (int *)&index[ch];
        ch++;
        assert (ch == ch_size);
    }
    return SQLITE_OK;
}


void copy_structs(dsData **ddsC, char **c_temp, const char *name, long int *mem) {
    dsData *dss = *ddsC;
    attrCarrier *attr_dss = dss->attr;
    int len;
    attr_dss->dsName = *c_temp;
    len = (int)strlen(name) + 1;
    memcpy(*c_temp, name, len);
    *c_temp += len;
    if ( mem != NULL ) {
        attr_dss->memory = mem;
        attr_dss->set_memory = NULL;
    }
    *ddsC = dss;
}


int set_carriers(dsArray **ddsC, char **pzErr) {
    dsArray *dsC = *ddsC, *tmp_dsC;
    dsData **tmp_data, **dsC_data;
    int x_size, nByte, size, c, i;
    char *c_temp;
    x_size = dsC->ds_size;
    nByte = sizeof(dsArray) + (sizeof(dsData *) + sizeof(dsData) + sizeof(attrCarrier)) * 6 + 50;
    tmp_dsC = (dsArray *)sqlite3_malloc(nByte);
    if (tmp_dsC != NULL) {
        memset(tmp_dsC, 0, nByte);
        tmp_dsC->init = 0;
        tmp_dsC->ds_size = 6;
        size = tmp_dsC->ds_size;
        tmp_dsC->ds = (dsData **)&tmp_dsC[1];
        tmp_data = tmp_dsC->ds;
        tmp_data[0] = (dsData *)&tmp_data[size];
        for (i = 1; i < size; i++)
            tmp_data[i] = (dsData *)&tmp_data[i-1][1];
        tmp_data[0]->attr = (attrCarrier *)&tmp_data[i-1][1];
        for (i = 1; i < size; i++)
            tmp_data[i]->attr = (attrCarrier *)&tmp_data[i-1]->attr[1];
        c_temp = (char *)&tmp_data[i-1]->attr[1];
        dsC_data = dsC->ds;
        c = 0;
        copy_structs(&tmp_data[c], &c_temp, dsC_data[c]->attr->dsName, dsC_data[c]->attr->memory);
        c++;
        copy_structs(&tmp_data[c], &c_temp, dsC_data[c]->attr->dsName, dsC_data[c]->attr->memory);
        c++;
        copy_structs(&tmp_data[c], &c_temp, "Truck", NULL);
        c++;
        copy_structs(&tmp_data[c], &c_temp, "Customers", NULL);
        c++;
        copy_structs(&tmp_data[c], &c_temp, "Customer", NULL);
        c++;
        copy_structs(&tmp_data[c], &c_temp, "Position", NULL);
        c++;
        assert(c == 6);
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
        sqlite3_free(tmp_dsC);
        *pzErr = sqlite3_mprintf("Error (re)allocating memory\n");
        return SQLITE_NOMEM;
    }
    return SQLITE_OK;
}


int realloc_carrier(sqlite3_vtab *pVtab, void *ds, const char *tablename, char **pzErr) {
    dsArray **ddsC = (dsArray **)ds;
    dsArray *dsC = *ddsC;
    int re;
#ifdef DEBUGGING
    printf("dsC->size: %i\n", dsC->ds_size);
#endif
    if (dsC->init)
        if ( (re = set_carriers(ddsC, pzErr)) != SQLITE_OK )
            return re;
    return set_dependencies(pVtab, *ddsC, tablename, pzErr);
}


int fill_resultset(sqlite3_vtab_cursor *cur) {
    stlTable *stl = (stlTable *)cur->pVtab;
    const char *table_name = stl->zName;
    stlTableCursor *stc = (stlTableCursor *)cur;
    dsData *d = (dsData *)stl->data;
    attrCarrier **parents, *parent_attr, *curr_attr = d->attr;;
    int pr, pr_size, ds_size;
    parents = d->parents;
    pr_size = d->parents_size;
    table_name = stl->zName;
    if ( !strcmp(table_name, "Trucks") ) {
        set<Truck*> *resultset = (set<Truck*> *)stc->resultSet;
        vector<Truck*> *any_dstr0 = (vector<Truck*> *)parent_attr->memory;
        vector<Truck*>::iterator iter0;
        ds_size0 = any_dstr0->size();
        iter0 = any_dstr0->begin();
        for (int i0; i0<ds_size0; i0++;) {
            resultset->insert((*iter0));
            iter0++;
        }
        stc->size = resultset->size();
    } else if ( !strcmp(table_name, "Truck") ) {
        set<Truck*> *resultset = (set<Truck*> *)stc->resultSet;
        pr = 0;
        while (pr < pr_size) {
            if ( !strcmp(parents[pr]->dsName, "Trucks") )
                break;
            pr++;
        }
        parent_attr = parents[pr];
        vector<Truck*> *any_dstr0 = (vector<Truck*> *)parent_attr->memory;
        vector<Truck*>::iterator iter0;
        ds_size0 = any_dstr0->size();
        iter0 = any_dstr0->begin();
        for (int i0; i0<ds_size0; i0++;) {
            resultset->insert((*iter0));
            iter0++;
        }
        stc->size = resultset->size();
    } else if ( !strcmp(table_name, "Customers") ) {
        map<Customer*,Truck*> *resultset = (map<Customer*,Truck*> *)stc->resultSet;
        pr = 0;
        while (pr < pr_size) {
            if ( !strcmp(parents[pr]->dsName, "Trucks") )
                break;
            pr++;
        }
        parent_attr = parents[pr];
        vector<Truck*> *any_dstr0 = (vector<Truck*> *)parent_attr->memory;
        vector<Truck*>::iterator iter0;
        vector<Customer*> *any_dstr1;
        vector<Customer*>::iterator iter1;
        ds_size0 = any_dstr0->size();
        iter0 = any_dstr0->begin();
        for (int i0; i0<ds_size0; i0++;) {
            any_dstr1 = (vector<Customer*> *)(*iter0)->get_Customers();
            ds_size1 = any_dstr1->size();
            iter1 = any_dstr1->begin();
            for (int i1; i1<ds_size1; i1++;) {
                resultset->insert(pair<Customer*,Truck*>((*iter1),*iter0));
                iter1++;
            }
            iter0++;
        }
        stc->size = resultset->size();
    } else if ( !strcmp(table_name, "Customer") ) {
        set<Customer*> *resultset = (set<Customer*> *)stc->resultSet;
        pr = 0;
        while (pr < pr_size) {
            if ( !strcmp(parents[pr]->dsName, "Trucks") )
                break;
            pr++;
        }
        parent_attr = parents[pr];
        vector<Truck*> *any_dstr0 = (vector<Truck*> *)parent_attr->memory;
        vector<Truck*>::iterator iter0;
        vector<Customer*> *any_dstr1;
        vector<Customer*>::iterator iter1;
        ds_size0 = any_dstr0->size();
        iter0 = any_dstr0->begin();
        for (int i0; i0<ds_size0; i0++;) {
            any_dstr1 = (vector<Customer*> *)(*iter0)->get_Customers();
            ds_size1 = any_dstr1->size();
            iter1 = any_dstr1->begin();
            for (int i1; i1<ds_size1; i1++;) {
                resultset->insert((*iter1));
                iter1++;
            }
            iter0++;
        }
        stc->size = resultset->size();
        pr = 0;
        while (pr < pr_size) {
            if ( !strcmp(parents[pr]->dsName, "MapIndex") )
                break;
            pr++;
        }
        parent_attr = parents[pr];
        map<int,Customer*> *any_dstr0 = (map<int,Customer*> *)parent_attr->memory;
        map<int,Customer*>::iterator iter0;
        ds_size0 = any_dstr0->size();
        iter0 = any_dstr0->begin();
        for (int i0; i0<ds_size0; i0++;) {
            resultset->insert((*iter0)->second);
            iter0++;
        }
        stc->size = resultset->size();
    } else if ( !strcmp(table_name, "Position") ) {
        set<Position*> *resultset = (set<Position*> *)stc->resultSet;
        pr = 0;
        while (pr < pr_size) {
            if ( !strcmp(parents[pr]->dsName, "Trucks") )
                break;
            pr++;
        }
        parent_attr = parents[pr];
        vector<Truck*> *any_dstr0 = (vector<Truck*> *)parent_attr->memory;
        vector<Truck*>::iterator iter0;
        vector<Customer*> *any_dstr1;
        vector<Customer*>::iterator iter1;
        ds_size0 = any_dstr0->size();
        iter0 = any_dstr0->begin();
        for (int i0; i0<ds_size0; i0++;) {
            any_dstr1 = (vector<Customer*> *)(*iter0)->get_Customers();
            ds_size1 = any_dstr1->size();
            iter1 = any_dstr1->begin();
            for (int i1; i1<ds_size1; i1++;) {
                resultset->insert((*iter1)->get_pos());
                iter1++;
            }
            iter0++;
        }
        stc->size = resultset->size();
        pr = 0;
        while (pr < pr_size) {
            if ( !strcmp(parents[pr]->dsName, "MapIndex") )
                break;
            pr++;
        }
        parent_attr = parents[pr];
        map<int,Customer*> *any_dstr0 = (map<int,Customer*> *)parent_attr->memory;
        map<int,Customer*>::iterator iter0;
        ds_size0 = any_dstr0->size();
        iter0 = any_dstr0->begin();
        for (int i0; i0<ds_size0; i0++;) {
            resultset->insert((*iter0)->get_pos());
            iter0++;
        }
        stc->size = resultset->size();
    } else if ( !strcmp(table_name, "MapIndex") ) {
        set<pair<int,Customer*>> *resultset = (set<pair<int,Customer*>> *)stc->resultSet;
        pair<int,Customer*> p;
        map<int,Customer*> *any_dstr0 = (map<int,Customer*> *)parent_attr->memory;
        map<int,Customer*>::iterator iter0;
        ds_size0 = any_dstr0->size();
        iter0 = any_dstr0->begin();
        for (int i0; i0<ds_size0; i0++;) {
            resultset->insert((*iter0));
            iter0++;
        }
        stc->size = resultset->size();
    return SQLITE_OK;
}


int Trucks_search(sqlite3_vtab_cursor *cur, char *constr, sqlite3_value *val){
    stlTable *stl = (stlTable *)cur->pVtab;
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    dsData *d = (dsData *)stl->data;
    vector<Truck*> *any_dstr = (vector<Truck*> *)d->attr->memory;
    vector<Truck*>:: iterator iter;
    int op, iCol, count = 0, i = 0, re = 0;
    if ( val==NULL ){
        for (int j=0; j<get_datastructure_size(cur->pVtab); j++){
            stcsr->resultSet[j] = j;
            stcsr->size++;
	}
        assert(stcsr->size <= stcsr->max_size);
        assert(&stcsr->resultSet[stcsr->size] <= &stcsr->resultSet[stcsr->max_size]);
    }else{
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
            for(int i=0; i<(int)any_dstr->size();i++){
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


int Truck_search(sqlite3_vtab_cursor *cur, char *constr, sqlite3_value *val){
    stlTable *stl = (stlTable *)cur->pVtab;
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    dsData *d = (dsData *)stl->data;
    Truck *any_dstr = (Truck *)d->attr->memory;
    int op, iCol, count = 0, i = 0, re = 0;
    if ( val==NULL ){
        stcsr->resultSet[count++] = i;
        stcsr->size++;
    }else{
        check_alloc((const char *)constr, op, iCol);
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
        if ( (re = compare_res(count, stcsr, temp_res)) != 0 )
            return re;
        sqlite3_free(temp_res);
    }
    return SQLITE_OK;
}


int Customers_search(sqlite3_vtab_cursor *cur, char *constr, sqlite3_value *val){
    stlTable *stl = (stlTable *)cur->pVtab;
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    dsData *d = (dsData *)stl->data;
    vector<Customer*> *any_dstr = (vector<Customer*> *)d->attr->memory;
    vector<Customer*>:: iterator iter;
    int op, iCol, count = 0, i = 0, re = 0;
    realloc_resultset(cur);
    if ( val==NULL ){
        for (int j=0; j<get_datastructure_size(cur->pVtab); j++){
            stcsr->resultSet[j] = j;
            stcsr->size++;
	}
        assert(stcsr->size <= stcsr->max_size);
        assert(&stcsr->resultSet[stcsr->size] <= &stcsr->resultSet[stcsr->max_size]);
    }else{
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
        if ( (re = compare_res(count, stcsr, temp_res)) != 0 )
            return re;
        sqlite3_free(temp_res);
    }
    return SQLITE_OK;
}


int Customer_search(sqlite3_vtab_cursor *cur, char *constr, sqlite3_value *val){
    stlTable *stl = (stlTable *)cur->pVtab;
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    dsData *d = (dsData *)stl->data;
    Customer *any_dstr = (Customer *)d->attr->memory;
    int op, iCol, count = 0, i = 0, re = 0;
    if ( val==NULL ){
        stcsr->resultSet[count++] = i;
        stcsr->size++;
    }else{
        check_alloc((const char *)constr, op, iCol);
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
        if ( (re = compare_res(count, stcsr, temp_res)) != 0 )
            return re;
        sqlite3_free(temp_res);
    }
    return SQLITE_OK;
}


int Position_search(sqlite3_vtab_cursor *cur, char *constr, sqlite3_value *val){
    stlTable *stl = (stlTable *)cur->pVtab;
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    dsData *d = (dsData *)stl->data;
    Position *any_dstr = (Position *)d->attr->memory;
    int op, iCol, count = 0, i = 0, re = 0;
    if ( val==NULL ){
        stcsr->resultSet[count++] = i;
        stcsr->size++;
    }else{
        check_alloc((const char *)constr, op, iCol);
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
    dsData *d = (dsData *)stl->data;
    map<int,Customer*> *any_dstr = (map<int,Customer*> *)d->attr->memory;
    map<int,Customer*>:: iterator iter;
    int op, iCol, count = 0, i = 0, re = 0;
    if ( val==NULL ){
        for (int j=0; j<get_datastructure_size(cur->pVtab); j++){
            stcsr->resultSet[j] = j;
            stcsr->size++;
	}
        assert(stcsr->size <= stcsr->max_size);
        assert(&stcsr->resultSet[stcsr->size] <= &stcsr->resultSet[stcsr->max_size]);
    }else{
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
            for(int i=0; i<(int)any_dstr->size();i++){
                if (compare((*iter).first, op, sqlite3_value_int(val)) )
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
    stlTable *stl = (stlTable *)cur->pVtab;
    dsData *d = (dsData *)stl->data;
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    vector<Truck*> *any_dstr = (vector<Truck*> *)d->attr->memory;
    vector<Truck*>:: iterator iter;
    int index = stcsr->current;
    iter = any_dstr->begin();
    for(int i=0; i<stcsr->resultSet[index]; i++){
        iter++;
    }
    switch( n ){
    case 0:
        sqlite3_result_text(con, "N/A", -1, SQLITE_STATIC);
        break;
    }
    return SQLITE_OK;
}


int Truck_retrieve(sqlite3_vtab_cursor *cur, int n, sqlite3_context *con){
    stlTable *stl = (stlTable *)cur->pVtab;
    dsData *d = (dsData *)stl->data;
    Truck *any_dstr = (Truck *)d->attr->memory;
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


int Customers_retrieve(sqlite3_vtab_cursor *cur, int n, sqlite3_context *con){
    stlTable *stl = (stlTable *)cur->pVtab;
    dsData *d = (dsData *)stl->data;
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    vector<Customer*> *any_dstr = (vector<Customer*> *)d->attr->memory;
    vector<Customer*>:: iterator iter;
    int index = stcsr->current;
    iter = any_dstr->begin();
    for(int i=0; i<stcsr->resultSet[index]; i++){
        iter++;
    }
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


int Customer_retrieve(sqlite3_vtab_cursor *cur, int n, sqlite3_context *con){
    stlTable *stl = (stlTable *)cur->pVtab;
    dsData *d = (dsData *)stl->data;
    Customer *any_dstr = (Customer *)d->attr->memory;
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


int Position_retrieve(sqlite3_vtab_cursor *cur, int n, sqlite3_context *con){
    stlTable *stl = (stlTable *)cur->pVtab;
    dsData *d = (dsData *)stl->data;
    Position *any_dstr = (Position *)d->attr->memory;
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


int MapIndex_retrieve(sqlite3_vtab_cursor *cur, int n, sqlite3_context *con){
    stlTable *stl = (stlTable *)cur->pVtab;
    dsData *d = (dsData *)stl->data;
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    map<int,Customer*> *any_dstr = (map<int,Customer*> *)d->attr->memory;
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
        sqlite3_result_text(con, "N/A", -1, SQLITE_STATIC);
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
