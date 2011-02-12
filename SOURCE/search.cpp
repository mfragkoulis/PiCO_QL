#include "search.h"
#include <string>
#include "assert.h"
#include <vector>
#include <vector>
#include "Truck.h"

using namespace std;

int get_datastructure_size(void *st){
    stlTable *stl = (stlTable *)st;
    if( !strcmp(stl->zName, "Trucks") ){
        vector<Truck*> *any_dstr = (vector<Truck*> *)stl->data;
        return ((int)any_dstr->size());
    }
}


int traverse(int dstr_value, int op, int value){
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


int traverse(double dstr_value, int op, double value){
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

// compare addresses???
int traverse(const void *dstr_value, int op, const void *value){
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


int traverse(const unsigned char *dstr_value, int op, 
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


int cmp_int(char *tok_tr, char *tok_res){
    int index_tr, index_res;
    sscanf( tok_tr, "%d", &index_tr);
    sscanf( tok_res, "%d", &index_res);
    if ( index_tr < index_res )
        return -1;
    else if (index_tr > index_res)
    	return 1;
    else
	return 0;
}


int str_token(char *source, char *token, char dlm){
    char * match;
    char * copy = (char *)sqlite3_malloc(sizeof(char) * 20);
    char * init_address;
    int position = 0, k = 0;
    strcpy(copy, source);
    match = strchr( copy, dlm);
    if ( match != NULL ){
        position = match - copy;
	strcpy(token, copy);
	token[position] = '\0';
	printf("token: %s\n", token);
	init_address = copy;
        while (k <= position){
	    copy++;
	    k += 1;
	}
	strcpy( source, copy);
	printf("source: %s\n", source);
	sqlite3_free(init_address);
	return 1;
    }else{
	sqlite3_free(copy);
        return 0;
    }
}


int cmp_str(char *str_tr, char *str_res){
    int ci = 0, c_tr, c_res;
    char copy_tr[20], copy_res[20], tok_tr[20], tok_res[20];
    strcpy(copy_tr, str_tr);
    strcpy(copy_res, str_res);
    c_tr = str_token(copy_tr, tok_tr, '.');
    c_res = str_token(copy_res, tok_res, '.');
    if ( (!c_tr) && (!c_res) )
        return cmp_int(copy_tr, copy_res);
    else{
	while ( (c_tr) && (c_res) && (ci == 0 ) ){
	    ci = cmp_int(tok_tr, tok_res);
    	    c_tr = str_token(copy_tr, tok_tr, '.');
    	    c_res = str_token(copy_res, tok_res, '.');
	}
    	if ( (!c_tr) && (!c_res) )
            ci = cmp_int(copy_tr, copy_res);
	return ci;
    }
}


void Trucks_search(void *stc, char *constr, sqlite3_value *val){
    sqlite3_vtab_cursor *cur = (sqlite3_vtab_cursor *)stc;
    stlTable *stl = (stlTable *)cur->pVtab;
    stlTableCursor *stcsr = (stlTableCursor *)stc;
    vector<Truck*> *any_dstr = (vector<Truck*> *)stl->data;
    vector<Truck*>:: iterator iter;
    vector<Customer*> *emb_dstr0;
    vector<Customer*>:: iterator iter0;
    int index0;
    int op, iCol, count = 0, total_size = 0;
    iter = any_dstr->begin();
    for (int i=0; i<(int)any_dstr->size(); i++){
        emb_dstr0 = (*iter)->get_Customers();
        index0 = emb_dstr0->size();
        total_size += index0;

	iter++;
        assert(count <= stcsr->max_size);
        assert(&stcsr->resultSet[count] <= &stcsr->resultSet[stcsr->max_size]);
    }
    char **temp_res;
    int memory_size = (sizeof(char *) + 
			 sizeof(char) * 20) * total_size;
    temp_res = (char**)sqlite3_malloc(memory_size);
     memset(temp_res, '\0', memory_size);
     *temp_res = (char *)&temp_res[total_size];
     int tr;
	  for (tr=0; tr<total_size; tr++){
         temp_res[tr+1] = &temp_res[tr][20];
	  }
    if (total_size > stcsr->max_size){
        printf("TOTAL_SIZE: %d, stcsr->max_size: %d", 
			    total_size, stcsr->max_size);
        char **res;
        int memory_size = (sizeof(char*) + sizeof(char) * 20) * total_size;
        res = (char **)sqlite3_realloc(stcsr->resultSet, memory_size);
        if (res!=NULL){
            stcsr->resultSet = res;
            memset(stcsr->resultSet, '\0',
                       memory_size);
	    *stcsr->resultSet = (char *)&stcsr->resultSet[total_size];
	    int i;
	    for (i=0; i<total_size; i++){
	        stcsr->resultSet[i+1] = &stcsr->resultSet[i][20];
	    }
            stcsr->max_size = total_size;
            printf("\nReallocating resultSet..now max size %i \n\n", 
				stcsr->max_size);
        }else{
            free(res);
            printf("Error (re)allocating memory\n");
            exit(1);
        }
    }
// val==NULL then constr==NULL also
    if ( val==NULL ){
        iter = any_dstr->begin();
        for (int i=0; i<(int)any_dstr->size(); i++){
            emb_dstr0 = (*iter)->get_Customers();
            index0 = emb_dstr0->size();
            for(int i0=0; i0<index0; i0++){
                sprintf(temp_res[count++], "%d.%d", i, i0);
                
            }
	    iter++;
        }
        assert(count <= stcsr->max_size);
        assert(&stcsr->resultSet[count] <= &stcsr->resultSet[stcsr->max_size]);
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
            NULL;
            break;
        }

        iCol = constr[1] - 'a' + 1;
        switch( iCol ){
// i=0. search using PK?memory location?or no PK?
// no can't do.PK will be memory location. PK in every table
// PK search
        case 0: 
            iter=any_dstr->begin();
            for(int i=0; i<(int)any_dstr->size(); i++){
	        sprintf(temp_res[count++], "%d", i);
            }
            assert(count <= stcsr->max_size);
            break;
        case 1:
            iter=any_dstr->begin();
            for(int i=0;i<(int)any_dstr->size();i++){
                if( traverse((*iter)->get_cost(), op, sqlite3_value_double(val))){
                    emb_dstr0 = (*iter)->get_Customers();
                    index0 = emb_dstr0->size();
                    for(int i0=0; i0<index0; i0++){
                        sprintf(temp_res[count++], "%d.%d",  i, i0);
                    }
                    iter0++;
		}
                iter++;
            }
            assert(count <= stcsr->max_size);
            break;
        case 2:
            iter=any_dstr->begin();
            for(int i=0;i<(int)any_dstr->size();i++){
                if( traverse((*iter)->get_delcapacity(), op, sqlite3_value_int(val))){
                    emb_dstr0 = (*iter)->get_Customers();
                    index0 = emb_dstr0->size();
                    for(int i0=0; i0<index0; i0++){
                        sprintf(temp_res[count++], "%d.%d",  i, i0);
                    }
                    iter0++;
		}
                iter++;
            }
            assert(count <= stcsr->max_size);
            break;
        case 3:
            iter=any_dstr->begin();
            for(int i=0;i<(int)any_dstr->size();i++){
                if( traverse((*iter)->get_pickcapacity(), op, sqlite3_value_int(val))){
                    emb_dstr0 = (*iter)->get_Customers();
                    index0 = emb_dstr0->size();
                    for(int i0=0; i0<index0; i0++){
                        sprintf(temp_res[count++], "%d.%d",  i, i0);
                    }
                    iter0++;
		}
                iter++;
            }
            assert(count <= stcsr->max_size);
            break;
        case 4:
            iter=any_dstr->begin();
            for(int i=0;i<(int)any_dstr->size();i++){
                if( traverse((*iter)->get_rlpoint(), op, sqlite3_value_int(val))){
                    emb_dstr0 = (*iter)->get_Customers();
                    index0 = emb_dstr0->size();
                    for(int i0=0; i0<index0; i0++){
                        sprintf(temp_res[count++], "%d.%d",  i, i0);
                    }
                    iter0++;
		}
                iter++;
            }
            assert(count <= stcsr->max_size);
            break;
        case 5:
            iter=any_dstr->begin();
            for(int i=0;i<(int)any_dstr->size();i++){
                emb_dstr0 = (*iter)->get_Customers();
                iter0 = emb_dstr0->begin();
                for(int i0=0;i0<(int)emb_dstr0->size();i0++){
                    if (traverse((*iter0)->get_demand(), op, sqlite3_value_int(val))){
                        sprintf(temp_res[count++],"%d.%d", i, i0);
		    }
                    iter0++;
                }
	        iter++;
            }
            assert(count <= stcsr->max_size);
            break;
        case 6:
            iter=any_dstr->begin();
            for(int i=0;i<(int)any_dstr->size();i++){
                emb_dstr0 = (*iter)->get_Customers();
                iter0 = emb_dstr0->begin();
                for(int i0=0;i0<(int)emb_dstr0->size();i0++){
                    if (traverse((const unsigned char *)(*iter0)->get_code().c_str(), op, sqlite3_value_text(val))){
                        sprintf(temp_res[count++],"%d.%d", i, i0);
		    }
                    iter0++;
                }
	        iter++;
            }
            assert(count <= stcsr->max_size);
            break;
        case 7:
            iter=any_dstr->begin();
            for(int i=0;i<(int)any_dstr->size();i++){
                emb_dstr0 = (*iter)->get_Customers();
                iter0 = emb_dstr0->begin();
                for(int i0=0;i0<(int)emb_dstr0->size();i0++){
                    if (traverse((*iter0)->get_serviced(), op, sqlite3_value_int(val))){
                        sprintf(temp_res[count++],"%d.%d", i, i0);
		    }
                    iter0++;
                }
	        iter++;
            }
            assert(count <= stcsr->max_size);
            break;
        case 8:
            iter=any_dstr->begin();
            for(int i=0;i<(int)any_dstr->size();i++){
                emb_dstr0 = (*iter)->get_Customers();
                iter0 = emb_dstr0->begin();
                for(int i0=0;i0<(int)emb_dstr0->size();i0++){
                    if (traverse((*iter0)->get_pickdemand(), op, sqlite3_value_int(val))){
                        sprintf(temp_res[count++],"%d.%d", i, i0);
		    }
                    iter0++;
                }
	        iter++;
            }
            assert(count <= stcsr->max_size);
            break;
        case 9:
            iter=any_dstr->begin();
            for(int i=0;i<(int)any_dstr->size();i++){
                emb_dstr0 = (*iter)->get_Customers();
                iter0 = emb_dstr0->begin();
                for(int i0=0;i0<(int)emb_dstr0->size();i0++){
                    if (traverse((*iter0)->get_starttime(), op, sqlite3_value_int(val))){
                        sprintf(temp_res[count++],"%d.%d", i, i0);
		    }
                    iter0++;
                }
	        iter++;
            }
            assert(count <= stcsr->max_size);
            break;
        case 10:
            iter=any_dstr->begin();
            for(int i=0;i<(int)any_dstr->size();i++){
                emb_dstr0 = (*iter)->get_Customers();
                iter0 = emb_dstr0->begin();
                for(int i0=0;i0<(int)emb_dstr0->size();i0++){
                    if (traverse((*iter0)->get_servicetime(), op, sqlite3_value_int(val))){
                        sprintf(temp_res[count++],"%d.%d", i, i0);
		    }
                    iter0++;
                }
	        iter++;
            }
            assert(count <= stcsr->max_size);
            break;
        case 11:
            iter=any_dstr->begin();
            for(int i=0;i<(int)any_dstr->size();i++){
                emb_dstr0 = (*iter)->get_Customers();
                iter0 = emb_dstr0->begin();
                for(int i0=0;i0<(int)emb_dstr0->size();i0++){
                    if (traverse((*iter0)->get_finishtime(), op, sqlite3_value_int(val))){
                        sprintf(temp_res[count++],"%d.%d", i, i0);
		    }
                    iter0++;
                }
	        iter++;
            }
            assert(count <= stcsr->max_size);
            break;
        case 12:
            iter=any_dstr->begin();
            for(int i=0;i<(int)any_dstr->size();i++){
                emb_dstr0 = (*iter)->get_Customers();
                iter0 = emb_dstr0->begin();
                for(int i0=0;i0<(int)emb_dstr0->size();i0++){
                    if (traverse((*iter0)->get_revenue(), op, sqlite3_value_int(val))){
                        sprintf(temp_res[count++],"%d.%d", i, i0);
		    }
                    iter0++;
                }
	        iter++;
            }
            assert(count <= stcsr->max_size);
            break;
	}
    }
    if ( stcsr->init_constr ){
	if ( count > 0 )
	    memcpy(*stcsr->resultSet, *temp_res, 
		sizeof(char) * 20 * total_size);
	stcsr->init_constr = 0;
	stcsr->size = count;
    }else{
	if ( count == 0 ){
	    memset(stcsr->resultSet, '\0', memory_size);
	    stcsr->size = 0;
	}else{
	    char ** copy_res = (char **)sqlite3_malloc(memory_size); 
	    memset(copy_res, '\0', memory_size); 
  	    *copy_res = (char *)&copy_res[total_size];
  	    int cr;
  	    for (cr=0; cr<total_size; cr++){
    		copy_res[cr+1] = &copy_res[cr][20];
  	    }
	    int success = 0, ci;
	    char  str_tr[20], str_res[20];
	    for(int w=0; w<count; w++){
		strcpy(str_tr, temp_res[w]);
	        for (int k=0; k<stcsr->size; k++){
		    strcpy(str_res, stcsr->resultSet[k]);
		    printf("tr: %s, set: %s\n", str_tr, 
			   str_res);
		    ci = cmp_str(str_tr, str_res);
		    if (ci < 0)
		        break;
		    else if (ci == 0){
			strcpy(copy_res[success++], temp_res[w]);
			printf("Success\n");
		    }
		}
	    }
	    memcpy(*stcsr->resultSet, *copy_res, memory_size);
	    stcsr->size = success;
	    sqlite3_free(copy_res);
	}
    }
    sqlite3_free(temp_res);
}




int Trucks_retrieve(void *stc, int n, sqlite3_context *con){
    sqlite3_vtab_cursor *cur = (sqlite3_vtab_cursor *)stc;
    stlTable *stl = (stlTable *)cur->pVtab;
    stlTableCursor *stcsr = (stlTableCursor *)stc;
    vector<Truck*> *any_dstr = (vector<Truck*> *)stl->data;
    vector<Truck*>:: iterator iter;
    char record[20];
    strcpy(record, stcsr->resultSet[stcsr->current]);
    char * top_index = strtok(record, ".");
    int index;
    if (top_index != NULL)
        sscanf( top_index, "%d", &index);
    else
        sscanf( record, "%d", &index);
    char *colName = stl->azColumn[n];
// iterator implementation. serial traversing or hit?
    iter = any_dstr->begin();
// serial traversing. simple and generic. visitor pattern is next step.
    for(int i=0; i<index; i++){
        iter++;
    }
    vector<Customer*> *emb_dstr0;
    vector<Customer*>:: iterator iter0;
    int index0;
    char * emb_index0 = strtok( NULL, ".");
    if (emb_index0 != NULL)
        sscanf(emb_index0, "%d",&index0);
    else
        printf("FAULT detokenising\n");
    
    emb_dstr0 = (*iter)->get_Customers();
    iter0 = emb_dstr0->begin();
    for(int i0=0; i0<index0; i0++){
        iter0++;
    }
    switch( n ){
    case 0:
        sqlite3_result_text(con, "(null)", -1, SQLITE_STATIC);
        break;
    case 1:
        sqlite3_result_double(con, (*iter)->get_cost());
        break;
    case 2:
        sqlite3_result_int(con, (*iter)->get_delcapacity());
        break;
    case 3:
        sqlite3_result_int(con, (*iter)->get_pickcapacity());
        break;
    case 4:
        sqlite3_result_int(con, (*iter)->get_rlpoint());
        break;
    case 5:
        sqlite3_result_int(con, (*iter0)->get_demand());
        break;
    case 6:
        sqlite3_result_text(con, (const char *)(*iter0)->get_code().c_str(), -1, SQLITE_STATIC);
        break;
    case 7:
        sqlite3_result_int(con, (*iter0)->get_serviced());
        break;
    case 8:
        sqlite3_result_int(con, (*iter0)->get_pickdemand());
        break;
    case 9:
        sqlite3_result_int(con, (*iter0)->get_starttime());
        break;
    case 10:
        sqlite3_result_int(con, (*iter0)->get_servicetime());
        break;
    case 11:
        sqlite3_result_int(con, (*iter0)->get_finishtime());
        break;
    case 12:
        sqlite3_result_int(con, (*iter0)->get_revenue());
        break;
    }
    return SQLITE_OK;
}


void search(void* stc, char *constr, sqlite3_value *val){
    sqlite3_vtab_cursor *cur = (sqlite3_vtab_cursor *)stc;
    stlTable *stl = (stlTable *)cur->pVtab;
    if( !strcmp(stl->zName, "Trucks") )
        Trucks_search(stc, constr, val);
}

int retrieve(void* stc, int n, sqlite3_context *con){
    sqlite3_vtab_cursor *cur = (sqlite3_vtab_cursor *)stc;
    stlTable *stl = (stlTable *)cur->pVtab;
    if( !strcmp(stl->zName, "Trucks") )
        return Trucks_retrieve(stc, n, con);
}

