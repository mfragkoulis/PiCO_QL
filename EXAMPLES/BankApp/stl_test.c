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
      fprintf(f, "\n\nDone\n");
    }else if( result==SQLITE_OK ){
      fprintf(f, "\n\nOK\n");
    }else if( result==SQLITE_ERROR ){
      fprintf(f, "\n\nSQL error or missing database\n");
    }else if( result==SQLITE_MISUSE ){
      fprintf(f, "\n\nLibrary used incorrectly\n");
    }else {
      fprintf(f, "\n\nError code: %i.\nPlease advise Sqlite error codes (http://w\
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
  int result, i = 1;
  char *q;

  q = "select * from SuperAccounts;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  result = test_prep_exec(f, db, q);

  q = "";
  //  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  //  result = test_prep_exec(f, db, q);

  q = "select * from SuperAccounts where isbn > 0 and balance < 500 and account_no>1000 order by id;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  result = test_prep_exec(f, db, q);

  q = "select * from Accounts;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  result = test_prep_exec(f, db, q);

  q = "select * from Accounts where rate >10 and balance<20000 order by account_no;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  result = test_prep_exec(f, db, q);

  q = "select * from SpecialAccounts;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  result = test_prep_exec(f, db, q);

  q = "select * from SpecialAccounts where bonus > 20 and rate >15 order by account_no;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  result = test_prep_exec(f, db, q);

  /*
  q = "select * from trucks,truck,customers,customer where truck.base=trucks.truck_ptr and cost <800 and delcapacity>0 and customers.base=truck.customers and customer.base=customers.customer_ptr and code>100 and demand>10 order by code;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  result = test_prep_exec(f, db, q);

  q = "select code, demand, x_coord, y_coord from trucks,truck,customers, customer, position where truck.base=trucks.truck_ptr and customers.base=truck.customers and customer.base=customers.customer_ptr and position.base=customer.position_ptr and code like '%99' union select code, demand, x_coord,y_coord from mapindex,customer,position where customer.base=mapindex.customer_ptr and position.base=customer.position_ptr and x_coord>133;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  result = test_prep_exec(f, db, q);

  q = "select c.code, c.demand, c.position_ptr, p.x_coord, p.y_coord,u.code, u.demand, u.position_ptr, o.x_coord, o.y_coord from trucks,truck,customers, customer c,mapindex,customer u, position p,position o where truck.base=trucks.truck_ptr and customers.base=truck.customers and c.base=customers.customer_ptr and p.base=c.position_ptr and c.code like '%99' and u.base=mapindex.customer_ptr and o.base=u.position_ptr and o.x_coord>133 and p.y_coord=o.y_coord;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  result = test_prep_exec(f, db, q);

  q = "select * from customer;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  result = test_prep_exec(f, db, q);

  q = "select * from trucks, customers;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  result = test_prep_exec(f, db, q);

  q = "select * from trucks, customers where customers.base=trucks.truck_ptr;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  result = test_prep_exec(f, db, q);

  q = "select c.code, c.demand, c.position_ptr, p.x_coord, p.y_coord,u.code, u.demand, u.position_ptr, o.x_coord, o.y_coord from trucks,truck,customers, customer c,mapindex,customer u, position p,position o where truck.base=trucks.truck_ptr and customers.base=truck.customers and c.base=customers.customer_ptr and p.base=c.position_ptr and c.code like '%99' and u.base=mapindex.customer_ptr and o.base=truck.base and o.x_coord>133 and p.y_coord=o.y_coord;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  result = test_prep_exec(f, db, q);

  q = "select code, demand, x_coord, y_coord from trucks,truck,customers, customer, position where truck.base=trucks.truck_ptr and customers.base=truck.customers and customer.base=trucks.truck_ptr and position.base=customer.position_ptr and code like '%99' union select code, demand, x_coord,y_coord from mapindex,customer,position where customer.base=mapindex.customer_ptr and position.base=customer.position_ptr and x_coord>133;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  result = test_prep_exec(f, db, q);
  */
  fclose(f);
  FILE *pipe;
  pipe = popen("diff test_success.txt test_current.txt", "r");
  if ( pipe==NULL ) return SQLITE_ERROR;
  char buffer[128], output[1280];
  int j = 0, status;
  while (fgets(buffer, 128, pipe) != NULL) {
    if (j == 0) strcpy(output, buffer);
    else strcat(output, buffer);
    j++;
  }
  status = pclose(pipe);
  if ( status==-1 )
    return SQLITE_ERROR;
  f = fopen("test_current.txt", "a+");
  if (strlen(output) == 0) fprintf(f, "\nTEST SUCCESSFUL\n");
  else fprintf(f, "\nTEST FAILED\n");
  fprintf(f, "diff command returned: %s\n", output);
  fclose(f);
  return SQLITE_OK;
}
