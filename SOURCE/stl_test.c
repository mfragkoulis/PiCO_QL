#include "stl_test.h"
#include <stdio.h>
#include <string.h>

// Takes care of query preparation and execution.
int test_prep_exec(FILE *f, sqlite3 *db, const char *q){
  sqlite3_stmt  *stmt;
  int result, col, prepare;
  if( (prepare = sqlite3_prepare_v2(db, q, -1, &stmt, 0)) == SQLITE_OK ){
    fprintf(f,"Statement prepared.\n");
    for (col = 0; col < sqlite3_column_count(stmt); col++){
      fprintf(f, "%s ", sqlite3_column_name(stmt, col));
    }
    fprintf(f, "\n");
    while ((result = sqlite3_step(stmt)) == SQLITE_ROW){
      fprintf(f, "\n");
      for (col = 0; col < sqlite3_column_count(stmt); col++){
	switch(sqlite3_column_type(stmt, col)) {
	case 1:
	  fprintf(f, "%i ", sqlite3_column_int(stmt, col));
	  break;
	case 2:
	  fprintf(f, "%f ", sqlite3_column_double(stmt, col));
	  break;
	case 3:
	  fprintf(f, "%s ", sqlite3_column_text(stmt, col));
	  break;
	case 4:
	  fprintf(f, "%s ", (char *)sqlite3_column_blob(stmt, col));
	  break;
	case 5:
	  fprintf(f, "(null) ");
	  break;
	}
      }
    }
    if( result==SQLITE_DONE ){
      fprintf(f, "Done\n");
    }else if( result==SQLITE_OK ){
      fprintf(f, "OK\n");
    }else if( result==SQLITE_ERROR ){
      fprintf(f, "SQL error or missing database\n");
    }else if( result==SQLITE_MISUSE ){
      fprintf(f, "Library used incorrectly\n");
    }else {
      fprintf(f, "Error code: %i.\nPlease advise Sqlite error codes (http://w\
ww.sqlite.org/c3ref/c_abort.html)", result);
    }
    fprintf(f, "\n");
  } else {
    fprintf(f, "Error in preparation of query: error no %i\n", prepare);
    return prepare;
  }
  sqlite3_finalize(stmt);
  return result;
}


int call_test(sqlite3 *db) {
  FILE *f;
  f = fopen("test_current.txt", "w");
  int result;
  result = test_prep_exec(f, db, "select * from trucks;");

  result = test_prep_exec(f, db, "select * from truck;");

  result = test_prep_exec(f, db, "select * from trucks,truck where truck.base=trucks.truck_ptr");

  result = test_prep_exec(f, db, "select * from trucks,truck where truck.base=trucks.truck_ptr and cost<800 and delcapacity>0;");

  result = test_prep_exec(f, db, "select * from trucks,truck,customers,customer where truck.base=trucks.truck_ptr and cost <800 and delcapacity>0 and customers.base=truck.customers and customer.base=customers.customer_ptr and code>100 and demand>10;");

  result = test_prep_exec(f, db, "select * from trucks,truck,customers,customer where truck.base=trucks.truck_ptr and cost <800 and delcapacity>0 and customers.base=truck.customers and customer.base=customers.customer_ptr and code>100 and demand>10 order by code;");

  result = test_prep_exec(f, db, "select code, demand, x_coord, y_coord from trucks,truck,customers, customer, position where truck.base=trucks.truck_ptr and customers.base=truck.customers and customer.base=customers.customer_ptr and position.base=customer.position_ptr and code like '%99' union select code, demand, x_coord,y_coord from mapindex,customer,position where customer.base=mapindex.customer_ptr and position.base=customer.position_ptr and x_coord>133;");

  result = test_prep_exec(f, db, "select c.code, c.demand, c.position_ptr, p.x_coord, p.y_coord,u.code, u.demand, u.position_ptr, o.x_coord, o.y_coord from trucks,truck,customers, customer c,mapindex,customer u, position p,position o where truck.base=trucks.truck_ptr and customers.base=truck.customers and c.base=customers.customer_ptr and p.base=c.position_ptr and c.code like '%99' and u.base=mapindex.customer_ptr and o.base=u.position_ptr and o.x_coord>133 and p.y_coord=o.y_coord;");

  result = test_prep_exec(f, db, "select * from customer;");

  result = test_prep_exec(f, db, "select * from trucks, customers;");

  result = test_prep_exec(f, db, "select * from trucks, customers where customers.base=trucks.truck_ptr;");

  result = test_prep_exec(f, db, "select c.code, c.demand, c.position_ptr, p.x_coord, p.y_coord,u.code, u.demand, u.position_ptr, o.x_coord, o.y_coord from trucks,truck,customers, customer c,mapindex,customer u, position p,position o where truck.base=trucks.truck_ptr and customers.base=truck.customers and c.base=customers.customer_ptr and p.base=c.position_ptr and c.code like '%99' and u.base=mapindex.customer_ptr and o.base=truck.base and o.x_coord>133 and p.y_coord=o.y_coord;");

  result = test_prep_exec(f, db, "select code, demand, x_coord, y_coord from trucks,truck,customers, customer, position where truck.base=trucks.truck_ptr and customers.base=truck.customers and customer.base=trucks.truck_ptr and position.base=customer.position_ptr and code like '%99' union select code, demand, x_coord,y_coord from mapindex,customer,position where customer.base=mapindex.customer_ptr and position.base=customer.position_ptr and x_coord>133;");

  fclose(f);
  FILE* pipe = popen("diff test_success.txt test_current.txt", "r");
  if (!pipe) return SQLITE_ERROR;
  char buffer[128], output[1280];
  int i = 0;
  while (fgets(buffer, 128, pipe) != NULL) {
    if (i == 0) strcpy(output, buffer);
    else strcat(output, buffer);
    i++;
  }
  pclose(pipe);
  f = fopen("test_current.txt", "a+");
  if (strlen(output) == 0) fprintf(f, "\nTEST SUCCESSFUL\n");
  else fprintf(f, "\nTEST FAILED\n");
  fprintf(f, "diff command returned: %s\n", output);
  fclose(f);
  return SQLITE_OK;
}
