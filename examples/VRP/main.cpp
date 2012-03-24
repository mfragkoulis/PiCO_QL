/*
 *   Implement the main method for VRP.
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
#include <fstream>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include "Customer.h"
#include "Position.h"
#include "Truck.h"
#include "Fleet.h"
#include "mtrand.h"
#include "stl_search.h"     // For SQTL

using namespace std;

unsigned long init[4] = {0x123, 0x234, 0x345, 0x456};
unsigned long length = 4;  
MTRand_int32 irand(init, length);
/* To access in SQTL scope */
vector<Truck*> *vehicles;
map<int, Customer *> test;
/*-------------------------*/


int main(int argc, const char *argv[]) {
    ifstream fin(argv[1]);
    cout << argv[1] << endl;
    if (!fin) {
	cout << "Cannot open file.\n" << endl;
	exit(1);
    }

    char total_code_string[10];    
    // Enough for ten billion customers.
    int keep_track_cpt, lowest_dem = 0, lines = 0, 
      code_length;
    string data;
    clock_t start_clock = clock(), finish_clock;
    double c_time;
    Fleet *candidate_fl = new Fleet;
    candidate_fl->add();                
    /* Create first truck of fleet to record info about 
     * truck capacity. 
     */
    keep_track_cpt = Truck::get_initcapacity();
    lowest_dem = Truck::get_initcapacity();    
    /* Symbolic initialization: the highest demand 
     * possible. 
     */

    while ((!fin.eof()) && (!fin.fail())) {   
        // Count lines from file.
	getline(fin, data);
	lines++;
    }
    fin.close();
    sprintf(total_code_string, "%d", lines);
    code_length = (int)strlen(total_code_string);
    int x, y, demand, number, total_dem = 0;
    string code;
    bool is_depot;
    vector<Position*> positions;
    vector<LinehaulCustomer*> line;
    number = 1;
    ifstream finput(argv[1]);
    while ( (!finput.eof()) && (!finput.fail()) ) {   
	finput >> data;                          
	// Reading data from file.
	if (number > 18) {                       
	    // <= 18: data file info.
	    switch ((number - 19) % 7) {           
	    // 7 tokens per line.
	    case 0:
	        code = data.substr(0, data.length());
		break;
	    case 1:
		x = atoi(data.c_str());
		break;
	    case 2:
		y = atoi(data.c_str());
		break;
	    case 3:
		demand = atoi(data.c_str());
		break;
	    case 4:                     
	      /* Do not care about the fourth and 
	       * subsequent tokens of line. Time to 
	       * insert our line data.
	       */
		total_dem += demand;
		positions.push_back(new Position(x,y));
		while ((int)code.length() < code_length) 
		    code = "0" + code;
		if (number == 23)
		  is_depot = true;
		else 
		  is_depot = false; // 19 + case 4.
		line.push_back(new LinehaulCustomer(code, demand, positions.back(), is_depot));
		test.insert(pair<int, Customer *>(number, line.back()));
		/* cout << "number: " << 
		 * (number / 7) - 2 << 
		 * ", code: " << code << 
		 * " x: " << x << " y: " << y << 
		 * " demand: " << 
		 * demand << ", serviced: " << 
		 * is_depot << endl; 
		 */
	    }
	}
	number++;
    }
    finput.close();
    /* cout << "Data file size: " << number << 
     * " customers." << endl;
     */

    /* Code block: check positions coincidence, 
     * customer code coincidence.
     */
    vector<LinehaulCustomer*>::iterator iter1;
    vector<LinehaulCustomer*>::iterator iter2;
    vector<Position*>::iterator it1;
    vector<Position*>::iterator it2;
    int pos_v = 0;
    int lcode_v = 0;
    for (it1 = positions.begin(); it1 != positions.end(); it1++) {
        /* cout << (*it1)->get_x() << " " << 
	 * (*it1)->get_y() << endl;
	 */
	for (it2 = it1 + 1; it2 != positions.end(); it2++) {
	    if (((*it1)->get_x()==(*it2)->get_x()) && 
		 ((*it1)->get_y()==(*it2)->get_y())) {
		 cout << (*it1)->get_x() << " " << 
		   (*it1)->get_y() << endl;
		 cout << (*it2)->get_x() << " " << 
		   (*it2)->get_y() << endl;
		pos_v++;
	    }
	}
    }
    for (iter1 = line.begin(); iter1 != line.end(); iter1++) {
	for (iter2 = iter1 + 1; iter2 != line.end(); iter2++) {
	    /* cout << (*iter1)->get_code() << "+" << 
	     * (*iter2)->get_code() << endl; 
	     */
	    if ((*iter1)->get_code() == (*iter2)->get_code()) {  
		cout << "SAME: " << 
		  (*iter1)->get_code() << "-" << 
		  (*iter2)->get_code() << endl;
		lcode_v++;
	    }
	}
    }
    if (pos_v > 0) 
      cout << pos_v << 
	" pairs of customers have the same position." << 
	endl;
    else 
      cout << " All positions valid " << endl; 
    if (lcode_v > 0) 
      cout << lcode_v << 
      " pairs of linehaul customers have the same code." <<
	endl;
    else 
      cout << " All linehaul codes valid " << endl;

    cout << endl << 
      "Computing distances between linehaul customers. " <<
      endl << endl;
    LinehaulCustomer::compute_dist();
    
    int pos, extra_shots = 0, i, restarts = atoi(argv[2]), 
      cust_demand;
    LinehaulCustomer* l = NULL;
    Fleet optimised_fl, best_fl;
    for (i = 0; i <= restarts; i++) {
	pos = 0;
	if (i > 0) {
	    candidate_fl = new Fleet;
	    candidate_fl->add();
	    keep_track_cpt = Truck::get_initcapacity();
	    cout << "RESTART No" << i << endl << endl;
	}
	while (!Customer::get_allserviced()) {
	    // cout << "LOADING NEW TRUCK " << endl;
	    l = LinehaulCustomer::random_sel(pos, i);
	    if (l != NULL) {
		extra_shots = 0;
		while (extra_shots < 5) {     
		    // Utilise best a Trucks's space.
		    cust_demand = l->get_demand();
		    keep_track_cpt -= cust_demand;
		    /* cout << " Customer's demand : " << 
		     * cust_demand;
		     */
		    if (keep_track_cpt >= 0) {
			candidate_fl->get_current()->load(l);
			LinehaulCustomer::erase_c(pos, i);
			extra_shots = 4;
		    } else if (keep_track_cpt >= 
			       lowest_dem - cust_demand) { 
		        // Worth extra shot.
			keep_track_cpt += cust_demand;
			l = LinehaulCustomer::random_sel(pos, i);
		    } else if (keep_track_cpt < 
			       lowest_dem - l->get_demand()) {
                        //No chance,load new.
			candidate_fl->get_current()->return_todepot();
			/* cout << 
			 * "The total cost of delivery" <<
			 * for the truck amounts to " << 
			 * candidate_fl->get_current()->get_cost() << 
			 * " and there are " << 
			 * candidate_fl->get_current()->get_delcapacity() << 
			 * "/" << keep_track_cpt <<
			 * " units of unused space left." << endl << endl; 
			 */
			candidate_fl->add();
			keep_track_cpt = Truck::get_initcapacity();
			extra_shots = 4;
		    }
		    extra_shots += 1;
		}
	    } else {
		candidate_fl->get_current()->return_todepot();
		extra_shots = 5;
		/* cout << "NULL:The total cost of delivery for the " <<
		 * "truck amounts to " << 
		 * candidate_fl->get_current()->get_cost() << 
		 * " and there are " << 
		 * candidate_fl->get_current()->get_delcapacity() << 
		 * "/" << keep_track_cpt <<
		 * " units of unused space left." << 
		 * endl << endl; 
		 */
	    }
	}
	Customer::set_allserviced();   // Reset the flag.
	candidate_fl->set_delspace();
	candidate_fl->set_totalcost();
	cout << " END OF SCHEDULING " << endl << endl;
	cout << "The total cost of transportation for" <<
	  " the fleet of " << candidate_fl->get_size() << 
	  " trucks amounts to " << 
	    candidate_fl->get_totalcost() << 
	  " distance units.";
	if (i == 0) {
	    best_fl.set_size(candidate_fl->get_size());
	    best_fl.assign_all(*candidate_fl);
	}
	cout << endl << "Optimising schedule now..." << 
	  endl << endl;
	optimised_fl.set_size(candidate_fl->get_size());
	candidate_fl->optimise(optimised_fl);
	cout << "The optimised fleet of " << 
	  optimised_fl.get_size() << 
	  " trucks amounts to " << 
	  optimised_fl.get_totalcost() << 
	  " distance units";
	cout << " and there are " <<  
	  optimised_fl.get_delspace() << 
	    " units of unused delivery space." << endl << 
	  endl;
	if (optimised_fl.get_totalcost() <= best_fl.get_totalcost()) {
	    best_fl.deallocate();
	    best_fl.set_size(optimised_fl.get_size());
	    best_fl.assign_all(optimised_fl);
	}
	optimised_fl.deallocate();
	candidate_fl->deallocate();
	delete candidate_fl;
    } 
    if (restarts > i) {
	for (iter1 = line.begin() + 1; iter1 != line.end(); iter1++) {
	    (*iter1)->set_serviced();
	    // cout << (*iter1)->get_code() << endl;
	}
    }

    vehicles = best_fl.get_fleet();     // For SQTL
    int re_sqlite = call_sqtl();
    printf("Thread sqlite returned %i\n", re_sqlite);
    
    cout << endl << "Optimised solution after " << 
        restarts << 
	" restarts includes " << best_fl.get_size() << 
	" trucks and has cost " << 
	best_fl.get_totalcost() <<" distance units";
    cout << " with unused delivery space of " << 
        best_fl.get_delspace() << 
	" units." << endl << endl;
    cout << endl << 
      "Ideally (feasibility not guaranteed) " << 
      "minimum size of fleet would be " << 
      (double)total_dem / Truck::get_initcapacity() <<
	" trucks." << endl << endl;
    finish_clock = clock();
    c_time = (double(finish_clock) - 
	      double(start_clock)) / CLOCKS_PER_SEC;
    cout << "Ellapsed time given by c++ : " << c_time << 
      "s." << endl << endl;
    
    best_fl.deallocate();    
    for (it1 = positions.begin(); it1 != positions.end(); it1++)
	delete (*it1);
    for(iter1 = line.begin(); iter1 != line.end(); iter1++)
	delete (*iter1);
}
