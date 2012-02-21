#include <iostream>
#include <string>
#include "Truck.h"

using namespace std;

int Truck::init_capacity = 0;

bool Truck::starting = true;

extern MTRand_int32 irand;

Truck::Truck( int c ) {
  del_capacity=c;
  pick_capacity=c;
  if (starting) {
    starting=false;
    init_capacity=del_capacity;
  }
  cost=0;
  d_lowest=c;
  p_lowest=c;
  d_highest=0;
  p_highest=0;
}


void Truck::load(LinehaulCustomer* load) {
  // cout << "loading" << endl;
    string half,other;
    int d;
    double c;
    if ( starting ){
	half = LinehaulCustomer::get_depot()->get_code();
	starting = false;
    } else half = cargoArray.back()->get_code();
    cargoArray.push_back(load);
    other = load->get_code();
    if ( half < other ) c = LinehaulCustomer::get_dist( half + other );
    else c = LinehaulCustomer::get_dist( other + half );
    load->set_serviced();
    d = load->get_demand();
    del_capacity -= d;
     // cout << "Del Capacity left after load : " << del_capacity << endl; 
     if (d < d_lowest) d_lowest=d;
     if (d > d_highest) d_highest=d;
     cost += c;
     // cout << half << " + " <<  other << " : " << cost << endl;
     info.push_back(cost);                           
     // cout << "travel time (cost) recorded: " << cost << endl;
}

}

int Truck::get_initcapacity() {
    starting = true;
    return init_capacity;
}

double Truck::get_cost() {
  return cost;
}

int Truck::get_delcapacity() {
  return del_capacity;
}


vector < Customer * > * Truck::get_Customers(){

    return &cargoArray;

}

int Truck::get_cargosize() {
  return cargoArray.size();
}

void Truck::return_todepot() {
  cost += BackhaulCustomer::get_dist( 
      BackhaulCustomer::get_depot()->get_code() + 
      cargoArray.back()->get_code() );
  /* cout << cargoArray.back()->get_code() << " + " << 
     BackhaulCustomer::get_depot()->get_code() << ": " << cost << 
     " , end of truck route. " << endl; */
  /* cout << "Free space left: " << pick_capacity << "/" << init_capacity << 
     endl; */
}


vector < Customer* >::iterator Truck::start() {
  vector < Customer* >::iterator iter;
  iter=cargoArray.begin();
  return iter;
}

vector < Customer* >::iterator Truck::finish() {
  vector < Customer* >::iterator iter;
  iter=cargoArray.end();
  return iter;
}

void Truck::exchange( Truck* t, bool& trap, bool same) {
    // cout << "exchange" << endl;
    trap=false;                              // redundant
    int pos1, pos2, i, d1, pd1, d2, pd2, s1, s2;
    double b1, a1, b2, a2, new_time1, new_time2;
    string pre1="", mid1="", next1="", pre2="", mid2="", next2="", depot;
    i=0;
    if ( (same) && (cargoArray.size()==1) ){    // Won't be able to 
	                                        // exchange positions.
	trap=true;
	goto stop;
    }
    // cout << "1" << endl;
    if ( (d_lowest > t->d_highest + t->del_capacity) || 
	 (t->d_lowest > d_highest + del_capacity) ) {    // No valid exchange 
                                                         // exists between the 
                                                         // two trucks.
	trap=true;
	goto stop;
    }
again:
    if (i==50) {
	trap=true;
	// cout << "TRAP!!!" << endl;
	goto stop;
    }

    i++;
    // cout << " i : " << i << endl;
    pos1=irand()%cargoArray.size();
    pos2=irand()%t->cargoArray.size();
    while ( (same) && (pos1==pos2) ){	
	pos1=irand()%cargoArray.size(); 
	pos2=irand()%t->cargoArray.size();
	/* cout << "stuck " << pos1 << " " << pos2 << " " << same << " " << 
	   cargoArray.size() << endl; */
    }
    
    d1 = cargoArray[pos1]->get_demand();
    d2 = t->cargoArray[pos2]->get_demand();

    /*
      cout << " selected " << pos1 << " with " << pos2 << " " << 
      cargoArray.size() << " " << t->cargoArray.size() << endl;
      cout << "d1: " << d1 << "   del_capacity: " << del_capacity << 
      "   d2: " << d2 << "   t->del_capacity: " << t->del_capacity << 
      "  pd1: " << pd1 << " pd2: " << pd2 << endl;
    */

    if (pos1 < rl_point) {
	/* 
	   cout << "del_cap: " << del_capacity << 
	   "   cargoArray[pos1]->get_demand(): " << 
	   cargoArray[pos1]->get_demand() << 
	   "   t->cargoArray[pos2]->get_demand(): " << 
	   t->cargoArray[pos2]->get_demand() << 
	   "   t->del_capacity: " << t->del_capacity << endl;
	*/
	
	del_capacity = del_capacity + d1 - d2;
	t->del_capacity = t->del_capacity + d2 - d1;
	
	/* 
	   cout << "del_cap: " << del_capacity << 
	   "   cargoArray[pos1]->get_demand(): " << 
	   cargoArray[pos1]->get_demand() << 
	   "   t->cargoArray[pos2]->get_demand(): " << 
	   t->cargoArray[pos2]->get_demand() << 
	   "   t->del_capacity: " << t->del_capacity << endl;
	*/
    }
    
    Customer* c;              // Finally swap customers between the two trucks.
    c=cargoArray[pos1];
    cargoArray[pos1]=t->cargoArray[pos2];
    t->cargoArray[pos2]=c;

    if ( (same) && (pos2+1==pos1) ) {
	add_costs(pos1, mid1, mid2, next1);     
	t->add_costs(pos2, pre2, mid1, mid2);   // Add the costs that 
	                                        // are produced as a 
	                                        // result of the changed route.
    } else {
	add_costs(pos1, pre1, mid2, next1);      
	t->add_costs(pos2, pre2, mid1, next2);
    }
stop:;
}

