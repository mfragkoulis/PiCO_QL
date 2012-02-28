#include <math.h>
#include <stdlib.h>
#include "Fleet.h"
#include <iostream>
#include <fstream>

using namespace std;

extern MTRand_int32 irand;

// Constructs a new Fleet object.
Fleet::Fleet() {
    total_cost=0;
    unused_delspace=0;
}

// Returns the fleet data structure.
vector<Truck *> * Fleet::get_fleet() {
    return &fleet;
}

// Adds a new truck to the fleet.
void Fleet::add() {
    fleet.push_back(new Truck(200));
}

// Returns the Truck currently being loaded.
Truck* Fleet::get_current() {
    return fleet.back();
}

// Sets the total cost for the fleet.
void Fleet::set_totalcost() {
    total_cost=0;
    vector < Truck* >:: iterator iter;
    for ( iter = fleet.begin(); iter != fleet.end(); iter++ ) {
	total_cost += (*iter)->get_cost();
	// cout << total_cost << " " << (*iter)->get_cost() << endl;
    }
}

// Sets the total cost for the fleet.
void Fleet::set_totalcost(int cost) {
    total_cost=cost;
}

// Returns the total cost for the fleet.
double Fleet::get_totalcost() {
    return total_cost;
}

// Sets the aggregate space utilised by customer goods.
void Fleet::set_delspace() {
    unused_delspace=0;
    vector < Truck* >:: iterator iter;
    for( iter = fleet.begin(); iter != fleet.end(); iter++ ) {
	unused_delspace += (*iter)->get_delcapacity();
    }
}

// Sets the aggregate space utilised by customer goods.
void Fleet::set_delspace(int space) {
    unused_delspace=space;
}

// Returns the aggregate space utilised by customer goods.
int Fleet::get_delspace() {
    return unused_delspace;
}

// Returns the number of trucks in the fleet.
int Fleet::get_size() {
    return fleet.size();
}

// Sets the number of trucks in the Fleet.
void Fleet::set_size(int size) {
    for (int i=0; i!=size; i++) {
	fleet.push_back(new Truck(Truck::get_initcapacity()));
    }
}

// Deallocates the fleet data structure.
void Fleet::deallocate() {
    vector < Truck* >::iterator iter;
    for (iter=fleet.begin(); iter!=fleet.end(); iter++) {
	delete (*iter);
    }
    fleet.clear();
}

// Optimises the customer allocations in the fleet.
void Fleet::optimise(Fleet& optimised_fl) {

    double iterations=10000.0, temperature=100.0,  // Parameters for Simulated
	m=20.0, cool=0.9, spend=1.1, counter=0 ;   // Annealing algorithm.
    vector < Truck* >::iterator iter;
    vector < Customer* >::iterator it;
    
    Fleet new_fleet;
    new_fleet.set_size(this->get_size());
    new_fleet.assign_all(*this);

    optimised_fl.assign_all(*this);

    while (counter <= iterations) {
	/* 
	   cout << "Optimised_fl: " << optimised_fl.total_cost << 
	   " | " << endl;
	   cout << " COUNTER : " << counter << endl;
	   for (iter=(optimised_fl.fleet).begin(); 
	   iter!=(optimised_fl.fleet).end(); iter++) {
	   cout << "Optimised truck : " << iter - optimised_fl.fleet.begin() <<
	   endl;
	   for (it=(*iter)->start(); it!=(*iter)->finish(); it++) {
	   cout << (*it)->get_code() << endl;
	   }
	   cout << endl;
	   }
	   cout << "before nested new_cost is:  " << new_fleet.total_cost << 
	   endl;
	*/
	
	nested(new_fleet, &optimised_fl, temperature, m);
	/* cout << "after nested new_cost is:  " << new_fleet.total_cost << 
	   endl; */

	counter += m;
	m *= spend;
	temperature *= cool;
	
	/* 
	   cout << optimised_fl.total_cost << " " << 
	   optimised_fl.unused_delspace << " " << 
	   optimised_fl.unused_pickspace << endl;
	   vector < Truck* >::iterator iter;
	   vector < Customer* >::iterator it;
	   for (iter=(optimised_fl.fleet).begin(); 
	   iter!=(optimised_fl.fleet).end(); iter++) {
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

// Schedules exchanges and keeps track of the optimisation process.
void Fleet::nested(Fleet& new_fleet, Fleet* optimised_fl, double temperature, 
		   double m) {
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
      
	if (total_cost < optimised_fl->total_cost) {
	    optimised_fl->assign_best(*this);
/*
  for (iter=(optimised_fl->fleet).begin(); iter!=(optimised_fl->fleet).end(); 
  iter++) {
  cout << "best truck : " << iter - optimised_fl->fleet.begin() << 
  ", cost: " << (*iter)->get_cost() << endl;
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
    }
    m -= 1;
/*
  cout << " AFTER ASSIGNMENT " << endl;
  cout << "Current cost is : " << total_cost << endl;
  cout << "New cost is : " << new_fleet.total_cost << endl;
  cout << "Best cost is : " << optimised_fl->total_cost << endl << endl;
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

// Arranges which trucks will engage in exchange of customers as part of 
// the optimisation process.
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
    (pos1==pos2) ? same=true : same=false;
    // cout << " go exchange " << pos1 << " with " << pos2 << endl;
    fleet[pos1]->exchange(fleet[pos2], trap, same);
    if (trap) goto again;
    set_totalcost();
    set_delspace();

    /*
     cout << " truck position : " << pos1 << endl;
     for (it=fleet[pos1]->start(); it!=fleet[pos1]->finish(); it++) {
	 cout << (*it)->get_code() << endl;
     }
     cout << endl;
     cout << " truck position : " << pos2 << endl;
     for (it=fleet[pos2]->start(); it!=fleet[pos2]->finish(); it++) {
	 cout << (*it)->get_code() << endl;
     }
     cout << endl; 
    */
}

// Arranges the swapping of Customers between two trucks as part of the 
// optimisation process (when accepting a solution).
// Brute force: Swap customers between the two trucks engaged in the 
// last exchange.
void Fleet::assignT(int pos1, int pos2, Fleet source) {
    // cout << "assignT" << endl;
    fleet[pos1]->reassignC(source.fleet[pos1]);
    fleet[pos2]->reassignC(source.fleet[pos2]);
}

// Assigns a fleet object to "this".
void Fleet::assign_all(Fleet source) {
    // cout << "assign all" << endl;
    for (int i=0; i!=(int)source.fleet.size(); i++) {
	fleet[i]->assignC(source.fleet[i]);
    }
    // cout << "fleet size: " << fleet.size() << endl;
    total_cost=source.total_cost;
    unused_delspace=source.unused_delspace;
}

// Reassigns to a fleet (optimised) the best solution found so far.
void Fleet::assign_best(Fleet source) {
    // cout << "assign best" << endl;
    for (int i=0; i!=(int)source.fleet.size(); i++) {
	fleet[i]->reassignC(source.fleet[i]);
    }
    // cout << "fleet size: " << fleet.size() << endl;
    total_cost=source.total_cost;
    unused_delspace=source.unused_delspace;
}
