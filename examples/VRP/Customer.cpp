/*
 *   Implement the member methods of Customer class.
 *
 *   Copyright 2012 Marios Fragkoulis
 *
 *   Licensed under the Apache License, Version 2.0
 *   (the "License");you may not use this file except in
 *   compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in
 *   writing, software distributed under the License is
 *   distributed on an "AS IS" BASIS.
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 *   express or implied.
 *   See the License for the specific language governing
 *   permissions and limitations under the License.
 */

#include <iostream>
#include <cmath>
#include <cstdlib>
#include "Customer.h"

using namespace std;


Customer::Customer() {}

Customer::Customer(string c, int d, Position* pos, 
		   bool depot) {
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

Position* Customer::get_pos() {return NULL;}


bool Customer::get_allserviced() {
    return all_serviced;
}

void Customer::set_allserviced() {
    if (!all_serviced) all_serviced = true;
    else all_serviced = false;
    // cout << "all_serviced: " << all_serviced << endl;
}


Customer* Customer::get_depot() {return NULL;}

Customer* Customer::random_sel(int& pos, int i) {return NULL;}


void Customer::erase_c(int random, int i) {}


double Customer::get_dist(string pair) {return 0;}

void Customer::compute_dist() {}
