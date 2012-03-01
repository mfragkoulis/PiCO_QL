#include <iostream>
#include <math.h>
#include <cstdlib>
#include "Customer.h"

using namespace std;


Customer::Customer() {}

Customer::Customer(string c, int d, Position* pos, bool depot) {
    code = c;
    demand = d;
    (depot) ? serviced = true : serviced = false;
}

Customer::~Customer() {}

bool Customer::all_serviced = false;

int Customer::count = 0 ;

bool Customer::get_serviced() {
    return serviced;
}

void Customer::set_serviced() {
    if (!serviced) serviced = true;
    else serviced = false;
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

Position* Customer::get_pos() {}


bool Customer::get_allserviced() {
    return all_serviced;
}

void Customer::set_allserviced() {
    if (!all_serviced) all_serviced = true;
    else all_serviced = false;
    // cout << "all_serviced: " << all_serviced << endl;
}


Customer* Customer::get_depot() {}

Customer* Customer::random_sel(int& pos, int i) {}


void Customer::erase_c(int random, int i) {}


double Customer::get_dist(string pair) {}

void Customer::compute_dist() {}
