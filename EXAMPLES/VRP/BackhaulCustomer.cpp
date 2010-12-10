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

BackhaulCustomer* BackhaulCustomer::approve( BackhaulCustomer* bc, int& pos, Position* p, int i, int& keep_track_cpt) {
  // cout << "approve" << endl;
  int k=0;
  double d;
  BackhaulCustomer* b;
  b=this;
  if (i%2==0) {
  again:
    if (keep_track_cpt - b->get_demand() <0) return NULL;
    if (bc==NULL)  d=b->get_pos()->distance(p);
    else {
      if ( b->get_code() < bc->get_code() )   d=get_dist(b->get_code() + bc->get_code());
      else d=get_dist(bc->get_code() + b->get_code());
      p=bc->get_pos();
    }
    if ( (k!=list_b.size() +1) && (b->get_revenue() - d - get_dist(get_depot()->get_code() + b->get_code()) + p->distance(get_depot()->get_pos())<0) ) {
      // cout << b->get_revenue() << " " <<  d << " " << get_dist(get_depot()->get_code() + b->get_code()) << " " << p->distance(get_depot()->get_pos()) << endl;
      b=random_sel( pos, i);
      k++;
      goto again;
    }
    if (k==list_b.size()+1) {
      // cout << "approved null" << endl;
      return NULL;
    }else {
      keep_track_cpt -= b->get_demand();
      // cout << b->get_code() << " approved." << endl;
      return b;
    }
  }else {
  ag:
    if (keep_track_cpt - b->get_demand() <0) return NULL;
    if (bc==NULL)  d=b->get_pos()->distance(p);
    else {
      if ( b->get_code() < bc->get_code() )   d=get_dist(b->get_code() + bc->get_code());
      else d=get_dist(bc->get_code() + b->get_code());
      p=bc->get_pos();
    }
    if ( (k!=list_bb.size() +1) && (b->get_revenue() - d - get_dist(get_depot()->get_code() + b->get_code()) + p->distance(get_depot()->get_pos())<0) ) {
      // cout << b->get_revenue() << " " <<  d << " " << get_dist(get_depot()->get_code() + b->get_code()) << " " << p->distance(get_depot()->get_pos()) << endl;
      b=random_sel( pos, i);
      k++;
      goto ag;
    }
    if (k==list_bb.size()+1) {
      // cout << "NULL approved" << endl;
      return NULL;
    } else {
      keep_track_cpt -= b->get_demand();
      // cout << b->get_code() << " approved." << endl;
      return b;
    }
  }
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

BackhaulCustomer* BackhaulCustomer::random_sel( int& pos, int i) {
  // cout << " list_b size : " << list_b.size() << endl;
  // cout << " list_bb size : " << list_bb.size() << endl;
  if (i%2==0) {
    if ( list_b.size() > 1 ) {                                 // the depot will (should) never be considered, therefore 1
      pos=irand()%list_b.size();
      while  ( list_b[pos]->get_serviced() ) {              // assumption: depot will always be the first input "customer"
	// cout << "stuck" << endl;	
	pos=irand()%list_b.size();                           // cnt: apparently depot is not eligible for selection
      }
      // cout << "1-Backhaul customer selected " << list_b[pos]->get_code() << " at position " << pos << endl;
      return list_b[pos];
    } else return NULL;
  } else {
    if ( list_bb.size() > 1 ) {                                 // the depot will (should) never be considered, therefore 1
      pos=irand()%list_bb.size();
      while  ( list_bb[pos]->get_serviced() ) {              // assumption: depot will always be the first input "customer"
	pos=irand()%list_bb.size();                           // cnt: apparently depot is not eligible for selection
	//cout << list_b[pos]->code << endl;
      }
      // cout << "2-Backhaul customer selected " << list_bb[pos]->get_code() << " at position " << pos << endl;
      return list_bb[pos];
    } else return NULL;
  }
}


void BackhaulCustomer::erase_c(int random, int i) {
  if (i%2==0) {
    // cout << "1-Backhaul customer : " << list_b[random]->get_code() << " is being erased from position " << random << " of " << list_b.size() << " element list" << endl;
    list_bb.push_back(list_b[random]);
    list_b.erase(list_b.begin() + random);
    // cout << "1-Now list contains : " << list_b.size() << " elements." << endl;
  } else {
    // cout << "2-Backhaul customer : " << list_bb[random]->get_code() << " is being erased from position " << random << " of " << list_bb.size() << " element list" << endl;
    list_b.push_back(list_bb[random]);
    list_bb.erase(list_bb.begin() + random);
    // cout << "2-Now list contains : " << list_bb.size() << " elements." << endl;
  }

}

double BackhaulCustomer::get_dist(string pair) {
  return dist_b[pair];

  /*  map < string, double >::iterator iter= dist_b.find(pair);
  if (iter != dist_b.end() ) return iter->second;
  else return 1000000.0;
  */
}
 

void BackhaulCustomer::compute_dist() {
  map < string, Position* >:: iterator top;
  map < string, Position* >:: iterator nested;
  map < string, Position* >:: iterator next;
  for (top=coord_b.begin(); top!=coord_b.end(); top++) {
    //   cout << top->first << endl;
    next=top;
    next++;
    for (nested=next; nested!=coord_b.end(); nested++) {
      double temp = sqrt( pow( nested->second->get_x() - top->second->get_x(),2 ) + pow( nested->second->get_y() - top->second->get_y(),2 ) );
      dist_b.insert(make_pair(top->first + nested->first, temp));
      // cout << "The distance between " << top->first << " and " << nested->first << " backhaul participators is : " << BackhaulCustomer::get_dist(top->first + nested->first) << " units" <<  endl;
    }
  }
}

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
