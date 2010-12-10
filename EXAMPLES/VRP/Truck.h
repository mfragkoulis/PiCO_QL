#ifndef TRUCK_H
#define TRUCK_H

#include <vector>
#include "Customer.h"
#include "mtrand.h"

using namespace std;

class Truck {

 private:
  int del_capacity;
  int pick_capacity;
  vector < Customer* > cargoArray;
  vector < double > info;
  double cost;
  int rl_point;
  int d_lowest;
  int d_highest;
  int p_lowest;
  int p_highest;
  static int init_capacity;
  static bool starting;

 public:
  Truck(int c);
  void load( LinehaulCustomer* before_load, LinehaulCustomer* load, int diff );
  void reload( BackhaulCustomer* before_load, BackhaulCustomer* load );
  void reload( BackhaulCustomer* before_load, BackhaulCustomer* load, Position* pos );
  void return_todepot();
  void back_noPickup(int bound);
  int get_cargosize();
  double get_cost();
  int get_delcapacity();
  int get_pickcapacity();
  int get_rlpoint();
  vector < Customer * > * get_Customers();
  vector < Customer * >::iterator start();
  vector < Customer * >::iterator finish();
  static int get_initcapacity();
  void exchange( Truck* t, bool& trap, bool same);
  void rearrange(int pos, string& pre, string& mid, string& next);
  void add_costs(int pos, string pre, string mid, string next);
  void assignC(Truck* t);
  void reassignC(Truck* t);
};

#endif
