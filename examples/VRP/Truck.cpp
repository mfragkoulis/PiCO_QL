/*
 *
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
#include <string>
#include <math.h>
#include "Truck.h"

using namespace std;

int Truck::init_capacity = 0;


extern MTRand_int32 irand;

// Constructs a new Truck object given its capacity.
Truck::Truck(int c) {
  del_capacity = c;
  init_capacity = del_capacity;
  starting = true;
  cost = 0;
  d_lowest = c;
  d_highest = 0;
}

/* Loads a Customer's goods to the truck, adds the Customer
 * to the stops.
 */
void Truck::load(LinehaulCustomer* load) {
  // cout << "loading" << endl;
    string half,other;
    int d;
    double c;
    if (starting) {
	half = LinehaulCustomer::get_depot()->get_code();
	starting = false;
    } else 
        half = cargoArray.back()->get_code();
    cargoArray.push_back(load);
    other = load->get_code();
    if (half < other) 
        c = LinehaulCustomer::get_dist(half + other);
    else 
        c = LinehaulCustomer::get_dist(other + half);
    load->set_serviced();
    d = load->get_demand();
    del_capacity -= d;
    /* cout << "Del Capacity left after load : " << 
     * del_capacity << endl;
     */
    if (d < d_lowest) 
        d_lowest=d;
    if (d > d_highest) 
        d_highest=d;
    cost += c;
    /* cout << half << " + " <<  other << " : " << cost << 
     * endl;
     */
    info.push_back(cost);                           
    /* cout << "travel time (cost) recorded: " << cost << 
     * endl;
     */
}

// Returns the truck's initial capacity.
int Truck::get_initcapacity() {
    return init_capacity;
}

// Returns the truck's cost (distance to travel).
double Truck::get_cost() {
  return cost;
}

// Returns the truck's remaining capacity.
int Truck::get_delcapacity() {
  return del_capacity;
}

// Returns the truck's list of customers to service.
vector < Customer * > * Truck::get_Customers() {
    return &cargoArray;
}

// returns the number of customers the truck has to serve.
int Truck::get_cargosize() {
  return cargoArray.size();
}

// Instructs the truck that the routing is completed.
void Truck::return_todepot() {
    cost += LinehaulCustomer::get_dist( 
	LinehaulCustomer::get_depot()->get_code() + 
	cargoArray.back()->get_code());
    /* cout << cargoArray.back()->get_code() << " + " << 
     * LinehaulCustomer::get_depot()->get_code() << 
     * ": " << cost << 
     * " , end of truck route. " << endl; 
     */
    /* cout << "Free space left: " << del_capacity << 
     * "/" << init_capacity << 
     * endl; 
     */
}

// Returns iterator to beginning of Truck.
vector <Customer*>::iterator Truck::start() {
    vector<Customer*>::iterator iter;
    iter = cargoArray.begin();
    return iter;
}

// Returns iterator to end of Truck.
vector <Customer*>::iterator Truck::finish() {
    vector <Customer*>::iterator iter;
    iter = cargoArray.end();
    return iter;
}


/* Carries out the exchange of customers between two trucks
 * as part of the optimisation process.
 */
