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
    int c = 0, ch, dc, ch_size;
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
        ch = 0;
        dc = 0;
        while (dc < dsC_size) {
            if ( !strcmp(tmp_data[dc]->attr->dsName, "Truck") )
                break;
            dc++;
        }
	children[ch] = tmp_data[dc]->attr;
	curr_child = children[ch];
	curr_child->set_memory = (int *)&curr_child[1];
        ch++;
        assert (ch == ch_size);
    } else if( !strcmp(table_name, "Truck") ) {
	stl->data = curr_data;
        curr_data->children = (attrCarrier **)sqlite3_malloc((sizeof(attrCarrier *) + sizeof(int)) * 1);
	children = curr_data->children;
        curr_data->children_size = 1;
	ch_size = curr_data->children_size;
        ch = 0;
        dc = 0;
        while (dc < dsC_size) {
            if ( !strcmp(tmp_data[dc]->attr->dsName, "Customers") )
                break;
            dc++;
        }
	children[ch] = tmp_data[dc]->attr;
	curr_child = children[ch];
	curr_child->set_memory = (int *)&curr_child[1];
        ch++;
        assert (ch == ch_size);
    } else if( !strcmp(table_name, "Customers") ) {
	stl->data = curr_data;
        curr_data->children = (attrCarrier **)sqlite3_malloc((sizeof(attrCarrier *) + sizeof(int)) * 1);
	children = curr_data->children;
        curr_data->children_size = 1;
	ch_size = curr_data->children_size;
        ch = 0;
        dc = 0;
        while (dc < dsC_size) {
            if ( !strcmp(tmp_data[dc]->attr->dsName, "Customer") )
                break;
            dc++;
        }
	children[ch] = tmp_data[dc]->attr;
	curr_child = children[ch];
	curr_child->set_memory = (int *)&curr_child[1];
        ch++;
        assert (ch == ch_size);
    } else if( !strcmp(table_name, "Customer") ) {
	stl->data = curr_data;
        curr_data->children = (attrCarrier **)sqlite3_malloc((sizeof(attrCarrier *) + sizeof(int)) * 1);
	children = curr_data->children;
        curr_data->children_size = 1;
	ch_size = curr_data->children_size;
        ch = 0;
        dc = 0;
        while (dc < dsC_size) {
            if ( !strcmp(tmp_data[dc]->attr->dsName, "Position") )
                break;
            dc++;
        }
	children[ch] = tmp_data[dc]->attr;
	curr_child = children[ch];
	curr_child->set_memory = (int *)&curr_child[1];
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
        ch = 0;
        dc = 0;
        while (dc < dsC_size) {
            if ( !strcmp(tmp_data[dc]->attr->dsName, "Customer") )
                break;
            dc++;
        }
	children[ch] = tmp_data[dc]->attr;
	curr_child = children[ch];
	curr_child->set_memory = (int *)&curr_child[1];
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
    dsArray *dsC = *ddsC;
    dsArray *tmp_dsC;
    dsData **tmp_data, **dsC_data;
    int x_size, nByte, size, i, c;
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
	free(tmp_dsC);
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


int fill_check_dependencies(sqlite3_vtab *pVtab) {
    stlTable *stl = (stlTable *)pVtab;
    const char *table_name = stl->zName;
    dsData *d = (dsData *)stl->data;
    attrCarrier **children, *curr_attr = d->attr, *child_attr;
    int c = 0, ch, ch_size;
    
    if( !strcmp(table_name, "Trucks") ) {
	children = d->children;
	ch_size = d->children_size;

        vector<Truck*> *any_dstr = (vector<Truck*> *)curr_attr->memory;
        vector<Truck*>::iterator iter;
        iter = any_dstr->begin();
        ch = 0;
        while (ch < ch_size) {
            if ( !strcmp(children[ch]->dsName, "Truck") )
                break;
            ch++;
        }
	child_attr = children[ch];
	child_attr->memory = (long int *)(*iter);
	*child_attr->set_memory = 1;

        ch++;
        assert (ch == ch_size);
    } else if( !strcmp(table_name, "Truck") ) {
        if ( *curr_attr->set_memory == 0 )
            return SQLITE_MISUSE;
        *curr_attr->set_memory = 0;
	children = d->children;
        d->children_size = 1;
	ch_size = d->children_size;
        Truck *any_dstr = (Truck *)curr_attr->memory;
        ch = 0;
        while (ch < ch_size) {
            if ( !strcmp(children[ch]->dsName, "Customers") )
                break;
            ch++;
        }
	child_attr = children[ch];
	child_attr->memory = (long int *)any_dstr->get_Customers();
	*child_attr->set_memory = 1;

        ch++;
        assert (ch == ch_size);
    } else if( !strcmp(table_name, "Customers") ) {
        if ( *curr_attr->set_memory == 0 )
            return SQLITE_MISUSE;
        *curr_attr->set_memory = 0;
	children = d->children;
        d->children_size = 1;
	ch_size = d->children_size;
        vector<Customer*> *any_dstr = (vector<Customer*> *)curr_attr->memory;
        vector<Customer*>::iterator iter;
        iter = any_dstr->begin();

        ch = 0;
        while (ch < ch_size) {
            if ( !strcmp(children[ch]->dsName, "Customer") )
                break;
            ch++;
        }
	child_attr = children[ch];
	child_attr->memory = (long int *)(*iter);
	*child_attr->set_memory = 1;
        ch++;
        assert (ch == ch_size);
    } else if( !strcmp(table_name, "Customer") ) {
        if ( *curr_attr->set_memory == 0 )
            return SQLITE_MISUSE;
        *curr_attr->set_memory = 0;
	children = d->children;
        d->children_size = 1;
	ch_size = d->children_size;
        Customer *any_dstr = (Customer *)curr_attr->memory;
        ch = 0;
        while (ch < ch_size) {
            if ( !strcmp(children[ch]->dsName, "Position") )
                break;
            ch++;
        }
	child_attr = children[ch];
	child_attr->memory = (long int *)any_dstr;
	*child_attr->set_memory = 1;

        ch++;
        assert (ch == ch_size);
    } else if( !strcmp(table_name, "Position") ) {
        if ( *curr_attr->set_memory == 0 )
            return SQLITE_MISUSE;
        *curr_attr->set_memory = 0;
    } else if( !strcmp(table_name, "MapIndex") ) {
	children = d->children;
        d->children_size = 1;
	ch_size = d->children_size;
        map<int,Customer*> *any_dstr = (map<int,Customer*> *)curr_attr->memory;
        map<int,Customer*>::iterator iter;
        iter = any_dstr->begin();
        ch = 0;
        while (ch < ch_size) {
            if ( !strcmp(children[ch]->dsName, "Customer") )
                break;
            ch++;
        }
	child_attr = children[ch];
	child_attr->memory = (long int *)(*iter).second;
	*child_attr->set_memory = 1;

        ch++;
        assert (ch == ch_size);
    }
    return SQLITE_OK;
}


int update_structures(sqlite3_vtab_cursor *cur) {
    stlTable *stl = (stlTable *)cur->pVtab;
    const char *table_name = stl->zName;
    dsData *d = (dsData *)stl->data;
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    attrCarrier **children, *curr_attr = d->attr, *child_attr;
    int ch, index, ch_size;
    if( !strcmp(table_name, "Trucks") ) {
        index = stcsr->current;
        vector<Truck*> *any_dstr = (vector<Truck*> *)curr_attr->memory;
        vector<Truck*>::iterator iter;
        iter = any_dstr->begin();
        for(int i=0; i<stcsr->resultSet[index]; i++){
            iter++;
        }
        ch = 0;
        children = d->children;
        ch_size = d->children_size;
        while (ch < ch_size) {
	    child_attr = children[ch];
            if ( !strcmp(child_attr->dsName, "Truck") ) {
                child_attr->memory = (long int *)(*iter);
                *child_attr->set_memory = 1;
                break;
            }
            ch++;
        }
    } else if( !strcmp(table_name, "Truck") ) {
        index = stcsr->current;
        if ( (index == 0) && (*curr_attr->set_memory == 0) )
            return SQLITE_MISUSE;
        *curr_attr->set_memory = 0;
        Truck *any_dstr = (Truck *)curr_attr->memory;
        ch = 0;
        children = d->children;
        ch_size = d->children_size;
        while (ch < ch_size) {
	    child_attr = children[ch];
            if ( !strcmp(child_attr->dsName, "Customers") ) {
                child_attr->memory = (long int *)any_dstr->get_Customers();
                *child_attr->set_memory = 1;
                break;
            }
            ch++;
        }
    } else if( !strcmp(table_name, "Customers") ) {
        index = stcsr->current;
        if ( (index == 0) && (*curr_attr->set_memory == 0) )
            return SQLITE_MISUSE;
        *curr_attr->set_memory = 0;
        vector<Customer*> *any_dstr = (vector<Customer*> *)curr_attr->memory;
        vector<Customer*>::iterator iter;
        iter = any_dstr->begin();
        for(int i=0; i<stcsr->resultSet[index]; i++){
            iter++;
        }
        ch = 0;
        children = d->children;
        ch_size = d->children_size;
        while (ch < ch_size) {
	    child_attr = children[ch];
            if ( !strcmp(child_attr->dsName, "Customer") ) {
                child_attr->memory = (long int *)(*iter);
                *child_attr->set_memory = 1;
                break;
            }
            ch++;
        }
    } else if( !strcmp(table_name, "Customer") ) {
        index = stcsr->current;
        if ( (index == 0) && (*curr_attr->set_memory == 0) )
            return SQLITE_MISUSE;
        *curr_attr->set_memory = 0;
        Customer *any_dstr = (Customer *)curr_attr->memory;
        ch = 0;
        children = d->children;
        ch_size = d->children_size;
        while (ch < ch_size) {
	    child_attr = children[ch];
            if ( !strcmp(child_attr->dsName, "Position") ) {
                child_attr->memory = (long int *)any_dstr->get_pos();
                *child_attr->set_memory = 1;
                break;
            }
            ch++;
        }
    } else if( !strcmp(table_name, "Position") ) {
        index = stcsr->current;
        if ( (index == 0) && (*curr_attr->set_memory == 0) )
            return SQLITE_MISUSE;
        *curr_attr->set_memory = 0;
    } else if( !strcmp(table_name, "MapIndex") ) {
        index = stcsr->current;
        map<int,Customer*> *any_dstr = (map<int,Customer*> *)curr_attr->memory;
        map<int,Customer*>::iterator iter;
        iter = any_dstr->begin();
        for(int i=0; i<stcsr->resultSet[index]; i++){
            iter++;
        }
        ch = 0;
        children = d->children;
        ch_size = d->children_size;
        while (ch < ch_size) {
	    child_attr = children[ch];
            if ( !strcmp(child_attr->dsName, "Customer") ) {
                child_attr->memory = (long int *)(*iter).second;
                *child_attr->set_memory = 1;
                break;
            }
            ch++;
        }
    }
    return SQLITE_OK;
}

// Update structures is called before is_eof so an update of mem happens 
// but is not implemented (correctly), i.e. sqlite3 terminates query 
// execution (the end of the resultset).
// This function unsets the last assignment.
void unset_mem(sqlite3_vtab_cursor *cur) {
    stlTable *stl=(stlTable *)cur->pVtab;
    dsData *d = (dsData *)stl->data;
    attrCarrier **children = d->children, *curr_attr = d->attr;
    if ( curr_attr->set_memory != NULL )
	*curr_attr->set_memory = 0;
    int ch_size = d->children_size, i = 0;
    for (i = 0; i < ch_size; i++) {
	*children[i]->set_memory = 0;
    }
}


int get_datastructure_size(sqlite3_vtab *pVtab){
    stlTable *stl = (stlTable *)pVtab;
    if( !strcmp(stl->zName, "Trucks") ){
        dsData *d = (dsData *)stl->data;
        vector<Truck*> *any_dstr = (vector<Truck*> *)d->attr->memory;
        return ((int)any_dstr->size());
    }
    if( !strcmp(stl->zName, "Customers") ){
        dsData *d = (dsData *)stl->data;
        vector<Customer*> *any_dstr = (vector<Customer*> *)d->attr->memory;
        return ((int)any_dstr->size());
    }
    if( !strcmp(stl->zName, "MapIndex") ){
        dsData *d = (dsData *)stl->data;
        map<int,Customer*> *any_dstr = (map<int,Customer*> *)d->attr->memory;
        return ((int)any_dstr->size());
    }
    return 1;
}


int realloc_resultset(sqlite3_vtab_cursor *cur) {
    stlTable *stl = (stlTable *)cur->pVtab;
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    int arraySize;
    int *res;
    arraySize = get_datastructure_size(cur->pVtab);
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
            free(i_res);
            printf("Error (re)allocating memory\n");
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


int Trucks_search(sqlite3_vtab_cursor *cur, char *constr, sqlite3_value *val){
    stlTable *stl = (stlTable *)cur->pVtab;
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    dsData *d = (dsData *)stl->data;
    vector<Truck*> *any_dstr = (vector<Truck*> *)d->attr->memory;
    vector<Truck*>:: iterator iter;
    int op, iCol, count = 0, i = 0, re = 0;
    int *temp_res;
    if ( val==NULL ){
        for (int j=0; j<get_datastructure_size(cur->pVtab); j++){
            stcsr->resultSet[j] = j;
            stcsr->size++;
	}
        assert(stcsr->size <= stcsr->max_size);
        assert(&stcsr->resultSet[stcsr->size] <= &stcsr->resultSet[stcsr->max_size]);
    }else{
	check_alloc((const char *)constr, op, iCol);
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
    int *temp_res;
    if ( val==NULL ){
        stcsr->resultSet[count++] = i;
        stcsr->size++;
    }else{
	check_alloc((const char *)constr, op, iCol);
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
    int *temp_res;
    if ( (re = realloc_resultset(cur)) != 0 )
	return re;
    if ( val==NULL ){
        for (int j=0; j<get_datastructure_size(cur->pVtab); j++){
            stcsr->resultSet[j] = j;
            stcsr->size++;
	}
        assert(stcsr->size <= stcsr->max_size);
        assert(&stcsr->resultSet[stcsr->size] <= &stcsr->resultSet[stcsr->max_size]);
    }else{
	check_alloc((const char *)constr, op, iCol);
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
    int *temp_res;
    if ( val==NULL ){
        stcsr->resultSet[count++] = i;
        stcsr->size++;
    }else{
	check_alloc((const char *)constr, op, iCol);
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
    int *temp_res;
    if ( val==NULL ){
        stcsr->resultSet[count++] = i;
        stcsr->size++;
    }else{
	check_alloc((const char *)constr, op, iCol);
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
    int *temp_res;
    if ( val==NULL ){
        for (int j=0; j<get_datastructure_size(cur->pVtab); j++){
            stcsr->resultSet[j] = j;
            stcsr->size++;
	}
        assert(stcsr->size <= stcsr->max_size);
        assert(&stcsr->resultSet[stcsr->size] <= &stcsr->resultSet[stcsr->max_size]);
    }else{
	check_alloc((const char *)constr, op, iCol);
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
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    stlTable *stl = (stlTable *)cur->pVtab;
    dsData *d = (dsData *)stl->data;
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
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    dsData *d = (dsData *)stl->data;
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
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    dsData *d = (dsData *)stl->data;
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


int retrieve(sqlite3_vtab_cursor* cur, int n, sqlite3_context *con){
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
