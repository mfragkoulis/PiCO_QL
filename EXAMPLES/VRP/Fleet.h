#ifndef FLEET_H
#define FLEET_H

#include <vector>
#include "Truck.h"
#include "mtrand.h"

using namespace std;

class Fleet {

 private:
  vector < Truck* > fleet;
  double total_cost;
  int unused_delspace;

 public:
  Fleet();
  vector<Truck *> * get_fleet();
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
  void optimise( Fleet& best );
  void nested(Fleet& new_fleet, Fleet* best, double temperature, double m );
  void generate_new( int& pos1, int& pos2 );
  void assign_all(Fleet source);
  void assign_best(Fleet source);
  void assignT( int pos1, int pos2, Fleet source );
};

#endif