void Truck::rearrange(int pos, string& pre, string& mid, string& next) {
  // cout << "rearrange" << endl;
  // string pre, mid, next;
  //  cout << " rl_point : " << rl_point << endl;
  // cout << "Cost before : " << cost << endl;
  if (pos==rl_point) {
    // cout << "1" << endl;
    if (rl_point==0) BackhaulCustomer::get_depot()->get_pos()->distance(cargoArray[pos]->get_pos());
    else  cost -= cargoArray[pos-1]->get_pos()->distance(cargoArray[pos]->get_pos());
    // cout <<  cargoArray[pos-1]->get_pos()->distance(cargoArray[pos]->get_pos()) << endl;
    mid=cargoArray[pos]->get_code();
    if (pos+1 < cargoArray.size()) next=cargoArray[pos+1]->get_code();
    else next=BackhaulCustomer::get_depot()->get_code();
    if (mid < next) {
      cost -= BackhaulCustomer::get_dist(mid + next);
      // cout <<  BackhaulCustomer::get_dist(mid + next) << endl;
    } else {
      cost -= BackhaulCustomer::get_dist(next + mid);
      // cout <<  BackhaulCustomer::get_dist(next + mid) << endl;
    }
    // cout << "1" << endl;
  }else if (pos+1==rl_point)  {
    // cout << "2" << endl;
    mid=cargoArray[pos]->get_code();
    if (pos>0) pre=cargoArray[pos-1]->get_code();
    else pre=LinehaulCustomer::get_depot()->get_code();
    if (pre < mid) {
      cost -= LinehaulCustomer::get_dist(pre + mid);
      // cout <<  LinehaulCustomer::get_dist(pre + mid) << endl;
    } else {
      cost -= LinehaulCustomer::get_dist(mid +pre);
      // cout <<  LinehaulCustomer::get_dist(mid +pre) << endl;
    }
    cost -= cargoArray[pos]->get_pos()->distance(cargoArray[pos+1]->get_pos());
    // cout <<  cargoArray[pos]->get_pos()->distance(cargoArray[pos+1]->get_pos()) << endl;
    // cout << "2" << endl;
  }else if (pos+1 < rl_point)  {
    // cout << "3" << endl;
    mid=cargoArray[pos]->get_code();
    if (pos>0) pre=cargoArray[pos-1]->get_code();
    else pre=LinehaulCustomer::get_depot()->get_code();
    if (pre < mid) {
      cost -= LinehaulCustomer::get_dist(pre +mid);
      // cout <<  LinehaulCustomer::get_dist(pre +mid) << endl;
    } else {
      cost -= LinehaulCustomer::get_dist(mid +pre);
      // cout << LinehaulCustomer::get_dist(mid +pre) << endl;
    }
    if (pos+1 < cargoArray.size()) next=cargoArray[pos+1]->get_code();
    else next=LinehaulCustomer::get_depot()->get_code();
    if (mid < next) {
      cost -= LinehaulCustomer::get_dist(mid + next);
      // cout << LinehaulCustomer::get_dist(mid + next) << endl;
    } else {
      cost -= LinehaulCustomer::get_dist(next + mid);
      // cout <<  LinehaulCustomer::get_dist(next + mid) << endl;
    }
    // cout << "3" << endl;
  }else if (pos > rl_point) {
    // cout << "4" << endl;
    mid=cargoArray[pos]->get_code();
    pre=cargoArray[pos-1]->get_code();
    if (pre < mid) {
      cost -=  BackhaulCustomer::get_dist(pre + mid);
      // cout <<  BackhaulCustomer::get_dist(pre + mid) << endl;
    } else {
      cost -= BackhaulCustomer::get_dist(mid + pre);
      // cout <<  BackhaulCustomer::get_dist(mid + pre) << endl;
    }
    if (pos+1 < cargoArray.size()) next=cargoArray[pos+1]->get_code();
    else next=LinehaulCustomer::get_depot()->get_code();
    if (mid < next) {
      cost -= BackhaulCustomer::get_dist(mid + next);
      // cout <<  BackhaulCustomer::get_dist(mid + next) << endl;
    } else {
      cost -= BackhaulCustomer::get_dist(next + mid);
      // cout <<  BackhaulCustomer::get_dist(next + mid) << endl;
    }
    // cout << "4" << endl;
  }
  // cout << "Cost after : " << cost << endl;
  // cout << "pre: " << pre << ", mid: " << mid << ", next: " << next << endl;
} 


