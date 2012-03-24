/*
 *   Declare the interface to Fleet class.
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

#ifndef FLEET_H
#define FLEET_H

#include <vector>
#include "Truck.h"
#include "mtrand.h"

using namespace std;

class Fleet {

 private:
  vector<Truck*> fleet;
  double total_cost;
  int unused_delspace;

 public:
  Fleet();
  vector<Truck *>* get_fleet();
  void  add();
  Truck* get_current();
  void set_totalcost();
  void set_totalcost(int cost);
  double get_totalcost();
  void set_delspace();
  void set_delspace(int space);
  int get_delspace();
  int get_size();
  void set_size(int size);
  void deallocate();
  void optimise(Fleet& best);
  void nested(Fleet& new_fleet, Fleet* best, 
	      double temperature, double m);
  void generate_new(int& pos1, int& pos2);
  void assign_all(Fleet source);
  void assign_best(Fleet source);
  void assignT(int pos1, int pos2, Fleet source);
};

#endif