void Truck::exchange( Truck* t, bool& trap, bool same) {
    // cout << "exchange" << endl;
    trap = false;                     // redundant
    int pos1, pos2, i, d1, d2;
    string pre1 = "", mid1 = "", next1 = "", pre2 = "", 
      mid2 = "", next2 = "", depot;
    i = 0;
    if ((same) && (cargoArray.size() == 1)) {
        // Won't be able to exchange positions.
	trap = true;
	goto stop;
    }
    // cout << "1" << endl;
    if ( (d_lowest > t->d_highest + t->del_capacity) || 
	 (t->d_lowest > d_highest + del_capacity) ) {
        // No valid exchange exists between the two trucks.
	trap = true;
	goto stop;
    }
    pos1 = irand() % cargoArray.size(); 
    d1 = cargoArray[pos1]->get_demand();
    pos2 = irand() % t->cargoArray.size();
    d2 = t->cargoArray[pos2]->get_demand();
    while (((same) && (pos1 == pos2)) || 
	    (d1 + del_capacity < d2) ||
	    (d2 + t->del_capacity < d1)) {	
	if (i == 50) {
	    trap = true;
	    // cout << "TRAP!!!" << endl;
	    goto stop;
	}
	pos1 = irand() % cargoArray.size(); 
	d1 = cargoArray[pos1]->get_demand();
	pos2 = irand() % t->cargoArray.size();
	d2 = t->cargoArray[pos2]->get_demand();
	i++;
	// cout << " i : " << i << endl;
	/* cout << "stuck " << pos1 << " " << pos2 << 
	 * " " << same << " " << 
	 * cargoArray.size() << endl; 
	 */
    }

    /*
     * cout << " selected " << pos1 << "->" << 
     * cargoArray[pos1]->get_code() << 
     * " with " << pos2 << "->" << 
     * t->cargoArray[pos2]->get_code() << 
     * " of size " << 
     * cargoArray.size() << " " << 
     * t->cargoArray.size() << endl;
     * cout << "d1: " << d1 << "   del_capacity: " << 
     * del_capacity << "   d2: " << d2 << 
     * "   t->del_capacity: " << t->del_capacity << 
     * endl;
     *
     * cout << "del_cap: " << del_capacity << 
     * "   cargoArray[pos1]->get_demand(): " << 
     * cargoArray[pos1]->get_demand() << 
     * "   t->cargoArray[pos2]->get_demand(): " << 
     * t->cargoArray[pos2]->get_demand() << 
     * "   t->del_capacity: " << t->del_capacity << endl;
     */  
    
    /* cout << " go rearrange " << pos1 << " with " << 
     * pos2 << " " << 
     * << endl << endl; 
     */
    if ((same) && (pos1 + 1 == pos2))
	rearrange(pos1, pre1, mid1, mid2, next2); 
        // Subtract costs related to positions in change.
    else if ((same) && (pos2 + 1 == pos1))
	rearrange(pos2, pre2, mid2, mid1, next1);
    else {
	rearrange(pos1, pre1, mid1, next1);    
	// Subtract costs related to positions in change.
	t->rearrange(pos2, pre2, mid2, next2);
    }

    del_capacity = del_capacity + d1 - d2;     
    /* Rearrange capacities in trucks to reflect the 
     * correct ones after movements.
     */
    t->del_capacity = t->del_capacity + d2 - d1;
     
    /* cout << "del_cap: " << del_capacity << 
     * "   cargoArray[pos1]->get_demand(): " << 
     * cargoArray[pos1]->get_demand() << 
     * "   t->cargoArray[pos2]->get_demand(): " << 
     * t->cargoArray[pos2]->get_demand() << 
     *"   t->del_capacity: " << t->del_capacity << endl; 
     */
	
    Customer* c;   
    // Finally swap customers between the two trucks.
    c = cargoArray[pos1];
    cargoArray[pos1] = t->cargoArray[pos2];
    t->cargoArray[pos2] = c;

    if ((same) && (pos1 + 1 == pos2)) {
	add_costs(pos1, pre1, mid2, mid1, next2);    
	/* Add the costs that are produced as a result 
	 * of the changed route.
	 */
    } else if ( (same) && (pos2 + 1 == pos1)) {
	add_costs(pos2, pre2, mid1, mid2, next1);
    } else {
	add_costs(pos1, pre1, mid2, next1);
	t->add_costs(pos2, pre2, mid1, next2);
    }
stop:;
}

/* Subtracts costs of ssubroutes related to customers 
 * selected to swap places.
 */
void Truck::rearrange(int pos, string& pre, string& mid, 
		      string& next) {
    // cout << "pos: " << pos << endl;
    // cout << "Cost before : " << cost << endl;
    mid = cargoArray[pos]->get_code();
    if (pos > 0) 
      pre = cargoArray[pos-1]->get_code();
    else 
      pre = LinehaulCustomer::get_depot()->get_code();
    if (pre < mid) {
	cost -= LinehaulCustomer::get_dist(pre +mid);
	/* cout <<   pre << "-" << mid << ": " << 
	 * LinehaulCustomer::get_dist(pre +mid) << endl; 
	 */
    } else {
	cost -= LinehaulCustomer::get_dist(mid +pre);
	/* cout <<  mid << "-" << pre << ": " << 
	 *  LinehaulCustomer::get_dist(mid + pre) << endl; 
	 */
    }
    if (pos + 1 < (int)cargoArray.size()) 
      next = cargoArray[pos+1]->get_code();
    else 
      next = LinehaulCustomer::get_depot()->get_code();
    if (mid < next) {
	cost -= LinehaulCustomer::get_dist(mid + next);
	/* cout <<  mid << "-" << next << ": " << 
	 * LinehaulCustomer::get_dist(mid + next) << endl; 
	*/
    } else {
	cost -= LinehaulCustomer::get_dist(next + mid);
	/* cout << next << "-" << mid << ": " << 
	 * LinehaulCustomer::get_dist(next + mid) << endl; 
	 */
    }
    // cout << "3" << endl;
    /* cout << "Cost after : " << cost << endl;
     * cout << "pre: " << pre << ", mid: " << mid << 
     * ", next: " << next << endl; 
     */
}

