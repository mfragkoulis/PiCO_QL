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
  fleet.push_back(new Truck(200));           // I dont like this..amateur
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

void Fleet::visualise() {

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
  ListDigraph::ArcMap<int> acolors(g);
  ListDigraph::ArcMap<int> widths(g);

  vector < Node > all_nodes;
  
  vector < Truck* >::iterator iter;
  vector < Customer* >::iterator it;
  Arc a;
  int differ, pos, inner, k,size,count;
  stringstream out;
  string count_string, filename, size_string;

  size=0;
  for (iter=fleet.begin(); iter!=fleet.end(); iter++) {          // for every truck in fleet
    if ( (*iter)->get_cargosize() > size ) size = (*iter)->get_cargosize();
  }

  pos=0;
  differ=5;
  
  all_nodes.push_back(g.addNode());
  label[all_nodes[pos]]="D";  // LinehaulCustomer::get_depot()->get_code();
  coords[all_nodes[pos]]=Point( LinehaulCustomer::get_depot()->get_pos()->get_x(), LinehaulCustomer::get_depot()->get_pos()->get_y() );    
  sizes[all_nodes[pos]]=1.5;
  colors[all_nodes[pos]]=4;
  shapes[all_nodes[pos]]=2;
  
  for (iter=fleet.begin(); iter!=fleet.end(); iter++) {          // for every truck in fleet
    inner=0;
    // first_it=true;
    for (it=(*iter)->start(); it!=(*iter)->finish(); it++) {     // for every customer in truck
      //if (it - (*iter)->start() == count) {
      pos++;
      all_nodes.push_back(g.addNode());
      label[all_nodes[pos]]=(*it)->get_code();
      coords[all_nodes[pos]]=Point((*it)->get_pos()->get_x(),(*it)->get_pos()->get_y());
      sizes[all_nodes[pos]]=1;
      colors[all_nodes[pos]]=differ;
      k=(*iter)->get_rlpoint();
      if (inner < k)   shapes[all_nodes[pos]]=1;           // distinguish between linehaul (square) and backhaul (circle) customers in graph according to shape
      else shapes[all_nodes[pos]]=0;
      inner++;
    }	  
    differ++;
  }
  

  count=-1;
  while (count < size) {
    count++;
    pos =1;
    differ=5;
    for (iter=fleet.begin(); iter!=fleet.end(); iter++) {          // for every truck in fleet
      if (count==0) {
	a=g.addArc(all_nodes[0],all_nodes[pos + count]);
	acolors[a]=differ;
	widths[a]=1.2;
      }else if ( count < (*iter)->get_cargosize() ) {
	a=g.addArc(all_nodes[pos + count -1], all_nodes[pos + count]);
	acolors[a]=differ;
	widths[a]=1.2;
      }
      if (count == (*iter)->get_cargosize()) {
	a=g.addArc(all_nodes[pos + count -1], all_nodes[0]);
	acolors[a]=differ;
	widths[a]=1.2;
      }
      differ++;
      pos += (*iter)->get_cargosize();
    }
    
    out.str("");
    out << count+1;
    count_string = out.str();
    out.str("");
    out << size+1;
    size_string=out.str();
    count_string="View " + count_string + " of " + size_string;
    cout << count_string << endl;
    filename = "Fleet_Visual_Phasing.eps";

    cout << "Scheduling Visualisation Phase " << count+1 << "  complete " << endl;
    graphToEps(g, filename).
    title(count_string).
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
    enableParallel().parArcDist(1).
    drawArrows().arrowWidth(0.7).arrowLength(0.7).
    run();
    system("ghostview Fleet_Visual_Phasing.eps");
  }

  vector < BackhaulCustomer* >::iterator bi;
  // cout << "almost" << endl;
  bi=BackhaulCustomer::get_nonser();
  // cout << (*bi)->get_code() << endl;
  while (*bi!=BackhaulCustomer::get_depot()) {
    all_nodes.push_back(g.addNode());
    label[all_nodes.back()]=(*bi)->get_code();
    coords[all_nodes.back()]=Point((*bi)->get_pos()->get_x(),(*bi)->get_pos()->get_y());
    sizes[all_nodes.back()]=1;
    colors[all_nodes.back()]=differ;
    shapes[all_nodes.back()]=4;
    bi--;
  }
      
  // IdMap<ListDigraph,Node> id(g);
  
//  choose running configuration
//  somehow show which customers are linehaul and which are backhaul
  
  cout << endl << "Scheduling Visualisation complete " << endl;
  graphToEps(g,"Fleet_Visual_Final.eps").
  title("Final View").
  copyright("(C) 2003-2009 LEMON Project").
  scaleToA4().
  absoluteNodeSizes().absoluteArcWidths().
  nodeColors(composeMap(palette,colors)).
  coords(coords).
  nodeScale(1).
  nodeSizes(sizes).
  nodeShapes(shapes).
  arcColors(composeMap(palette,acolors)).
    //arcWidthScale(.4).
  arcWidths(widths).
  nodeTexts(label).nodeTextSize(1).
  enableParallel().parArcDist(1).
  drawArrows().arrowWidth(0.7).arrowLength(0.7).
  run();
  system("ghostview Fleet_Visual_Final.eps");
  // system("kill 1452");
}

