#include "search.h"
#include <string>
#include <assert.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <set>
#include <map>

#include "Truck.h"
#include "Customer.h"

using namespace std;


/*
#define DEBUGGING
*/

int set_dependencies(sqlite3_vtab *pVtab, dsArray *dsC, const char *table_name, char **pzErr) {
    stlTable *stl = (stlTable *)pVtab;
    dsData **tmp_data = dsC->ds, *curr_data;
    attrCarrier **parents, *curr_parent, *curr_attr;
    nonNative **nntv;
    int c = 0, pr, dc, pr_size, nn, nntv_size;
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
        curr_data->parents =  NULL;
	curr_data->parents_size = 0;
	curr_data->nntv_size = 0;
	curr_data->nntv = NULL;
    } else if( !strcmp(table_name, "Truck") ) {
        stl->data = curr_data;
        curr_data->parents = (attrCarrier **)sqlite3_malloc(sizeof(attrCarrier *) * 1);
        parents = curr_data->parents;
        curr_data->parents_size = 1;
        pr_size = curr_data->parents_size;
        pr = 0;
        dc = 0;
        while (dc < dsC_size) {
            if ( !strcmp(tmp_data[dc]->attr->dsName, "Trucks") )
                break;
            dc++;
        }
        parents[pr] = tmp_data[dc]->attr;
        curr_parent = parents[pr];
        pr++;
        assert (pr == pr_size);
	curr_data->nntv_size = 0;
	curr_data->nntv = NULL;
    } else if( !strcmp(table_name, "Customers") ) {
        stl->data = curr_data;
        curr_data->parents = (attrCarrier **)sqlite3_malloc(sizeof(attrCarrier *) * 1);
        parents = curr_data->parents;
        curr_data->parents_size = 1;
        pr_size = curr_data->parents_size;
        pr = 0;
        dc = 0;
        while (dc < dsC_size) {
            if ( !strcmp(tmp_data[dc]->attr->dsName, "Trucks") )
                break;
            dc++;
        }
        parents[pr] = tmp_data[dc]->attr;
        curr_parent = parents[pr];
        pr++;
        assert (pr == pr_size);
	curr_data->nntv_size = 1;
	nntv_size = curr_data->nntv_size;
	curr_data->nntv = (nonNative **)sqlite3_malloc((sizeof(nonNative *) + sizeof(nonNative)) * nntv_size);
	nntv = curr_data->nntv;
	nntv[0] = (nonNative *)&nntv[nntv_size];
        nn = 0;
        dc = 0;
        while (dc < dsC_size) {
            if ( !strcmp(tmp_data[dc]->attr->dsName, "Truck") )
                break;
            dc++;
        }
	nntv[nn]->name = tmp_data[dc]->attr->dsName;
	nntv[nn]->active = 0;
	nn++;
	assert (nn = nntv_size);
    } else if( !strcmp(table_name, "Customer") ) {
        stl->data = curr_data;
        curr_data->parents = (attrCarrier **)sqlite3_malloc(sizeof(attrCarrier *) * 2);
        parents = curr_data->parents;
        curr_data->parents_size = 2;
        pr_size = curr_data->parents_size;
        pr = 0;
        dc = 0;
        while (dc < dsC_size) {
            if ( !strcmp(tmp_data[dc]->attr->dsName, "Trucks") )
                break;
            dc++;
        }
        parents[pr] = tmp_data[dc]->attr;
        curr_parent = parents[pr];
        pr++;
        dc = 0;
        while (dc < dsC_size) {
            if ( !strcmp(tmp_data[dc]->attr->dsName, "MapIndex") )
                break;
            dc++;
        }
        parents[pr] = tmp_data[dc]->attr;
        curr_parent = parents[pr];
        pr++;
        assert (pr == pr_size);
	curr_data->nntv_size = 0;
	curr_data->nntv = NULL;
    } else if( !strcmp(table_name, "Position") ) {
        stl->data = curr_data;
        curr_data->parents = (attrCarrier **)sqlite3_malloc(sizeof(attrCarrier *) * 2);
        parents = curr_data->parents;
        curr_data->parents_size = 2;
        pr_size = curr_data->parents_size;
        pr = 0;
        dc = 0;
        while (dc < dsC_size) {
            if ( !strcmp(tmp_data[dc]->attr->dsName, "Trucks") )
                break;
            dc++;
        }
        parents[pr] = tmp_data[dc]->attr;
        curr_parent = parents[pr];
        pr++;
        dc = 0;
        while (dc < dsC_size) {
            if ( !strcmp(tmp_data[dc]->attr->dsName, "MapIndex") )
                break;
            dc++;
        }
        parents[pr] = tmp_data[dc]->attr;
        curr_parent = parents[pr];
        pr++;
        assert (pr == pr_size);
	curr_data->nntv_size = 0;
	curr_data->nntv = NULL;
    } else if( !strcmp(table_name, "MapIndex") ) {
        stl->data = curr_data;
        curr_data->parents = NULL;
	curr_data->parents_size = 0;
	curr_data->nntv_size = 0;
	curr_data->nntv = NULL;
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
    int c = 0, pr, pr_size, ds_size, ds1_size, ds2_size;
    parents = d->parents;
    pr_size = d->parents_size;
    table_name = stl->zName;
    if( !strcmp(table_name, "Trucks") ) {
	set<long int> *resultset = (set<long int> *)stc->resultSet;
	vector<Truck*> *any_dstr = (vector<Truck*> *)curr_attr->memory;
	vector<Truck*>::iterator iter;
	ds_size = any_dstr->size();
	iter = any_dstr->begin();
	for (int i=0; i<ds_size; i++)
	    resultset->insert((long int)*(iter++));
	stc->size = resultset->size();
    } else if( !strcmp(table_name, "Truck") ) {
	set<long int> *resultset = (set<long int> *)stc->resultSet;
        pr = 0;
        while (pr < pr_size) {
            if ( !strcmp(parents[pr]->dsName, "Trucks") )
                break;
            pr++;
        }
        parent_attr = parents[pr];
	vector<Truck*> *any_dstr = (vector<Truck*> *)parent_attr->memory;
	vector<Truck*>::iterator iter;
	ds_size = any_dstr->size();
	iter = any_dstr->begin();
	for (int i=0; i < ds_size; i++)	
	    resultset->insert((long int)*(iter++));
        pr++;
        assert (pr == pr_size);
	stc->size = resultset->size();
    } else if( !strcmp(table_name, "Customers") ) {
	map<long int,long int> *resultset = (map<long int,long int> *)stc->resultSet;
	pr = 0;
	while (pr < pr_size) {
	    if ( !strcmp(parents[pr]->dsName, "Trucks") )
		break;
	    pr++;
	}
	parent_attr = parents[pr];
	vector<Truck*> *any_dstr = (vector<Truck*> *)parent_attr->memory;
	vector<Truck*>::iterator iter;
	vector<Customer*> *any_dstr1;
	vector<Customer*>::iterator iter1;
	ds_size = any_dstr->size();
	iter = any_dstr->begin();
	for (int i=0; i<ds_size; i++) {
	    any_dstr1 = (vector<Customer*> *)(*iter)->get_Customers();
	    ds1_size = any_dstr1->size();
	    iter1 = any_dstr1->begin();
	    for (int i1 = 0; i1<ds1_size; i1++)
		resultset->insert(pair<long int,long int>((long int)*(iter1++),(long int)*iter));
	    iter++;
	}
	pr++;
	assert (pr == pr_size);
	stc->size = resultset->size();
    } else if( !strcmp(table_name, "Customer") ) {
	set<long int> *resultset = (set<long int> *)stc->resultSet;
	pr = 0;
	while (pr < pr_size) {
	    if ( !strcmp(parents[pr]->dsName, "Trucks") )
		break;
	    pr++;
	}
	parent_attr = parents[pr];
	vector<Truck*> *any_dstr = (vector<Truck*> *)parent_attr->memory;
	vector<Truck*>::iterator iter;
	vector<Customer*> *any_dstr1;
	vector<Customer*>::iterator iter1;
	ds_size = any_dstr->size();
	iter = any_dstr->begin();
	for (int i=0; i < ds_size; i++) {
	    any_dstr1 = (vector<Customer*> *)(*iter)->get_Customers();
	    ds1_size = any_dstr1->size();
	    iter1 = any_dstr1->begin();
	    for (int i1 = 0; i1 < ds1_size; i1++)
		resultset->insert((long int)*(iter1++));
	    iter++;
	}
	pr++;
	while (pr < pr_size) {
	    if ( !strcmp(parents[pr]->dsName, "MapIndex") )
		break;
	    pr++;
	}
	parent_attr = parents[pr];
	map<int,Customer*> *any_dstr2 = (map<int,Customer*> *)parent_attr->memory;
	map<int,Customer*>::iterator iter2;
	ds2_size = any_dstr2->size();
	iter2 = any_dstr2->begin();
	for (int i = 0; i<ds2_size; i++)
	    resultset->insert((long int)(*(iter2++)).second);
	pr++;
	assert (pr == pr_size);
	stc->size = resultset->size();
    } else if( !strcmp(table_name, "Position") ) {
	set<long int> *resultset = (set<long int> *)stc->resultSet;
	pr = 0;
	while (pr < pr_size) {
	    if ( !strcmp(parents[pr]->dsName, "Trucks") )
		break;
	    pr++;
	}
	parent_attr = parents[pr];
	vector<Truck*> *any_dstr = (vector<Truck*> *)parent_attr->memory;
	vector<Truck*>::iterator iter;
	vector<Customer*> *any_dstr1;
	vector<Customer*>::iterator iter1;
	ds_size = any_dstr->size();
	iter = any_dstr->begin();
	for (int i=0; i < ds_size; i++) {
	    any_dstr1 = (vector<Customer*> *)(*iter)->get_Customers();
	    ds1_size = any_dstr1->size();
	    iter1 = any_dstr1->begin();
	    for (int i1 = 0; i1<ds1_size; i1++)
		resultset->insert((long int)(*(iter1++))->get_pos());
	    iter++;
	}
	pr++;
	while (pr < pr_size) {
	    if ( !strcmp(parents[pr]->dsName, "MapIndex") )
		break;
	    pr++;
	}
	parent_attr = parents[pr];
	map<int,Customer*> *any_dstr2 = (map<int,Customer*> *)parent_attr->memory;
	map<int,Customer*>::iterator iter2;
	ds2_size = any_dstr2->size();
	iter2 = any_dstr2->begin();
	for (int i = 0; i<ds2_size; i++)
	    resultset->insert((long int)(*(iter2++)).second->get_pos());
	pr++;
	assert (pr == pr_size);
	stc->size = resultset->size();
    } else if( !strcmp(table_name, "MapIndex") ) {
	set<long int> *resultset = (set<long int> *)stc->resultSet;
	map<int,Customer*> *any_dstr = (map<int,Customer*> *)curr_attr->memory;
	map<int,Customer*>::iterator iter;
	ds_size = any_dstr->size();
	iter = any_dstr->begin();
	for (int i = 0; i<ds_size; i++)
	    resultset->insert((long int)&*(iter++));
	stc->size = resultset->size();
    }
    return SQLITE_OK;
}


