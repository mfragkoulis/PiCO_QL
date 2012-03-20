#ifndef LINEHAULCUSTOMER_H
#define LINEHAULCUSTOMER_H

#include "Customer.h"
#include "mtrand.h"

using namespace std;

class LinehaulCustomer: public Customer {

 public:
  LinehaulCustomer(string c, int d, Position* pos, bool depot);
  Position* get_pos();
  static bool all_Lserviced;
  static map < string, double > dist_l;
  static map < string, Position* > coord_l;
  static int count_l;
  static vector < LinehaulCustomer* > list_l;
  static vector < LinehaulCustomer* > list_ll;
  static LinehaulCustomer* get_depot();
  static bool get_allLserviced();
  static void compute_dist();
  static int get_countl();
  static double get_dist(string pair);
  static void set_allLserviced();
  static LinehaulCustomer* random_sel(int& pos, int i);
  static void erase_c(int pos, int i);
};


#endif
