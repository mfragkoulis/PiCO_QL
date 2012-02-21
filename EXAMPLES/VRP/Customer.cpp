#include <iostream>
#include <math.h>
#include <cstdlib>
#include "Customer.h"

using namespace std;


Customer::Customer() {}

Customer::Customer(string c, int d, Position* pos, bool depot) {
  code=c;
  demand=d;
  serviced=true;
  // all_demand+=demand;
}


Customer::Customer(string c, int d,Position* pos) {
  code=c;
  demand=d;
  serviced=false;
  count++;
  //  all_demand+=demand;
}

Customer::~Customer() {}

bool Customer::all_serviced=false;

int Customer::count = 0 ;

bool Customer::get_serviced() {
  return serviced;
}

void Customer::set_serviced() {
  if (!serviced)  serviced=true;
  else serviced=false;
}

string Customer::get_code() {
  return code;
}

int Customer::get_demand() {
  return demand;
}

int Customer::get_count() {
  return count;
}

int Customer::get_revenue() {}

int Customer::get_pickdemand() {}

int Customer::get_starttime() {}

int Customer::get_servicetime() {}

int Customer::get_finishtime() {}

Position* Customer::get_pos() {}

bool Customer::get_allserviced() {
  return all_serviced;
}

void Customer::set_allserviced() {
  if (!all_serviced) all_serviced=true;
  else all_serviced=false;
}


Customer* Customer::get_depot() {}

Customer* Customer::random_sel(int& pos) {}


void Customer::erase_c(int random) {}


double Customer::get_dist(string pair) {}

void Customer::compute_dist() {}