int bind_res(sqlite3_vtab_cursor *cur) {
// update active
}


void reset_nonNative(sqlite3_vtab_cursor *cur) {
    stlTable *stl=(stlTable *)cur->pVtab;
    dsData *d = (dsData *)stl->data;
    nonNative **nntv = d->nntv;
    int nntv_size = d->nntv_size, i = 0;
    for (i = 0; i < nntv_size; i++) {
        nntv[i]->active = 0;
    }
}


void free_resultset(sqlite3_vtab_cursor *cur) {
    stlTableCursor *stc = (stlTableCursor *)cur;
    stlTable *stl=(stlTable *)cur->pVtab;
    dsData *d = (dsData *)stl->data;
    int nntv_size = d->nntv_size;
    if ( nntv_size == 0 )
	delete((set<long int> *)stc->resultSet);
    else
	delete((map<long int,long int> *)stc->resultSet);
}

/*
int convert_res(sqlite3_vtab_cursor *cur, const char *fk_name) {
    stlTable *stl = (stlTable *)cur->pVtab;
    const char *table_name = stl->zName;
    dsData *d = (dsData *)stl->data;
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    attrCarrier **parents, *curr_attr = d->attr, *child_attr;
    int ch, index, index1, ch_size, col = 0,  match, res_size, conv_size;
    long int r, tr;
    set<long int *> *res, *temp_res;
    if( !strcmp(table_name, "Trucks") ) {
	if ( !strcmp(fk_name, "Truck") )
	    ;
    } else if( !strcmp(table_name, "Truck") ) {
    } else if( !strcmp(table_name, "Customers") ) {
	if ( !strcmp(fk_name, "Truck") ) {
	    res = (set<long int> *)stc->resultset;
	    set<long int>::iterator iter;
	    temp_res = new set;
	    iter = res->begin();
	    res_size = res->size();
	    vector<Customer*> *conv;
	    vector<Customer*>::iterator iter1;
	    for (index = 0; index < res_size; index++) {
		conv = ((Truck *)*iter)->get_Customers();
		conv_size = conv->size();
		iter1 = conv->begin();
		for (index1 = 0; index1 < conv_size; index1++) {
		    temp_res->erase((long int)*iter1);
		    iter1++;
		}
		iter++;
	    }
	if ( !strcmp(fk_name, "Customer") )
	    ;
	}
	stc->resultset = &temp_res;
	delete(res);
    } else if( !strcmp(table_name, "Customer") ) {
	if ( !strcmp(fk_name, "Position") ) {
	    res = (set<long int *> *)stc->resultset;
	    res_size = res->size();
	    fill_resultset(cur, temp_res, NULL);
	    temp_res = (set<long int *> *)stc->resultset;
 	    temp_size = temp_res->size();
	    set<long int *>::iterator iter, iter1;
	    iter = res->begin();
	    iter1 = temp_res->begin();
	    for (index = 0; index < temp_size; index++) {
		tr = (long int)((Customer *)*iter1)->get_pos();
		for (index1 = 0; index < res_size; index1++) {
		    r = (long int)(*iter);
		    if ( tr == r ) {
			break;
		    } else if (tr < r ) {
			temp_res.erase(iter1);
			temp_size--;
			break;
		    }
		    iter++;
		}
		iter1++;
	    }
	}
	delete(res);
    } else if( !strcmp(table_name, "Position") ) {
    } else if( !strcmp(table_name, "MapIndex") ) {
	if ( !strcmp(fk_name, "Customer") ) {
	    res = (set<long int *> *)stc->resultset;
	    res_size = res->size();
	    fill_resultset(cur, temp_res, NULL);
	    temp_res = (set<long int *> *)stcsr->resultset;
 	    temp_size = temp_res->size();
	    set<long int *>::iterator iter, iter1;
	    iter = res->begin();
	    iter1 = temp_res->begin();
	    for (index = 0; index < temp_size; index++) {
		tr = (long int)((Customer *)*iter1).second;
		for (index1 = 0; index < res_size; index1++) {
		    r = (long int)(*iter);
		    if ( tr == r ) {
			break;
		    } else if (tr < r ) {
			temp_res.erase(iter1);
			temp_size--;
			break;
		    }
		    iter++;
		}
		iter1++;
	    }
	}
	delete(res);
    }
    stcsr->toConvert = -1;
    return SQLITE_OK;
}

/*
x

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

x

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
            sqlite3_free(res);
            printf("Error (re)allocating memory\n");
            return SQLITE_NOMEM;
        }
    }
    return SQLITE_OK;
}
*/

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


