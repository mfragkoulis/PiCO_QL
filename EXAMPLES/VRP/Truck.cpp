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


void Truck::load( LinehaulCustomer* before_load, LinehaulCustomer* load, int diff ) {
  // cout << "loading" << endl;
  // loading=true;
   string half,other;
   int d;
   double c;
   if ( del_capacity>0 ) {                            // a bit redundant but as they say in basketball two hands for safety!
     cargoArray.push_back(load);
     load->set_serviced();
     // if ( load->get_pickdemand() > load->get_demand() )  d=load->get_pickdemand();             // only in VRPPD
     //  else d=load->get_demand();
     d=load->get_demand();
     del_capacity -= d;
     // cout << "Del Capacity left after load : " << del_capacity << endl; 
     if (del_capacity < 0) del_capacity += d;                            // redundant. SHOULD NEVER BE THE CASE
     if (d < d_lowest) d_lowest=d;                                        // used in optimization.could be problematic in VRPPD.
     if (d > d_highest) d_highest=d;
     if ( before_load != NULL )  half = before_load->get_code();
     else  half = LinehaulCustomer::get_depot()->get_code();
     other=cargoArray.back()->get_code();
     if ( half < other ) c = LinehaulCustomer::get_dist( half + other );
     else c = LinehaulCustomer::get_dist( other + half );
     cost +=c;
     // cout << half << " + " <<  other << " : " << cost << endl;
     if (LinehaulCustomer::get_vrppd()) {
       info.push_back((double)diff);
       //cout << "diff recorded: " << diff << endl;
     }else if (LinehaulCustomer::get_vrptw()) {
       if (cost < load->get_starttime()) {
	 //cout << "WAITING TIME: " << load->get_starttime() - cost << endl;
	 cost=load->get_starttime();
	 // cout << "now cost: " << cost << endl;
       }
       cost += load->get_servicetime();
       info.push_back(cost);                           
       // cout << "travel time (cost) recorded: " << cost << endl;
     }
  }
}

void Truck::reload( BackhaulCustomer* before_load, BackhaulCustomer* load ) {
   string half,other;
   int d;
   if ( pick_capacity > 0 ) {                            // a bit redundant but as they say in basketball two hands for safety!
     cargoArray.push_back(load);
     cargoArray.back()->set_serviced();
     d=cargoArray.back()->get_demand();
     pick_capacity -= d;
     if (pick_capacity < 0) pick_capacity += d;
     if (d < p_lowest) p_lowest=d;
     if (d > p_highest) p_highest=d;
     if ( before_load != NULL )  half = before_load->get_code();
     else cout << "RELOAD ERROR" << endl;
     other=cargoArray.back()->get_code();
     if ( half < other ) cost = cost + BackhaulCustomer::get_dist( half + other ) - load->get_revenue();
     else cost = cost + BackhaulCustomer::get_dist( other + half ) - load->get_revenue();
     // cout << half << " + " <<  other << " : " << cost << endl;
     // cout << "pick_capacity: " << pick_capacity << endl;    
  }
}

void Truck::reload( BackhaulCustomer* before_load, BackhaulCustomer* load, Position* pos ) {
   string half,other;
   int d;
   rl_point=cargoArray.size();                      // not yet pushed back so size gives position
   if ( pick_capacity > 0 ) {                            // a bit redundant but as they say in basketball two hands for safety!
     cargoArray.push_back(load);
     cargoArray.back()->set_serviced();
     d=cargoArray.back()->get_demand();
     pick_capacity -= d;
     if (pick_capacity < 0) pick_capacity += d;
     if (d < p_lowest) p_lowest=d;
     if (d > p_highest) p_highest=d;
     if ( before_load == NULL ) {
       cost = cost + pos->distance( load->get_pos() ) - load->get_revenue();
       //cout << "START PICKING UP NOW : " << load->get_code() << " and cost is now : " << cost << endl;
       // cout << "pick_capacity: " << pick_capacity << endl;    
     } else cout << "RELOAD ERROR 2" << endl;
     cost -= load->get_revenue();
   }
}

int Truck::get_initcapacity() {
  return init_capacity;
}

double Truck::get_cost() {
  return cost;
}

int Truck::get_delcapacity() {
  return del_capacity;
}

int Truck::get_pickcapacity() {
  return pick_capacity;
}

int Truck::get_rlpoint() {
  return rl_point;
}

vector < Customer * > * Truck::get_Customers(){

    return &cargoArray;

}