void Fleet::schedule() {
  int command;
  vector < Truck* >::iterator iter;
  vector < Customer* >::iterator it;
  ofstream out;
  out.open("schedule.txt");
  out << endl;
  out << "Service towards customers will happen as follows: " << endl << endl;
  for (iter=fleet.begin(); iter!=fleet.end(); iter++) {
    out << "Truck " << iter +1 - fleet.begin() << " services the following customers with codes: " << endl << endl;
    for (it=(*iter)->start(); it!=(*iter)->finish(); it++) {
      out << (*it)->get_code() << "       ";
    }
    out << endl << endl << endl;
  }
  out.close();
  command =  system("emacs schedule.txt");
  if (command==0) cout << endl << "Opening of schedule was successful." << endl << endl;
  else cout << endl << "Opening of schedule failed." << endl << endl; 
}



void Fleet::optimise( Fleet& best) {

  double iterations=10000.0, temperature=100.0, m=20.0, cool=0.9, spend=1.1, counter=0 ;
  vector < Truck* >::iterator iter;
  vector < Customer* >::iterator it;

  Fleet new_fleet;
  new_fleet.set_size(this->get_size());
  new_fleet.assign_all(*this);

  best.assign_all(*this);

  while (counter <= iterations) {

    // cout << "best: " << best.total_cost << " | " << endl;
    // cout << " COUNTER : " << counter << endl;
    /*for (iter=(best.fleet).begin(); iter!=(best.fleet).end(); iter++) {
      cout << "best truck : " << iter - best.fleet.begin() << endl;
      for (it=(*iter)->start(); it!=(*iter)->finish(); it++) {
	cout << (*it)->get_code() << endl;
      }
      cout << endl;
      }*/
    // cout << "before nested new_cost is:  " << new_fleet.total_cost << endl;

    nested(new_fleet, &best, temperature, m);

    // cout << "after nested new_cost is:  " << new_fleet.total_cost << endl;

    counter += m;
    m *= spend;
    temperature *= cool;

    // cout << best.total_cost << " " << best.unused_delspace << " " << best.unused_pickspace << endl;
    /*    vector < Truck* >::iterator iter;
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
    // cout << "new: " << new_fleet.total_cost << "~";
    // cout << " m : " << m << endl;
   
    new_fleet.generate_new(pos1, pos2);

    /* for (iter=fleet.begin(); iter!=fleet.end(); iter++) {
      cout << "current truck : " << iter - fleet.begin() << ", cost: " << (*iter)->get_cost() << endl;
      for (it=(*iter)->start(); it!=(*iter)->finish(); it++) {
	cout << (*it)->get_code() << endl;
      }
      cout << endl;
    }
    for (iter=new_fleet.fleet.begin(); iter!=new_fleet.fleet.end(); iter++) {
      cout << "new fleet truck : " << iter - new_fleet.fleet.begin() << ", cost: " << (*iter)->get_cost() << endl;
      for (it=(*iter)->start(); it!=(*iter)->finish(); it++) {
	cout << (*it)->get_code() << endl;
      }
      cout << endl;
      }*/

    // cout << endl << "Current cost is : " << total_cost << endl;
    // cout << "New cost is : " << new_fleet.total_cost << endl;
    // cout << "Best cost is : " << best->total_cost << endl;

    v= (double) irand() / (double) RAND_MAX;

    if (new_fleet.total_cost < total_cost) {

      // cout << "better" << endl;
      /*for (iter=fleet.begin(); iter!=fleet.end(); iter++) {
	cout << "current truck : " << iter - fleet.begin() << endl;
	for (it=(*iter)->start(); it!=(*iter)->finish(); it++) {
	  cout << (*it)->get_code() << endl;
	}
	cout << endl;
	}*/

      assignT(pos1, pos2, new_fleet);

      /*      for (iter=fleet.begin(); iter!=fleet.end(); iter++) {
	cout << "current truck : " << iter - fleet.begin() << endl;
	for (it=(*iter)->start(); it!=(*iter)->finish(); it++) {
	  cout << (*it)->get_code() << endl;
	}
	cout << endl;
	}*/

      total_cost=new_fleet.total_cost;
      unused_delspace=new_fleet.unused_delspace;
      unused_pickspace=new_fleet.unused_pickspace;

      if (total_cost < best->total_cost) {
	best->assign_best(*this);

	/*for (iter=(best->fleet).begin(); iter!=(best->fleet).end(); iter++) {
	  cout << "best truck : " << iter - best->fleet.begin() << ", cost: " << (*iter)->get_cost() << endl;
	  for (it=(*iter)->start(); it!=(*iter)->finish(); it++) {
	    cout << (*it)->get_code() << endl;
	  }
	  cout << endl;
	  }*/
      }
    }
    else if ( v < exp(-(new_fleet.total_cost - total_cost) / temperature)) {
      // cout << "worse but accepted" << endl;
      assignT(pos1, pos2, new_fleet);
      total_cost=new_fleet.total_cost;
      unused_delspace=new_fleet.unused_delspace;
      unused_pickspace=new_fleet.unused_pickspace;
    } else {

      // cout << "not accepted" << endl; 
      /* for (iter=fleet.begin(); iter!=fleet.end(); iter++) {
	cout << "current truck : " << iter - fleet.begin() << endl;
	for (it=(*iter)->start(); it!=(*iter)->finish(); it++) {
	  cout << (*it)->get_code() << endl;
	}
	cout << endl;
	}*/

      new_fleet.assignT(pos1, pos2, *this);

      // cout << new_fleet.total_cost << "  ->  ";

      new_fleet.total_cost=total_cost;

      // cout << new_fleet.total_cost << endl;

      new_fleet.unused_delspace=unused_delspace;
      new_fleet.unused_pickspace=unused_pickspace;
    }
    m -= 1;

    // cout << " AFTER ASSIGNMENT " << endl;
    // cout << "Current cost is : " << total_cost << endl;
    // cout << "New cost is : " << new_fleet.total_cost << endl;
    // cout << "Best cost is : " << best->total_cost << endl << endl;
    /* for (iter=new_fleet.fleet.begin(); iter!=new_fleet.fleet.end(); iter++) {
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
      }*/

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
  set_pickspace();

  /* cout << " truck position : " << pos1 << "rl_point : " << fleet[pos1]->get_rlpoint() << endl;
  for (it=fleet[pos1]->start(); it!=fleet[pos1]->finish(); it++) {
    cout << (*it)->get_code() << endl;
  }
  cout << endl;
  cout << " truck position : " << pos2 << "rl_point : " << fleet[pos2]->get_rlpoint() << endl;
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

