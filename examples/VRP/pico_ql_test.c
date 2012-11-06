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

  q = "select rownum,cost,delcapacity from truck;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select * from customer;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  fprintf(f, "PLAIN QUERIES + ROWNUM\n\n\n");

  q = "select rownum,cost,delcapacity from truck where rownum=7;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select * from truck where rownum=20;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select * from customer where rownum=20;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  fprintf(f, "JOIN QUERIES EXPLICIT\n\n\n");

  q = "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum, demand, code from truck JOIN customer ON customer.base=truck.customers_id LIMIT 20;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum, demand, code from truck INNER JOIN customer ON customer.base=truck.customers_id LIMIT 20;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum, demand, code from truck LEFT JOIN customer ON customer.base=truck.customers_id LIMIT 20;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum, demand, code from truck CROSS JOIN customer ON customer.base=truck.customers_id LIMIT 20;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum, demand, code from truck CROSS JOIN customer;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select * from truck JOIN customer;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  fprintf(f, "JOIN QUERIES IMPLICIT\n\n\n");

  q = "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum, demand, code from truck, customer WHERE customer.base=truck.customers_id LIMIT 20;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select * from truck, customer;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  fprintf(f, "JOIN QUERIES EXPLICIT+ ROWNUM\n\n\n");

  q = "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum, demand, code from truck JOIN customer ON customer.base=truck.customers_id WHERE customer.rownum=4;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum, demand, code from truck JOIN customer ON customer.base=truck.customers_id WHERE truck.rownum=11 and customer.rownum=4;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum, map_index from truck JOIN customer ON customer.base=truck.customers_id,mapindex where customer.rownum=mapindex.map_index;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  fprintf(f, "JOIN QUERIES IMPLICIT+ ROWNUM\n\n\n");

  q = "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum from truck,customer where customer.base=truck.customers_id and customer.rownum=4;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum from truck,customer where customer.base=truck.customers_id and truck.rownum=11 and customer.rownum=4;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum, map_index from truck,mapindex,customer where customer.base=truck.customers_id and customer.rownum=mapindex.map_index;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  fprintf(f, "JOIN QUERIES EXPLICIT + CONSTRAINTS\n\n\n");

  q = "select truck.rownum, cost, delcapacity, delcapacity_root from truck WHERE cost<800 and delcapacity>0;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select truck.rownum, cost, delcapacity, delcapacity_root, code, demand from truck JOIN customer ON customer.base=truck.customers_id WHERE (cost <100 or code>170) and delcapacity>0 and demand>10;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum, code, demand from truck left join customer on customer.base=truck.customers_id where (cost <100 or delcapacity>0) and (code>170 or demand<10);";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum, x_coord, y_coord from truck left join customer on customer.base=truck.customers_id where (cost <100 or delcapacity>0) and (x_coord>170 or y_coord<10);";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select truck.rownum, cost, delcapacity, delcapacity_root, s.rownum, x_coord, y_coord from truck left join customer s on s.base=truck.customers_id where (cost <100 or delcapacity>0) and (x_coord>170 or y_coord<10);";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select truck.rownum, cost, delcapacity, delcapacity_root, s.rownum, code, x_coord, y_coord from truck left join customer s on s.base=truck.customers_id where (cost <100 or delcapacity>0) and code<'050' and (x_coord>170 or y_coord<10);";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select u.rownum, u.cost, u.delcapacity, u.delcapacity_root, c.rownum, c.code, c.demand,k.rownum, k.cost, k.delcapacity, k.delcapacity_root, s.rownum, s.code, s.demand from truck u JOIN customer c ON c.base=u.customers_id, truck k JOIN customer s ON s.base=k.customers_id where (u.cost <100 or u.delcapacity>0) and (c.code>170 or c.demand<10) and (s.x_coord>40 or c.y_coord<20) LIMIT 20;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select u.rownum, u.cost, u.delcapacity, u.delcapacity_root, c.rownum, c.code, c.demand,k.rownum, k.cost, k.delcapacity, k.delcapacity_root, s.rownum, s.code, s.demand from truck u, customer c, truck k, customer s where (u.cost <100 or u.delcapacity>0) and c.base=u.customers_id and s.base=k.customers_id and (c.code>170 or c.demand<10) and (s.x_coord>40 or c.y_coord<20) LIMIT 20;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  fprintf(f, "JOIN QUERIES EXPLICIT + CONSTRAINTS + ROWNUM\n\n\n");

  q = "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum, code, demand from truck join customer on customer.base=truck.customers_id where (cost <100 or delcapacity>0) and (code>170 or demand<10) and customer.rownum=10;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  fprintf(f, "JOIN QUERIES IMPLICIT + CONSTRAINTS\n\n\n");

  q = "select truck.rownum, cost, delcapacity, delcapacity_root from truck where cost<800 and delcapacity>0;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select truck.rownum, cost, delcapacity, delcapacity_root, code, demand from truck, customer where (cost <100 or code>170) and delcapacity>0 and customer.base=truck.customers_id and demand>10;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum, code, demand from truck, customer where (cost <100 or delcapacity>0) and customer.base=truck.customers_id and (code>170 or demand<10);";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum, code, demand,x_coord,y_coord from truck, customer where (cost <100 or delcapacity>0) and customer.base=truck.customers_id and (x_coord>170 or y_coord<10);";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select truck.rownum, cost, delcapacity, delcapacity_root, c.rownum, c.code, c.demand, c.x_coord, c.y_coord from truck,customer c, customer s where (cost <100 or delcapacity>0) and c.base=truck.customers_id and s.base=truck.customers_id and (s.x_coord>170 or c.y_coord<10);";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select truck.rownum, cost, delcapacity, delcapacity_root, c.rownum, c.code, c.demand, c.x_coord, c.y_coord from truck,customer c, customer s where (cost <100 or delcapacity>0) and c.base=truck.customers_id and c.code<50 and s.base=truck.customers_id and (s.x_coord>170 or s.y_coord<10);";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select truck.rownum, cost, delcapacity, delcapacity_root, c.rownum, c.code, c.demand,c.x_coord,c.y_coord from truck,customer c, customer s where (cost <100 or delcapacity>0) and c.base=truck.customers_id and s.base=truck.customers_id and (c.code>170 or c.demand<10) and (s.x_coord>40 or c.y_coord<20);";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select u.rownum, u.cost, u.delcapacity, u.delcapacity_root, c.rownum, c.code, c.demand, c.x_coord, c.y_coord,k.rownum, k.cost, k.delcapacity, k.delcapacity_root, s.rownum, s.code, s.demand,s.x_coord,s.y_coord from truck u, truck k,customer c, customer s where (u.cost <100 or u.delcapacity>0) and c.base=u.customers_id and s.base=k.customers_id and (c.code>170 or c.demand<10) and (s.x_coord>40 or s.y_coord<20) LIMIT 20;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select u.rownum, u.cost, u.delcapacity, u.delcapacity_root, c.rownum, c.code, c.demand,k.rownum, k.cost, k.delcapacity, k.delcapacity_root, s.rownum, s.code, s.demand,s.x_coord,s.y_coord from truck u, customer c,truck k, customer s where (u.cost <100 or u.delcapacity>0) and c.base=u.customers_id and s.base=k.customers_id and (c.code>170 or c.demand<10) and (s.x_coord>40 or s.y_coord<20) LIMIT 20;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select u.rownum, u.cost, u.delcapacity, u.delcapacity_root, c.rownum, c.code, c.demand, s.code,s.demand from truck u, customer c,truck k, customer s where (u.cost <100 or u.delcapacity>150) and c.base=u.customers_id and s.base=k.customers_id and (c.code>s.code or c.demand<s.demand) LIMIT 10;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select c.code, c.demand, c.x_coord, c.y_coord, c.code, c.demand, c.x_coord, c.y_coord from truck, customer c, mapindex where c.base=truck.customers_id and c.code like '%99' and mapindex.x_coord>133 and c.y_coord=mapindex.y_coord;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);



  fprintf(f, "JOIN QUERIES IMPLICIT + CONSTRAINTS + ROWNUM\n\n\n");


  q = "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum, code, demand from truck,customer where customer.base=truck.customers_id and (cost <100 or delcapacity>0) and (code>170 or demand<10) and customer.rownum=10;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);


  fprintf(f, "SUBQUERIES\n\n\n");


  q = "select t.cost, t.delcapacity_root, rownum, code,demand from (select DISTINCT customers_id,cost, delcapacity_root from Truck) t left join Customer on customer.base=t.customers_id where t.cost and t.delcapacity_root=0 and demand>30;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select t.cost, t.delcapacity_root, rownum, code, demand from (select distinct customers_id,cost, delcapacity_root from Truck) t, Customer where customer.base=t.customers_id and t.cost and t.delcapacity_root=0 and code>180;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select t.rownum, t.cost,t.delcapacity,customer.rownum from (select distinct rownum, cost,delcapacity,customers_id from Truck) t left join customer on customer.base=t.customers_id where customer.rownum=7;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select t.rownum, t.cost,t.delcapacity,customer.rownum from (select distinct rownum, cost, delcapacity,customers_id from Truck) t,Customer where customer.base=t.customers_id and customer.rownum=7;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select t.cost, t.delcapacity,t.delcapacity_root, rownum from (select distinct customers_id,cost, delcapacity,delcapacity_root from Truck) t left join customer on customer.base=t.customers_id where t.cost<500 and (t.delcapacity>50 or t.delcapacity_root>4) and customer.rownum=3;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select t.rownum,t.cost,t.delcapacity,t.delcapacity_root, customer.rownum from (select distinct customers_id,rownum,cost,delcapacity,delcapacity_root from Truck) t left join Customer on customer.base=t.customers_id where customer.rownum=7 and t.rownum=12;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select t.rownum,t.cost,t.delcapacity,t.delcapacity_root, customer.rownum from (select distinct customers_id,rownum,cost,delcapacity,delcapacity_root from Truck) t left join Customer on customer.base=t.customers_id where customer.rownum=7 and t.rownum=12 and t.cost<500 and (t.delcapacity=0 or t.delcapacity_root=0);";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select t.rownum,t.cost,t.delcapacity,t.delcapacity_root, customer.rownum from (select distinct customers_id,rownum,cost,delcapacity,delcapacity_root from Truck) t left join Customer on customer.base=t.customers_id where customer.rownum=7 and t.rownum=12 and t.cost<500 and t.delcapacity>0 and t.delcapacity_root>0;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);


  fprintf(f, "COMPOUND QUERIES\n\n\n");


  q = "select code, demand, x_coord, y_coord from truck, customer where customer.base=truck.customers_id and code like '%99' UNION select code, demand, x_coord,y_coord from mapindex where x_coord>133;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select code, demand, x_coord, y_coord from truck, customer where customer.base=truck.customers_id and code like '%99' UNION ALL select code, demand, x_coord,y_coord from mapindex where x_coord>133;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select code, demand, x_coord, y_coord from truck, customer where customer.base=truck.customers_id and code like '%99' INTERSECT select code, demand, x_coord,y_coord from mapindex where x_coord>133;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select code, demand, x_coord, y_coord from truck, customer where customer.base=truck.customers_id and code like '%99' EXCEPT select code, demand, x_coord,y_coord from mapindex where x_coord>133;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);


  fprintf(f, "AGGREGATIONS\n\n\n");


  q = "select sum(m.map_index), m.demand from (select * from mapindex where map_index<187) as m GROUP BY m.demand";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select sum(m.map_index), m.demand from (select * from mapindex where map_index<187) as m GROUP BY m.demand HAVING sum(m.map_index)>200;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);


  fprintf(f, "ORDERING\n\n\n");


  q = "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum, code, demand from truck,customer where cost <800 and delcapacity>0 and customer.base=truck.customers_id and code>100 and demand>10 ORDER BY code;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum, code, demand from truck,customer where cost <800 and delcapacity>0 and customer.base=truck.customers_id and code>100 and demand>10 ORDER BY code DESC;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  fprintf(f, "ARITHMETIC OPERATORS\n\n\n");

  q = "select u.rownum, u.cost, u.delcapacity, u.delcapacity_root, c.rownum, c.code, c.demand, s.code,s.demand from truck u, customer c,truck k, customer s where (u.cost <100 or u.delcapacity>0) and c.base=u.customers_id and s.base=k.customers_id and (c.code>s.code or c.demand - s.demand >20) LIMIT 10;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select u.rownum, u.cost, u.delcapacity, u.delcapacity_root, c.rownum, c.code>>1, c.demand, s.code,s.demand from truck u, customer c,truck k, customer s where (u.cost <100 or u.delcapacity>0) and c.base=u.customers_id and s.base=k.customers_id and (c.code>>1>s.code or c.demand - s.demand >20) LIMIT 10 OFFSET 10;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select u.rownum, u.cost, u.delcapacity, u.delcapacity_root, c.rownum, c.code>>1, c.demand, LENGTH(s.code),s.demand from truck u, customer c,truck k, customer s where (u.cost <100 or u.delcapacity>0) and c.base=u.customers_id and s.base=k.customers_id and (c.code>>1>s.code or c.demand - s.demand >20) LIMIT 10 OFFSET 5;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select u.rownum, u.cost, u.delcapacity, u.delcapacity_root, c.rownum, c.code>>1, c.demand, LENGTH(s.code),~s.demand from truck u, customer c,truck k, customer s where (u.cost <100 or u.delcapacity>0) and c.base=u.customers_id and s.base=k.customers_id and (c.code>>1>s.code or c.demand - ~s.demand >20) LIMIT 15;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);


  fprintf(f, "EXISTS\n\n\n");


  q = "select rownum from truck where exists (select * from truck);";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select truck.rownum, cost, delcapacity,customer.rownum, customer.code, customer.demand from truck join customer on customer.base=truck.customers_id where exists (select * from Mapindex where mapindex.code='159' and mapindex.demand>customer.demand) LIMIT 10;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);


  fprintf(f, "FUNCTIONS\n\n\n");


  q = "select truck.rownum, cost, delcapacity,c.rownum, c.code, c.demand from truck join customer c on c.base=truck.customers_id where c.code glob '*59';";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select code, demand, x_coord, y_coord from truck, customer where customer.base=truck.customers_id and code LIKE '%69' ESCAPE '%' INTERSECT select code, demand, x_coord,y_coord from mapindex where x_coord>133;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);


  fprintf(f, "CASE\n\n\n");


  q = "select summed, (select case when demand>30 then 'high' when demand between 25 and 30 then 'moderate' when demand between 20 and 24 then 'towards moderate' when demand between 10 and 19 then 'low' when demand<10 then 'tiny' end) tag from (select sum(map_index) summed, demand from (select * from mapindex where map_index<187) as m group by demand) grouped;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select c.code, c.x_coord, c.y_coord, mapindex.code, mapindex.x_coord, mapindex.y_coord, (select case when (c.x_coord*c.x_coord-mapindex.x_coord*mapindex.x_coord) + (c.y_coord*c.y_coord-mapindex.y_coord*mapindex.y_coord)<=100 then c.code||'-'||mapindex.code||' are close' when(c.x_coord*c.x_coord-mapindex.x_coord*mapindex.x_coord) + (c.y_coord*c.y_coord-mapindex.y_coord*mapindex.y_coord)>100 then c.code||'-'||mapindex.code||' are far' end) distanceP2 from truck, customer c, mapindex where c.base=truck.customers_id and c.code like '%99' and mapindex.code like '_50';";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select c.code, c.x_coord, c.y_coord, mapindex.code, mapindex.x_coord, mapindex.y_coord, (select case when (c.x_coord*c.x_coord-mapindex.x_coord*mapindex.x_coord) + (c.y_coord*c.y_coord-mapindex.y_coord*mapindex.y_coord)<=100 then c.code||'-'||mapindex.code||' are close' when(c.x_coord*c.x_coord-mapindex.x_coord*mapindex.x_coord) + (c.y_coord*c.y_coord-mapindex.y_coord*mapindex.y_coord)>100 then c.code||'-'||mapindex.code||' are far' end) distanceP2 from truck, customer c, mapindex where c.base=truck.customers_id and c.code like '%99' and mapindex.code in ('050','011');";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  result = test_prep_exec(f, db, q);

  q = "select * from MyTrucks;";
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
