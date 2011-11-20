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

int set_dependencies(void *st, void *ds, const char *table_name, char **pzErr) {
    stlTable *stl = (stlTable *)st;
    dsArray *dsC = (dsArray *)ds;
    dsCarrier **tmp_Carrier = dsC->ds, *curr_ds;
    attrCarrier **children, *curr_child, *attr_child, *curr_attr;
    relationship *tmp_rel;
    int c = 0, ch, dc, rlt_no, ch_size, i;
    int dsC_size = dsC->ds_size;
    
    while (c < dsC_size) {
        if ( !strcmp(table_name, tmp_Carrier[c]->attr->dsName) )
            break;
        c++;
    }
    curr_ds = tmp_Carrier[c];
    curr_attr = curr_ds->attr;
    if( !strcmp(table_name, "Trucks") ) {
        data *d = (data *)sqlite3_malloc(sizeof(data) + (sizeof(attrCarrier *) + sizeof(attrCarrier)) * 1);
        d->set_mem = NULL;
        d->children = (attrCarrier **)&d[1];
	children = d->children;
        d->children_size = 1;
	ch_size = d->children_size;
	children[0] = (attrCarrier *)&children[ch_size];
	for (i = 1; i < ch_size; i++)
	    children[i] = &children[i-1][1];
        d->mem = curr_attr->memory;
        vector<Truck*> *any_dstr = (vector<Truck*> *)d->mem;
        vector<Truck*>::iterator iter;
        iter = any_dstr->begin();
        ch = 0;
	curr_child = children[ch];
        dc = 0;
        while (dc < dsC_size) {
            if ( !strcmp(tmp_Carrier[dc]->attr->dsName, "Truck") )
		
                break;
            dc++;
        }
	attr_child = tmp_Carrier[dc]->attr;
        curr_child->memory = (long int *)(*iter);
	attr_child->memory = (long int *)&curr_child->memory;
	rlt_no = 0;
	tmp_rel = curr_ds->rlt[rlt_no];
	while (rlt_no < curr_ds->rlt_size) {
	    if( !strcmp(tmp_rel->child, "Truck") )
		break;
	    rlt_no++;
	}
	curr_child->set_memory = &tmp_rel->set;
	attr_child->set_memory = &tmp_rel->set;
        *curr_child->set_memory = 1;
        curr_child->dsName = attr_child->dsName;
        ch++;
        assert (ch == ch_size);
        stl->data = (void *)d;
    } else if( !strcmp(table_name, "Truck") ) {
        if ( *curr_attr->set_memory == 0 ) {
            *pzErr = sqlite3_mprintf("Attempted to open the VT %s before its ancestor forgot to include ancestor in FROM clause. Please pay attention to the order of VTs in the FROM clause.\n", table_name);
            return SQLITE_MISUSE;
        }
        data *d = (data *)sqlite3_malloc(sizeof(data) + (sizeof(attrCarrier *) + sizeof(attrCarrier)) * 1);
        d->set_mem = curr_attr->set_memory;
        *d->set_mem = 0;
        d->children = (attrCarrier **)&d[1];
	children = d->children;
        d->children_size = 1;
	ch_size = d->children_size;
	children[0] = (attrCarrier *)&children[ch_size];
	for (i = 1; i < ch_size; i++)
	    children[i] = &children[i-1][1];
        d->mem = curr_attr->memory;
        Truck *any_dstr = (Truck *)*d->mem;
        ch = 0;
	curr_child = children[ch];
        dc = 0;
        while (dc < dsC_size) {
            if ( !strcmp(tmp_Carrier[dc]->attr->dsName, "Customers") )
                break;
            dc++;
        }
	attr_child = tmp_Carrier[dc]->attr;
        curr_child->memory = (long int *)any_dstr->get_Customers();
	attr_child->memory = (long int *)&curr_child->memory;
	rlt_no = 0;
	tmp_rel = curr_ds->rlt[rlt_no];
	while (rlt_no < curr_ds->rlt_size) {
	    if( !strcmp(tmp_rel->child, "Customers") )
		break;
	    rlt_no++;
	}
	curr_child->set_memory = &tmp_rel->set;
	attr_child->set_memory = &tmp_rel->set;
        *curr_child->set_memory = 1;
        curr_child->dsName = attr_child->dsName;
        ch++;
        assert (ch == ch_size);
        stl->data = (void *)d;
    } else if( !strcmp(table_name, "Customers") ) {
        if ( *curr_attr->set_memory == 0 ) {
            *pzErr = sqlite3_mprintf("Attempted to open the VT %s before its ancestor forgot to include ancestor in FROM clause. Please pay attention to the order of VTs in the FROM clause.\n", table_name);
            return SQLITE_MISUSE;
        }
        data *d = (data *)sqlite3_malloc(sizeof(data) + (sizeof(attrCarrier *) + sizeof(attrCarrier)) * 1);
        d->set_mem = curr_attr->set_memory;
        *d->set_mem = 0;
        d->children = (attrCarrier **)&d[1];
	children = d->children;
        d->children_size = 1;
	ch_size = d->children_size;
	children[0] = (attrCarrier *)&children[ch_size];
	for (i = 1; i < ch_size; i++)
	    children[i] = &children[i-1][1];
        d->mem = curr_attr->memory;
        vector<Customer*> *any_dstr = (vector<Customer*> *)*d->mem;
        vector<Customer*>::iterator iter;
        iter = any_dstr->begin();
        ch = 0;
	curr_child = children[ch];
        dc = 0;
        while (dc < dsC_size) {
            if ( !strcmp(tmp_Carrier[dc]->attr->dsName, "Customer") )
                break;
            dc++;
        }
	attr_child = tmp_Carrier[dc]->attr;
        curr_child->memory = (long int *)(*iter);
	attr_child->memory = (long int *)&curr_child->memory;
	rlt_no = 0;
	tmp_rel = curr_ds->rlt[rlt_no];
	while (rlt_no < curr_ds->rlt_size) {
	    if( !strcmp(tmp_rel->child, "Customer") )
		break;
	    rlt_no++;
	}
	curr_child->set_memory = &tmp_rel->set;
	attr_child->set_memory = &tmp_rel->set;
        *curr_child->set_memory = 1;
        curr_child->dsName = attr_child->dsName;
        ch++;
        assert (ch == ch_size);
        stl->data = (void *)d;
    } else if( !strcmp(table_name, "Customer") ) {
        if ( *curr_attr->set_memory == 0 ) {
            *pzErr = sqlite3_mprintf("Attempted to open the VT %s before its ancestor forgot to include ancestor in FROM clause. Please pay attention to the order of VTs in the FROM clause.\n", table_name);
            return SQLITE_MISUSE;
        }
        data *d = (data *)sqlite3_malloc(sizeof(data) + (sizeof(attrCarrier *) + sizeof(attrCarrier)) * 1);
        d->set_mem = curr_attr->set_memory;
        *d->set_mem = 0;
        d->children = (attrCarrier **)&d[1];
	children = d->children;
        d->children_size = 1;
	ch_size = d->children_size;
	children[0] = (attrCarrier *)&children[ch_size];
	for (i = 1; i < ch_size; i++)
	    children[i] = &children[i-1][1];
        d->mem = curr_attr->memory;
        Customer *any_dstr = (Customer *)*d->mem;
        ch = 0;
	curr_child = children[ch];
        dc = 0;
        while (dc < dsC_size) {
            if ( !strcmp(tmp_Carrier[dc]->attr->dsName, "Position") )
                break;
            dc++;
        }
	attr_child = tmp_Carrier[dc]->attr;
        curr_child->memory = (long int *)any_dstr->get_pos();
	attr_child->memory = (long int *)&curr_child->memory;
	rlt_no = 0;
	tmp_rel = curr_ds->rlt[rlt_no];
	while (rlt_no < curr_ds->rlt_size) {
	    if( !strcmp(tmp_rel->child, "Customer") )
		break;
	    rlt_no++;
	}
	curr_child->set_memory = &tmp_rel->set;
	attr_child->set_memory = &tmp_rel->set;
        *curr_child->set_memory = 1;
        curr_child->dsName = attr_child->dsName;
        ch++;
        assert (ch == ch_size);
        stl->data = (void *)d;
    } else if( !strcmp(table_name, "Position") ) {
        if ( *curr_attr->set_memory == 0 ) {
            *pzErr = sqlite3_mprintf("Attempted to open the VT %s before its ancestor forgot to include ancestor in FROM clause. Please pay attention to the order of VTs in the FROM clause.\n", table_name);
            return SQLITE_MISUSE;
        }
        data *d = (data *)sqlite3_malloc(sizeof(data));
        d->set_mem = curr_attr->set_memory;
        *d->set_mem = 0;
        d->mem = curr_attr->memory;
        d->children = NULL;
        stl->data = (void *)d;
    } else if( !strcmp(table_name, "MapIndex") ) {
	    data *d = (data *)sqlite3_malloc(sizeof(data) + (sizeof(attrCarrier *) + sizeof(attrCarrier)) * 1);
        d->set_mem = NULL;
        d->children = (attrCarrier **)&d[1];
	children = d->children;
        d->children_size = 1;
	ch_size = d->children_size;
	children[0] = (attrCarrier *)&children[ch_size];
	for (i = 1; i < ch_size; i++)
	    children[i] = &children[i-1][1];
        d->mem = curr_attr->memory;
        map<int,Customer*> *any_dstr = (map<int,Customer*> *)d->mem;
        map<int,Customer*>::iterator iter;
        iter = any_dstr->begin();
        ch = 0;
	curr_child = children[ch];
        dc = 0;
        while (dc < dsC_size) {
            if ( !strcmp(tmp_Carrier[dc]->attr->dsName, "Customer") )
                break;
            dc++;
        }
	attr_child = tmp_Carrier[dc]->attr;
        curr_child->memory = (long int *)(*iter).second;
	attr_child->memory = (long int *)&curr_child->memory;
	rlt_no = 0;
	tmp_rel = curr_ds->rlt[rlt_no];
	while (rlt_no < curr_ds->rlt_size) {
	    if( !strcmp(tmp_rel->child, "Customer") )
		break;
	    rlt_no++;
	}
	curr_child->set_memory = &tmp_rel->set;
	attr_child->set_memory = &tmp_rel->set;
        *curr_child->set_memory = 1;
        curr_child->dsName = attr_child->dsName;
        ch++;
        assert (ch == ch_size);
        stl->data = (void *)d;
    }
    return SQLITE_OK;
}