/* Subtracts costs of subroutes related to customers in 
 * the same Truck selected to swap places.
 */
void Truck::rearrange(int pos, string& pre, string& mid, 
		      string& other_mid, string& next) {
    // cout << "rearrange" << endl;
    // cout << "Cost before in same: " << cost << endl;
    mid = cargoArray[pos]->get_code();
    if (pos + 1 >= (int)cargoArray.size()) 
      cout << "Pos at end of truck.\n";
    other_mid = cargoArray[pos+1]->get_code();
    if (pos > 0) 
      pre=cargoArray[pos-1]->get_code();
    else 
      pre = LinehaulCustomer::get_depot()->get_code();
    if (pre < mid) {
	cost -= LinehaulCustomer::get_dist(pre +mid);
	/* cout <<  pre << "-" << mid << ": " << 
	 * LinehaulCustomer::get_dist(pre +mid) << endl; 
	 */
    } else {
	cost -= LinehaulCustomer::get_dist(mid +pre);
	/* cout <<  mid << "-" << pre << ": " << 
	 * LinehaulCustomer::get_dist(mid +pre) << endl; 
	 */
    }
    if (mid < other_mid) {
	cost -= LinehaulCustomer::get_dist(mid + other_mid);
	/* cout << mid << "-" << other_mid << ": " << 
	 * LinehaulCustomer::get_dist(mid + other_mid) << 
	 * endl; 
	 */
    } else {
	cost -= LinehaulCustomer::get_dist(other_mid + mid);
	/* cout <<  other_mid << "-" << mid << ": " << 
	 * LinehaulCustomer::get_dist(other_mid + mid) << 
	 * endl; 
	 */
    }
    if (pos + 2 < (int)cargoArray.size()) 
      next=cargoArray[pos+2]->get_code();
    else 
      next = LinehaulCustomer::get_depot()->get_code();
    if (other_mid < next) {
	cost -= LinehaulCustomer::get_dist(other_mid + next);
	/* cout <<  other_mid << "-" << next << ": " << 
	 * LinehaulCustomer::get_dist(other_mid + next) << 
	 * endl; 
	 */
    } else {
	cost -= LinehaulCustomer::get_dist(next + other_mid);
	/* cout << next << "-" << other_mid << ": " << 
	 * LinehaulCustomer::get_dist(next + other_mid) << 
	 * endl; 
	 */
    }
    // cout << "3" << endl;
    /* cout << "Cost after in same: " << cost << endl;
     * cout << "pre: " << pre << ", mid: " << mid << 
     * ", other_mid: " << 
     * other_mid << ", next: " << next << endl; 
     */
}


/* Recalculates costs of the engaged trucks after the 
 * exchange.
 */
void Truck::add_costs(int pos, string pre, string mid, 
		      string next) {
    // cout << "add" << endl;
    //int c1, c2;
    //c1 = cost;
    /* cout << "pre: " << pre << ", mid: " << mid << 
    ", next: " << next << endl; 
    cout << "Before adding : " << cost << endl; */
    if (pre < mid) {
	cost += LinehaulCustomer::get_dist(pre + mid);
	/* cout <<   pre << "+" << mid << ": " << 
	 * LinehaulCustomer::get_dist(pre + mid) << endl; 
	 */
    } else {
	cost += LinehaulCustomer::get_dist(mid +pre);
	/* cout <<  mid << "+" << pre << ": " << 
	 * LinehaulCustomer::get_dist(mid + pre) << endl; 
	 */
    }
    if (mid < next) {
	cost += LinehaulCustomer::get_dist(mid + next);
	/* cout <<  mid << "+" << next << ": " << 
	 * LinehaulCustomer::get_dist(mid + next) << endl; 
	 */
    } else {
	cost += LinehaulCustomer::get_dist(next + mid);
	/* cout << next << "+" << mid << ": " << 
	 * LinehaulCustomer::get_dist(next + mid) << endl; 
	 */
    }
    // cout << "6" << endl;
    if (next.length() == 0) {
	cost += 
	    cargoArray[pos+1]->get_pos()->distance(cargoArray[pos]->get_pos());
	cout <<  "NEXT 0 " << 
	cargoArray[pos+1]->get_pos()->distance(cargoArray[pos]->get_pos()) << 
	    endl << endl << endl;
	if (pre < mid) {
	    cost += LinehaulCustomer::get_dist(pre + mid);
	    /* cout << 
	     * LinehaulCustomer::get_dist(pre + mid) << 
	     * endl;
	     */
	} else {
	    cost += LinehaulCustomer::get_dist(mid + pre);
	    /* cout << 
	     * LinehaulCustomer::get_dist(mid + pre) << 
	     * endl;
	     */
	}
	// cout << "8" << endl;
    }
    // c2=cost;
    // cout << "After addition : " << cost << endl << endl;
    if (cost < 0) cout << "COST VIOLATION.\n\n";
}

