#include <vector>
#include "search.h"
#include <string>
#include "Type.h"
#include "Account.h"

using namespace std;



int get_datastructure_size(void *st){
    stlTable *stl = (stlTable *)st;
    vector<Account> *any_dstr = (vector<Account> *)stl->data;
    return any_dstr->size();
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


int traverse(const unsigned char *dstr_value, int op, const unsigned char *value){
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


void search(void *stc, char *constr, sqlite3_value *val){
    sqlite3_vtab_cursor *cur = (sqlite3_vtab_cursor *)stc;
    stlTable *stl = (stlTable *)cur->pVtab;
    stlTableCursor *stcsr = (stlTableCursor *)stc;
    vector<Account> *any_dstr = (vector<Account> *)stl->data;
    vector<Account>:: iterator iter;
    Type value;
    int op, count=0;
// val==NULL then constr==NULL also
    if ( val==NULL ){
        for (int j=0; j<get_datastructure_size((void *)stl); j++){
            stcsr->resultSet[j] = j;
            stcsr->size++;
        }
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

        int iCol;
        iCol = constr[1] - 'a' + 1;
        char *colName = stl->azColumn[iCol];


// handle colName

        switch( iCol ){
        case 0:
// why necessarily iter->second in associative?if non pointer then second. else second->
            iter=any_dstr->begin();
            for(int i=0; i<(int)any_dstr->size(); i++){
                if( traverse((const unsigned char *)iter->get_account_no(), op, sqlite3_value_text(val)) )
                    stcsr->resultSet[count++] = i;
                    iter++;
            }
            stcsr->size += count;
            break;
        case 1:
// why necessarily iter->second in associative?if non pointer then second. else second->
            iter=any_dstr->begin();
            for(int i=0; i<(int)any_dstr->size(); i++){
                if( traverse(iter->get_balance(), op, sqlite3_value_double(val)) )
                    stcsr->resultSet[count++] = i;
                    iter++;
            }
            stcsr->size += count;
            break;
// more datatypes and ops exist
        }
    }
}


int retrieve(void *stc, int n, sqlite3_context* con){
    sqlite3_vtab_cursor *svc = (sqlite3_vtab_cursor *)stc;
    stlTable *stl = (stlTable *)svc->pVtab;
    stlTableCursor *stcsr = (stlTableCursor *)stc;
    vector<Account> *any_dstr = (vector<Account> *)stl->data;
    vector<Account>:: iterator iter;
    char *colName = stl->azColumn[n];
    int index = stcsr->current;
// iterator implementation. serial traversing or hit?
    iter = any_dstr->begin();
// serial traversing. simple and generic. visitor pattern is next step.
    for(int i=0; i<stcsr->resultSet[index]; i++){
        iter++;
    }
    int datatype;
    datatype = stl->colDataType[n];
    const char *pk = "PK";
    const char *fk = "FK";
    if ( (n==0) && (!strcmp(stl->azColumn[0], pk)) ){
// attention!
        sqlite3_result_blob(con, (const void *)&(*iter),-1,SQLITE_STATIC);
    }else if( !strncmp(stl->azColumn[n], fk, 2) ){
        sqlite3_result_blob(con, (const void *)&(*iter),-1,SQLITE_STATIC);
// need work
    }else{
// in automated code: "iter->get_" + col_name + "()" will work.safe?no.doxygen.
        switch ( n ){
// why necessarily iter->second in associative?
        case 0:
            sqlite3_result_text(con, (const char *)iter->get_account_no(),-1,SQLITE_STATIC);
            break;
        case 1:
            sqlite3_result_double(con, iter->get_balance());
            break;
        }
    }
    return SQLITE_OK;
}