/*
x

int compare_res(stlTableCursor *stcsr, set<long int> *temp_res) {
    long int ia, ib;
    int i_count = 0, size, count;
    if ( (stcsr->size == 0) && (stcsr->first_constr == 1) ){
        memcpy(stcsr->resultSet, temp_res, sizeof(int) *
               stcsr->max_size);
	stcsr->resultset = &temp_res;
        stcsr->size = temp_res->size();
        stcsr->first_constr = 0;
    }else if (stcsr->size > 0){
	set<long int> *res = (set<long int> *)stcsr->resultset;
	set<long int>::iterator iter, temp_iter;
	iter = res->begin();
	temp_iter = temp_res->begin();
	size = res->size();
	count = temp_res->size();
        for(int a = 0; a < size; a++){
	    ia = *iter;
            for(int b = 0; b < count; b++){
                ib = *temp_iter;
                if( ia == ib ){
		    break;
                }else if( ia < ib ) {
		    res.erase(iter);
		    size--;
                    break;
		}
		temp_iter++;
            }
	    iter++;
        }
        stcsr->size = size;
    }
    return SQLITE_OK;
}
*/

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
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    int op, iCol, count = 0, i = 0, re = 0, ds_size;
    set<long int> *temp_res;
    if ( stcsr->first_constr ) {
	temp_res = new set<long int>();
	stcsr->resultSet = temp_res;
	fill_resultset(cur);
	stcsr->first_constr = 0;
    } else
	temp_res = (set<long int> *)stcsr->resultSet;
    ds_size = stcsr->size;
    if ( (val!=NULL) && (ds_size>0) ){
	set<long int>::iterator iter, er_iter;
	iter = temp_res->begin();
        check_alloc((const char *)constr, op, iCol);
        switch( iCol ){
        case 0:
            for(int i=0; i<ds_size; i++){
		if (!compare(*iter, op, (long int)sqlite3_value_int64(val)) ) {
		    er_iter = iter++;
		    temp_res->erase(er_iter);
		} else
		    iter++;
            }
            break;
        }
	stcsr->size = temp_res->size();
    }
    return SQLITE_OK;
}


