#include "Customer.h"
#include "Position.h"
#include "Truck.h"
#include "Fleet.h"
#include <iostream>
#include <lemon/time_measure.h>
#include <lemon/list_graph.h>
#include <lemon/graph_to_eps.h>
#include <lemon/math.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <ctime>
#include "mtrand.h"
#include <stdio.h>
// For sqtl
#include "stl_search.h"
//

using namespace std;
using namespace lemon;



unsigned long init[4]={0x123, 0x234, 0x345, 0x456};
unsigned long length=4;  
MTRand_int32 irand(init, length);
// For sqtl
vector<Truck*> *vehicles;
map<int, Customer *> test;
//

/* comparison function for datastructure if needed
struct classcomp{
    bool operator() (const USER_CLASS& uc1, const USER_CLASS& uc2) const{
        return (uc1.get_known_type()<uc2.get_known_type());
    }
};
// in main: include classcomp in template arguments
*/




int main(int argc, const char *argv[]) {

  int line_cust, restarts, total=-9, lowest_dem, total_dem=0;
  string variant, mode, data, total_string;
  Fleet bb;
/*
  cout << endl;
  cout << "Welcome to VRP solver." << endl;
  cout << "Please type in the variant that you would like to be solved." << endl << endl;
  cout << "Note: " << endl;
  cout << "Type 'cvrp' if your demand concerns the capacitated vehicle routing problem." << endl;
  cout << "Type 'vrpb' if your demand concerns mandatory service of backhaul customers." << endl;
  cout << "Type 'vrpsb' if your demand concerns optional service of backhaul customers." << endl;
  cout << "Type 'vrppd' if your demand concerns pick-up and delivery of the customer set." << endl;
  cout << "Type 'vrptw' if your demand includes time windows for the service of customers." << endl;
  cout << endl << "You typed: ";
  cin >> variant;
  cout << endl;
  while ( (variant!="cvrp") && (variant!="vrpb") && (variant!="vrpsb") && (variant!="vrppd") && (variant!="vrptw") ) {
    cout << "Please type in the variant that you would like to be solved." << endl << endl;
    cout << "Note: " << endl;
    cout << "Type 'cvrp' if your demand concerns the capacitated vehicle routing problem." << endl;
    cout << "Type 'vrpb' if your demand concerns mandatory service of backhaul customers." << endl;
    cout << "Type 'vrpsb' if your demand concerns optional service of backhaul customers." << endl;
    cout << "Type 'vrppd' if your demand concerns pick-up and delivery of the customer set." << endl;
    cout << "Type 'vrptw' if your demand includes time windows for the service of customers." << endl;
    cout << endl << "You typed: ";
    cin >> variant;
    cout << endl;
  }
  if ( (variant=="vrpb") || (variant=="vrpsb") ) {
    while (1) {
      cout << "Please enter the number of linehaul customers:   ";
      cin >> line_cust;
      if (cin.fail()) {
	cin.clear();
	cin.ignore(1000,'\n');
	continue;
      }
      break;
    }
    line_cust++;
    cout << endl;
  }

  filename=variant+"/";
  string ch;
 
 try_again:
  cout << "To decide the name of the input file please answer the following. " << endl;
  cout << "There are three categories concerning customers locations: 'c' (clustered customers), 'r' (uniformly distributed customers), 'rc' (a mix of the previous two types)." << endl;
  cout << "Please type the letter(s) that correspond(s) to your preference:  ";
  cin >>  ch;
  while ( (ch!="c") && (ch!="r") && (ch!="rc") ) {
    cout << endl;
    cout << "There are three categories concerning customers locations: 'c' (clustered customers), 'r' (uniformly distributed customers), 'rc' (a mix of the previous two types)." << endl;
    cout << "Please type the letter(s) that correspond(s) to your preference:  ";
    cin >>  ch;
  }
  if (ch=="c")  filename += "C";
  else if (ch=="r") filename += "R";
  else if (ch=="rc") filename += "RC";

  // cout << filename  << " ***" << endl;

  cout << "Two set of problems are proposed for each of the three categories." << endl;
  cout << "Therefore, type '1' or '2' depending on the set of problems you choose to use:  " << endl;
  cout << "Please type:  ";
  cin >> ch;
  while ( (ch!="1") && (ch!="2") ) {
    cout << endl;
    cout << "Two set of problems are proposed for each of the three categories." << endl;
    cout << "Therefore, type '1' or '2' depending on the set of problems you choose to use:  " << endl;
    cout << "Please type:  ";
    cin >> ch;
  }
  filename += ch;

  // cout << filename << " *** " << endl;

  cout << "To choose the size of the problem (200, 400, 600, 800 and 1000 available) type the number of hundreds only." << endl;
  cout << "eg. type '10' if you want to choose problem size of 1000 customers:  " << endl;
  cout << "Please type:  " ;
  cin >> ch;
  while ( (ch!="2") && (ch!="4") && (ch!="6") && (ch!="8") && (ch!="10") ) {
    cout << endl;
    cout << "To choose the size of the problem (200, 400, 600, 800 and 1000 available) type the number of hundreds only." << endl;
    cout << "eg. type '10' if you want to choose problem size of 1000 customers:  " << endl;
    cout << "Please type:  " ;
    cin >> ch;
  }
  if (ch=="10") filename +=ch;
  else filename += "_" +ch;

  // cout << filename << " ***" << endl;

  cout << "Ten problem instances correspond to each set." << endl;
  cout << "Please type a number from '1' till '10' :  " ;
  cin >> ch;
  while ( (ch!="1") && (ch!="2") && (ch!="3") && (ch!="4") && (ch!="5") && (ch!="6") && (ch!="7") && (ch!="8") && (ch!="9") && (ch!="10") ) {
    cout << endl;
    cout << "Ten problem instances correspond to each set." << endl;
    cout << "Please type a number from '1' till '10' :  " ;
    cin >> ch;
  }
  if (ch=="10") filename += ch;
  else filename += "_" +ch;
  filename += ".TXT";
  // cout << filename << " **** " << endl;
*/
  ifstream fin(argv[1]);
  cout << argv[1] << endl;
  if(!fin) {
    cout << "Cannot open file.\n" << endl;
    exit(1);
//    goto try_again;
  }

  // other input formats

  while (getline(fin, data)) {                      // might need tuning. input file dependent
    total++;
    //  cout << data << endl;
  }
  fin.close();
  // cout << total << endl;
  stringstream out;
  out << total;
  total_string=out.str();

  // cout << "total_string: " << total_string << endl;
  restarts = 0;
/*
  while (1) {
    cout << endl << "Please enter the number of restarts : " ;
    cin >> restarts;
    if(cin.fail()) {
      cin.clear();
      cin.ignore(1000,'\n');
      continue;
    }
    break;
  }
  cout << endl;
*/
  if (variant=="cvrp") {
    LinehaulCustomer::set_variant(true);
    line_cust=total;
  }else if (variant=="vrpb") BackhaulCustomer::set_selective(false);
  else if (variant=="vrpsb") BackhaulCustomer::set_selective(true);
  else if (variant=="vrppd") {
    LinehaulCustomer::set_PD(true);
    line_cust=total;
  }else if (variant=="vrptw") {
    LinehaulCustomer::set_TW(true);
    line_cust=total;
  }
  // cout << "lines: " << line_cust << endl;  

  while (line_cust > total) {
    while (1) {
      cout << "Linehaul customers cannot exceed total number of customers." << endl;
      cout << "Please enter the number of linehaul customers:   ";
      cin >> line_cust;
      if (cin.fail()) {
	cin.clear();
	cin.ignore(1000,'\n');
	continue;
      }
      break;
    }
    line_cust++;
  }

  clock_t start_clock,finish_clock;
  double c_time;
  start_clock = clock();
  // which of the two?
  Timer t;
  t.start();  

  string code;


  int x, y, demand, pick_demand, depends, start, service, finish, revenue, number, z=3;
  vector < Position* > positions;
  vector < LinehaulCustomer* > line;
  vector < BackhaulCustomer* >  back;
  Fleet* optimal=new Fleet;
  optimal->add();                        // create first truck of fleet. reason for doing it here: record info about truck capacity
  lowest_dem=Truck::get_initcapacity();      // symbolic initialization (the highest demand possible)  
 
/*  fin.open(filename.c_str());
  if(!fin) {
    cout << "Cannot open file.\n";
    return 1;
  }
  number=1;
  while (number<10) {                   // stupid input files!!!
    getline(fin, data);
    // cout << data << endl;
    number++;
  }
*/
  number=0;
  fin >> code;
  fin >> x;
  fin >> y;
  fin >> demand;
  fin >> depends;
  fin >> finish;
  fin >> service;
  // cout << "code: " << code << "x: " << x << "y: " << y << "d: " << demand << endl;
  while ( (!fin.eof()) && (!fin.fail()) ) {
    if (LinehaulCustomer::get_vrppd()) pick_demand=demand +z;                 // adopt to specific variant
    else if (LinehaulCustomer::get_vrptw()) start=depends;
    else if (BackhaulCustomer::get_selective()) revenue=100;
    positions.push_back( new Position(x,y) );
    while (code.length() < total_string.length()) code = "0" + code;                        // automate. hard-coded. not anymore
    z *= -1;
    total_dem += demand;
    if ( (demand >0) && (demand < lowest_dem) ) lowest_dem = demand;  // cout << lowest_dem << endl;}
    if (demand <= Truck::get_initcapacity()) {                                       // CVRP!!!
      if (number==0) {
	  if (LinehaulCustomer::get_vrptw()) {
	      line.push_back( new LinehaulCustomer(code, demand, 0, positions.back(), true, start, finish ) );
	  } else line.push_back( new LinehaulCustomer(code, demand, 0, positions.back(), true, 0, 0) );
	back.push_back( new BackhaulCustomer(code, demand, 0, positions.back(), true ) );
      }  else if ( (number> 0) && (number < line_cust) ) {                                                     // "hard-coded". not anymore
	if (LinehaulCustomer::get_vrptw()) line.push_back( new LinehaulCustomer(code, demand, 0, positions.back(), start, service, finish ) );
	else if (LinehaulCustomer::get_vrppd()) line.push_back( new LinehaulCustomer(code, demand, pick_demand, positions.back(), 0, 0, 0 ) );
	else {
	  line.push_back( new LinehaulCustomer(code, demand, 0, positions.back(), 0, 0, 0) );
	  test.insert(pair<int, Customer *>(number, line.back()));
	}
      }  else if ( (number >= line_cust) && (number < total) ) {
	if (!BackhaulCustomer::get_selective())  back.push_back( new BackhaulCustomer(code, demand, 0, positions.back() ) );
	else  back.push_back( new BackhaulCustomer(code, demand, revenue, positions.back() ) );
      }
    } else cout << " Customer with code " << code << " cannot be serviced due to capacity constraint violation." << endl << endl;

    // cout << "number: " << number << ", code: " << code << " " << x << " " << y << " " << demand;
    // cout << ", depends: " << depends << ", start: " << start << ", service: " << service << ", finish: " << finish << endl;
    number++;
    fin >> code;
    fin >> x;
    fin >> y;
    fin >> demand;
    fin >> depends;
    fin >> finish;
    fin >> service;
  }
  fin.close();
  // cout << "final number: " << number << endl;  
  // check positions coincidence, code coincidence
  vector < LinehaulCustomer* >::iterator iter1;
  vector < LinehaulCustomer* >::iterator iter2;
  vector < BackhaulCustomer* >::iterator itr1;
  vector < BackhaulCustomer* >::iterator itr2;
  vector < Position* >::iterator it1;
  vector < Position* >::iterator it2;
  int pos_v=0;
  int lcode_v=0;
  int bcode_v=0;
  int code_v=0;
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
  for (itr1=back.begin(); itr1!=back.end(); itr1++) {
    //    cout << (*itr1)->get_code() << endl;
    for (itr2=itr1+1; itr2!=back.end(); itr2++) {
      if ( (*itr1)->get_code()==(*itr2)->get_code() )  bcode_v++;
    }
  }
/*  for (itr1=back.begin() +1; itr1!=back.end(); itr1++) {        // not checking the depot, if such violation exists it would have already been revealed
    for (iter2=line.begin() +1; iter2!=line.end(); iter2++) {
      if ( (*itr1)->get_code()==(*iter2)->get_code() )  code_v++;
    }
    }*/
  if (pos_v>0) cout << pos_v << " pairs of customers have the same position." << endl;
  else cout << " All positions valid " << endl; 
  if (lcode_v>0) cout << lcode_v << " pairs of linehaul customers have the same code." << endl;
  else cout << " All linehaul codes valid " << endl;
  if (bcode_v>0) cout << bcode_v << " pairs of backhaul customers have the same code." << endl;
  else cout << " All backhaul codes valid " << endl; 
  if (code_v>0) cout << code_v << " pairs of linehaul-backhaul customers have the same code." << endl;
  else cout << " All linehaul-backhaul codes valid " << endl; 

  
  cout << endl << "Computing distances between linehaul customers. " << endl;
  LinehaulCustomer::compute_dist();
  cout << endl << "Computing distances between backhaul customers. " << endl;
  BackhaulCustomer::compute_dist();
  cout << endl;

  Palette palette(true, 50);
  
  // Create a small digraph
  ListDigraph g;
  typedef ListDigraph::Node Node;
  typedef ListDigraph::NodeIt NodeIt;
  typedef ListDigraph::Arc Arc;
  typedef dim2::Point<int> Point;

  ListDigraph::NodeMap<string> label(g);
  ListDigraph::NodeMap<Point> coords(g);
  ListDigraph::NodeMap<double> sizes(g);
  ListDigraph::NodeMap<int> colors(g);
  ListDigraph::NodeMap<int> shapes(g);
  ListDigraph::ArcMap <double> alabel(g);
  ListDigraph::ArcMap<int> acolors(g);
  ListDigraph::ArcMap<int> widths(g);


  vector < Node > all_nodes;
  Arc a;
  
  all_nodes.push_back(g.addNode());
  label[all_nodes[0]]="D";  // LinehaulCustomer::get_depot()->get_code();
  coords[all_nodes[0]]=Point( LinehaulCustomer::get_depot()->get_pos()->get_x(), LinehaulCustomer::get_depot()->get_pos()->get_y() );    
  sizes[all_nodes[0]]=1.5;
  colors[all_nodes[0]]=4;
  shapes[all_nodes[0]]=2;
  
  int pos=1;

  for (iter1=line.begin()+1; iter1!=line.end(); iter1++) {          // for every Linehaulcustomer
    all_nodes.push_back(g.addNode());
    label[all_nodes[pos]]=(*iter1)->get_code();
    coords[all_nodes[pos]]=Point((*iter1)->get_pos()->get_x(),(*iter1)->get_pos()->get_y());
    sizes[all_nodes[pos]]=1;
    colors[all_nodes[pos]]=6;
    shapes[all_nodes[pos]]=1;
    pos++;
  }
  for (itr1=back.begin() +1; itr1!=back.end(); itr1++) {          // for every Backhaulcustomer
    all_nodes.push_back(g.addNode());
    label[all_nodes[pos]]=(*itr1)->get_code();
    coords[all_nodes[pos]]=Point((*itr1)->get_pos()->get_x(),(*itr1)->get_pos()->get_y());
    sizes[all_nodes[pos]]=1;
    colors[all_nodes[pos]]=3;
    shapes[all_nodes[pos]]=0;
    pos++;
  }
  
 cout << "Visualization of Problem Description complete " << endl; 
 graphToEps(g,"Description_Visual.eps").
 title("Problem Instance Visual").
 copyright("(C) 2003-2009 LEMON Project").
 scaleToA4().
 absoluteNodeSizes().absoluteArcWidths().
 nodeColors(composeMap(palette,colors)).
 coords(coords).
 nodeScale(1).
 nodeSizes(sizes).
 nodeShapes(shapes).
 arcColors(composeMap(palette,acolors)).
 arcWidths(widths).
 nodeTexts(label).nodeTextSize(1).
 run();
 system("ghostview Description_Visual.eps");


  double truck_cost;
  int keep_track_cpt, n, m, k, diff, bound, temp;
  bool load=false, reload=false, succeed=false, fail=false;
  LinehaulCustomer* l;
  LinehaulCustomer* before_l;
  Position* p;
  BackhaulCustomer* b;
  BackhaulCustomer* before_b;
  Fleet best;

  for (int i=0; i<=restarts; i++) {
    if (LinehaulCustomer::get_vrptw()) LinehaulCustomer::sort_list(i);
    m=0;
    n=0;
    k=0;
    pos=0;
    if (i>0) {
      optimal=new Fleet;
      optimal->add();
      cout << "RESTART No" << i << endl << endl;
    }
    while (!Customer::get_allserviced()) {              // VRPB,VRPSB
      // cout << "LOADING NEW TRUCK " << endl;
      keep_track_cpt=Truck::get_initcapacity();
      diff=0;
      bound=0;
      temp=0;
      fail=false;
      before_l=NULL;

      if (LinehaulCustomer::get_vrptw()) truck_cost=optimal->get_current()->get_cost();
      else truck_cost=0;
      
      l=LinehaulCustomer::random_sel( before_l, pos, i, truck_cost);                       // can't do it better??
      if (l != NULL) {
	if (LinehaulCustomer::get_vrppd()) {
	  // cout << "old diff: " << diff << endl;
	  diff = diff + l->get_pickdemand() - l->get_demand();
	  if (diff > temp) temp = diff;
	  // cout << "new diff: " << diff << endl;
	  // cout << "temp bound: " << temp << endl;
	  if (temp + l->get_demand() > keep_track_cpt)  {                // delivery validation
	    fail=true;
	    temp=bound;                                              // restore temp
	    diff = diff + l->get_demand() - l->get_pickdemand();
	  }
	  if ( (diff + l->get_demand() > keep_track_cpt) && (!fail) ) {            // pick-up validation. needed?
	    temp=bound;
	    diff = diff + l->get_demand() - l->get_pickdemand();
	    fail=true;
	    cout << "IN HERE" << endl;
	  }
	  if (fail) {
	    // cout << "FAIL" << endl;
	    // cout << "now diff: " << diff << endl;
	  }else bound=temp;
	}else if (LinehaulCustomer::get_vrptw()) {
	  fail=l->test(before_l, optimal->get_current()->get_cost());                 //*** philosophy gather 'n' provide
	  if (fail) {
	    cout << "Failed on first customer of route" << endl;
	    keep_track_cpt=-1;          // don't want to go in attempt utilize or while loop           
	    goto utilize;
	  }
	}
	keep_track_cpt -= l->get_demand();
	// cout << " Customer's demand : " << l->get_demand();
	// if (LinehaulCustomer::get_vrppd()) cout << ", pick demand : " << l->get_pickdemand();
	//cout << ", del capacity left : " << keep_track_cpt << endl;
      }
      if ( (LinehaulCustomer::is_variant()) && ((keep_track_cpt<0) || (fail)) && (-bound + keep_track_cpt + l->get_demand() >= lowest_dem) ) {      // 1st objective: BPP ??
	l=l->attempt_utilize(i, pos, keep_track_cpt, diff, succeed, fail, temp, bound, before_l, optimal->get_current()->get_cost());
	  if (!succeed) {
	    cout << "TERRIBLY WRONG" << endl;
	  }else fail=false;                                       // just in case. matters only in VRPPD
      }
    utilize:
      while ( (keep_track_cpt >= 0) && (!fail) && ( n < LinehaulCustomer::get_countl() -1 ) ) {
	// cout << "count : " <<  LinehaulCustomer::get_countl() -1 << endl;
	optimal->get_current()->load(before_l, l, diff);
	load=true;
	LinehaulCustomer::erase_c(pos,i);
	n++;
	// cout << n << " linehaul customers serviced so far." << endl << endl;
	before_l=l;
	m++;                                            // customer counter, might need it if implement another VRP variation

	if (LinehaulCustomer::get_vrptw()) truck_cost=optimal->get_current()->get_cost();
	else truck_cost=0;

	l=LinehaulCustomer::random_sel( before_l, pos, i, truck_cost);
	if (LinehaulCustomer::get_vrptw()) {
	  if ( (l!=NULL) && (keep_track_cpt >= l->get_demand()) ) fail=l->test(before_l, optimal->get_current()->get_cost());
	  else fail=true;
	    if (fail) {
	      keep_track_cpt=-1;
	      goto utilize; 
	    }
	}
	if (l != NULL)  {
	  if (LinehaulCustomer::get_vrppd()) {
	    // cout << "old diff: " << diff << endl;
	    diff = diff + l->get_pickdemand() - l->get_demand();
	    if (diff > temp) temp = diff;
	    // cout << "new diff: " << diff << endl;
	    // cout << "temp bound: " << temp << endl;
	    if (bound + l->get_demand() > keep_track_cpt)  {
	      fail=true;
	      temp=bound;                                              // restore temp
	      diff = diff + l->get_demand() - l->get_pickdemand();
	    }
	    if ( (diff + l->get_demand() > keep_track_cpt) && (!fail) ) {
	      temp=bound;
	      diff = diff + l->get_demand() - l->get_pickdemand();
	      fail=true;
	    }
	    if (fail) {
	      // cout << "FAIL" << endl;
	      // cout << "now diff: " << diff << endl;
	    }else bound=temp;
	  }
	  keep_track_cpt -= l->get_demand();
	  // cout << " Customer's demand : " << l->get_demand();
	  // if (LinehaulCustomer::get_vrppd()) cout << ", pick demand : " << l->get_pickdemand();
	  // cout << ", del capacity left : " << keep_track_cpt << endl;
	}
	if ( (LinehaulCustomer::is_variant()) && ((keep_track_cpt <0) || (fail)) && (-bound + keep_track_cpt + l->get_demand()  >= lowest_dem) ) {      // 1st objective: BPP
	  // cout << "jbilghjb" << endl;
	  l=l->attempt_utilize(i, pos, keep_track_cpt, diff, succeed, fail, temp, bound, before_l, optimal->get_current()->get_cost());
	  if (succeed) {
	    fail=false;
	    goto utilize;
	  }
	}
      }
      // cout << "service not allowed." << endl;
      optimal->set_delspace();
      // cout << "The total cost of delivery for the truck amounts to " << optimal->get_current()->get_cost() << " and there are " << optimal->get_current()->get_delcapacity() << " units of unused space left." << endl << endl;
      if ( (l!=NULL) && (before_l==NULL) ) {                                          // demand > capacity. ignore customer
	n++;
	cout << endl << "*** Demand - Capacity violation happened at linehaul customer " << l->get_code() << endl << endl;
      }                                                // you don't necessary catch it.only if it happens at opening.so pre-check to be sure and get rid of additional complexity
      else if ( (l!=NULL) && (before_l!=NULL) )   p=l->get_pos();                     // usual case in the end. all linehaul customers serviced, record position of last linehaul customer
      else if  ( (before_l!=NULL) && (l==NULL) )  p=before_l->get_pos();              // most usual case. no more truck capacity
      else {
	p=BackhaulCustomer::get_depot()->get_pos();
	// cout << " back only " << endl;
      }
      keep_track_cpt=Truck::get_initcapacity();
      //  optimal->get_current()->reset_capacity();                  //reset initial capacity.after distinguishing capacities no longer needed
      before_b=NULL;
      b=BackhaulCustomer::random_sel( pos,i);                       // can't do it better??
      if ( (b != NULL) && (!BackhaulCustomer::get_selective()) ) {
	keep_track_cpt -= b->get_demand();
	// cout << "Backhaul Customer's demand : " << b->get_demand() << "  truck capacity left :  " << keep_track_cpt << endl;
      }
      while ( ( keep_track_cpt >= 0 ) && ( k < BackhaulCustomer::get_countb() -1 ) ) {         // entering the loop means b is not null. same in above loop
	// cout << "just in while loop " <<  b->get_code() << endl;                                                // if truck empty so far loading is forbidden using the loading var
	if (BackhaulCustomer::get_selective()) {
	  b=b->approve( before_b, pos, p, i, keep_track_cpt);
	  if (b==NULL) {
	    // cout << "in main: b null" << endl;
	    goto trynext;
	  }
	}
	// cout << b->get_code() << endl;
	if (before_b == NULL)       optimal->get_current()->reload( before_b, b, p );
	else                        optimal->get_current()->reload( before_b, b );
	reload=true;
	// if (reload) cout << "reload" << keep_track_cpt << endl;
	BackhaulCustomer::erase_c(pos,i);
	k++;
	// cout << k << " backhaul customers serviced so far." << endl << endl;
	before_b=b;
	m++;
	b=BackhaulCustomer::random_sel( pos,i);
	// if (b==NULL) cout << "surprise" << endl;
	if ( (b != NULL) && (!BackhaulCustomer::get_selective()) )  {
	  keep_track_cpt -= b->get_demand();
	  // cout << "Backhaul customer's demand : " << b->get_demand() << " truck capacity left " << keep_track_cpt << endl;
	}
      }

    trynext:
      if ( (load) && (!reload) ) optimal->get_current()->back_noPickup(bound);                    // if no backhaul customer is left to service
      else if ( (!load) && (!reload) )  cout << "WHAT??" << endl ; 
      else optimal->get_current()->return_todepot();
      reload=false;
      load=false;
      // cout << "The total cost of transportation for the truck amounts to " << optimal->get_current()->get_cost() << " and there are " << optimal->get_current()->get_delcapacity() << " units of unused delivery space and " << optimal->get_current()->get_pickcapacity() << " units of unused pick-up space left." << endl << endl;
      // cout << " end of (both) nested loop " << endl << endl;                           // to catch the extreme scenario that demand > capacity. contrasts basic assumption
                                                                        // that a customer is serviced by one vehicle only. sleep on it..
      if ( (m == Customer::get_count()) || ((BackhaulCustomer::get_selective()) && (n==LinehaulCustomer::get_countl() -1) && (b==NULL)) ) {              
	Customer::set_allserviced();
	cout << " END OF SCHEDULING " << endl << endl;
      } else     optimal->add();                                               // "add" new empty truck to fleet. will be loaded in forthcoming loop
    }
    Customer::set_allserviced();
    optimal->set_totalcost();
    optimal->set_pickspace();
    cout << "The total cost of transportation for the fleet of " << optimal->get_size() << " trucks amounts to " << optimal->get_totalcost();
    if (LinehaulCustomer::get_vrptw()) cout << " time units";
    else cout <<" distance units";
    cout << " and there are " << optimal->get_delspace() << " units of unused delivery space and " << optimal->get_pickspace() << " units of unused pickup space left." << endl << endl;
    BackhaulCustomer::non_serviced(i);
    // optimal->visualise();
    if (i==0) {
      bb.set_size(optimal->get_size());
      bb.assign_all(*optimal);
// For sqtl
      vehicles = bb.get_fleet();
      int re_sqlite = call_sqtl();
      printf("Thread sqlite returned %i\n", re_sqlite);
//
    }
    cout << endl << "Optimising schedule now..." << endl << endl;

    best.set_size(optimal->get_size());
    
    optimal->optimise(best);
    cout << "The optimised fleet of " << best.get_size() << " trucks amounts to " << best.get_totalcost();
    if (LinehaulCustomer::get_vrptw()) cout << " time units";
    else cout <<" distance units";
    cout << " and there are " <<  best.get_delspace() << " units of unused delivery space and " << best.get_pickspace() << " units of unused pickup space left." << endl << endl;
    if (best.get_totalcost() <= bb.get_totalcost()) {
      bb.deallocate();
      bb.set_size(best.get_size());
      bb.assign_all(best);
      BackhaulCustomer::cp_list(i);
    }
    BackhaulCustomer::clear_list(i);
    best.deallocate();
    optimal->deallocate();
    delete optimal;
    if (restarts>i) {
      for (iter1=line.begin()+1; iter1!=line.end(); iter1++) {
	(*iter1)->set_serviced();
	// cout << (*iter1)->get_code() << endl;
      }
      for (itr1=back.begin()+1; itr1!=back.end(); itr1++) {
	(*itr1)->set_serviced();
	// cout << (*itr1)->get_code() << endl;
      }
    }
  }
  cout << endl << "Optimised solution after " << restarts << " restarts includes " << bb.get_size() << " trucks and has cost " << bb.get_totalcost();
  if (LinehaulCustomer::get_vrptw()) cout << " time units";
  else cout <<" distance units";
  cout << " with unused delivery space of " << bb.get_delspace() << " and unused pick-up space of " << bb.get_pickspace() << " units." << endl << endl;
  if (LinehaulCustomer::is_variant()) cout << endl << "Ideally (feasibility not guaranteed) minimum size of fleet would be " << (double)total_dem/Truck::get_initcapacity() << " trucks." << endl << endl;
  
  t.stop();
  cout << endl << "Optimised solution constructed after " << t << endl << endl;
  
  finish_clock = clock();
  c_time = (double(finish_clock)-double(start_clock))/CLOCKS_PER_SEC;
  cout << "Ellapsed time given by c++ : " << c_time << "s." << endl << endl;

  bb.visualise();
  bb.schedule();
  bb.deallocate();

  for (it1=positions.begin(); it1!=positions.end(); it1++) {
    delete (*it1);
  }

  for(iter1=line.begin(); iter1!= line.end(); iter1++) {
    delete (*iter1);
  }

  for(itr1=back.begin(); itr1!= back.end(); itr1++) {
    delete (*itr1);
  }
}
