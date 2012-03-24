/*
 *   Declare the interface to BackhaulCustomer class.
 *
 *   Copyright 2012 Marios Fragkoulis
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

#ifndef BACKHAULCUSTOMER_H
#define BACKHAULCUSTOMER_H

#include "mtrand.h"
#include "Customer.h"

using namespace std;

class BackhaulCustomer : public Customer {

 private:
  int revenue;
  static map<string, double> dist_b;
  static map<string, Position*> coord_b;
  static int count_b;
  static vector<BackhaulCustomer*> list_b;
  static vector<BackhaulCustomer*> list_bb;
  static vector<BackhaulCustomer*> nonser_list;
  static bool selective;

 public:
  BackhaulCustomer(string c, int d, int r, Position* pos, 
		   bool depot); 
  Position* get_pos();
  int get_revenue();
  static BackhaulCustomer* get_depot();
  static vector < BackhaulCustomer* >::iterator get_nonser();
  static void compute_dist();
  static int get_countb();
  static double get_dist(string pair);
  static BackhaulCustomer* random_sel(int& pos, int i);
  static void erase_c(int pos, int i);
  static void non_serviced(int i);
  static void clear_list(int i);
  static void cp_list(int i);
  static bool get_selective();
  static void set_selective(bool s);
};

#endif
