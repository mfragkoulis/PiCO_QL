#include <iostream>
#include <math.h>
#include <cstdlib>
#include "Customer.h"

using namespace std;

// extern MTRand_int32 irand;

Customer::Customer() {

}

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

Customer::~Customer() {

}

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

int Customer::get_revenue() {

}

int Customer::get_pickdemand() {

}


int Customer::get_starttime() {

}

int Customer::get_servicetime() {

}

int Customer::get_finishtime() {

}

Position* Customer::get_pos() { 
  // return coord[this->get_code()];

  //  map < string, Position* >:: iterator fp=coord.find(this->get_code());
  // if (fp!=coord.end() ) return fp->second;
  // else return NULL;
}

bool Customer::get_allserviced() {
  return all_serviced;
}

void Customer::set_allserviced() {
  if (!all_serviced) all_serviced=true;
  else all_serviced=false;
}


Customer* Customer::get_depot() {
/* // map < string, Position* >:: iterator iter;
  // iter=coord.begin();
  return list[0];        // list does not exist in Customer
*/
}

Customer* Customer::random_sel(int& pos) { 
  /*  if ( list_c.size() > 1 ) {                                 // the depot will (should) never be considered, therefore 1
    int random=irand()%list_c.size();
    while  ( list_c[random]->get_serviced() ) {              // assumption: depot will always be the first input "customer"
      random=irand()%list_c.size();                           // cnt: apparently depot is not eligible for selection
    }
    cout << list_c[random]->get_code() << endl;
    pos=random;
    return list_c[random];
  } else return NULL;
  */
}


void Customer::erase_c(int random) {
  //  list_c.erase(list_c.begin() + random);

}


double Customer::get_dist(string pair) {
  //  return dist[pair];

  /*  map < string, double >::iterator iter= dist.find(pair);
  if (iter != dist.end() ) return iter->second;
  else return 1000000.0;
*/
}

void Customer::compute_dist() { 
  /*  map < string, Position* >:: iterator top;
  map < string, Position* >:: iterator nested;
  map < string, Position* >:: iterator next;
  for (top=coord.begin(); top!=coord.end(); top++) {
  next=top;
  next++;
    for (nested=next; nested!=coord.end(); nested++) {
      double temp = sqrt( pow( nested->second->get_x() - top->second->get_x(),2 ) + pow( nested->second->get_y() - top->second->get_y(),2 ) );
      dist.insert(make_pair(top->first + nested->first, temp));
      cout << "The distance between " << top->first << " and " << nested->first << " participators is : " <<
	Customer::get_dist(top->first + nested->first) << " units" <<  endl;
    }
  }
  */
}

