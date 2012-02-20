#include "Customer.h"
#include "Position.h"
#include "Truck.h"
#include "Fleet.h"
#include <iostream>
#include <lemon/time_measure.h>
#include <lemon/list_graph.h>
#include <lemon/graph_to_eps.h>
#include <lemon/math.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <ctime>
#include "mtrand.h"
#include <stdio.h>
// For sqtl
#include "stl_search.h"
//

using namespace std;
using namespace lemon;



unsigned long init[4]={0x123, 0x234, 0x345, 0x456};
unsigned long length=4;  
MTRand_int32 irand(init, length);
// For sqtl
vector<Truck*> *vehicles;
map<int, Customer *> test;
//


int main(int argc, const char *argv[]) {

  int line_cust, restarts, total=-9, lowest_dem, total_dem=0;
  string variant, mode, data, total_string;
  Fleet bb;
  restarts = 0;
  if (variant=="cvrp") {
    LinehaulCustomer::set_variant(true);
    line_cust=total;
  }
  // cout << "lines: " << line_cust << endl;  

  clock_t start_clock,finish_clock;
  double c_time;
  start_clock = clock();
  // which of the two?
  Timer t;
  t.start();  

  string code;
  int x, y, demand, pick_demand, depends, start, service, finish, revenue, number, z=3;
  vector < Position* > positions;
  vector < LinehaulCustomer* > line;
  vector < BackhaulCustomer* >  back;
  Fleet* optimal=new Fleet;
  optimal->add();                        // create first truck of fleet. reason for doing it here: record info about truck capacity
  lowest_dem=Truck::get_initcapacity();      // symbolic initialization (the highest demand possible)  
  ifstream fin(argv[1]);
  cout << argv[1] << endl;
  if(!fin) {
    cout << "Cannot open file.\n" << endl;
    exit(1);
  }
  number=1;
  while (number<10) {
    getline(fin, data);
    // cout << data << endl;
    number++;
  }
  number=0;
  fin >> code;
  fin >> x;
  fin >> y;
  fin >> demand;
  fin >> depends;
  fin >> finish;
  fin >> service;
  // cout << "code: " << code << "x: " << x << "y: " << y << "d: " << demand << endl;
  while ( (!fin.eof()) && (!fin.fail()) ) {
      positions.push_back( new Position(x,y) );
      while (code.length() < total_string.length()) code = "0" + code;                        // automate. hard-coded. not anymore
      z *= -1;
      total_dem += demand;
      if ( (demand >0) && (demand < lowest_dem) ) lowest_dem = demand; 
      // cout << lowest_dem << endl;}
      if (demand <= Truck::get_initcapacity()) {                                       // CVRP!!!
	  line.push_back( new LinehaulCustomer(code, demand, 0, positions.back(), 0, 0, 0) );
	  test.insert(pair<int, Customer *>(number, line.back()));
      } else cout << " Customer with code " << code << " cannot be serviced due to capacity constraint violation." << endl << endl;

    // cout << "number: " << number << ", code: " << code << " " << x << " " << y << " " << demand;
    // cout << ", depends: " << depends << ", start: " << start << ", service: " << service << ", finish: " << finish << endl;
    number++;
    fin >> code;
    fin >> x;
    fin >> y;
    fin >> demand;
    fin >> depends;
    fin >> finish;
    fin >> service;
  }
  fin.close();
  // cout << "final number: " << number << endl;  
  // check positions coincidence, code coincidence
  vector < LinehaulCustomer* >::iterator iter1;
  vector < LinehaulCustomer* >::iterator iter2;
  vector < BackhaulCustomer* >::iterator itr1;
  vector < BackhaulCustomer* >::iterator itr2;
  vector < Position* >::iterator it1;
  vector < Position* >::iterator it2;
  int pos_v=0;
  int lcode_v=0;
  int bcode_v=0;
  int code_v=0;
  for (it1=positions.begin(); it1!=positions.end(); it1++) {
    //    cout << (*it1)->get_x() << " " << (*it1)->get_y() << endl;
    for (it2=it1+1; it2!=positions.end(); it2++) {
      if ( ((*it1)->get_x()==(*it2)->get_x()) && ((*it1)->get_y()==(*it2)->get_y()) )  pos_v++;
    }
  }
  for (iter1=line.begin(); iter1!=line.end(); iter1++) {
    //    cout << (*iter1)->get_code() << endl;
    for (iter2=iter1+1; iter2!=line.end(); iter2++) {
      if ( (*iter1)->get_code()==(*iter2)->get_code() )  lcode_v++;
    }
  }
  if (pos_v>0) cout << pos_v << " pairs of customers have the same position." << endl;
  else cout << " All positions valid " << endl; 
  if (lcode_v>0) cout << lcode_v << " pairs of linehaul customers have the same code." << endl;
  else cout << " All linehaul codes valid " << endl;
  if (bcode_v>0) cout << bcode_v << " pairs of backhaul customers have the same code." << endl;
  else cout << " All backhaul codes valid " << endl; 
  if (code_v>0) cout << code_v << " pairs of linehaul-backhaul customers have the same code." << endl;
  else cout << " All linehaul-backhaul codes valid " << endl; 

  cout << endl << "Computing distances between linehaul customers. " << endl;
  LinehaulCustomer::compute_dist();
  cout << endl << "Computing distances between backhaul customers. " << endl;
  BackhaulCustomer::compute_dist();
  cout << endl;

  double truck_cost;
  int keep_track_cpt, n, m, k, diff, bound, temp;
  bool load=false, reload=false, succeed=false, fail=false;
  LinehaulCustomer* l;
  LinehaulCustomer* before_l;
  Position* p;
  BackhaulCustomer* b;
  BackhaulCustomer* before_b;
  Fleet best;

  for (int i=0; i<=2; i++) {
    m=0;
    n=0;
    k=0;
    pos=0;
    if (i>0) {
      optimal=new Fleet;
      optimal->add();
      cout << "RESTART No" << i << endl << endl;
    }
    while (!Customer::get_allserviced()) {              // VRPB,VRPSB
      // cout << "LOADING NEW TRUCK " << endl;
      keep_track_cpt=Truck::get_initcapacity();
      diff=0;
      bound=0;
      temp=0;
      fail=false;
      before_l=NULL;

      if (LinehaulCustomer::get_vrptw()) truck_cost=optimal->get_current()->get_cost();
      else truck_cost=0;
      
      l=LinehaulCustomer::random_sel( before_l, pos, i, truck_cost);                       // can't do it better??
      if (l != NULL) {
	}
	keep_track_cpt -= l->get_demand();
	// cout << " Customer's demand : " << l->get_demand();
      }
      if ( (LinehaulCustomer::is_variant()) && ((keep_track_cpt<0) || (fail)) && (-bound + keep_track_cpt + l->get_demand() >= lowest_dem) ) {      // 1st objective: BPP ??
	l=l->attempt_utilize(i, pos, keep_track_cpt, diff, succeed, fail, temp, bound, before_l, optimal->get_current()->get_cost());
	  if (!succeed) {
	    cout << "TERRIBLY WRONG" << endl;
	  }else fail=false;                                       // just in case. matters only in VRPPD
      }
    utilize:
      while ( (keep_track_cpt >= 0) && (!fail) && ( n < LinehaulCustomer::get_countl() -1 ) ) {
	// cout << "count : " <<  LinehaulCustomer::get_countl() -1 << endl;
	optimal->get_current()->load(before_l, l, diff);
	load=true;
	LinehaulCustomer::erase_c(pos,i);
	n++;
	// cout << n << " linehaul customers serviced so far." << endl << endl;
	before_l=l;
	m++;                                            // customer counter, might need it if implement another VRP variation

	if (LinehaulCustomer::get_vrptw()) truck_cost=optimal->get_current()->get_cost();
	else truck_cost=0;

	l=LinehaulCustomer::random_sel( before_l, pos, i, truck_cost);
	if (l != NULL)  {
	  keep_track_cpt -= l->get_demand();
	  // cout << " Customer's demand : " << l->get_demand();
	  // if (LinehaulCustomer::get_vrppd()) cout << ", pick demand : " << l->get_pickdemand();
	  // cout << ", del capacity left : " << keep_track_cpt << endl;
	}
	if ( (LinehaulCustomer::is_variant()) && ((keep_track_cpt <0) || (fail)) && (-bound + keep_track_cpt + l->get_demand()  >= lowest_dem) ) {      // 1st objective: BPP
	  // cout << "jbilghjb" << endl;
	  l=l->attempt_utilize(i, pos, keep_track_cpt, diff, succeed, fail, temp, bound, before_l, optimal->get_current()->get_cost());
	  if (succeed) {
	    fail=false;
	    goto utilize;
	  }
	}
      }
      // cout << "service not allowed." << endl;
      optimal->set_delspace();
      // cout << "The total cost of delivery for the truck amounts to " << optimal->get_current()->get_cost() << " and there are " << optimal->get_current()->get_delcapacity() << " units of unused space left." << endl << endl;
      if ( (l!=NULL) && (before_l==NULL) ) {                                          // demand > capacity. ignore customer
	n++;
	cout << endl << "*** Demand - Capacity violation happened at linehaul customer " << l->get_code() << endl << endl;
      }                                                // you don't necessary catch it.only if it happens at opening.so pre-check to be sure and get rid of additional complexity
      else if ( (l!=NULL) && (before_l!=NULL) )   p=l->get_pos();                     // usual case in the end. all linehaul customers serviced, record position of last linehaul customer
      else if  ( (before_l!=NULL) && (l==NULL) )  p=before_l->get_pos();              // most usual case. no more truck capacity
      keep_track_cpt=Truck::get_initcapacity();
      //  optimal->get_current()->reset_capacity();                  //reset initial capacity.after distinguishing capacities no longer needed

    trynext:
      if ( (load) && (!reload) ) optimal->get_current()->back_noPickup(bound);                    // if no backhaul customer is left to service
      else if ( (!load) && (!reload) )  cout << "WHAT??" << endl ; 
      else optimal->get_current()->return_todepot();
      reload=false;
      load=false;
      // cout << "The total cost of transportation for the truck amounts to " << optimal->get_current()->get_cost() << " and there are " << optimal->get_current()->get_delcapacity() << " units of unused delivery space and " << optimal->get_current()->get_pickcapacity() << " units of unused pick-up space left." << endl << endl;
      // cout << " end of (both) nested loop " << endl << endl;                           // to catch the extreme scenario that demand > capacity. contrasts basic assumption
                                                                        // that a customer is serviced by one vehicle only. sleep on it..
      if ( (m == Customer::get_count()) || ((BackhaulCustomer::get_selective()) && (n==LinehaulCustomer::get_countl() -1) && (b==NULL)) ) {              
	Customer::set_allserviced();
	cout << " END OF SCHEDULING " << endl << endl;
      } else     optimal->add();                                               // "add" new empty truck to fleet. will be loaded in forthcoming loop
    }
    Customer::set_allserviced();
    optimal->set_totalcost();
    optimal->set_pickspace();
    cout << "The total cost of transportation for the fleet of " << optimal->get_size() << " trucks amounts to " << optimal->get_totalcost();
    if (LinehaulCustomer::get_vrptw()) cout << " time units";
    else cout <<" distance units";
    cout << " and there are " << optimal->get_delspace() << " units of unused delivery space and " << optimal->get_pickspace() << " units of unused pickup space left." << endl << endl;
    BackhaulCustomer::non_serviced(i);
    // optimal->visualise();
    if (i==0) {
      bb.set_size(optimal->get_size());
      bb.assign_all(*optimal);
// For sqtl
      vehicles = bb.get_fleet();
      int re_sqlite = call_sqtl();
      printf("Thread sqlite returned %i\n", re_sqlite);
//
    }
    cout << endl << "Optimising schedule now..." << endl << endl;

    best.set_size(optimal->get_size());
    
    optimal->optimise(best);
    cout << "The optimised fleet of " << best.get_size() << " trucks amounts to " << best.get_totalcost();
    if (LinehaulCustomer::get_vrptw()) cout << " time units";
    else cout <<" distance units";
    cout << " and there are " <<  best.get_delspace() << " units of unused delivery space and " << best.get_pickspace() << " units of unused pickup space left." << endl << endl;
    if (best.get_totalcost() <= bb.get_totalcost()) {
      bb.deallocate();
      bb.set_size(best.get_size());
      bb.assign_all(best);
      BackhaulCustomer::cp_list(i);
    }
    BackhaulCustomer::clear_list(i);
    best.deallocate();
    optimal->deallocate();
    delete optimal;
    if (restarts>i) {
      for (iter1=line.begin()+1; iter1!=line.end(); iter1++) {
	(*iter1)->set_serviced();
	// cout << (*iter1)->get_code() << endl;
      }
    }
  }
  cout << endl << "Optimised solution after " << restarts << " restarts includes " << bb.get_size() << " trucks and has cost " << bb.get_totalcost();
  if (LinehaulCustomer::get_vrptw()) cout << " time units";
  else cout <<" distance units";
  cout << " with unused delivery space of " << bb.get_delspace() << " and unused pick-up space of " << bb.get_pickspace() << " units." << endl << endl;
  if (LinehaulCustomer::is_variant()) cout << endl << "Ideally (feasibility not guaranteed) minimum size of fleet would be " << (double)total_dem/Truck::get_initcapacity() << " trucks." << endl << endl;
  
  t.stop();
  cout << endl << "Optimised solution constructed after " << t << endl << endl;
  
  finish_clock = clock();
  c_time = (double(finish_clock)-double(start_clock))/CLOCKS_PER_SEC;
  cout << "Ellapsed time given by c++ : " << c_time << "s." << endl << endl;

  bb.visualise();
  bb.schedule();
  bb.deallocate();

  for (it1=positions.begin(); it1!=positions.end(); it1++) {
    delete (*it1);
  }

  for(iter1=line.begin(); iter1!= line.end(); iter1++) {
    delete (*iter1);
  }