void copy_structs(void *ds, char **c_temp, const char *name, long int *mem) {
    dsCarrier **ddsC = (dsCarrier **)ds;
    dsCarrier *dss = *ddsC;
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

void set_relationships(void *ds, void *mem) {
    dsArray *dsa = (dsArray *)ds;
    dsCarrier **tmp_Carrier = dsa->ds, *curr_ds;
    relationship **holder;
    int ds_size = dsa->ds_size, i, rlt_size, rlt_no, tmp_rlt_size, tmp_rlt_no, dc, ch;

// 5: hard-coded
    tmp_rlt_size = 5;
    relationship **tmp_rlt;
    tmp_rlt = (relationship **)mem;
    tmp_rlt[0] = (relationship *)&tmp_rlt[tmp_rlt_size];
    for (i = 1; i < tmp_rlt_size; i++)
	tmp_rlt[i] = &tmp_rlt[i-1][1];

    dc = 0;
    while (dc < ds_size) {
	if ( !strcmp(tmp_Carrier[dc]->attr->dsName, "Trucks") ) {
	    curr_ds = tmp_Carrier[dc];
	    curr_ds->rlt_size = 1;
	    rlt_size = curr_ds->rlt_size;
	    tmp_rlt_no = 0;
	    rlt_no = 0;
	    ch = 0;
	    while (ch < ds_size) {
		if ( !strcmp(tmp_Carrier[ch]->attr->dsName, "Truck") ) {
		    curr_ds->rlt = &tmp_rlt[tmp_rlt_no];
		    holder = curr_ds->rlt;
		    holder[rlt_no] = tmp_rlt[tmp_rlt_no];
		    holder[rlt_no]->child = tmp_Carrier[ch]->attr->dsName;
		    holder[rlt_no]->set = 0;
		    break;
		}
		ch++;
	    }
	    break;
	}
	dc++;
    }

    dc = 0;
    while (dc < ds_size) {
	if ( !strcmp(tmp_Carrier[dc]->attr->dsName, "Truck") ) {
	    curr_ds = tmp_Carrier[dc];
	    curr_ds->rlt_size = 1;
	    rlt_size = curr_ds->rlt_size;
	    tmp_rlt_no = 0;
	    rlt_no = 0;
	    ch = 0;
	    while (ch < ds_size) {
		if ( !strcmp(tmp_Carrier[ch]->attr->dsName, "Customers") ) {
		    curr_ds->rlt = &tmp_rlt[tmp_rlt_no];
		    holder = curr_ds->rlt;
		    holder[rlt_no] = tmp_rlt[tmp_rlt_no];
		    holder[rlt_no]->child = tmp_Carrier[ch]->attr->dsName;
		    holder[rlt_no]->set = 0;
		    break;
		}
		ch++;
	    }
	    break;
	}
	dc++;
    }

    dc = 0;
    while (dc < ds_size) {
	if ( !strcmp(tmp_Carrier[dc]->attr->dsName, "Customers") ) {
	    curr_ds = tmp_Carrier[dc];
	    curr_ds->rlt_size = 1;
	    rlt_size = curr_ds->rlt_size;
	    tmp_rlt_no = 0;
	    rlt_no = 0;
	    ch = 0;
	    while (ch < ds_size) {
		if ( !strcmp(tmp_Carrier[ch]->attr->dsName, "Customer") ) {
		    curr_ds->rlt = &tmp_rlt[tmp_rlt_no];
		    holder = curr_ds->rlt;
		    holder[rlt_no] = tmp_rlt[tmp_rlt_no];
		    holder[rlt_no]->child = tmp_Carrier[ch]->attr->dsName;
		    holder[rlt_no]->set = 0;
		    break;
		}
		ch++;
	    }
	    break;
	}
	dc++;
    }

    dc = 0;
    while (dc < ds_size) {
	if ( !strcmp(tmp_Carrier[dc]->attr->dsName, "Customer") ) {
	    curr_ds = tmp_Carrier[dc];
	    curr_ds->rlt_size = 1;
	    rlt_size = curr_ds->rlt_size;
	    tmp_rlt_no = 0;
	    rlt_no = 0;
	    ch = 0;
	    while (ch < ds_size) {
		if ( !strcmp(tmp_Carrier[ch]->attr->dsName, "Position") ) {
		    curr_ds->rlt = &tmp_rlt[tmp_rlt_no];
		    holder = curr_ds->rlt;
		    holder[rlt_no] = tmp_rlt[tmp_rlt_no];
		    holder[rlt_no]->child = tmp_Carrier[ch]->attr->dsName;
		    holder[rlt_no]->set = 0;
		    break;
		}
		ch++;
	    }
	    break;
	}
	dc++;
    }

    dc = 0;
    while (dc < ds_size) {
	if ( !strcmp(tmp_Carrier[dc]->attr->dsName, "MapIndex") ) {
	    curr_ds = tmp_Carrier[dc];
	    curr_ds->rlt_size = 1;
	    rlt_size = curr_ds->rlt_size;
	    tmp_rlt_no = 0;
	    rlt_no = 0;
	    ch = 0;
	    while (ch < ds_size) {
		if ( !strcmp(tmp_Carrier[ch]->attr->dsName, "Customer") ) {
		    curr_ds->rlt = &tmp_rlt[tmp_rlt_no];
		    holder = curr_ds->rlt;
		    holder[rlt_no] = tmp_rlt[tmp_rlt_no];
		    holder[rlt_no]->child = tmp_Carrier[ch]->attr->dsName;
		    holder[rlt_no]->set = 0;
		    break;
		}
		ch++;
	    }
	    break;
	}
	dc++;
    }
}

int realloc_carrier(void *st, void *ds, const char *tablename, char **pzErr) {
    dsArray **ddsC = (dsArray **)ds;
    dsArray *dsC = *ddsC;
    dsArray *tmp_dsC;
    dsCarrier **tmp_Carrier, **dsC_Carrier;
//
    int x_size, nByte, c, i, size;
    char *c_temp;
#ifdef DEBUGGING
    printf("dsC->size: %i\n", dsC->ds_size);
#endif
    if (dsC->ds_size != 6) {

        x_size = dsC->ds_size;
        nByte = sizeof(dsArray) + (sizeof(dsCarrier *) + sizeof(dsCarrier) + sizeof(attrCarrier)) * 6 + (sizeof(relationship *) + sizeof(relationship)) * 5 + 50;
        tmp_dsC = (dsArray *)sqlite3_malloc(nByte);
        if (tmp_dsC != NULL) {
            memset(tmp_dsC, 0, nByte);
            tmp_dsC->ds_size = 6;
	    size = tmp_dsC->ds_size;
	    tmp_dsC->ds = (dsCarrier **)&tmp_dsC[1];
	    tmp_Carrier = tmp_dsC->ds;
	    tmp_Carrier[0] = (dsCarrier *)&tmp_Carrier[size];
	    for (i = 1; i < size; i++)
		tmp_Carrier[i] = (dsCarrier *)&tmp_Carrier[i-1][1];

	    tmp_Carrier[0]->attr = (attrCarrier *)&tmp_Carrier[i-1][1];
	    for (i = 1; i < size; i++)
		tmp_Carrier[i]->attr = (attrCarrier *)&tmp_Carrier[i-1]->attr[1];
	    c_temp = (char *)&tmp_Carrier[i-1]->attr[1];
	    dsC_Carrier = dsC->ds;
            c = 0;
            copy_structs(&tmp_Carrier[c], &c_temp, dsC_Carrier[c]->attr->dsName, dsC_Carrier[c]->attr->memory);
	    c++;
            copy_structs(&tmp_Carrier[c], &c_temp, dsC_Carrier[c]->attr->dsName, dsC_Carrier[c]->attr->memory);
	    c++;
            copy_structs(&tmp_Carrier[c], &c_temp, "Truck", NULL);
	    c++;
            copy_structs(&tmp_Carrier[c], &c_temp, "Customers", NULL);
	    c++;
            copy_structs(&tmp_Carrier[c], &c_temp, "Customer", NULL);
	    c++;
            copy_structs(&tmp_Carrier[c], &c_temp, "Position", NULL);
	    c++;
            assert(c == 6);
	    set_relationships(tmp_dsC, c_temp);
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
    }
    return set_dependencies(st, *ddsC, tablename, pzErr);
}


int update_structures(void *cur) {
    sqlite3_vtab_cursor *stc = (sqlite3_vtab_cursor *)cur;
    stlTable *stl = (stlTable *)stc->pVtab;
    const char *table_name = stl->zName;
    data *d = (data *)stl->data;
    stlTableCursor *stcsr = (stlTableCursor *)stc;
    attrCarrier **children, *attr_child;
    int dc, index, no_child;
    if( !strcmp(table_name, "Trucks") ) {
        index = stcsr->current;
        vector<Truck*> *any_dstr = (vector<Truck*> *)d->mem;
        vector<Truck*>::iterator iter;
        iter = any_dstr->begin();
        for(int i=0; i<stcsr->resultSet[index]; i++){
            iter++;
        }
        dc = 0;
        children = d->children;
        no_child = d->children_size;
        while (dc < no_child) {
	    attr_child = children[dc];
            if ( !strcmp(attr_child->dsName, "Truck") ) {
                attr_child->memory = (long int *)(*iter);
                *attr_child->set_memory = 1;
                break;
            }
            dc++;
        }
    } else if( !strcmp(table_name, "Truck") ) {
        index = stcsr->current;
        if ( (index == 0) && (*d->set_mem == 0) )
            return SQLITE_MISUSE;
        *d->set_mem = 0;
        Truck *any_dstr = (Truck *)*d->mem;
        dc = 0;
        children = d->children;
        no_child = d->children_size;
        while (dc < no_child) {
	    attr_child = children[dc];
            if ( !strcmp(attr_child->dsName, "Customers") ) {
                attr_child->memory = (long int *)any_dstr->get_Customers();
                *attr_child->set_memory = 1;
                break;
            }
            dc++;
        }
    } else if( !strcmp(table_name, "Customers") ) {
        index = stcsr->current;
        if ( (index == 0) && (*d->set_mem == 0) )
            return SQLITE_MISUSE;
        *d->set_mem = 0;
        vector<Customer*> *any_dstr = (vector<Customer*> *)*d->mem;
        vector<Customer*>::iterator iter;
        iter = any_dstr->begin();
        for(int i=0; i<stcsr->resultSet[index]; i++){
            iter++;
        }
        dc = 0;
        children = d->children;
        no_child = d->children_size;
        while (dc < no_child) {
	    attr_child = children[dc];
            if ( !strcmp(attr_child->dsName, "Customer") ) {
                attr_child->memory = (long int *)(*iter);
                *attr_child->set_memory = 1;
                break;
            }
            dc++;
        }
    } else if( !strcmp(table_name, "Customer") ) {
        index = stcsr->current;
        if ( (index == 0) && (*d->set_mem == 0) )
            return SQLITE_MISUSE;
        *d->set_mem = 0;
        Customer *any_dstr = (Customer *)*d->mem;
        dc = 0;
        children = d->children;
        no_child = d->children_size;
        while (dc < no_child) {
	    attr_child = children[dc];
            if ( !strcmp(attr_child->dsName, "Position") ) {
                attr_child->memory = (long int *)any_dstr->get_pos();
                *attr_child->set_memory = 1;
                break;
            }
            dc++;
        }
    } else if( !strcmp(table_name, "Position") ) {
        index = stcsr->current;
        if ( (index == 0) && (*d->set_mem == 0) )
            return SQLITE_MISUSE;
        *d->set_mem = 0;
    } else if( !strcmp(table_name, "MapIndex") ) {
        index = stcsr->current;
        map<int,Customer*> *any_dstr = (map<int,Customer*> *)d->mem;
        map<int,Customer*>::iterator iter;
        iter = any_dstr->begin();
        for(int i=0; i<stcsr->resultSet[index]; i++){
            iter++;
        }
        dc = 0;
        children = d->children;
        no_child = d->children_size;
        while (dc < no_child) {
	    attr_child = children[dc];
            if ( !strcmp(attr_child->dsName, "Customer") ) {
                attr_child->memory = (long int *)(*iter).second;
                *attr_child->set_memory = 1;
                break;
            }
            dc++;
        }
    }
    return SQLITE_OK;
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
    if( !strcmp(stl->zName, "MapIndex") ){
        data *d = (data *)stl->data;
        map<int,Customer*> *any_dstr = (map<int,Customer*> *)d->mem;
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


void MapIndex_search(void *stc, char *constr, sqlite3_value *val){
    sqlite3_vtab_cursor *cur = (sqlite3_vtab_cursor *)stc;
    stlTable *stl = (stlTable *)cur->pVtab;
    stlTableCursor *stcsr = (stlTableCursor *)stc;
    data *d = (data *)stl->data;
    map<int,Customer*> *any_dstr = (map<int,Customer*> *)d->mem;
    map<int,Customer*>:: iterator iter;
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
    if( !strcmp(stl->zName, "MapIndex") )
        MapIndex_search(stc, constr, val);
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


int MapIndex_retrieve(void *stc, int n, sqlite3_context *con){
    sqlite3_vtab_cursor *cur = (sqlite3_vtab_cursor *)stc;
    stlTable *stl = (stlTable *)cur->pVtab;
    stlTableCursor *stcsr = (stlTableCursor *)stc;
    data *d = (data *)stl->data;
    map<int,Customer*> *any_dstr = (map<int,Customer*> *)d->mem;
    map<int,Customer*>:: iterator iter;
    int index = stcsr->current;
    iter = any_dstr->begin();
    for(int i=0; i<stcsr->resultSet[index]; i++){
        iter++;
    }
    char *colName = stl->azColumn[n];
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
    if( !strcmp(stl->zName, "MapIndex") )
        return MapIndex_retrieve(stc, n, con);
}