void Truck::add_costs(int pos, string pre, string mid, string next) {
  // cout << "add" << endl;
  //int c1, c2;
  //c1=cost;
  // cout << endl << "pre: " << pre << ", mid: " << mid << ", next: " << next << endl;
  // cout << "Before adding : " << cost << endl;
  if (pos+1 < rl_point) {
    if (pre < mid) {
      cost += LinehaulCustomer::get_dist(pre +mid);
      // cout <<  LinehaulCustomer::get_dist(pre +mid) << endl;
    } else {
      cost += LinehaulCustomer::get_dist(mid +pre);
      // cout << LinehaulCustomer::get_dist(mid +pre) << endl;
    }
    if (mid < next) {
      cost += LinehaulCustomer::get_dist(mid + next);
      // cout << LinehaulCustomer::get_dist(mid + next) << endl;
    } else {
      cost += LinehaulCustomer::get_dist(next + mid);
      // cout <<  LinehaulCustomer::get_dist(next + mid) << endl;
    }
    // cout << "6" << endl;
  }
  if (next.length()==0) {
    cost += cargoArray[pos+1]->get_pos()->distance(cargoArray[pos]->get_pos());
    // cout <<  cargoArray[pos+1]->get_pos()->distance(cargoArray[pos]->get_pos()) << endl;
    if (pre < mid) {
      cost += LinehaulCustomer::get_dist(pre + mid);
      // cout <<  LinehaulCustomer::get_dist(pre + mid) << endl;
    } else {
      cost += LinehaulCustomer::get_dist(mid + pre);
      // cout <<  LinehaulCustomer::get_dist(mid + pre) << endl;
    }
    // cout << "8" << endl;
  }
  // c2=cost;
  // cout << "After addition : " << cost << endl;
}

void Truck::assignC(Truck* t) {
  // cout << "assignC" << endl;
  for (int i=0; i!=t->cargoArray.size();i++) {
    // cout << i << endl;
    cargoArray.push_back(t->cargoArray[i]);
    // cout << t->cargoArray
  }
  del_capacity=t->del_capacity;
  pick_capacity=t->pick_capacity;
  cost=t->cost;
  d_lowest=t->d_lowest;
  p_lowest=t->p_lowest;
  d_highest=t->d_highest;
  p_highest=t->p_highest;
  rl_point=t->rl_point;
  info=t->info;
}


void Truck::reassignC(Truck* t) {
  // cout << "reassignC" << endl;
  for (int i=0; i!=t->cargoArray.size();i++) {
    // cout << i << endl;
    cargoArray[i]=t->cargoArray[i];
    // cout << t->cargoArray
  }
  del_capacity=t->del_capacity;
  pick_capacity=t->pick_capacity;
  cost=t->cost;
  d_lowest=t->d_lowest;
  p_lowest=t->p_lowest;
  d_highest=t->d_highest;
  p_highest=t->p_highest;
  rl_point=t->rl_point;
  info=t->info;
}