int Truck_search(sqlite3_vtab_cursor *cur, char *constr, sqlite3_value *val){
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    int op, iCol, count = 0, i = 0, re = 0, ds_size;
    set<long int> *temp_res;
    if ( stcsr->first_constr ) {
	temp_res = new set<long int>();
	stcsr->resultSet = temp_res;
	fill_resultset(cur);
	stcsr->first_constr = 0;
    } else
	temp_res = (set<long int> *)stcsr->resultSet;
    ds_size = stcsr->size;
    if ( (val!=NULL) && (ds_size>0) ){
	set<long int>::iterator iter, er_iter;
	iter = temp_res->begin();
        check_alloc((const char *)constr, op, iCol);
        switch( iCol ){
        case 0:
            for(int i=0; i<ds_size; i++){
		if (!compare(*iter, op, (long int)sqlite3_value_int64(val)) ){
		    er_iter = iter++;
		    temp_res->erase(er_iter);
		}else
		    iter++;
	    }
            break;
        case 1:
            for(int i=0; i<ds_size; i++){
		if (!compare(((Truck *)*iter)->get_cost(), op, sqlite3_value_double(val)) ){
		    er_iter = iter++;
		    temp_res->erase(er_iter);
		}else
		    iter++;
	    }
	    break;
        case 2:
            for(int i=0; i<ds_size; i++){
		if (!compare(((Truck *)*iter)->get_delcapacity(), op, sqlite3_value_int(val)) ){
		    er_iter = iter++;
		    temp_res->erase(er_iter);
		}else
		    iter++;
	    }
            break;
	case 3:
            for(int i=0; i<ds_size; i++){
		if (!compare(((Truck *)*iter)->get_pickcapacity(), op, sqlite3_value_int(val)) ){
		    er_iter = iter++;
		    temp_res->erase(er_iter);
		}else
		    iter++;
	    }
            break;
	case 4:
            for(int i=0; i<ds_size; i++){
		if (!compare(((Truck *)*iter)->get_rlpoint(), op, sqlite3_value_int(val)) ){
		    er_iter = iter++;
		    temp_res->erase(er_iter);
		}else
		    iter++;
	    }
	}
	stcsr->size = temp_res->size();
    }
    return SQLITE_OK;
}


