
#ifndef SEARCH_H
#define SEARCH_H

//#include "stl_to_sql.h"
#include "sqlite3.h"
#include "bridge.h"
#include "Account.h"
#include <vector>

using namespace std;

class Type {

 private:
  char *text;
  float doub;
  int integ;
  void *blob;

 public:
  Type() {
    text='\0';
    doub=0.0;
    integ=0;
    blob=0;
  }
  void set_int(int i) {
    integ=i;
  }
  void set_doub(float f) {
    doub=f;
  }
  void set_text(const unsigned char * t) {
    text=(char *)t;
  }
  void set_blob(const void * b) {
    blob=(void *)b;
  }
  float get_doub() {
    return doub;
  }

};

//extern "C" int register_table(char *db, char * query, void *data);
void search(int *size, int *resultset, void *st, char *constr, sqlite3_value *val);
void traverse(vector < Account >::iterator iter, char *col_name, char *op, Type value);


#endif

