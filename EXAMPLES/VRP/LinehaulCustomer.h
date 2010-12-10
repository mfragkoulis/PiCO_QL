#ifndef LINEHAULCUSTOMER_H
#define LINEHAULCUSTOMER_H

#include "Customer.h"
#include "mtrand.h"

using namespace std;


class LinehaulCustomer: public Customer {

 private:
  int pick_demand;
  int start_time;
  int service_time;
  int finish_time;
  static bool all_Lserviced;
  static map < string, double > dist_l;
  static map < string, Position* > coord_l;
  static int count_l;
  static vector < LinehaulCustomer* > list_l;
  static vector < LinehaulCustomer* > list_ll;
  static bool variant;
  static int start_travel;
  static int deadline;
  static bool pd;
  static bool tw;
 
 public:
  LinehaulCustomer( string c, int d, int pd, Position* pos, bool depot, int start, int finish );
  LinehaulCustomer( string c, int d, int pd, Position* pos, int start, int service, int finish );
  int get_pickdemand();
  int get_starttime();
  int get_servicetime();
  int get_finishtime();
  Position* get_pos();
  bool test(LinehaulCustomer* before_l, double cost);
  LinehaulCustomer* attempt_utilize(int i, int& pos, int& keep_track_cpt, int& diff, bool& succeed, bool fail, int& temp, int& bound, LinehaulCustomer* before_l, double travel_time);
  static LinehaulCustomer* get_depot();
  static bool get_allLserviced();
  static void compute_dist();
  static int get_countl();
  static double get_dist( string pair );
  static void set_allLserviced();
  static int choose(LinehaulCustomer* before_l, int i, double cost);
  static LinehaulCustomer* random_sel( LinehaulCustomer* before_l, int& pos, int i, double cost );
  static void erase_c( int pos, int i );
  static bool is_variant();
  static void set_variant(bool f);
  static int get_starttravel();
  static int get_deadline();
  static bool get_vrppd();
  static bool get_vrptw();
  static void set_PD(bool f);
  static void set_TW(bool f);
  static void sort_list(int i);
  static void random_quicksort( int i, vector < LinehaulCustomer* >::iterator start, vector < LinehaulCustomer* >::iterator finish);
  static vector < LinehaulCustomer* >::iterator randomized_partition( int i, vector < LinehaulCustomer* >::iterator start, vector < LinehaulCustomer* >::iterator finish);
  static vector < LinehaulCustomer* >::iterator partition(vector < LinehaulCustomer* >::iterator start, vector < LinehaulCustomer* >::iterator finish);

};


#endif