int Customers_search(sqlite3_vtab_cursor *cur, char *constr, sqlite3_value *val){
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    int op, iCol, count = 0, i = 0, re = 0, ds_size;
    map<long int,long int> *temp_res;
    if ( stcsr->first_constr ) {
	temp_res = new map<long int,long int>();
	stcsr->resultSet = temp_res;
	fill_resultset(cur);
	stcsr->first_constr = 0;
    } else
	temp_res = (map<long int,long int> *)stcsr->resultSet;
    ds_size = stcsr->size;
    if ( (val!=NULL) && (ds_size>0) ){
	map<long int,long int>::iterator iter, er_iter;
	iter = temp_res->begin();
        check_alloc((const char *)constr, op, iCol);
        switch( iCol ){
        case 0:
            for(int i=0; i<ds_size; i++){
		if (!compare((*iter).second, op, (long int)sqlite3_value_int64(val)) ) {
		    er_iter = iter++;
		    temp_res->erase(er_iter);
		} else
		    iter++;
            }
            break;
        case 1:
            for(int i=0; i<ds_size; i++){
		if (!compare((*iter).first, op, (long int)sqlite3_value_int64(val)) ) {
		    er_iter = iter++;
		    temp_res->erase(er_iter);
		} else
		    iter++;
            }
            break;
        }
	stcsr->size = temp_res->size();
    }
    return SQLITE_OK;
}