// Recalculates costs in the same truck after the exchange.
void Truck::add_costs(int pos, string pre, string mid, 
		      string other_mid, string next) {
    // cout << "add" << endl;
    //int c1, c2;
    //c1 = cost;
    /* cout << "pre: " << pre << ", mid: " << mid << 
     * ", other_mid: " << 
     * other_mid << ", next: " << next << endl;
     */
    /* cout << "Before adding in same: " << cost << endl; 
     */
    if (pre < mid) {
	cost += LinehaulCustomer::get_dist(pre +mid);
	/* cout <<   pre << "+" << mid << ": " << 
	 * LinehaulCustomer::get_dist(pre + mid) << endl; 
	 */
    } else {
	cost += LinehaulCustomer::get_dist(mid +pre);
	/* cout <<  mid << "+" << pre << ": " << 
	 * LinehaulCustomer::get_dist(mid + pre) << endl; 
	 */
    }
    if (mid < other_mid) {
	cost += LinehaulCustomer::get_dist(mid + 
					   other_mid);
	/* cout <<  mid << "+" << other_mid << ": " << 
	 * LinehaulCustomer::get_dist(mid + other_mid) << 
	 * endl; 
	 */
    } else {
	cost += LinehaulCustomer::get_dist(other_mid + 
					   mid);
	/* cout << other_mid << "+" << mid << ": " << 
	 * LinehaulCustomer::get_dist(other_mid + mid) << 
	 * endl; 
	 */
    }
    if (other_mid < next) {
	cost += LinehaulCustomer::get_dist(other_mid + 
					   next);
	/* cout << other_mid << "+" << next << ": " << 
	 * LinehaulCustomer::get_dist(other_mid + next) << 
	 * endl; 
	 */
    } else {
	cost += LinehaulCustomer::get_dist(next + 
					   other_mid);
	/* cout << next << "+" << other_mid << ": " << 
	 * LinehaulCustomer::get_dist(next + other_mid) << 
	 * endl; 
	 */
    }
    // cout << "6" << endl;
    if (next.length() == 0) {
	cost += 
	    cargoArray[pos+1]->get_pos()->distance(cargoArray[pos]->get_pos());
	cout << "In same: NEXT 0 " <<  
	cargoArray[pos+1]->get_pos()->distance(cargoArray[pos]->get_pos()) << endl << endl << endl;
	if (pre < mid) {
	    cost += LinehaulCustomer::get_dist(pre + mid);
	    cout << 
	      LinehaulCustomer::get_dist(pre + mid) << 
	      endl;
	} else {
	    cost += LinehaulCustomer::get_dist(mid + pre);
	    cout << 
	      LinehaulCustomer::get_dist(mid + pre) << 
	      endl;
	}
	// cout << "8" << endl;
    }
    // c2 = cost;
    /* cout << "After adding in same : " << cost << 
     * endl << endl;
     */
    if (cost<0) cout << "COST VIOLATION IN SAME.\n\n";
}


/* Assigns a Truck service to "this" as part of the 
 * optimisation process (copy fleet).
 */
void Truck::assignC(Truck* t) {
  // cout << "assignC" << endl;
    for (int i = 0; i != (int)t->cargoArray.size(); i++) {
    // cout << i << endl;
    cargoArray.push_back(t->cargoArray[i]);
    // cout << t->cargoArray
  }
  del_capacity = t->del_capacity;
  cost = t->cost;
  d_lowest = t->d_lowest;
  d_highest = t->d_highest;
  info = t->info;
}

// Reassigns customers of Truck *t to "this" truck.
void Truck::reassignC(Truck* t) {
  // cout << "reassignC" << endl;
    for (int i = 0; i != (int)t->cargoArray.size(); i++) {
    // cout << i << endl;
    cargoArray[i] = t->cargoArray[i];
    // cout << t->cargoArray
  }
  del_capacity = t->del_capacity;
  cost = t->cost;
  d_lowest = t->d_lowest;
  d_highest = t->d_highest;
  info = t->info;
}

double get_delcapacity_math_root(int del_capacity) {
  return sqrt(del_capacity);
}