int Truck::get_cargosize() {
  return cargoArray.size();
}

void Truck::return_todepot() {
  cost += BackhaulCustomer::get_dist( BackhaulCustomer::get_depot()->get_code() + cargoArray.back()->get_code() );
  //cout << cargoArray.back()->get_code() << " + " << BackhaulCustomer::get_depot()->get_code() << ": " << cost << " , end of truck route. " << endl;
  //cout << "Free space left: " << pick_capacity << "/" << init_capacity << endl;
}

void Truck::back_noPickup(int bound) {
  string half, other;
  double c;
  rl_point=100;                                     // position represents absence of backhaul customers
  half=LinehaulCustomer::get_depot()->get_code();
  other=cargoArray.back()->get_code();
  // cout << "last customer: " << other << endl;
  c= LinehaulCustomer::get_dist( half + other );
  cost += c;
  if (LinehaulCustomer::get_vrppd()) {                  // VRPPD. make it better I don't like it. unreliable
    vector < double >:: iterator iter;
    info.push_back((double)bound);
    if (del_capacity <0) cout << "YOU SCREWED UP BIG TIME" << endl;
    for (iter=info.begin(); iter!=info.end()-1; iter++) {
      (*iter) = del_capacity - (*iter);                                // set final available space after each customer service (pick-up included) as it has been stabilised after route construction
      //  cout << (*iter) << endl;
    }
    // cout << "bound: " << (*iter) << endl;
  }else if (LinehaulCustomer::get_vrptw()) {
    // cout << "final travel time (cost) : " << cost << endl;
  }
  // cout << other << " + " << half << ": " << cost << " , end of truck route. " << endl;
  // cout << "Free space left: " << del_capacity << "/" << init_capacity << endl;
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
  if ( (LinehaulCustomer::get_vrptw()) && (same) ) {
    trap=true;
    goto stop;
  }
  if ( ((same) && (cargoArray.size()==1)) || ((same) && (rl_point+1==cargoArray.size())) ) {             // won't be able to exchange positions either l or b
    trap=true;
    goto stop;
  }
  // cout << "1" << endl;
  if ( (rl_point > 0) && (t->rl_point > 0)  &&  ((d_lowest > t->d_highest + t->del_capacity) || (t->d_lowest > d_highest + del_capacity)) ) {    //no valid exchange exists between the two trucks 
    trap=true;
    goto stop;
  }
  // cout << "2" << endl;
  if ( (rl_point < 100) && (t->rl_point < 100)  &&  ((p_lowest > t->p_highest + t->pick_capacity) || (t->p_lowest > p_highest + pick_capacity)) ) {
    trap=true;
    goto stop;
  }
  // cout << "3" << endl;
 again:
  // cout << endl;
  if (i==50) {
    trap=true;
    // cout << "TRAP!!!" << endl;
    goto stop;
  } 
  i++;
  // cout << " i : " << i << endl;
  pos1=irand()%cargoArray.size();
  pos2=irand()%t->cargoArray.size();
  while ( ((same) && (pos1==pos2)) || ((pos1<rl_point) && (pos2>=t->get_rlpoint()) || (pos1>=rl_point) && (pos2 < t->get_rlpoint())) ) {
    pos1=irand()%cargoArray.size(); 
    pos2=irand()%t->cargoArray.size();
    // cout << "stuck " << pos1 << " " << pos2 << " " << same << " " << cargoArray.size() << endl;
  }


  /*  if (pos1 < rl_point ) cout << " < " << pos1 << " " << pos2 << " " << rl_point << " " << t->rl_point << " " << cargoArray[pos1]->get_demand() + del_capacity << " " <<  t->cargoArray[pos2]->get_demand() << " " << t->cargoArray[pos2]->get_demand() + t->del_capacity << " " << cargoArray[pos1]->get_demand() << endl;
  if (pos1 >= rl_point ) cout << " >= " << pos1 << " " << pos2 << " " << rl_point << " " << t->rl_point << " " << cargoArray[pos1]->get_demand() + pick_capacity << " " <<  t->cargoArray[pos2]->get_demand() << " " << t->cargoArray[pos2]->get_demand() + t->pick_capacity << " " << cargoArray[pos1]->get_demand() << endl;
  cout << "cnd2" << endl;
  
  cout << "passed" << endl;
  cout << " selected " << pos1 << " with " << pos2 << " " << rl_point << " " << t->rl_point << endl;
  cout << "   del_capacity: " << del_capacity << "   t->del_capacity: " << t->del_capacity << endl;
  cout << "demand: " << cargoArray[pos1]->get_demand() << "  pickdemand(): " << cargoArray[pos1]->get_pickdemand() << endl;
  cout <<  "  t->demand: " << t->cargoArray[pos2]->get_demand() << "  t->pickdemand: " << t->cargoArray[pos2]->get_pickdemand() << endl;
  */

  // cout << "pos1: " << pos1 << ", pos2: " << pos2 << endl;

  if (LinehaulCustomer::get_vrptw()) {
      if ( (pos1 - pos2 >1) || (pos2 - pos1 >1) ) {                                        // the feasibility of exchanges in vrptw has a strong deterministic character
	if ( (pos2+1 < cargoArray.size()) && (pos2+1 < t->cargoArray.size()) ) pos1=pos2;
	else if ( (pos1+1 < t->cargoArray.size()) && (pos1+1 < cargoArray.size()) ) pos2=pos1;
	else {
	  trap=true;
	  goto stop;
	}
      }else if ( (pos1+1 >= t->cargoArray.size()) || (pos2+1 >= cargoArray.size()) || (pos1+1==cargoArray.size()) || (pos2+1==t->cargoArray.size()) ) {
	if ( (pos2+1 < cargoArray.size()) && (pos2+1 < t->cargoArray.size()) ) pos1=pos2;
	else if ( (pos1+1 < t->cargoArray.size()) && (pos1+1 < cargoArray.size()) ) pos2=pos1;
	else {
	  trap=true;
	  goto stop;
	}
      }
  }

  // cout << "where" << endl;

  d1 = cargoArray[pos1]->get_demand();
  pd1 = cargoArray[pos1]->get_pickdemand();
  d2 = t->cargoArray[pos2]->get_demand();
  pd2 = t->cargoArray[pos2]->get_pickdemand();

  // cout << "not here" << endl;


  // cout << " selected " << pos1 << " with " << pos2 << " " << cargoArray.size() << " " << t->cargoArray.size() << endl;
  // cout << "d1: " << d1 << "   del_capacity: " << del_capacity << "   d2: " << d2 << "   t->del_capacity: " << t->del_capacity << "  pd1: " << pd1 << " pd2: " << pd2 << endl;
  if (LinehaulCustomer::get_vrppd()) {
    if ( ((pos1 < rl_point) && (d1 + del_capacity - info.back() < d2)) || ((pos1 >= rl_point) && (d1 + pick_capacity < d2)) || ((pos2 < t->rl_point) && (d2 + t->del_capacity - t->info.back() < d1)) || ((pos2 >= t->rl_point) && (d2 + t->pick_capacity < d1)) )      goto again;

  }else {
    if ( ((pos1 < rl_point) && (d1 + del_capacity < d2)) || ((pos1 >= rl_point) && (d1 + pick_capacity < d2)) || ((pos2 < t->rl_point) && (d2 + t->del_capacity < d1)) || ((pos2 >= t->rl_point) && (d2 + t->pick_capacity < d1)) )      goto again;
  }

  if ( LinehaulCustomer::get_vrppd() ) {                      // VRPPD. make it better.unreliable
    // vector < double >::iterator iter;
    for (int i=0; i<pos1; i++) {
      if (info[i] + d1 - d2 <0) goto again;
    }
    for (int i=pos1; i<info.size() -1; i++) {
      if (info[i] + pd1 - pd2 <0) goto again;
    }
    for (int i=0; i<pos2; i++) {
      if (t->info[i] + d2 - d1 <0) goto again;
    }
    for (int i=pos2; i<t->info.size() -1; i++) {
      if (t->info[i] + pd2 - pd1 <0) goto again;
    }
  }

  else if (LinehaulCustomer::get_vrptw()) {
    // cout << "pro ton pilon" << endl;

    s1=cargoArray[pos1]->get_servicetime();
    s2=t->cargoArray[pos2]->get_servicetime();

    depot=LinehaulCustomer::get_depot()->get_code();
    if (pos1>0) pre1=cargoArray[pos1-1]->get_code();
    mid1=cargoArray[pos1]->get_code();
    if (pos1+1 < cargoArray.size()) next1=cargoArray[pos1+1]->get_code();
    if (pos2>0) pre2=t->cargoArray[pos2-1]->get_code();
    mid2=t->cargoArray[pos2]->get_code();
    if (pos2+1 < t->cargoArray.size()) next2=t->cargoArray[pos2+1]->get_code();

    // cout << "pre1: " << pre1 << ", mid1: " << mid1 << ", next1: " << next1 << ", pre2: " << pre2 << ", mid2: " << mid2 << ", next2 : " << next2 << endl; 

    if (pos1==0) b1=LinehaulCustomer::get_dist(depot + mid2);        //s2*3
    else {
      if (pre1 < mid2) b1= info[pos1-1] + LinehaulCustomer::get_dist(pre1 + mid2);
      else b1= info[pos1-1] + LinehaulCustomer::get_dist(mid2 + pre1);
    }

    // cout << "b1: " << b1 << endl;

    if ( b1 > t->cargoArray[pos2]->get_finishtime()) goto again;

    // if (pos1>0) cout << "info[pos1-1]: " << info[pos1-1] << endl;

    if (b1 < t->cargoArray[pos2]->get_starttime()) b1= t->cargoArray[pos2]->get_starttime();

    if (next1.length()==0) a1=LinehaulCustomer::get_dist(depot + mid2);
    else {
      if (mid2 < next1) a1=LinehaulCustomer::get_dist(mid2 + next1);
      else a1=LinehaulCustomer::get_dist(next1 + mid2);
    }

    // cout << "a1: " << a1 << endl;

    if (next1.length()==0) new_time1 = cost - b1 - a1 - s2;
    else new_time1 = info[pos1+1] - cargoArray[pos1+1]->get_servicetime() - b1 - a1 - s2;

    // cout << "new time1: " << new_time1 << ", cost: " << cost << endl;

    if ( (cost - new_time1) > (double)LinehaulCustomer::get_deadline() ) goto again;



    if (pos2==0) b2=LinehaulCustomer::get_dist(depot + mid1);           //s1*3
    else {
      if (pre2 < mid1) b2= t->info[pos2-1] + LinehaulCustomer::get_dist(pre2 + mid1);
      else b2= t->info[pos2-1] + LinehaulCustomer::get_dist(mid1 + pre2);
    }

    // cout << "b2: " << b2 << endl;

    if ( b2 > cargoArray[pos1]->get_finishtime()) goto again;

    // if (pos2>0) cout << "t->info[pos2-1]: " << t->info[pos2-1] << endl;

    if (b2 < cargoArray[pos1]->get_starttime()) b2= cargoArray[pos1]->get_starttime();

    if (next2.length()==0) a2=LinehaulCustomer::get_dist(depot + mid1);
    else {
      if (mid1 < next2) a2=LinehaulCustomer::get_dist(mid1 + next2);
      else a2=LinehaulCustomer::get_dist(next2 + mid1);
    }

    // cout << "a2: " << a2 << endl;

    if (next2.length()==0) new_time2 = t->cost - b2 - a2 - s1;
    else new_time2 = t->info[pos2+1] - t->cargoArray[pos2+1]->get_servicetime() - b2 - a2 - s1;

    // cout << "new time2: " << new_time2 << ", t->cost: " << t->cost << endl;

    if ( (t->cost - new_time2) > (double)LinehaulCustomer::get_deadline() ) goto again;

    // cout << "mpeeee" << endl;
  }

  else if ( (BackhaulCustomer::get_selective()) && (pos1>rl_point) ) {                           // VRPSB , preserve validity of solution as perceived from specs (existence vs inexistence)
    // cout << "in" << endl;
    if (pos2>0) {
      if (cargoArray[pos1]->get_revenue() - cargoArray[pos1]->get_pos()->distance(t->cargoArray[pos2-1]->get_pos()) - BackhaulCustomer::get_dist(BackhaulCustomer::get_depot()->get_code() + cargoArray[pos1]->get_code()) + BackhaulCustomer::get_depot()->get_pos()->distance( t->cargoArray[pos2-1]->get_pos())  <0) goto again;
    }else {
      if (cargoArray[pos1]->get_revenue() - 2*BackhaulCustomer::get_dist(BackhaulCustomer::get_depot()->get_code() + cargoArray[pos1]->get_code())  <0) goto again;
    } 
    if (pos1>0) {
      if (t->cargoArray[pos2]->get_revenue() - t->cargoArray[pos2]->get_pos()->distance(cargoArray[pos1-1]->get_pos()) - BackhaulCustomer::get_dist(BackhaulCustomer::get_depot()->get_code() + t->cargoArray[pos2]->get_code()) + BackhaulCustomer::get_depot()->get_pos()->distance( cargoArray[pos1-1]->get_pos())  <0) goto again;
    }else {
      if (t->cargoArray[pos2]->get_revenue() - 2* BackhaulCustomer::get_dist(BackhaulCustomer::get_depot()->get_code() + t->cargoArray[pos2]->get_code())  <0) goto again;
    } 
  }

  if (!LinehaulCustomer::get_vrptw()) {
    // cout << " go rearrange " << pos1 << " with " << pos2 << " " << rl_point << " " << t->rl_point << endl << endl;
    rearrange(pos1, pre1, mid1, next1);                                                        // subtract costs connected with positions in change
    t->rearrange(pos2, pre2, mid2, next2);                                                     // rearrange capacities in trucks to reflect the correct ones after movements
  }
  if (pos1 < rl_point) {

    // cout << "del_cap: " << del_capacity << "   cargoArray[pos1]->get_demand(): " << cargoArray[pos1]->get_demand() << "   t->cargoArray[pos2]->get_demand(): " << t->cargoArray[pos2]->get_demand() << "   t->del_capacity: " << t->del_capacity << endl;
    // cout << "pick_cap: " << pick_capacity << "   cargoArray[pos1]->get_pickdemand(): " << cargoArray[pos1]->get_pickdemand() << "   t->cargoArray[pos2]->get_pickdemand(): " << t->cargoArray[pos2]->get_pickdemand() << "   t->pick_capacity: " << t->pick_capacity << endl;

    del_capacity = del_capacity + d1 - d2;
    t->del_capacity = t->del_capacity + d2 - d1;

    // cout << "del_cap: " << del_capacity << "   cargoArray[pos1]->get_demand(): " << cargoArray[pos1]->get_demand() << "   t->cargoArray[pos2]->get_demand(): " << t->cargoArray[pos2]->get_demand() << "   t->del_capacity: " << t->del_capacity << endl;
    // cout << "pick_cap: " << pick_capacity << "   cargoArray[pos1]->get_pickdemand(): " << cargoArray[pos1]->get_pickdemand() << "   t->cargoArray[pos2]->get_pickdemand(): " << t->cargoArray[pos2]->get_pickdemand() << "   t->pick_capacity: " << t->pick_capacity << endl;

    if (LinehaulCustomer::get_vrptw()) {
      cost -= new_time1;
      t->cost -= new_time2;
      // cout << "new_time1: " <<  new_time1 << ", cost: " << cost << ", new time2: " << new_time2 << ", t->cost: " << t->cost << endl;
    }
  } else {
    // cout << "pos >= rl_point" << endl;
    pick_capacity = pick_capacity + d1 - d2;
    t->pick_capacity = t->pick_capacity + d2 - d1;
    cost = cost + cargoArray[pos1]->get_revenue() - t->cargoArray[pos2]->get_revenue();                     // VRPSB
    t->cost = t->cost + t->cargoArray[pos2]->get_revenue() - cargoArray[pos1]->get_revenue();
  }


  if ( LinehaulCustomer::get_vrppd() ) {                      // VRPPD. make it better.unreliable
    // vector < double >::iterator iter;             // update available spaces and bounds
    for (int i=0; i<pos1; i++) {
      // cout << info[i] << " - ";
      info[i] = info[i] + d1 - d2;
      // cout << info[i] << endl;
    }
    for (int i=pos1; i<info.size() -1; i++) {
      // cout << info[i] << " - ";
      info[i] = info[i] + pd1 - pd2;
      // cout << info[i] << endl;
    }
    // cout << endl << "t now: " << endl << endl;
    for (int i=0; i<pos2; i++) {
      // cout << t->info[i] << " - ";
      t->info[i] = t->info[i] + d2 - d1;
      // cout << t->info[i] << endl;
    }
    for (int i=pos2; i<t->info.size() -1; i++) {
      // cout << t->info[i] << " - ";
      t->info[i] = t->info[i] + pd2 - pd1;
      // cout << t->info[i] << endl;
    }
    // cout << "bound: " << info.back() << " - ";
    info.back() =0;
    for (int i=0; i< info.size() -1; i++) {
      if (del_capacity - info[i] > info.back()) info.back() = del_capacity - info[i]; 
    }
    // cout << info.back() << endl;
    // cout << "t->bound: " << t->info.back() << " - ";
    t->info.back() =0;
    for (int i=0; i< t->info.size() -1; i++) {
      if (t->del_capacity - t->info[i] > t->info.back()) t->info.back() = t->del_capacity - t->info[i]; 
    }
    // cout << t->info.back() << endl << endl;
  }else if (LinehaulCustomer::get_vrptw()) {
    // cout << "before, info[pos1]: " << info[pos1] << endl;
    info[pos1]=b1 + s2;
    // cout << "after, info[pos1]: " << info[pos1] << endl;
    if (pos1+1 < info.size()) {
      // cout << "before, info[pos1+1]: " << info[pos1+1] << endl;
      info[pos1+1] = info[pos1] + a1 + cargoArray[pos1+1]->get_servicetime();
      // cout << "after, info[pos1+1]: " << info[pos1+1] << endl;
    }
    for (int i=pos1+2; i<info.size(); i++) {
      // cout << "before, info[i]: " << info[i] << endl;
      info[i]= info[i] - new_time1;
      // cout << "after, info[i]: " << info[i] << endl;
    }
    // cout <<"before, t->info[pos2]: " << t->info[pos2] << endl;
    t->info[pos2]=b2 + s1;
    // cout << "after, t->info[pos2]: " << t->info[pos2] << endl;
    if (pos2+1 < t->info.size()) {
      // cout <<"before, t->info[pos2+1]: " << t->info[pos2+1] << endl;
      t->info[pos2+1] = t->info[pos2] + a2 + t->cargoArray[pos2+1]->get_servicetime();
      // cout << "after, t->info[pos2+1]: " << t->info[pos2+1] << endl;
    }
    for (int i=pos2+2; i<t->info.size(); i++) {
      // cout <<"before, t->info[i]: " << t->info[i] << endl;
      t->info[i]= t->info[i] - new_time2;
      // cout << "after, t->info[i]: " << t->info[i] << endl;
    }
   
  }



  Customer* c;                                                                               // finally swap customers between the two trucks
  c=cargoArray[pos1];
  cargoArray[pos1]=t->cargoArray[pos2];
  t->cargoArray[pos2]=c;

  if (!LinehaulCustomer::get_vrptw()) {
    // cout << " go add " << endl;
    if ( (same) && (pos1+1==pos2) ) {
      add_costs(pos1, pre1, mid2, mid1);                                                       // add the costs that are produced as a result of the changed route
      t->add_costs(pos2, mid2, mid1, next2);
    } else if ( (same) && (pos2+1==pos1) ) {
      add_costs(pos1, mid1, mid2, next1);                                                       // add the costs that are produced as a result of the changed route
      t->add_costs(pos2, pre2, mid1, mid2);
    } else {
      add_costs(pos1, pre1, mid2, next1);                                                       // add the costs that are produced as a result of the changed route
      t->add_costs(pos2, pre2, mid1, next2);
    }
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
  if (pre.length()==0) {
    if (rl_point==0) cost += BackhaulCustomer::get_depot()->get_pos()->distance(cargoArray[pos]->get_pos());
    else cost += cargoArray[pos-1]->get_pos()->distance(cargoArray[pos]->get_pos());
    // cout <<  cargoArray[pos-1]->get_pos()->distance(cargoArray[pos]->get_pos()) << endl;
    if (mid < next) {
      cost += BackhaulCustomer::get_dist(mid + next);
      // cout <<  BackhaulCustomer::get_dist(mid + next) << endl;
    } else {
      cost += BackhaulCustomer::get_dist(next + mid);
      // cout <<  BackhaulCustomer::get_dist(next + mid) << endl;
    }
    // cout << "5" << endl;
  }else if (pos+1 < rl_point) {
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
  }else if (pos > rl_point) {
    if (pre < mid) {
      cost +=  BackhaulCustomer::get_dist(pre + mid);
      // cout <<  BackhaulCustomer::get_dist(pre + mid) << endl;
    } else {
      cost += BackhaulCustomer::get_dist(mid + pre);
      // cout <<  BackhaulCustomer::get_dist(mid + pre) << endl;
    }
    if (mid < next) {
      cost += BackhaulCustomer::get_dist(mid + next);
      // cout <<  BackhaulCustomer::get_dist(mid + next) << endl;
    } else {
      cost += BackhaulCustomer::get_dist(next + mid);
      // cout <<  BackhaulCustomer::get_dist(next + mid) << endl;
    }
    // cout << "7" << endl;
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