int Customer_search(sqlite3_vtab_cursor *cur, char *constr, sqlite3_value *val){
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    int op, iCol, count = 0, i = 0, re = 0, ds_size;
    set<long int> *temp_res;
    if ( stcsr->first_constr ) {
	temp_res = new set<long int>();
	stcsr->resultSet = temp_res;
	fill_resultset(cur);
	stcsr->first_constr = 0;
    } else
	temp_res = (set<long int> *)stcsr->resultSet;
    ds_size = stcsr->size;
    if ( (val!=NULL) && (ds_size>0) ){
	set<long int>::iterator iter, er_iter;
	iter = temp_res->begin();
        check_alloc((const char *)constr, op, iCol);
        switch( iCol ){
        case 0:
            for(int i=0; i<ds_size; i++){
		if (!compare(*iter, op, (long int)sqlite3_value_int64(val)) ) {
		    er_iter = iter++;
		    temp_res->erase(er_iter);
		} else
		    iter++;
	    }
            break;
        case 1:
            for(int i=0; i<ds_size; i++){
		if (!compare(((Customer *)*iter)->get_demand(), op, sqlite3_value_int(val)) ) {
		    er_iter = iter++;
		    temp_res->erase(er_iter);
		} else
		    iter++;
	    }
            break;
        case 2:
            for(int i=0; i<ds_size; i++){
		if (!compare((const unsigned char *)((Customer *)*iter)->get_code().c_str(), op, sqlite3_value_text(val)) ) {
		    er_iter = iter++;
		    temp_res->erase(er_iter);
		} else
		    iter++;
	    }
            break;
        case 3:
            for(int i=0; i<ds_size; i++){
		if (!compare(((Customer *)*iter)->get_serviced(), op, sqlite3_value_int(val)) ) {
		    er_iter = iter++;
		    temp_res->erase(er_iter);
		} else
		    iter++;
	    }
            break;
        case 4:
            for(int i=0; i<ds_size; i++){
		if (!compare(((Customer *)*iter)->get_pickdemand(), op, sqlite3_value_int(val)) ) {
		    er_iter = iter++;
		    temp_res->erase(er_iter);
		} else
		    iter++;
	    }
            break;
        case 5:
            for(int i=0; i<ds_size; i++){
		if (!compare(((Customer *)*iter)->get_starttime(), op, sqlite3_value_int(val)) ) {
		    er_iter = iter++;
		    temp_res->erase(er_iter);
		} else
		    iter++;
	    }
            break;
        case 6:
            for(int i=0; i<ds_size; i++){
		if (!compare(((Customer *)*iter)->get_servicetime(), op, sqlite3_value_int(val)) ) {
		    er_iter = iter++;
		    temp_res->erase(er_iter);
		} else
		    iter++;
	    }
            break;
        case 7:
            for(int i=0; i<ds_size; i++){
		if (!compare(((Customer *)*iter)->get_finishtime(), op, sqlite3_value_int(val)) ) {
		    er_iter = iter++;
		    temp_res->erase(er_iter);
		} else
		    iter++;
	    }
            break;
        case 8:
            for(int i=0; i<ds_size; i++){
		if (!compare(((Customer *)*iter)->get_revenue(), op, sqlite3_value_int(val)) ) {
		    er_iter = iter++;
		    temp_res->erase(er_iter);
		} else
		    iter++;
	    }
            break;
        case 9:
            for(int i=0; i<ds_size; i++){
		if (!compare(*iter, op, (long int)sqlite3_value_int64(val)) ) {
		    er_iter = iter++;
		    temp_res->erase(er_iter);
		} else
		    iter++;
	    }
            break;
        }
	stcsr->size = temp_res->size();
    }
    return SQLITE_OK;
}


