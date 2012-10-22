/*
 *   Set up local query preparation and execution
 *   environment for testing purposes.
 *   Execute the queries user has included and write the
 *   output in test_current.txt.
 *
 *   Copyright [2012] [Marios Fragkoulis]
 *
 *   Licensed under the Apache License, Version 2.0
 *   (the "License");you may not use this file except in
 *   compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in
 *   writing, software distributed under the License is
 *   distributed on an "AS IS" BASIS.
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 *   express or implied.
 *   See the License for the specific language governing
 *  permissions and limitations under the License.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico_ql_test.h"
#include "pico_ql_internal.h"

// Takes care of query preparation and execution.
int test_prep_exec(FILE *f, sqlite3 *db, const char *q) {
  sqlite3_stmt  *stmt;
  int result, col, prepare;
  if ((prepare = sqlite3_prepare_v2(db, q, -1, &stmt, 0)) == SQLITE_OK) {
    fprintf(f,"Statement prepared.\n");
    for (col = 0; col < sqlite3_column_count(stmt); col++) {
      fprintf(f, "%s ", sqlite3_column_name(stmt, col));
    }
    fprintf(f, "\n");
    while ((result = sqlite3_step(stmt)) == SQLITE_ROW) {
      fprintf(f, "\n");
      for (col = 0; col < sqlite3_column_count(stmt); col++) {
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
    if (result == SQLITE_DONE) {
      fprintf(f, "\n\nDone\n");
    }else if (result == SQLITE_OK) {
      fprintf(f, "\n\nOK\n");
    }else if (result == SQLITE_ERROR) {
      fprintf(f, "\n\nSQL error or missing database\n");
    }else if (result == SQLITE_MISUSE) {
      fprintf(f, "\n\nLibrary used incorrectly\n");
    }else {
      fprintf(f, "\n\nError code: %i.\nPlease advise Sqlite error codes (http://www.sqlite.org/c3ref/c_abort.html)", result);
    }
    fprintf(f, "\n");
  } else {
    fprintf(f, "Error in preparation of query: error no %i\n", prepare);
    return prepare;
  }
  deinit_temp_structs();
  sqlite3_finalize(stmt);
  return result;
}


int call_test(sqlite3 *db) {
  FILE *f;
  f = fopen("pico_ql_test_current.txt", "w");
  int result, i = 1;
  char *q;

  fprintf(f, "PLAIN QUERIES\n\n\n");

  q = "select rownum from trucks;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select * from truck;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  fprintf(f, "PLAIN QUERIES + ROWNUM\n\n\n");

  q = "select rownum from trucks where rownum=7;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select * from trucks where rownum=20;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select * from customers where rownum=20;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  fprintf(f, "JOIN QUERIES EXPLICIT\n\n\n");

  q = "select trucks.rownum, cost, delcapacity, delcapacity_root from trucks JOIN truck ON truck.base=trucks.truck_id;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select trucks.rownum, cost, delcapacity, delcapacity_root from trucks INNER JOIN truck on truck.base=trucks.truck_id;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select trucks.rownum, cost, delcapacity, delcapacity_root from trucks LEFT JOIN truck on truck.base=trucks.truck_id;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select trucks.rownum, cost, delcapacity, delcapacity_root from trucks CROSS JOIN truck on truck.base=trucks.truck_id;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select trucks.rownum, cost, delcapacity, delcapacity_root from trucks CROSS JOIN truck;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select * from trucks JOIN customers;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  fprintf(f, "JOIN QUERIES IMPLICIT\n\n\n");

  q = "select trucks.rownum, cost, delcapacity, delcapacity_root from trucks,truck where truck.base=trucks.truck_id;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select * from trucks, customers;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  fprintf(f, "JOIN QUERIES EXPLICIT+ ROWNUM\n\n\n");

  q = "select trucks.rownum, cost, delcapacity, delcapacity_root, customers.rownum from trucks JOIN truck ON truck.base=trucks.truck_id JOIN customers ON customers.base=truck.customers_id where customers.rownum=4;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select trucks.rownum, cost, delcapacity, delcapacity_root, customers.rownum from trucks JOIN truck ON truck.base=trucks.truck_id JOIN customers ON customers.base=truck.customers_id where trucks.rownum=11 and customers.rownum=4;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select trucks.rownum, cost, delcapacity, delcapacity_root, customers.rownum, map_index from trucks JOIN truck ON truck.base=trucks.truck_id JOIN customers ON customers.base=truck.customers_id,mapindex where customers.rownum=mapindex.map_index;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select trucks.rownum, cost, delcapacity, delcapacity_root, customers.rownum from trucks JOIN truck ON truck.base=trucks.truck_id JOIN customers where customers.rownum=4;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  fprintf(f, "JOIN QUERIES IMPLICIT+ ROWNUM\n\n\n");

  q = "select trucks.rownum, cost, delcapacity, delcapacity_root, customers.rownum from trucks,truck,customers where truck.base=trucks.truck_id and customers.base=truck.customers_id and customers.rownum=4;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select trucks.rownum, cost, delcapacity, delcapacity_root, customers.rownum from trucks,truck,customers where truck.base=trucks.truck_id and customers.base=truck.customers_id and trucks.rownum=11 and customers.rownum=4;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select trucks.rownum, cost, delcapacity, delcapacity_root, customers.rownum, map_index from trucks,truck,mapindex,customers where truck.base=trucks.truck_id and customers.base=truck.customers_id and customers.rownum=mapindex.map_index;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select trucks.rownum, cost, delcapacity, delcapacity_root, customers.rownum from trucks,truck,customers where truck.base=trucks.truck_id and customers.rownum=4;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  fprintf(f, "JOIN QUERIES EXPLICIT + CONSTRAINTS\n\n\n");

  q = "select trucks.rownum, cost, delcapacity, delcapacity_root from trucks JOIN truck ON truck.base=trucks.truck_id and cost<800 and delcapacity>0;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select trucks.rownum, cost, delcapacity, delcapacity_root, code, demand from trucks JOIN truck ON truck.base=trucks.truck_id JOIN customers ON customers.base=truck.customers_id JOIN customer ON customer.base=customers.customer_id where (cost <100 or code>170) and delcapacity>0 and demand>10;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select trucks.rownum, cost, delcapacity, delcapacity_root, customers.rownum, code, demand from trucks left join truck on truck.base=trucks.truck_id left join customers on customers.base=truck.customers_id left join customer on customer.base=customers.customer_id where (cost <100 or delcapacity>0) and (code>170 or demand<10);";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);


  q = "select trucks.rownum, cost, delcapacity, delcapacity_root, customers.rownum, x_coord, y_coord from trucks left join truck on truck.base=trucks.truck_id left join customers on customers.base=truck.customers_id left join position on position.base=customers.customer_id where (cost <100 or delcapacity>0) and (x_coord>170 or y_coord<10);";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select trucks.rownum, cost, delcapacity, delcapacity_root, s.rownum, x_coord, y_coord from trucks left join truck on truck.base=trucks.truck_id left join customers s on s.base=truck.customers_id left join customer on customer.base=s.customer_id, customers c left join position on position.base=c.customer_id where (cost <100 or delcapacity>0) and (x_coord>170 or y_coord<10);";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select trucks.rownum, cost, delcapacity, delcapacity_root, s.rownum, x_coord, y_coord from trucks left join truck on truck.base=trucks.truck_id left join customers s on s.base=truck.customers_id left join customer on customer.base=s.customer_id, customers c left join position on position.base=c.customer_id where (cost <100 or delcapacity>0) and code<50 and (x_coord>170 or y_coord<10);";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select t.rownum, u.cost, u.delcapacity, u.delcapacity_root, c.rownum, code, demand from trucks t JOIN truck u ON u.base=t.truck_id JOIN customers c ON c.base=u.customers_id JOIN customer ON customer.base=c.customer_id, trucks tr JOIN truck k on k.base=tr.truck_id JOIN customers s ON s.base=k.customers_id JOIN position ON position.base=s.customer_id where (u.cost <100 or u.delcapacity>0) and (code>170 or demand<10) and (x_coord>40 or y_coord<20);";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select t.rownum, u.cost, u.delcapacity, u.delcapacity_root, c.rownum, code, demand from trucks t, truck u, customers c,trucks tr ,truck k,customer, customers s, position where u.base=t.truck_id and k.base=tr.truck_id and (u.cost <100 or u.delcapacity>0) and c.base=u.customers_id and customer.base=c.customer_id and s.base=k.customers_id and position.base=s.customer_id and (code>170 or demand<10) and (x_coord>40 or y_coord<20);";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  fprintf(f, "JOIN QUERIES EXPLICIT + CONSTRAINTS + ROWNUM\n\n\n");

  q = "select trucks.rownum, cost, delcapacity, delcapacity_root, customers.rownum, code, demand from trucks left join truck on truck.base=trucks.truck_id left join customers on customers.base=truck.customers_id left join customer on customer.base=customers.customer_id where (cost <100 or delcapacity>0) and (code>170 or demand<10) and customers.rownum=10;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  fprintf(f, "JOIN QUERIES IMPLICIT + CONSTRAINTS\n\n\n");

  q = "select trucks.rownum, cost, delcapacity, delcapacity_root from trucks,truck where truck.base=trucks.truck_id and cost<800 and delcapacity>0;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select trucks.rownum, cost, delcapacity, delcapacity_root, code, demand from trucks,truck,customers,customer where truck.base=trucks.truck_id and (cost <100 or code>170) and delcapacity>0 and customers.base=truck.customers_id and customer.base=customers.customer_id and demand>10;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select trucks.rownum, cost, delcapacity, delcapacity_root, customers.rownum, code, demand from trucks,truck,customers,customer where truck.base=trucks.truck_id and (cost <100 or delcapacity>0) and customers.base=truck.customers_id and customer.base=customers.customer_id and (code>170 or demand<10);";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select trucks.rownum, cost, delcapacity, delcapacity_root, customers.rownum, code, demand,x_coord,y_coord from trucks,truck,customers,customer, position where truck.base=trucks.truck_id and (cost <100 or delcapacity>0) and customers.base=truck.customers_id and customer.base=customers.customer_id and position.base=customer.position_id and (x_coord>170 or y_coord<10);";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);



  q = "select trucks.rownum, cost, delcapacity, delcapacity_root, c.rownum, code, demand, x_coord, y_coord from trucks,truck,customers c,customer, customers s, position where truck.base=trucks.truck_id and (cost <100 or delcapacity>0) and c.base=truck.customers_id and customer.base=c.customer_id and s.base=truck.customers_id and position.base=s.customer_id and (x_coord>170 or y_coord<10);";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select trucks.rownum, cost, delcapacity, delcapacity_root, c.rownum, code, demand, x_coord, y_coord from trucks,truck,customers c,customer, customers s, position where truck.base=trucks.truck_id and (cost <100 or delcapacity>0) and c.base=truck.customers_id and customer.base=c.customer_id and code<50 and s.base=truck.customers_id and position.base=s.customer_id and (x_coord>170 or y_coord<10);";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select trucks.rownum, cost, delcapacity, delcapacity_root, c.rownum, code, demand,x_coord,y_coord from trucks,truck,customers c,customer, customers s, position where truck.base=trucks.truck_id and (cost <100 or delcapacity>0) and c.base=truck.customers_id and customer.base=c.customer_id and s.base=truck.customers_id and position.base=s.customer_id and (code>170 or demand<10) and (x_coord>40 or y_coord<20);";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select t.rownum, u.cost, u.delcapacity, u.delcapacity_root, c.rownum, code, demand, x_coord, y_coord from trucks t, trucks tr, truck u,truck k,customers c,customer, customers s, position where u.base=t.truck_id and k.base=tr.truck_id and (u.cost <100 or u.delcapacity>0) and c.base=u.customers_id and customer.base=c.customer_id and s.base=k.customers_id and position.base=s.customer_id and (code>170 or demand<10) and (x_coord>40 or y_coord<20);";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select t.rownum, u.cost, u.delcapacity, u.delcapacity_root, c.rownum, code, demand from trucks t, truck u, customers c,trucks tr ,truck k,customer, customers s, position where u.base=t.truck_id and k.base=tr.truck_id and (u.cost <100 or u.delcapacity>0) and c.base=u.customers_id and customer.base=c.customer_id and s.base=k.customers_id and position.base=s.customer_id and (code>170 or demand<10) and (x_coord>40 or y_coord<20);";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select t.rownum, u.cost, u.delcapacity, u.delcapacity_root, c.rownum, cu.code, cu.demand, st.code,st.demand from trucks t, truck u, customers c,customer cu,trucks tr ,truck k, customers s, customer st where u.base=t.truck_id and k.base=tr.truck_id and (u.cost <100 or u.delcapacity>150) and c.base=u.customers_id and cu.base=c.customer_id and s.base=k.customers_id and st.base=s.customer_id and (cu.code>st.code or cu.demand<st.demand) LIMIT 10;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select c.code, c.demand, c.position_id, p.x_coord, p.y_coord, u.code, u.demand, u.position_id, o.x_coord, o.y_coord from trucks,truck,customers, customer c, position p, mapindex,customer u, position o where truck.base=trucks.truck_id and customers.base=truck.customers_id and c.base=customers.customer_id and p.base=c.position_id and c.code like '%99' and u.base=mapindex.customer_id and o.base=u.position_id and o.x_coord>133 and p.y_coord=o.y_coord;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);



  fprintf(f, "JOIN QUERIES IMPLICIT + CONSTRAINTS + ROWNUM\n\n\n");


  q = "select trucks.rownum, cost, delcapacity, delcapacity_root, customers.rownum, code, demand from trucks,truck,customers,customer where truck.base=trucks.truck_id and customers.base=truck.customers_id and customer.base=customers.customer_id and (cost <100 or delcapacity>0) and (code>170 or demand<10) and customers.rownum=10;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);


  fprintf(f, "SUBQUERIES\n\n\n");


  q = "select cost, delcapacity_root from (select DISTINCT truck_id from Trucks) t left join Truck on truck.base=t.truck_id where cost and not delcapacity and delcapacity_root=0;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select cost, delcapacity_root from (select distinct truck_id from Trucks) t, Truck where truck.base=t.truck_id and cost and not delcapacity and delcapacity_root=0;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select customers.rownum from (select distinct truck_id from Trucks) t left join Truck on truck.base=t.truck_id left join Customers on customers.base=truck.customers_id where customers.rownum=7;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select customers.rownum from (select distinct truck_id from Trucks) t left join Customers on customers.base=t.truck_id where customers.rownum=7;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select customers.rownum from (select distinct truck_id from Trucks) t,truck, Customers where truck.base=t.truck_id and customers.base=truck.customers_id and customers.rownum=7;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select customers.rownum from (select distinct truck_id from Trucks) t, Customers where customers.base=t.truck_id and customers.rownum=7;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select cost, delcapacity,delcapacity_root from (select distinct truck_id from Trucks) t left join Truck on truck.base=t.truck_id where cost<500 and (delcapacity>50 or delcapacity_root>4);";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select t.rownum,cost,delcapacity,delcapacity_root, customers.rownum from (select distinct truck_id,rownum from Trucks) t left join Truck on truck.base=t.truck_id left join Customers on customers.base=truck.customers_id where customers.rownum=7 and t.rownum=12;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select t.rownum,cost,delcapacity,delcapacity_root, customers.rownum from (select distinct truck_id,rownum from Trucks) t left join Truck on truck.base=t.truck_id left join Customers on customers.base=truck.customers_id where customers.rownum=7 and t.rownum=12 and cost<500 and (delcapacity=0 or delcapacity_root=0);";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select t.rownum,cost,delcapacity,delcapacity_root, customers.rownum from (select distinct truck_id,rownum from Trucks) t left join Truck on truck.base=t.truck_id left join Customers on customers.base=truck.customers_id where customers.rownum=7 and t.rownum=12 and cost<500 and delcapacity>0 and delcapacity_root>0;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);


  fprintf(f, "COMPOUND QUERIES\n\n\n");


  q = "select code, demand, x_coord, y_coord from trucks,truck,customers, customer, position where truck.base=trucks.truck_id and customers.base=truck.customers_id and customer.base=customers.customer_id and position.base=customer.position_id and code like '%99' UNION select code, demand, x_coord,y_coord from mapindex,customer,position where customer.base=mapindex.customer_id and position.base=customer.position_id and x_coord>133;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select code, demand, x_coord, y_coord from trucks,truck,customers, customer, position where truck.base=trucks.truck_id and customers.base=truck.customers_id and customer.base=customers.customer_id and position.base=customer.position_id and code like '%99' UNION ALL select code, demand, x_coord,y_coord from mapindex,customer,position where customer.base=mapindex.customer_id and position.base=customer.position_id and x_coord>133;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select code, demand, x_coord, y_coord from trucks,truck,customers, customer, position where truck.base=trucks.truck_id and customers.base=truck.customers_id and customer.base=customers.customer_id and position.base=customer.position_id and code like '%99' INTERSECT select code, demand, x_coord,y_coord from mapindex,customer,position where customer.base=mapindex.customer_id and position.base=customer.position_id and x_coord>133;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select code, demand, x_coord, y_coord from trucks,truck,customers, customer, position where truck.base=trucks.truck_id and customers.base=truck.customers_id and customer.base=customers.customer_id and position.base=customer.position_id and code like '%99' EXCEPT select code, demand, x_coord,y_coord from mapindex,customer,position where customer.base=mapindex.customer_id and position.base=customer.position_id and x_coord>133;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);


  fprintf(f, "AGGREGATIONS\n\n\n");


  q = "select sum(map_index), demand from (select * from mapindex where map_index<187) as m, Customer where customer.base=m.customer_id GROUP BY demand";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select sum(map_index), demand from (select * from mapindex where map_index<187) as m, Customer where customer.base=m.customer_id GROUP BY demand HAVING sum(map_index)>200;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);


  fprintf(f, "ORDERING\n\n\n");


  q = "select trucks.rownum, cost, delcapacity, delcapacity_root, customers.rownum, code, demand from trucks,truck,customers,customer where truck.base=trucks.truck_id and cost <800 and delcapacity>0 and customers.base=truck.customers_id and customer.base=customers.customer_id and code>100 and demand>10 ORDER BY code;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select trucks.rownum, cost, delcapacity, delcapacity_root, customers.rownum, code, demand from trucks,truck,customers,customer where truck.base=trucks.truck_id and cost <800 and delcapacity>0 and customers.base=truck.customers_id and customer.base=customers.customer_id and code>100 and demand>10 ORDER BY code DESC;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  fprintf(f, "ARITHMETIC OPERATORS\n\n\n");

  q = "select t.rownum, u.cost, u.delcapacity, u.delcapacity_root, c.rownum, cu.code, cu.demand, st.code,st.demand from trucks t, truck u, customers c,customer cu,trucks tr ,truck k, customers s, customer st where u.base=t.truck_id and k.base=tr.truck_id and (u.cost <100 or u.delcapacity>0) and c.base=u.customers_id and cu.base=c.customer_id and s.base=k.customers_id and st.base=s.customer_id and (cu.code>st.code or cu.demand - st.demand >20) LIMIT 10;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select t.rownum, u.cost,u.delcapacity, cu.code>>1, cu.demand, st.code,st.demand from trucks t, truck u, customers c,customer cu,trucks tr ,truck k, customers s, customer st where u.base=t.truck_id and k.base=tr.truck_id and (u.cost <100 or u.delcapacity>0) and c.base=u.customers_id and cu.base=c.customer_id and s.base=k.customers_id and st.base=s.customer_id and cu.code>>1>st.code and cu.demand - st.demand >20 LIMIT 10 OFFSET 10;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select t.rownum, u.cost,u.delcapacity, cu.code>>1, cu.demand, length(st.code),st.demand from trucks t, truck u, customers c,customer cu,trucks tr ,truck k, customers s, customer st where u.base=t.truck_id and k.base=tr.truck_id and (u.cost <100 or u.delcapacity>0) and c.base=u.customers_id and cu.base=c.customer_id and s.base=k.customers_id and st.base=s.customer_id and cu.code>>1>st.code and cu.demand - st.demand >20 LIMIT 10 OFFSET 5;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select t.rownum, u.cost, cu.code>>1, cu.demand, length(st.code),~st.demand from trucks t, truck u, customers c,customer cu,trucks tr ,truck k, customers s, customer st where u.base=t.truck_id and k.base=tr.truck_id and (u.cost <100 or u.delcapacity>0) and c.base=u.customers_id and cu.base=c.customer_id and s.base=k.customers_id and st.base=s.customer_id and cu.code>>1>st.code and cu.demand - ~st.demand >20 LIMIT 15;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);


  fprintf(f, "EXISTS\n\n\n");


  q = "select rownum from trucks where exists (select * from truck where truck.base = trucks.truck_id);";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select trucks.rownum, cost, delcapacity,customers.rownum, c.code, c.demand from trucks JOIN truck on truck.base = trucks.truck_id join customers on customers.base=truck.customers_id join Customer c on c.base=customers.customer_id where exists (select * from Mapindex join Customer u on u.base=mapindex.customer_id where u.code='159' and u.demand>c.demand) LIMIT 10;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);


  fprintf(f, "FUNCTIONS\n\n\n");


  q = "select trucks.rownum, cost, delcapacity,customers.rownum, c.code, c.demand from trucks JOIN truck on truck.base = trucks.truck_id join customers on customers.base=truck.customers_id join Customer c on c.base=customers.customer_id where c.code glob '*59';";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select code, demand, x_coord, y_coord from trucks,truck,customers, customer, position where truck.base=trucks.truck_id and customers.base=truck.customers_id and customer.base=customers.customer_id and position.base=customer.position_id and code LIKE '%69' ESCAPE '%' INTERSECT select code, demand, x_coord,y_coord from mapindex,customer,position where customer.base=mapindex.customer_id and position.base=customer.position_id and x_coord>133;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);


  fprintf(f, "CASE\n\n\n");


  q = "select summed, (select case when demand>30 then 'high' when demand between 25 and 30 then 'moderate' when demand between 20 and 24 then 'towards moderate' when demand between 10 and 19 then 'low' when demand<10 then 'tiny' end) tag from (select sum(map_index) summed, demand from (select * from mapindex where map_index<187) as m, Customer where customer.base=m.customer_id group by demand) grouped;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select c.code, p.x_coord, p.y_coord, u.code, o.x_coord, o.y_coord, (select case when (p.x_coord*p.x_coord-o.x_coord*o.x_coord) + (p.y_coord*p.y_coord-o.y_coord*o.y_coord)<=100 then c.code||'-'||u.code||' are close' when(p.x_coord*p.x_coord-o.x_coord*o.x_coord) + (p.y_coord*p.y_coord-o.y_coord*o.y_coord)>100 then c.code||'-'||u.code||' are far' end) distanceP2 from trucks,truck,customers, customer c, position p, mapindex,customer u, position o where truck.base=trucks.truck_id and customers.base=truck.customers_id and c.base=customers.customer_id and p.base=c.position_id and c.code like '%99' and u.base=mapindex.customer_id and u.code like '_50' and o.base=u.position_id;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select c.code, p.x_coord, p.y_coord, u.code, u.demand, o.x_coord, o.y_coord, (select case when (p.x_coord*p.x_coord-o.x_coord*o.x_coord) + (p.y_coord*p.y_coord-o.y_coord*o.y_coord)<=100 then c.code||'-'||u.code||' are close' when(p.x_coord*p.x_coord-o.x_coord*o.x_coord) + (p.y_coord*p.y_coord-o.y_coord*o.y_coord)>100 then c.code||'-'||u.code||' are far' end) distanceP2 from trucks,truck,customers, customer c, position p, mapindex,customer u, position o where truck.base=trucks.truck_id and customers.base=truck.customers_id and c.base=customers.customer_id and p.base=c.position_id and c.code like '%99' and u.base=mapindex.customer_id and o.base=u.position_id and u.code in ('050', '011');";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  deinit_vt_selectors();
  sqlite3_close(db);
  fclose(f);
  if (system("./pico_ql_diff_test.sh")) {
    printf("Invoking pico_ql_diff_test script failed.\n");
    exit(1);
  }
  return SQLITE_OK;
}
