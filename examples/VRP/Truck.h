/*
 *   Declare the interface to Truck class.
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
 *   permissions and limitations under the License.
 */

#ifndef TRUCK_H
#define TRUCK_H

#include <vector>
#include "Customer.h"
#include "mtrand.h"

using namespace std;

double get_delcapacity_math_root(int del_capacity);

class Truck {

 private:
  int del_capacity;
  vector<Customer*> cargoArray;
  vector<double> info;
  double cost;
  int d_lowest;
  int d_highest;
  bool starting;
  static int init_capacity;

 public:
  Truck(int c);
  void load(LinehaulCustomer* load);
  void return_todepot();
  int get_cargosize();
  double get_cost();
  int get_delcapacity();
  vector < Customer * > * get_Customers();
  static int get_initcapacity();
  vector < Customer* >::iterator finish();
  vector < Customer* >::iterator start();
  void exchange( Truck* t, bool& trap, bool same);
  void rearrange(int pos, string& pre, string& mid, 
		 string& other_mid, string& next);
  void rearrange(int pos, string& pre, string& mid, 
		 string& next);
  void add_costs(int pos, string pre, string mid, 
		 string next);
  void add_costs(int pos, string pre, string mid, 
		 string other_mid, string next);
  void assignC(Truck* t);
  void reassignC(Truck* t);
};

#endif
