#ifndef BACKHAULCUSTOMER_H
#define BACKHAULCUSTOMER_H

#include "Customer.h"
#include "mtrand.h"

using namespace std;

class BackhaulCustomer : public Customer {

 private:
  int revenue;
  static map < string, double > dist_b;
  static map < string, Position* > coord_b;
  static int count_b;
  static vector < BackhaulCustomer* > list_b;
  static vector < BackhaulCustomer* > list_bb;
  static vector < BackhaulCustomer* > nonser_list;
  static bool selective;

 public:
  BackhaulCustomer(string c, int d, int r, Position* pos, bool depot); 
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
