/*   Copyright [2012] [Marios Fragkoulis]
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

#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "Position.h"
#include <string>
#include <map>
#include <vector>

using namespace std;

class Customer {

 protected:
  int demand;
  string code;
  bool serviced;

 private:
  static int count;
  static bool all_serviced;
 

 public:
  Customer();
  Customer(string c, int d, Position* pos, bool depot);
  virtual ~Customer();
  bool get_serviced();
  void set_serviced();
  string get_code();
  int get_demand();
  virtual Position* get_pos();
  static Customer* get_depot();
  static void compute_dist();
  static int get_count();
  static double get_dist(string pair);
  static bool get_allserviced();
  static void set_allserviced();
  static Customer* random_sel(int& pos, int i);
  static void erase_c(int pos, int i);

};

#include "BackhaulCustomer.h"
#include "LinehaulCustomer.h"

#endif