int Position_search(sqlite3_vtab_cursor *cur, char *constr, sqlite3_value *val){
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    int op, iCol, count = 0, i = 0, re = 0, ds_size;
    set<long int> *temp_res;
    if ( stcsr->first_constr ) {
	temp_res = new set<long int>();
	stcsr->resultSet = temp_res;
	fill_resultset(cur);
	stcsr->first_constr = 0;
    } else
	temp_res = (set<long int> *)stcsr->resultSet;
    ds_size = stcsr->size;
    if ( (val!=NULL) && (ds_size>0) ){
	set<long int>::iterator iter, er_iter;
	iter = temp_res->begin();
        check_alloc((const char *)constr, op, iCol);
        switch( iCol ){
        case 0:
            for(int i=0; i<ds_size; i++){
		if (!compare(*iter, op, (long int)sqlite3_value_int64(val)) ) {
		    er_iter = iter++;
		    temp_res->erase(er_iter);
		} else
		    iter++;
	    }
            break;
	case 1:
            for(int i=0; i<ds_size; i++){
		if (!compare(((Position *)*iter)->get_x(), op, sqlite3_value_int(val)) ) {
		    er_iter = iter++;
		    temp_res->erase(er_iter);
		} else
		    iter++;
	    }
            break;
        case 2:
            for(int i=0; i<ds_size; i++){
		if (!compare(((Position *)*iter)->get_y(), op, sqlite3_value_int(val)) ) {
		    er_iter = iter++;
		    temp_res->erase(er_iter);
		} else
		    iter++;
	    }
	    break;
        }
	stcsr->size = temp_res->size();
    }
    return SQLITE_OK;
}


