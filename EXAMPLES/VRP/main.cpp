#include "Customer.h"
#include "Position.h"
#include "Truck.h"
#include "Fleet.h"
#include <iostream>
#include <stdio.h>
#include <string>
#include <sstream>
#include <ctime>
#include "mtrand.h"
#include <stdio.h>
#include "stl_search.h"     // For SQTL

using namespace std;

unsigned long init[4]={0x123, 0x234, 0x345, 0x456};
unsigned long length=4;  
MTRand_int32 irand(init, length);
/* To access in SQTL scope */
vector<Truck*> *vehicles;
map<int, Customer *> test;
/*-------------------------*/


int main(int argc, const char *argv[]) {
    ifstream fin(argv[1]);
    cout << argv[1] << endl;
    if(!fin) {
	cout << "Cannot open file.\n" << endl;
	exit(1);
    }

    clock_t start_clock = clock(), finish_clock;
    double c_time;
    Fleet *optimal=new Fleet;
    optimal->add();                   /* Create first truck of fleet to 
                                         record info about truck capacity. */
    lowest_dem=Truck::get_initcapacity();    /* Symbolic initialization:
                                                the highest demand possible. */

    int lowest_dem, total_dem=0;
    int x, y, demand, number;
    string data, total_string, code;    
    vector < Position* > positions;
    vector < LinehaulCustomer* > line;
    number=1;
    while (number<10) {      //Data file info.
	getline(fin, data);
	// cout << data << endl;
	number++;
    }
    number=-1;
    while ( (!fin.eof()) && (!fin.fail()) ) {
	number++;
	fin >> code;
	fin >> x;
	fin >> y;
	fin >> demand;
	total_dem += demand;
	positions.push_back( new Position(x,y) );
	while (code.length() < total_string.length()) code = "0" + code;
	line.push_back( new LinehaulCustomer(code, demand, 0, positions.back(),
					     0, 0, 0) );
	test.insert(pair<int, Customer *>(number, line.back()));
	/* cout << "number: " << number << ", code: " << code << " " << 
	   x << " " << y << " " << demand; */
    }
    fin.close();
    // cout << "Data file size: " << number << " customers." << endl;  

    // Code block: check positions coincidence, customer code coincidence.
    vector < LinehaulCustomer* >::iterator iter1;
    vector < LinehaulCustomer* >::iterator iter2;
    vector < Position* >::iterator it1;
    vector < Position* >::iterator it2;
    int pos_v=0;
    int lcode_v=0;
    for (it1=positions.begin(); it1!=positions.end(); it1++) {
	//    cout << (*it1)->get_x() << " " << (*it1)->get_y() << endl;
	for (it2=it1+1; it2!=positions.end(); it2++) {
	    if ( ((*it1)->get_x()==(*it2)->get_x()) && ((*it1)->get_y()==(*it2)->get_y()) )  pos_v++;
	}
    }
    for (iter1=line.begin(); iter1!=line.end(); iter1++) {
	//    cout << (*iter1)->get_code() << endl;
	for (iter2=iter1+1; iter2!=line.end(); iter2++) {
	    if ( (*iter1)->get_code()==(*iter2)->get_code() )  lcode_v++;
	}
    }
    if (pos_v>0) cout << pos_v << 
		     " pairs of customers have the same position." << endl;
    else cout << " All positions valid " << endl; 
    if (lcode_v>0) cout << lcode_v << 
		       " pairs of linehaul customers have the same code." << 
		       endl;
    else cout << " All linehaul codes valid " << endl;
    cout << endl << "Computing distances between linehaul customers. " << 
	endl << endl;
    LinehaulCustomer::compute_dist();
    
    int keep_track_cpt;
    LinehaulCustomer* l = NULL;
    Fleet best, bb;
    for (int i=0; i<=atoi(argv[2]); i++) {
	pos=0;
	if ( i>0 ) {
	    optimal=new Fleet;
	    optimal->add();
	    cout << "RESTART No" << i << endl << endl;
	}
	while ( !Customer::get_allserviced() ) {
	    // cout << "LOADING NEW TRUCK " << endl;
	    keep_track_cpt=Truck::get_initcapacity();
	    l=LinehaulCustomer::random_sel(pos);
	    if (l != NULL) {
		while ( extra_shots<5 ){        //utilise best a Trucks's space
		    keep_track_cpt -= l->get_demand();
		    // cout << " Customer's demand : " << l->get_demand();
		    switch (keep_truck_cpt){
		    case >0:
			optimal->get_current()->load(l);
			LinehaulCustomer::erase_c(pos);
			extra_shots = 4;
			break;
		    case >=( lowest_dem-l->get_demand() ):
			l=LinehaulCustomer::random_sel(pos);
			break;
		    case <0:
			optimal->get_current()->return_todepot();
			/* cout << "The total cost of delivery for the " <<
			"truck amounts to " << 
			optimal->get_current()->get_cost() << 
			" and there are " << 
			optimal->get_current()->get_delcapacity() 
			<< " units of unused space left." << endl << endl;*/
			optimal->add();
			keep_track_cpt=Truck::get_initcapacity();
			extra_shots = 4;
			break;
		    }
		    extra_shots += 1;
		}
	    }
	}
	optimal->set_delspace();
	optimal->set_totalcost();
	cout << " END OF SCHEDULING " << endl << endl;
	cout << "The total cost of transportation for the fleet of " << 
	    optimal->get_size() << " trucks amounts to " << 
	    optimal->get_totalcost(); << cout <<" distance units";
	if (i==0) {
	    bb.set_size(optimal->get_size());
	    bb.assign_all(*optimal);
	}
	cout << endl << "Optimising schedule now..." << endl << endl;
	best.set_size(optimal->get_size());
	optimal->optimise(best);
	cout << "The optimised fleet of " << best.get_size() << 
	    " trucks amounts to " << best.get_totalcost() << " distance units";
	cout << " and there are " <<  best.get_delspace() << " units of unused delivery space and " << best.get_pickspace() << endl << endl;
	if (best.get_totalcost() <= bb.get_totalcost()) {
	    bb.deallocate();
	    bb.set_size(best.get_size());
	    bb.assign_all(best);
	}
	best.deallocate();
	optimal->deallocate();
	delete optimal;
    } 
    if (argv[2]>i) {
	for (iter1=line.begin()+1; iter1!=line.end(); iter1++) {
	    (*iter1)->set_serviced();
	    // cout << (*iter1)->get_code() << endl;
	}
    }

    vehicles = bb.get_fleet();     // For SQTL
    int re_sqlite = call_sqtl();
    printf("Thread sqlite returned %i\n", re_sqlite);
    
    cout << endl << "Optimised solution after " << argv[2] << 
	" restarts includes " << bb.get_size() << " trucks and has cost " << 
	bb.get_totalcost() <<" distance units";
    cout << " with unused delivery space of " << bb.get_delspace() << 
	"distance units." << endl << endl;
    cout << endl << "Ideally (feasibility not guaranteed) minimum size " << 
	"of fleet would be " << (double)total_dem/Truck::get_initcapacity() <<
	" trucks." << endl << endl;
    finish_clock = clock();
    c_time = (double(finish_clock)-double(start_clock))/CLOCKS_PER_SEC;
    cout << "Ellapsed time given by c++ : " << c_time << "s." << endl << endl;
    
    bb.deallocate();    
    for (it1=positions.begin(); it1!=positions.end(); it1++)
	delete (*it1);
    for(iter1=line.begin(); iter1!= line.end(); iter1++)
	delete (*iter1);
}
