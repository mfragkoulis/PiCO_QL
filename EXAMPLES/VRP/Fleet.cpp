#include <math.h>
#include "Fleet.h"
#include <sstream>
#include <fstream>

using namespace std;

extern MTRand_int32 irand;

Fleet::Fleet() {
  total_cost=0;
  unused_delspace=0;
  unused_pickspace=0;
}

vector<Truck *> * Fleet::get_fleet() {

    return &fleet;

}

void Fleet::add() {
  fleet.push_back(new Truck(200));
}

Truck* Fleet::get_current() {
  return fleet.back();
}

void Fleet::set_totalcost() {
  total_cost=0;
  vector < Truck* >:: iterator iter;
  for ( iter = fleet.begin(); iter != fleet.end(); iter++ ) {
    total_cost += (*iter)->get_cost();
    // cout << total_cost << " " << (*iter)->get_cost() << endl;
    }
}

void Fleet::set_totalcost(int cost) {
  total_cost=cost;
}

double Fleet::get_totalcost() {
  return total_cost;
}

void Fleet::set_delspace() {
  unused_delspace=0;
  vector < Truck* >:: iterator iter;
  for( iter = fleet.begin(); iter != fleet.end(); iter++ ) {
    unused_delspace += (*iter)->get_delcapacity();
  }
}

void Fleet::set_delspace(int space) {
  unused_delspace=space;
}

void Fleet::set_pickspace() {
  unused_pickspace=0;
  vector < Truck* >:: iterator iter;
  for( iter = fleet.begin(); iter != fleet.end(); iter++ ) {
    unused_pickspace += (*iter)->get_pickcapacity();
  }
}

void Fleet::set_pickspace(int space) {
  unused_pickspace=space;
}

int Fleet::get_delspace() {
  return unused_delspace;
}

int Fleet::get_pickspace() {
  return unused_pickspace;
}

int Fleet::get_size() {
  return fleet.size();
}

void Fleet::set_size(int size) {
  for (int i=0; i!=size; i++) {
    fleet.push_back(new Truck(Truck::get_initcapacity()));
  }
}

void Fleet::deallocate() {
  vector < Truck* >::iterator iter;
  for (iter=fleet.begin(); iter!=fleet.end(); iter++) {
    delete (*iter);
  }
  fleet.clear();
}


void Fleet::optimise( Fleet& best) {

    double iterations=10000.0, temperature=100.0,  // Parameters for Simulated
	m=20.0, cool=0.9, spend=1.1, counter=0 ;   // Annealing algorithm.
  vector < Truck* >::iterator iter;
  vector < Customer* >::iterator it;

  Fleet new_fleet;
  new_fleet.set_size(this->get_size());
  new_fleet.assign_all(*this);

  best.assign_all(*this);

  while (counter <= iterations) {
      /* 
	 cout << "best: " << best.total_cost << " | " << endl;
	 cout << " COUNTER : " << counter << endl;
	 for (iter=(best.fleet).begin(); iter!=(best.fleet).end(); iter++) {
	 cout << "best truck : " << iter - best.fleet.begin() << endl;
	 for (it=(*iter)->start(); it!=(*iter)->finish(); it++) {
	 cout << (*it)->get_code() << endl;
	 }
	 cout << endl;
	 }
	 cout << "before nested new_cost is:  " << new_fleet.total_cost << 
	 endl;
      */

    nested(new_fleet, &best, temperature, m);
    // cout << "after nested new_cost is:  " << new_fleet.total_cost << endl;

    counter += m;
    m *= spend;
    temperature *= cool;

    /* 
       cout << best.total_cost << " " << best.unused_delspace << " " << 
       best.unused_pickspace << endl;
       vector < Truck* >::iterator iter;
       vector < Customer* >::iterator it;
       for (iter=(best.fleet).begin(); iter!=(best.fleet).end(); iter++) {
       cout << "rl_point : " << (*iter)->get_rlpoint() << endl;
       for (it=(*iter)->start(); it!=(*iter)->finish(); it++) {
       cout << (*it)->get_code() << endl;
       }
       cout << endl;
       }
    */
  }
  new_fleet.deallocate();  
}



