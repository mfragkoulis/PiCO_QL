/*   Copyright [2012] [Marios Fragkoulis]
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
 *  permissions and limitations under the License.
 */

#include <iostream>
#include <math.h>
#include "BackhaulCustomer.h"

using namespace std;

vector < BackhaulCustomer* > BackhaulCustomer::nonser_list;

vector < BackhaulCustomer* > BackhaulCustomer::list_b;

vector < BackhaulCustomer* > BackhaulCustomer::list_bb;

int BackhaulCustomer::count_b = 0 ;

map < string, double > BackhaulCustomer::dist_b; 

map < string , Position* > BackhaulCustomer::coord_b; 

bool BackhaulCustomer::selective=false;

extern MTRand_int32 irand;

// Constructs new Backhaul customer object.
BackhaulCustomer::BackhaulCustomer (string c, int d, int r, Position* pos, bool depot) : Customer(c, d, pos, depot ) {
  revenue=r;
  coord_b.insert( make_pair(c, pos) );
  list_b.push_back(this);
  count_b++;
  // cout <<"b" << endl;
}

// Returns the total number of Backhaul customers.
int BackhaulCustomer::get_countb() {
  return count_b;
}

// Returns the depot object.
BackhaulCustomer* BackhaulCustomer::get_depot() {
  return list_b[0];             // Careful not to delete depot in erase_c. 
}

// Returns the position of a Backhaul customer.
Position* BackhaulCustomer::get_pos() {
  return coord_b[this->get_code()];
}

// Returns the revenue associated with a backhaul customer.
int BackhaulCustomer::get_revenue() {
  return revenue;
}

// Returns true for VRPSelectiveBackhaul variant. 
// Not applicable in this distribution.
bool BackhaulCustomer::get_selective() {
  return selective;
}

// Sets truw if VRPSB.
void BackhaulCustomer::set_selective(bool s) {
  selective=s;
}

// Selects a Backhaul customer at random.
BackhaulCustomer* BackhaulCustomer::random_sel( int& pos, int i) {}

// Erases a Backhaul customer picked for service.
void BackhaulCustomer::erase_c(int random, int i) {}

// Returns the distance betwen two backhaul customers.
double BackhaulCustomer::get_dist(string pair) {
  return dist_b[pair];
}
 
// Computes the distance between all pairs of backhaul customers.
void BackhaulCustomer::compute_dist() {}

// Returns the non-serviced backhaul customers. Applicable in VRPSB. 
void BackhaulCustomer::non_serviced(int k) {}

// Returns an iterator to the end of the non-serviced list.
vector < BackhaulCustomer* >::iterator BackhaulCustomer::get_nonser() {
  vector < BackhaulCustomer* >::iterator it;
  // cout << "in" << endl;
  it=nonser_list.end() - 1;
  // cout << (*it)->get_code() << endl;
  return it;

}

// Clears the list of backhaul customers currently active.
// When the one is empty, the other is full.
void BackhaulCustomer::clear_list(int i) {
    if ((i % 2) == 0) {
    list_b.clear();
    list_b.push_back(list_bb[0]);
  } else {
      list_bb.clear();
      list_bb.push_back(list_b[0]);
  }
}

// Copies the non-serviced customers to the non-serviced list.x
void BackhaulCustomer::cp_list(int i) {
  // vector < BackhaulCustomer* >::iterator it;
    if ((i % 2) == 0) {
    nonser_list=list_b;
    /* cout << "code is ";
    for (it = list_b.begin(); it != list_b.end(); it++) {
      cout << (*it)->get_code() << endl;
      }*/
  } else nonser_list=list_bb;
}
