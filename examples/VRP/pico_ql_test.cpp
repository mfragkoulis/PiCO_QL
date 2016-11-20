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

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <sstream>
using namespace std;

#include "pico_ql.h"
using namespace picoQL;

int exec_tests() {
  stringstream s;
  fstream fs;
  fs.open("vrp_test_current.txt", fstream::out);
  int i = 1;
  char q[1024];

  fs << "PLAIN QUERIES\n\n\n";

  strcpy(q, "select rownum,cost,delcapacity from truck;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select * from customer;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  fs << "PLAIN QUERIES + ROWNUM\n\n\n";

  strcpy(q, "select rownum,cost,delcapacity from truck where rownum=7;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select * from truck where rownum=20;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select * from customer where rownum=20;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  fs << "JOIN QUERIES EXPLICIT\n\n\n";

  strcpy(q, "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum, demand, code from truck JOIN customer ON customer.base=truck.customers_id LIMIT 20;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum, demand, code from truck INNER JOIN customer ON customer.base=truck.customers_id LIMIT 20;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum, demand, code from truck LEFT JOIN customer ON customer.base=truck.customers_id LIMIT 20;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum, demand, code from truck CROSS JOIN customer ON customer.base=truck.customers_id LIMIT 20;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum, demand, code from truck CROSS JOIN customer;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select * from truck JOIN customer;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select * from truck JOIN customer ON customer.base=234;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  fs << "JOIN QUERIES IMPLICIT\n\n\n";

  strcpy(q, "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum, demand, code FROM truck, customer WHERE customer.base=truck.customers_id LIMIT 20;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select * from truck, customer;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select * from truck, customer WHERE customer.base=234;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  fs << "JOIN QUERIES EXPLICIT+ ROWNUM\n\n\n";

  strcpy(q, "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum, demand, code from truck JOIN customer ON customer.base=truck.customers_id WHERE customer.rownum=4;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum, demand, code from truck JOIN customer ON customer.base=truck.customers_id WHERE truck.rownum=11 and customer.rownum=4;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum, map_index from truck JOIN customer ON customer.base=truck.customers_id,mapindex where customer.rownum=mapindex.map_index;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  fs << "JOIN QUERIES IMPLICIT+ ROWNUM\n\n\n";

  strcpy(q, "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum from truck,customer where customer.base=truck.customers_id and customer.rownum=4;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum from truck,customer where customer.base=truck.customers_id and truck.rownum=11 and customer.rownum=4;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum, map_index from truck,mapindex,customer where customer.base=truck.customers_id and customer.rownum=mapindex.map_index;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  fs << "JOIN QUERIES EXPLICIT + CONSTRAINTS\n\n\n";

  strcpy(q, "select truck.rownum, cost, delcapacity, delcapacity_root from truck WHERE cost<800 and delcapacity>0;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select truck.rownum, cost, delcapacity, delcapacity_root, code, demand from truck JOIN customer ON customer.base=truck.customers_id WHERE (cost <100 or code>170) and delcapacity>0 and demand>10;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum, code, demand from truck left join customer on customer.base=truck.customers_id where (cost <100 or delcapacity>0) and (code>170 or demand<10);");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum, x_coord, x_coord_root, y_coord, y_coord_root from truck left join customer on customer.base=truck.customers_id where (cost <100 or delcapacity>0) and (x_coord>170 or y_coord<10);");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select truck.rownum, cost, delcapacity, delcapacity_root, s.rownum, x_coord, x_coord_root, y_coord, y_coord_root from truck left join customer s on s.base=truck.customers_id where (cost <100 or delcapacity>0) and (x_coord>170 or y_coord<10);");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select truck.rownum, cost, delcapacity, delcapacity_root, s.rownum, code, x_coord, x_coord_root, y_coord, y_coord_root from truck left join customer s on s.base=truck.customers_id where (cost <100 or delcapacity>0) and code<'050' and (x_coord>170 or y_coord<10);");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select u.rownum, u.cost, u.delcapacity, u.delcapacity_root, c.rownum, c.code, c.demand,k.rownum, k.cost, k.delcapacity, k.delcapacity_root, s.rownum, s.code, s.demand from truck u JOIN customer c ON c.base=u.customers_id, truck k JOIN customer s ON s.base=k.customers_id where (u.cost <100 or u.delcapacity>0) and (c.code>170 or c.demand<10) and (s.x_coord>40 or c.y_coord<20) LIMIT 20;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select u.rownum, u.cost, u.delcapacity, u.delcapacity_root, c.rownum, c.code, c.demand,k.rownum, k.cost, k.delcapacity, k.delcapacity_root, s.rownum, s.code, s.demand from truck u, customer c, truck k, customer s where (u.cost <100 or u.delcapacity>0) and c.base=u.customers_id and s.base=k.customers_id and (c.code>170 or c.demand<10) and (s.x_coord>40 or c.y_coord<20) LIMIT 20;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  fs << "JOIN QUERIES EXPLICIT + CONSTRAINTS + ROWNUM\n\n\n";

  strcpy(q, "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum, code, demand from truck join customer on customer.base=truck.customers_id where (cost <100 or delcapacity>0) and (code>170 or demand<10) and customer.rownum=10;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  fs << "JOIN QUERIES IMPLICIT + CONSTRAINTS\n\n\n";

  strcpy(q, "select truck.rownum, cost, delcapacity, delcapacity_root from truck where cost<800 and delcapacity>0;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select truck.rownum, cost, delcapacity, delcapacity_root, code, demand from truck, customer where (cost <100 or code>170) and delcapacity>0 and customer.base=truck.customers_id and demand>10;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum, code, demand from truck, customer where (cost <100 or delcapacity>0) and customer.base=truck.customers_id and (code>170 or demand<10);");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum, code, demand,x_coord,y_coord from truck, customer where (cost <100 or delcapacity>0) and customer.base=truck.customers_id and (x_coord>170 or y_coord<10);");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select truck.rownum, cost, delcapacity, delcapacity_root, c.rownum, c.code, c.demand, c.x_coord, c.x_coord_root, c.y_coord, c.y_coord_root, s.rownum, s.code, s.demand, s.x_coord, s.x_coord_root, s.y_coord, s.y_coord_root from truck,customer c, customer s where (cost <100 or delcapacity>0) and c.base=truck.customers_id and s.base=truck.customers_id and (s.x_coord>170 or c.y_coord<10);");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select truck.rownum, cost, delcapacity, delcapacity_root, c.rownum, c.code, c.demand, c.x_coord, c.x_coord_root, c.y_coord, c.y_coord_root, s.rownum, s.code, s.demand, s.x_coord, s.x_coord_root, s.y_coord, s.y_coord_root from truck,customer c, customer s where (cost <100 or delcapacity>0) and c.base=truck.customers_id and c.code<50 and s.base=truck.customers_id and (s.x_coord>170 or s.y_coord<10);");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select truck.rownum, cost, delcapacity, delcapacity_root, c.rownum, c.code, c.demand, c.x_coord, c.x_coord_root, c.y_coord, c.y_coord_root, s.rownum, s.code, s.demand, s.x_coord, s.x_coord_root, s.y_coord, s.y_coord_root from truck,customer c, customer s where (cost <100 or delcapacity>0) and c.base=truck.customers_id and s.base=truck.customers_id and (c.code>170 or c.demand<10) and (s.x_coord>40 or c.y_coord<20);");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select u.rownum, u.cost, u.delcapacity, u.delcapacity_root, c.rownum, c.code, c.demand, c.x_coord, c.x_coord_root, c.y_coord, c.y_coord_root, k.rownum, k.cost, k.delcapacity, k.delcapacity_root, s.rownum, s.code, s.demand,s.x_coord, s.x_coord_root, s.y_coord, s.y_coord_root from truck u, truck k,customer c, customer s where (u.cost <100 or u.delcapacity>0) and c.base=u.customers_id and s.base=k.customers_id and (c.code>170 or c.demand<10) and (s.x_coord>40 or s.y_coord<20) LIMIT 20;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select u.rownum, u.cost, u.delcapacity, u.delcapacity_root, c.rownum, c.code, c.demand,k.rownum, k.cost, k.delcapacity, k.delcapacity_root, s.rownum, s.code, s.demand,s.x_coord, s.x_coord_root, s.y_coord, s.y_coord_root from truck u, customer c,truck k, customer s where (u.cost <100 or u.delcapacity>0) and c.base=u.customers_id and s.base=k.customers_id and (c.code>170 or c.demand<10) and (s.x_coord>40 or s.y_coord<20) LIMIT 20;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select u.rownum, u.cost, u.delcapacity, u.delcapacity_root, c.rownum, c.code, c.demand, s.code,s.demand from truck u, customer c,truck k, customer s where (u.cost <100 or u.delcapacity>150) and c.base=u.customers_id and s.base=k.customers_id and (c.code>s.code or c.demand<s.demand) LIMIT 10;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select c.code, c.demand, c.x_coord, c.x_coord_root, c.y_coord, c.y_coord_root, mapindex.customercode, mapindex.customerdemand, mapindex.customerx_coord, mapindex.customerx_coord_root, mapindex.customery_coord_root from truck, customer c, mapindex where c.base=truck.customers_id and c.code like '%99' and mapindex.Customerx_coord>133 and c.y_coord=mapindex.Customery_coord;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  fs << "JOIN QUERIES IMPLICIT + CONSTRAINTS + ROWNUM\n\n\n";

  strcpy(q, "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum, code, demand from truck,customer where customer.base=truck.customers_id and (cost <100 or delcapacity>0) and (code>170 or demand<10) and customer.rownum=10;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  fs << "SUBQUERIES\n\n\n";

  strcpy(q, "select t.cost, t.delcapacity_root, rownum, code,demand from (select DISTINCT customers_id,cost, delcapacity_root from Truck) t left join Customer on customer.base=t.customers_id where t.cost and t.delcapacity_root=0 and demand>30;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select t.cost, t.delcapacity_root, rownum, code,demand from (select DISTINCT customers_id,cost, delcapacity_root from Truck where cost < 400) t left join Customer on customer.base=t.customers_id where demand>30 order by t.cost;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select t.cost, t.delcapacity_root, rownum, code, demand from (select distinct customers_id,cost, delcapacity_root from Truck) t, Customer where customer.base=t.customers_id and t.cost and t.delcapacity_root=0 and code>180 order by t.cost;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select t.rownum, t.cost,t.delcapacity,customer.rownum from (select distinct rownum, cost,delcapacity,customers_id from Truck) t left join customer on customer.base=t.customers_id where customer.rownum=7;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select t.rownum, t.cost,t.delcapacity,customer.rownum from (select distinct rownum, cost, delcapacity,customers_id from Truck) t,Customer where customer.base=t.customers_id and customer.rownum=7;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select t.cost, t.delcapacity,t.delcapacity_root, rownum from (select distinct customers_id,cost, delcapacity,delcapacity_root from Truck) t left join customer on customer.base=t.customers_id where t.cost<500 and (t.delcapacity>50 or t.delcapacity_root>4) and customer.rownum=3;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select t.rownum,t.cost,t.delcapacity,t.delcapacity_root, customer.rownum from (select distinct customers_id,rownum,cost,delcapacity,delcapacity_root from Truck) t left join Customer on customer.base=t.customers_id where customer.rownum=7 and t.rownum=12;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select t.rownum,t.cost,t.delcapacity,t.delcapacity_root, customer.rownum from (select distinct customers_id,rownum,cost,delcapacity,delcapacity_root from Truck) t left join Customer on customer.base=t.customers_id where customer.rownum=7 and t.rownum=12 and t.cost<500 and (t.delcapacity=0 or t.delcapacity_root=0);");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select t.rownum,t.cost,t.delcapacity,t.delcapacity_root, customer.rownum from (select distinct customers_id,rownum,cost,delcapacity,delcapacity_root from Truck) t left join Customer on customer.base=t.customers_id where customer.rownum=7 and t.rownum=12 and t.cost<500 and t.delcapacity>0 and t.delcapacity_root>0;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  fs << "COMPOUND QUERIES\n\n\n";

  strcpy(q, "select code, demand, x_coord, x_coord_root, y_coord, y_coord_root from truck, customer where customer.base=truck.customers_id and code like '%99' UNION select Customercode, Customerdemand, Customerx_coord, Customerx_coord_root, Customery_coord, Customery_coord_root from mapindex where Customerx_coord>133;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select code, demand, x_coord, x_coord_root, y_coord, y_coord_root from truck, customer where customer.base=truck.customers_id and code like '%99' UNION ALL select Customercode, Customerdemand, Customerx_coord, Customerx_coord_root, Customery_coord, Customery_coord_root from mapindex where Customerx_coord>133;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select code, demand, x_coord, x_coord_root, y_coord, y_coord_root from truck, customer where customer.base=truck.customers_id and code like '%99' INTERSECT select Customercode, Customerdemand, Customerx_coord, Customerx_coord_root,Customery_coord, Customery_coord_root from mapindex where Customerx_coord>133;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select code, demand, x_coord, x_coord_root, y_coord, y_coord_root from truck, customer where customer.base=truck.customers_id and code like '%99' EXCEPT select Customercode, Customerdemand, Customerx_coord,Customerx_coord_root,Customery_coord,Customery_coord_root from mapindex where Customerx_coord>133;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  fs << s.str();
  s.str("");

  fs << "AGGREGATIONS\n\n\n";

  strcpy(q, "select sum(m.map_index), m.Customerdemand from (select * from mapindex where map_index<187) as m GROUP BY m.Customerdemand");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select sum(m.map_index), m.Customerdemand from (select * from mapindex where map_index<187) as m GROUP BY m.Customerdemand HAVING sum(m.map_index)>200;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  fs << "ORDERING\n\n\n";

  strcpy(q, "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum, code, demand from truck,customer where cost <800 and delcapacity>0 and customer.base=truck.customers_id and code>100 and demand>10 ORDER BY code;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select truck.rownum, cost, delcapacity, delcapacity_root, customer.rownum, code, demand from truck,customer where cost <800 and delcapacity>0 and customer.base=truck.customers_id and code>100 and demand>10 ORDER BY code DESC;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  fs << "ARITHMETIC OPERATORS\n\n\n";

  strcpy(q, "select u.rownum, u.cost, u.delcapacity, u.delcapacity_root, c.rownum, c.code, c.demand, s.code,s.demand from truck u, customer c,truck k, customer s where (u.cost <100 or u.delcapacity>0) and c.base=u.customers_id and s.base=k.customers_id and (c.code>s.code or c.demand - s.demand >20) LIMIT 10;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select u.rownum, u.cost, u.delcapacity, u.delcapacity_root, c.rownum, c.code>>1, c.demand, s.code,s.demand from truck u, customer c,truck k, customer s where (u.cost <100 or u.delcapacity>0) and c.base=u.customers_id and s.base=k.customers_id and (c.code>>1>s.code or c.demand - s.demand >20) LIMIT 10 OFFSET 10;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select u.rownum, u.cost, u.delcapacity, u.delcapacity_root, c.rownum, c.code>>1, c.demand, LENGTH(s.code),s.demand from truck u, customer c,truck k, customer s where (u.cost <100 or u.delcapacity>0) and c.base=u.customers_id and s.base=k.customers_id and (c.code>>1>s.code or c.demand - s.demand >20) LIMIT 10 OFFSET 5;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select u.rownum, u.cost, u.delcapacity, u.delcapacity_root, c.rownum, c.code>>1, c.demand, LENGTH(s.code),~s.demand from truck u, customer c,truck k, customer s where (u.cost <100 or u.delcapacity>0) and c.base=u.customers_id and s.base=k.customers_id and (c.code>>1>s.code or c.demand - ~s.demand >20) LIMIT 15;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  fs << "EXISTS\n\n\n";

  strcpy(q, "select rownum from truck where exists (select * from truck);");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select truck.rownum, customer.rownum from truck join customer on customer.base=truck.customers_id where exists (select * from customer where base=customers_id);");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select truck.rownum from truck where exists (select * from customer where base=customers_id);");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select tc.rownum,customer.rownum from (select rownum,customers_id from truck where exists (select * from customer where base=customers_id)) tc join customer on customer.base=tc.customers_id;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select truck.rownum, cost, delcapacity,customer.rownum, customer.code, customer.demand from truck join customer on customer.base=truck.customers_id where exists (select * from Mapindex where mapindex.Customercode='159' and mapindex.Customerdemand>customer.demand) LIMIT 10;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  fs << "FUNCTIONS\n\n\n";

  strcpy(q, "select truck.rownum, cost, delcapacity,c.rownum, c.code, c.demand from truck join customer c on c.base=truck.customers_id where c.code glob '*59';");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select code, demand, x_coord, x_coord_root, y_coord, y_coord_root from truck, customer where customer.base=truck.customers_id and code LIKE '%69' ESCAPE '%' INTERSECT select Customercode, Customerdemand, Customerx_coord, Customerx_coord_root, Customery_coord, Customery_coord_root from mapindex where Customerx_coord>133;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  fs << "CASE\n\n\n";

  strcpy(q, "select summed, (select case when Customerdemand>30 then 'high' when Customerdemand between 25 and 30 then 'moderate' when Customerdemand between 20 and 24 then 'towards moderate' when Customerdemand between 10 and 19 then 'low' when Customerdemand<10 then 'tiny' end) tag from (select sum(map_index) summed, Customerdemand from (select * from mapindex where map_index<187) as m group by Customerdemand) grouped;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select c.code, c.x_coord, c.y_coord, mapindex.Customercode, mapindex.Customerx_coord, mapindex.Customery_coord, (select case when (c.x_coord*c.x_coord-mapindex.Customerx_coord*mapindex.Customerx_coord) + (c.y_coord*c.y_coord-mapindex.Customery_coord*mapindex.Customery_coord)<=100 then c.code||'-'||mapindex.Customercode||' are close' when(c.x_coord*c.x_coord-mapindex.Customerx_coord*mapindex.Customerx_coord) + (c.y_coord*c.y_coord-mapindex.Customery_coord*mapindex.Customery_coord)>100 then c.code||'-'||mapindex.Customercode||' are far' end) distanceP2 from truck, customer c, mapindex where c.base=truck.customers_id and c.code like '%99' and mapindex.Customercode like '_50';");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select c.code, c.x_coord, c.y_coord, mapindex.Customercode, mapindex.Customerx_coord, mapindex.Customery_coord, (select case when (c.x_coord*c.x_coord-mapindex.Customerx_coord*mapindex.Customerx_coord) + (c.y_coord*c.y_coord-mapindex.Customery_coord*mapindex.Customery_coord)<=100 then c.code||'-'||mapindex.Customercode||' are close' when(c.x_coord*c.x_coord-mapindex.Customerx_coord*mapindex.Customerx_coord) + (c.y_coord*c.y_coord-mapindex.Customery_coord*mapindex.Customery_coord)>100 then c.code||'-'||mapindex.Customercode||' are far' end) distanceP2 from truck, customer c, mapindex where c.base=truck.customers_id and c.code like '%99' and mapindex.Customercode in ('050','011');");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select * from MyTrucks;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
#ifdef PICO_QL_DEBUG
  printf("Query %i:\n %s\n\n", i, q);
#endif
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  //deinit_vt_selectors();
  //sqlite3_close(db);
  fs.close();
  return SQLITE_DONE;
}
