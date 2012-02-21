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

BackhaulCustomer::BackhaulCustomer (string c, int d, int r, Position* pos, bool depot) : Customer( c, d, pos, depot ) {
  revenue=r;
  coord_b.insert( make_pair(c, pos) );
  list_b.push_back(this);
  list_bb.push_back(this);
  count_b++;
  // cout <<"b" << endl;
}

BackhaulCustomer::BackhaulCustomer (string c, int d, int r, Position* pos) : Customer( c, d, pos ) {
  revenue=r;
  coord_b.insert( make_pair(c, pos) );
  list_b.push_back(this);
  count_b++;
  // cout << c << endl;
}

int BackhaulCustomer::get_countb() {
  return count_b;
}


BackhaulCustomer* BackhaulCustomer::get_depot() {
  // map < string, Position* >:: iterator iter;
  //iter=coord_b.begin();
  return list_b[0];             // careful not to delete depot in erase_c. kind of hard coded. depot should be the first to be input
}


Position* BackhaulCustomer::get_pos() {
  return coord_b[this->get_code()];

  /*  map < string, Position* >:: iterator fp=coord_b.find(this);
  if (fp!=coord_b.end() ) return fp->second;
  else return NULL;
  */
}


int BackhaulCustomer::get_revenue() {
  return revenue;
}


bool BackhaulCustomer::get_selective() {
  return selective;
}

void BackhaulCustomer::set_selective(bool s) {
  selective=s;
}

BackhaulCustomer* BackhaulCustomer::random_sel( int& pos){}


void BackhaulCustomer::erase_c(int random) {}

double BackhaulCustomer::get_dist(string pair) {
  return dist_b[pair];

  /*  map < string, double >::iterator iter= dist_b.find(pair);
  if (iter != dist_b.end() ) return iter->second;
  else return 1000000.0;
  */
}
 

void BackhaulCustomer::compute_dist() {}

void BackhaulCustomer::non_serviced(int k) {
  if (k%2==0) {
    if (list_b.size() > 1) {
      cout << list_b.size() -1 << " Backhaul customers not serviced after scheduling namely: " << endl ;
      for (int i=1; i!=list_b.size(); i++) {
	list_b[i]->set_serviced();
	list_bb.push_back(list_b[i]);
	cout  << list_b[i]->get_code() << endl;
      }
    }
  } else {
    if (list_bb.size() > 1) {
      cout << list_bb.size() -1 << " Backhaul customers not serviced after scheduling namely: " << endl ;
      for (int i=1; i!=list_bb.size(); i++) {
	list_bb[i]->set_serviced();
	list_b.push_back(list_bb[i]);
	cout  << list_bb[i]->get_code() << endl;
      }
    }
  }
}

vector < BackhaulCustomer* >::iterator BackhaulCustomer::get_nonser() {
  vector < BackhaulCustomer* >::iterator it;
  // cout << "in" << endl;
  it=nonser_list.end() - 1;
  // cout << (*it)->get_code() << endl;
  return it;

}

void BackhaulCustomer::clear_list(int i) {
  if (i%2==0) {
    list_b.clear();
    list_b.push_back(list_bb[0]);
  } else {
      list_bb.clear();
      list_bb.push_back(list_b[0]);
  }
}

void BackhaulCustomer::cp_list(int i) {
  // vector < BackhaulCustomer* >::iterator it;
  if (i%2==0) {
    nonser_list=list_b;
    /* cout << "code is ";
    for (it=list_b.begin();it!=list_b.end();it++) {
      cout << (*it)->get_code() << endl;
      }*/
  } else nonser_list=list_bb;
}