void Fleet::nested(Fleet& new_fleet, Fleet* best, double temperature, double m) {
  // cout << "nested" << endl;  
  vector < Truck* >::iterator iter;
  vector < Customer* >::iterator it;  
  double v;
  int pos1, pos2;

  while (m > 0) {
      /* cout << "new: " << new_fleet.total_cost << "~";
	 cout << " m : " << m << endl; */
      
      new_fleet.generate_new(pos1, pos2);

      /* 
	 for (iter=fleet.begin(); iter!=fleet.end(); iter++) {
	 cout << "current truck : " << iter - fleet.begin() << ", cost: " << 
	 (*iter)->get_cost() << endl;
	 for (it=(*iter)->start(); it!=(*iter)->finish(); it++) {
	 cout << (*it)->get_code() << endl;
	 }
	 cout << endl;
	 }
	 for (iter=new_fleet.fleet.begin(); iter!=new_fleet.fleet.end(); iter++) {
	 cout << "new fleet truck : " << iter - new_fleet.fleet.begin() << 
	 ", cost: " << (*iter)->get_cost() << endl;
	 for (it=(*iter)->start(); it!=(*iter)->finish(); it++) {
	 cout << (*it)->get_code() << endl;
	 }
	 cout << endl;
	 }
      

	 cout << endl << "Current cost is : " << total_cost << endl;
	 cout << "New cost is : " << new_fleet.total_cost << endl;
	 cout << "Best cost is : " << best->total_cost << endl;
      */
	 
    v= (double) irand() / (double) RAND_MAX;
    if (new_fleet.total_cost < total_cost) {
/*
  cout << "better" << endl;
  for (iter=fleet.begin(); iter!=fleet.end(); iter++) {
  cout << "current truck : " << iter - fleet.begin() << endl;
  for (it=(*iter)->start(); it!=(*iter)->finish(); it++) {
  cout << (*it)->get_code() << endl;
  }
  cout << endl;
  }
*/

      assignT(pos1, pos2, new_fleet);

      /*      
	      for (iter=fleet.begin(); iter!=fleet.end(); iter++) {
	      cout << "current truck : " << iter - fleet.begin() << endl;
	      for (it=(*iter)->start(); it!=(*iter)->finish(); it++) {
	      cout << (*it)->get_code() << endl;
	      }
	      cout << endl;
	      }
      */

      total_cost=new_fleet.total_cost;
      unused_delspace=new_fleet.unused_delspace;
      unused_pickspace=new_fleet.unused_pickspace;
      
      if (total_cost < best->total_cost) {
	  best->assign_best(*this);
/*
  for (iter=(best->fleet).begin(); iter!=(best->fleet).end(); iter++) {
  cout << "best truck : " << iter - best->fleet.begin() << ", cost: " << 
  (*iter)->get_cost() << endl;
  for (it=(*iter)->start(); it!=(*iter)->finish(); it++) {
  cout << (*it)->get_code() << endl;
  }
  cout << endl;
  }
*/
      }
    }
    else if ( v < exp(-(new_fleet.total_cost - total_cost) / temperature)) {
	// cout << "worse but accepted" << endl;
	assignT(pos1, pos2, new_fleet);
	total_cost=new_fleet.total_cost;
	unused_delspace=new_fleet.unused_delspace;
	unused_pickspace=new_fleet.unused_pickspace;
    } else {
	/*
	  cout << "not accepted" << endl; 
	  for (iter=fleet.begin(); iter!=fleet.end(); iter++) {
	  cout << "current truck : " << iter - fleet.begin() << endl;
	  for (it=(*iter)->start(); it!=(*iter)->finish(); it++) {
	  cout << (*it)->get_code() << endl;
	  }
	  cout << endl;
	  }
	*/

	new_fleet.assignT(pos1, pos2, *this);
	// cout << new_fleet.total_cost << "  ->  ";
	new_fleet.total_cost=total_cost;
	// cout << new_fleet.total_cost << endl;
	new_fleet.unused_delspace=unused_delspace;
	new_fleet.unused_pickspace=unused_pickspace;
    }
    m -= 1;
/*
  cout << " AFTER ASSIGNMENT " << endl;
  cout << "Current cost is : " << total_cost << endl;
  cout << "New cost is : " << new_fleet.total_cost << endl;
  cout << "Best cost is : " << best->total_cost << endl << endl;
  for (iter=new_fleet.fleet.begin(); iter!=new_fleet.fleet.end(); iter++) {
  cout << "new fleet truck : " << iter - new_fleet.fleet.begin() << endl;
  for (it=(*iter)->start(); it!=(*iter)->finish(); it++) {
  cout << (*it)->get_code() << endl;
  }
  cout << endl;
  }
  for (iter=fleet.begin(); iter!=fleet.end(); iter++) {
  cout << "current truck : " << iter - fleet.begin() << endl;
  for (it=(*iter)->start(); it!=(*iter)->finish(); it++) {
  cout << (*it)->get_code() << endl;
  }
  cout << endl;
  }
*/
  }
}

void Fleet::generate_new(int& pos1, int& pos2) {
  // cout << "gen" << endl;
  vector < Customer* >::iterator it;
  bool trap=false, same=false;
  int i=0;
 again:
  if (i==1000)  {
    cout << "INFINITE" << endl;
    i=0;
  }
  i++;
  pos1=irand()%fleet.size();
  pos2=irand()%fleet.size();
  if (pos1==pos2) same=true;
  else same=false;
  // cout << " go exchange " << pos1 << " with " << pos2 << endl;
  fleet[pos1]->exchange(fleet[pos2], trap, same);
  if (trap) goto again;
  set_totalcost();
  set_delspace();

  /* 
     cout << " truck position : " << pos1 << "rl_point : " << 
     fleet[pos1]->get_rlpoint() << endl;
     for (it=fleet[pos1]->start(); it!=fleet[pos1]->finish(); it++) {
     cout << (*it)->get_code() << endl;
     }
     cout << endl;
     cout << " truck position : " << pos2 << "rl_point : " << 
     fleet[pos2]->get_rlpoint() << endl;
     for (it=fleet[pos2]->start(); it!=fleet[pos2]->finish(); it++) {
     cout << (*it)->get_code() << endl;
     }
     cout << endl;
  */
}


void Fleet::assignT(int pos1, int pos2, Fleet source) {
  // cout << "assignT" << endl;
  fleet[pos1]->reassignC(source.fleet[pos1]);
  fleet[pos2]->reassignC(source.fleet[pos2]);
}

void Fleet::assign_all(Fleet source) {
  // cout << "assign all" << endl;
  for (int i=0; i!=source.fleet.size(); i++) {
    fleet[i]->assignC(source.fleet[i]);
  }
  // cout << "fleet size: " << fleet.size() << endl;
  total_cost=source.total_cost;
  unused_delspace=source.unused_delspace;
  unused_pickspace=source.unused_pickspace;
}


void Fleet::assign_best(Fleet source) {
  // cout << "assign best" << endl;
  for (int i=0; i!=source.fleet.size(); i++) {
    fleet[i]->reassignC(source.fleet[i]);
  }
  // cout << "fleet size: " << fleet.size() << endl;
  total_cost=source.total_cost;
  unused_delspace=source.unused_delspace;
  unused_pickspace=source.unused_pickspace;
}

