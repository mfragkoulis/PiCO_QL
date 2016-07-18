/*
 *   Implement the member methods to LinehaulCustomer 
 *   class.
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
#include <math.h> // cmath would fail for pow(int, int).
#include "LinehaulCustomer.h"


using namespace std;

vector<LinehaulCustomer*> LinehaulCustomer::list_l;

vector <LinehaulCustomer*> LinehaulCustomer::list_ll;

bool LinehaulCustomer::all_Lserviced=false;

int LinehaulCustomer::count_l = 0 ;

map<string, double> LinehaulCustomer::dist_l; 

map <string , Position*> LinehaulCustomer::coord_l; 

extern MTRand_int32 irand;

// Constructs a new LinehaulCustomer object.
LinehaulCustomer::LinehaulCustomer(string c, int d, 
				   Position* pos, 
				   bool depot) : 
    Customer(c, d, pos, depot) {
    coord_l.insert(make_pair(c, pos));
    list_l.push_back(this);
    if (depot) list_ll.push_back(this);
    /*  cout << this->get_code() << " " << c << << 
     * ", serviced: " << depot << endl;
     */
    count_l++;
}

/* Returns true if all Linehaul customers are serviced, 
 * false otherwise.
 */
bool LinehaulCustomer::get_allLserviced() {
    return  all_Lserviced;
}

// Sets all_Lserviced to true.
void LinehaulCustomer::set_allLserviced() {
    if (!all_Lserviced) {
	all_Lserviced = true;
    } else
      all_Lserviced = false;
    // This is the case in this distribution.
    Customer::set_allserviced();
}

// Returns the list of Linehaul customers.
int LinehaulCustomer::get_countl() {
    return count_l;
}

// Returns the depot object.
LinehaulCustomer* LinehaulCustomer::get_depot() {
    // map < string, Position* >:: iterator iter;
    // iter=coord_l.begin();
    // hard-coded.be careful that it is not deleted
    return list_l[0];
}

// Returns a linehaul customer's position.
Position* LinehaulCustomer::get_pos() {
    return coord_l[this->get_code()];
}

// Picks a linehaul customer from the list at random.
LinehaulCustomer* LinehaulCustomer::random_sel(int& pos, int i) {
    vector<LinehaulCustomer*> *list_a;
    if ((i % 2) == 0)
	list_a = &list_l;
    else
	list_a = &list_ll;
    // cout << " list_a size : " << list_a->size() << endl;
    // cout << "list_b size : " << list_b->size() << endl;
    if (list_a->size() > 1) {        
      /* the depot will (should) 
       * never be considered, therefore 1.
       */
	pos = irand() % list_a->size();
	while ((*list_a)[pos]->get_serviced()) {  
	  /* assumption: depot will always be the first 
	   * input "customer".
	   */
	  pos=irand()%list_a->size();       
	  /* cnt: apparently depot is 
	   * not eligible for selection.
	   */
	}
	/* cout <<  "Linehaul customer selected : " << 
	 * (*list_a)[pos]->get_code() << 
	 * " at position : " << pos << endl; 
	 */
	return (*list_a)[pos];
    } else {
	// cout << "NULL!!" << endl;
	LinehaulCustomer::set_allLserviced();
	return NULL;
    }
}


/* Erases a linehaul customer from the active list when 
 * picked for service.
 * The customer is placed in the other list.
 */
void LinehaulCustomer::erase_c(int pos, int i) {
    vector<LinehaulCustomer*> *list_a, *list_b;
    if ((i % 2) == 0) {
	list_a = &list_l;
	list_b = &list_ll;
    } else {
	list_a = &list_ll;
	list_b = &list_l;
    }
    /* cout << "1-Linehaul customer : " << 
     *  (*list_a)[pos]->get_code() << 
     * " is being erased from position " << pos << 
     * " of " << 
     * list_a->size() << " element list" << endl; 
     */
    (*list_a)[pos]->set_serviced();
    list_b->push_back((*list_a)[pos]);
    list_a->erase(list_a->begin() + pos);
    /* cout << "Now list " << i%2 << " contains : " << 
     * list_a->size() << 
     * " elements." << endl; 
     */
}

/* Returns the distance between a pair of linehaul 
 * customers.
 */
double LinehaulCustomer::get_dist(string pair) {
    return dist_l[pair];
}
 
/* Computes the distance between all pairs of linehaul 
 * customers.
 */
void LinehaulCustomer::compute_dist() {
    map<string, Position*>::iterator top;
    map<string, Position*>::iterator nested;
    map<string, Position*>::iterator next;
    for (top = coord_l.begin(); top != coord_l.end(); top++) {
	//    cout << top->first << endl;
	next = top;
	next++;
	for (nested=next; nested!=coord_l.end(); nested++) {
	    double temp = sqrt(pow(nested->second->get_x() - top->second->get_x(), 2) + pow(nested->second->get_y() - 
  top->second->get_y(), 2));
	    dist_l.insert(make_pair(top->first + 
				    nested->first, temp));
	    /* cout << "The distance between " << 
	     *  top->first << 
	     * " and " << nested->first << 
	     * " linehaul participators is : " << 
	     * LinehaulCustomer::get_dist(top->first + 
	     * nested->first) << 
	     * " units" <<  endl; 
	     */
	}
    }
}
