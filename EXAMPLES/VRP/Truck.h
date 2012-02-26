#ifndef TRUCK_H
#define TRUCK_H

#include <vector>
#include "Customer.h"
#include "mtrand.h"

using namespace std;

class Truck {

 private:
  int del_capacity;
  vector < Customer* > cargoArray;
  vector < double > info;
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
  void exchange( Truck* t, bool& trap, bool same);
  void rearrange(int pos, string& pre, string& mid, string& other_mid, 
		 string& next);
  void rearrange(int pos, string& pre, string& mid, string& next);
  void add_costs(int pos, string pre, string mid, string next);
  void add_costs(int pos, string pre, string mid, string other_mid, 
		 string next);
  void assignC(Truck* t);
  void reassignC(Truck* t);
};

#endif