int MapIndex_search(sqlite3_vtab_cursor *cur, char *constr, sqlite3_value *val){
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    int op, iCol, count = 0, i = 0, re = 0, ds_size;
    set<long int> *temp_res;
    if ( stcsr->first_constr ) {
	temp_res = new set<long int>();
	stcsr->resultSet = temp_res;
	fill_resultset(cur);
	stcsr->first_constr = 0;
    } else
	temp_res = (set<long int> *)stcsr->resultSet;
    ds_size = stcsr->size;
    if ( (val!=NULL) && (ds_size>0) ){
	set<long int>::iterator iter, er_iter;
	iter = temp_res->begin();
        check_alloc((const char *)constr, op, iCol);
        switch( iCol ){
        case 0:
            for(int i=0; i<ds_size; i++){
                if (!compare(((pair<int,Customer*> *)*iter)->first, op, sqlite3_value_int(val)) ) {
		    er_iter = iter++;
                    temp_res->erase(er_iter);
		} else
		    iter++;
            }
            break;
        case 1:
            for(int i=0; i<ds_size; i++){
                if (!compare((long int)((pair<int,Customer *> *)*iter)->second, op, (long int)sqlite3_value_int64(val)) ) {
		    er_iter = iter++;
                    temp_res->erase(er_iter);
		} else
		    iter++;
            }
            break;
        }
	stcsr->size = temp_res->size();
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
    set<long int> *temp_res = (set<long int> *)stcsr->resultSet;
    set<long int>::iterator iter;
    int index = stcsr->current, ds_size = temp_res->size();
    iter = temp_res->begin();
    for(int i=1; i<=index && i<ds_size; i++){
        iter++;
    }
    switch( n ){
    case 0:
        sqlite3_result_int64(con, *iter);
        break;
    }
    return SQLITE_OK;
}


int Truck_retrieve(sqlite3_vtab_cursor *cur, int n, sqlite3_context *con){
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    set<long int> *temp_res = (set<long int> *)stcsr->resultSet;
    set<long int>::iterator iter;
    int index = stcsr->current, ds_size = temp_res->size();
    iter = temp_res->begin();
    for(int i=1; i<=index && i<ds_size; i++){
        iter++;
    }
    switch( n ){
    case 0:
        sqlite3_result_int64(con, *iter);
        break;
    case 1:
        sqlite3_result_double(con, ((Truck *)*iter)->get_cost());
        break;
    case 2:
        sqlite3_result_int(con, ((Truck *)*iter)->get_delcapacity());
        break;
    case 3:
        sqlite3_result_int(con, ((Truck *)*iter)->get_pickcapacity());
        break;
    case 4:
        sqlite3_result_int(con, ((Truck *)*iter)->get_rlpoint());
        break;
    }
    return SQLITE_OK;
}


int Customers_retrieve(sqlite3_vtab_cursor *cur, int n, sqlite3_context *con){
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    map<long int,long int> *temp_res = (map<long int,long int> *)stcsr->resultSet;
    map<long int,long int>::iterator iter;
    int index = stcsr->current, ds_size = temp_res->size();
    iter = temp_res->begin();
    for(int i=1; i<=index && i<ds_size; i++){
        iter++;
    }
    switch( n ){
    case 0:
        sqlite3_result_int64(con, (*iter).second);
        break;
    case 1:
        sqlite3_result_int64(con, (*iter).first);
        break;
    }
    return SQLITE_OK;
}


int Customer_retrieve(sqlite3_vtab_cursor *cur, int n, sqlite3_context *con){
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    set<long int> *temp_res = (set<long int> *)stcsr->resultSet;
    set<long int>::iterator iter;
    int index = stcsr->current, ds_size = temp_res->size();
    iter = temp_res->begin();
    for(int i=1; i<=index && i<ds_size; i++){
        iter++;
    }
    switch( n ){
    case 0:
        sqlite3_result_int64(con, *iter);
        break;
    case 1:
        sqlite3_result_int(con, ((Customer *)*iter)->get_demand());
        break;
    case 2:
        sqlite3_result_text(con, (const char *)((Customer *)*iter)->get_code().c_str(), -1, SQLITE_STATIC);
        break;
    case 3:
        sqlite3_result_int(con, ((Customer *)*iter)->get_serviced());
        break;
    case 4:
        sqlite3_result_int(con, ((Customer *)*iter)->get_pickdemand());
        break;
    case 5:
        sqlite3_result_int(con, ((Customer *)*iter)->get_starttime());
        break;
    case 6:
        sqlite3_result_int(con, ((Customer *)*iter)->get_servicetime());
        break;
    case 7:
        sqlite3_result_int(con, ((Customer *)*iter)->get_finishtime());
        break;
    case 8:
        sqlite3_result_int(con, ((Customer *)*iter)->get_revenue());
        break;
    case 9:
        sqlite3_result_int64(con, (long int)((Customer *)*iter)->get_pos());
        break;
    }
    return SQLITE_OK;
}


int Position_retrieve(sqlite3_vtab_cursor *cur, int n, sqlite3_context *con){
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    set<long int> *temp_res = (set<long int> *)stcsr->resultSet;
    set<long int>::iterator iter;
    int index = stcsr->current, ds_size = temp_res->size();
    iter = temp_res->begin();
    for(int i=1; i<=index && i<ds_size; i++){
        iter++;
    }
    switch( n ){
    case 0:
        sqlite3_result_int64(con, *iter);
        break;
    case 1:
        sqlite3_result_int(con, ((Position *)*iter)->get_x());
        break;
    case 2:
        sqlite3_result_int(con, ((Position *)*iter)->get_y());
        break;
    }
    return SQLITE_OK;
}


int MapIndex_retrieve(sqlite3_vtab_cursor *cur, int n, sqlite3_context *con){
    stlTableCursor *stcsr = (stlTableCursor *)cur;
    set<long int> *temp_res = (set<long int> *)stcsr->resultSet;
    set<long int>::iterator iter;
    int index = stcsr->current, ds_size = temp_res->size();
    iter = temp_res->begin();
    for(int i=1; i<=index && i<ds_size; i++){
        iter++;
    }
    switch( n ){
    case 0:
        sqlite3_result_int(con, ((pair<int,Customer *> *)*iter)->first);
        break;
    case 1:
        sqlite3_result_int64(con, (long int)((pair<int,Customer *> *)*iter)->second);
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
