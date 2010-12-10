#ifndef FLEET_H
#define FLEET_H

#include <lemon/list_graph.h>
#include <lemon/graph_to_eps.h>
#include <lemon/math.h>
#include <vector>
#include "Truck.h"
#include "mtrand.h"

using namespace std;
using namespace lemon;

class Fleet {

 private:
  vector < Truck* > fleet;
  double total_cost;
  int unused_delspace;
  int unused_pickspace;

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
  void set_pickspace();
  void set_pickspace(int space);
  int get_pickspace();
  int get_size();
  void set_size(int size);
  void deallocate();
  void visualise();
  void schedule();
  void optimise( Fleet& best );
  void nested(Fleet& new_fleet, Fleet* best, double temperature, double m );
  void generate_new( int& pos1, int& pos2 );
  void assign_all(Fleet source);
  void assign_best(Fleet source);
  void assignT( int pos1, int pos2, Fleet source );
};

#endif
