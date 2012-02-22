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
  virtual int get_revenue();
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
