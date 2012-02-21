#include <iostream>
#include <math.h>
#include "LinehaulCustomer.h"


using namespace std;

vector < LinehaulCustomer* > LinehaulCustomer::list_l;

vector < LinehaulCustomer* > LinehaulCustomer::list_ll;

bool LinehaulCustomer::all_Lserviced=false;

int LinehaulCustomer::count_l = 0 ;

map < string, double > LinehaulCustomer::dist_l; 

map < string , Position* > LinehaulCustomer::coord_l; 

bool LinehaulCustomer::variant = false;

int LinehaulCustomer::start_travel = 0;

int LinehaulCustomer::deadline = 0;

bool LinehaulCustomer::pd = false;

bool LinehaulCustomer::tw = false;

extern MTRand_int32 irand;

LinehaulCustomer::LinehaulCustomer( string c, int d, int pd, Position* pos, bool depot, int start, int finish ) : Customer( c, d, pos, depot ) {
  pick_demand=pd;
  start_travel=start;
  service_time=0;
  deadline=finish;
  coord_l.insert( make_pair(c, pos) );
  list_l.push_back(this);
  list_ll.push_back(this);
  // cout << this->get_code() << " " << c << endl;
  count_l++;
}

LinehaulCustomer::LinehaulCustomer( string c, int d, int pd, Position* pos, int start, int service, int finish ) : Customer( c, d, pos ) {
  pick_demand=pd;
  start_time=start;
  service_time=service;
  finish_time=finish;
  coord_l.insert( make_pair(c, pos) );
  list_l.push_back(this);
  // cout << this->get_code() << " " << c << endl;
  count_l++;

}

bool LinehaulCustomer::get_allLserviced() {
  return  all_Lserviced;
}

void LinehaulCustomer::set_allLserviced() {
  if (!all_Lserviced)  all_Lserviced=true;
  else all_Lserviced=false;
}

int LinehaulCustomer::get_countl() {
  return count_l;
}

int LinehaulCustomer::get_deadline() {
  return deadline;
}

LinehaulCustomer* LinehaulCustomer::get_depot() {
  // map < string, Position* >:: iterator iter;
  //iter=coord_l.begin();
  return list_l[0];       //hard-coded.be careful that it is not deleted
}

int LinehaulCustomer::get_pickdemand() {
  return pick_demand;
}

int LinehaulCustomer::get_starttime() {
  return start_time;
}

int LinehaulCustomer::get_servicetime() {
  return service_time;
}

int LinehaulCustomer::get_finishtime() {
  return finish_time;
}

Position* LinehaulCustomer::get_pos() {
  return coord_l[this->get_code()];
}

bool LinehaulCustomer::test(LinehaulCustomer* before_l, double cost) {
  // cout << "testing now" << endl;
  // hard constraint for starters. define slack later on
  string half;
  int d,dback;
  if (before_l==NULL) half=get_depot()->code;
  else half=before_l->code;
  // cout << "before_l is: " << half << " and l is: " << code << endl;

  if (half < code) d=get_dist(half + code);
  else d=get_dist(code + half);

  dback=get_dist(get_depot()->code + code);
  
  if (cost + d + service_time + dback > deadline) return true;
  // cout << "behind deadline, travel time (cost) : " << cost << ", distance: " << d << " **, start time: " << start_time << ", finish time: " << finish_time << ", distance back: " << dback << endl; 
  if (cost + d > finish_time) return true;
  // cout << "test for code " << code << " succeeded" << endl;
  return false;
}


LinehaulCustomer* LinehaulCustomer::attempt_utilize(int i, int& pos, int& keep_track_cpt, int& diff, bool& succeed, bool fail, int& temp, int& bound, LinehaulCustomer* before_l, double cost) {
  // cout << "attempting utilize now" << endl;
  int k=0;
  LinehaulCustomer* l;
  l=this;
  while ( (k!=list_l.size() +1) && ((keep_track_cpt <0) || (fail)) ) {
      fail=false;
      keep_track_cpt += l->demand;
      l= random_sel( before_l, pos, i, cost);
      keep_track_cpt -= l->demand;
      k++;
    }
    if (k==list_l.size() +1) {
      succeed=false;
      // cout << "false" << endl;
    } else {
      succeed=true;
      // cout << "aye " << l->get_code() << "  cap: " << keep_track_cpt << "  dem: " << l->get_demand() << "   pick dem: " << l->get_pickdemand() << endl;
    }
  return l;
}

int LinehaulCustomer::choose(LinehaulCustomer* before_l, int i, double cost) {
  int pos=1, amount, number=0;
  string half, pre, other, next;
  double d, dd, ddd;
  if (before_l != NULL) half=before_l->code;
  else half=get_depot()->code; 
  // cout << "before_l is: " << half << endl;
  if (i%2==0) {
      amount=list_l.size()/2;
      if (pos>1) pre=list_l[pos-1]->code;
      else pre=list_l[1]->code;
      other=list_l[pos]->code;
      if (pos< list_l.size() -1) next=list_l[pos+1]->code;
      else {
	if (list_l.size() >1) next=list_l[list_l.size()-1]->code;
	else next=list_l[1]->code;
      }
      if (half < pre) ddd= get_dist(half + pre);
      else ddd= get_dist(pre + half);
      if (half < other) d= get_dist(half + other);
      else d= get_dist(other + half);
      if (half < next) dd= get_dist(half + next);
      else dd= get_dist(next + half);
      while ( ((cost +d > list_l[pos]->start_time) && (cost +d > list_l[pos]->finish_time) && (number<=20)) || ((cost +d < list_l[pos]->start_time) && (cost +d < list_l[pos]->finish_time) && (number<=20)) ) {
	number++;
	//cout << "pos is: " << pos << ",pos code: " << other << ", start time: " << list_l[pos]->start_time << ", pos +1 start_time: " << list_l[pos+1]->start_time << ", cost + d is: " << cost +d;
	//cout  << ", cost +dd is: " << cost +dd << ", pos+1 code: "<< next  << endl << endl;
	//cout  << "finish time: " << list_l[pos]->finish_time << ", pos+1 finish: " << list_l[pos+1]->finish_time << ", amount: " << amount << endl << endl;
	if (cost +d > list_l[pos]->start_time) {
	  pos= pos + amount;
	  if (pos >= list_l.size()) {
	    if (list_l.size()>1)   pos = list_l.size() -1;
	    else pos=1;
	  }
	}else if (cost +d < list_l[pos]->start_time) {
	  pos = pos - amount;
	  if (pos<=0) pos=1;
	}

	if (amount >2) amount=amount/2;
	else amount=1;

	if (pos>1) pre=list_l[pos-1]->code;
	else pre=list_l[1]->code;

	other=list_l[pos]->code;

	if (pos < list_l.size() -1) next=list_l[pos+1]->code;
	else {
	  if (list_l.size() >1) next=list_l[list_l.size()-1]->code;
	  else next=list_l[1]->code;
	}
     
	if (half < pre) ddd= get_dist(half + pre);
	else ddd= get_dist(pre + half);

	if (half < other) d= get_dist(half + other);
	else d= get_dist(other + half);

	if (half < next) dd= get_dist(half + next);
	else dd= get_dist(next + half);
      }

      // cout << "pos is: " << pos << endl;
      if (cost+d <= list_l[pos]->finish_time) {
	if (cost+d < list_l[pos]->start_time) d=list_l[pos]->start_time;
	else d= cost+d;
      }else d=-1;
      //cout << "d: " << d << endl;
      if (pos+1 < list_l.size() -1) {
	if (cost + dd <= list_l[pos +1]->finish_time) {
	  if (cost+ dd < list_l[pos +1]->start_time) dd=list_l[pos +1]->start_time;
	  else dd= cost+ dd;
	}else dd=-1;
      }else dd=-1;
      //cout << "dd: " << dd << endl;
      if (pos > 1) {
	if (cost + ddd <= list_l[pos -1]->finish_time) {
	  if (cost+ ddd < list_l[pos -1]->start_time) ddd=list_l[pos -1]->start_time;
	  else ddd= cost+ ddd;
	}else ddd=-1;
      }else ddd=-1;
      // cout << "ddd: " << ddd << endl;
      if ( (d<0) && (dd<0) && (ddd<0) ) {
	//cout << "failed to find eligible customer" << endl;
	pos=-1;
	return pos;
      }else if ( (d<0) && (dd<0) && (ddd>0) ) {
	//cout << "returning pos-1" << endl;
	return pos-1;
      }else if ( (d<0) && (dd>0) && (ddd<0) ) {
	//cout << "returning pos+1" << endl;
	return pos+1;
      }else if ( (d>0) && (dd<0) && (ddd<0) ) {
	//cout << "returning pos" << endl;
	return pos;
      }else if ( (d>0) && (dd>0) && (ddd<0) ) {
	if (d<=dd) {
	  // cout << "pos better than pos+1" << endl;
	  return pos;
	}else {
	  // cout << "pos+1 better than pos" << endl;
	  return pos+1;
	}
      }else if ( (d>0) && (dd<0) && (ddd>0) ) {
	if (d<=ddd) {
	  // cout << "pos better than pos-1" << endl;
	  return pos;
	}else {
	  // cout << "pos-1 better than pos" << endl;
	  return pos-1;
	}
      }else if ( (d<0) && (dd>0) && (ddd>0) ) {
	if (dd<=ddd) {
	  // cout << "pos+1 better than pos-1" << endl;
	  return pos+1;
	}else {
	  //cout << "pos-1 better than pos+1" << endl;
	  return pos-1;
	}
      }else if ( (d>0) && (dd>0) && (ddd>0) ) {
	if ( (d<=dd) && (d<=ddd) ) {
	  //cout << "pos best" << endl;
	  return pos;
	}else if ( (dd<d) && (dd<=ddd) ) {
	  // cout << "pos+1 best" << endl;
	  return pos+1;
	}else if ( (ddd<=d) && (ddd<dd) ) {
	  // cout << "pos-1 best" << endl;
	  return pos-1;
	}else  cout << "NO IDEA" << endl;
      }
  }else {
      amount=list_ll.size()/2;
      if (pos>1) pre=list_ll[pos-1]->code;
      else pre=list_ll[1]->code;
      other=list_ll[pos]->code;
      if (pos< list_ll.size() -1) next=list_ll[pos+1]->code;
      else {
	if (list_ll.size() >1) next=list_ll[list_ll.size()-1]->code;
	else next=list_ll[1]->code;
      }
      if (half < pre) ddd= get_dist(half + pre);
      else ddd= get_dist(pre + half);
      if (half < other) d= get_dist(half + other);
      else d= get_dist(other + half);
      if (half < next) dd= get_dist(half + next);
      else dd= get_dist(next + half);
      while ( ((cost +d > list_ll[pos]->start_time) && (cost +d > list_ll[pos]->finish_time) && (number<=20)) || ((cost +d < list_ll[pos]->start_time) && (cost +d < list_ll[pos]->finish_time) && (number<=20)) ) {
	number++;
	//cout << "pos is: " << pos << ",pos code: " << other << ", start time: " << list_ll[pos]->start_time << ", pos +1 start_time: " << list_ll[pos+1]->start_time << ", cost + d is: " << cost +d;
	//cout  << ", cost +dd is: " << cost +dd << ", pos+1 code: "<< next  << endl << endl;
	//cout  << "finish time: " << list_ll[pos]->finish_time << ", pos+1 finish: " << list_ll[pos+1]->finish_time << ", amount: " << amount << endl << endl;
	if (cost +d > list_ll[pos]->start_time) {
	  pos= pos + amount;
	  if (pos >= list_ll.size()) {
	    if (list_ll.size()>1)   pos = list_ll.size() -1;
	    else pos=1;
	  }
	}else if (cost +d < list_ll[pos]->start_time) {
	  pos = pos - amount;
	  if (pos<=0) pos=1;
	}

	if (amount >2) amount=amount/2;
	else amount=1;

	if (pos>1) pre=list_ll[pos-1]->code;
	else pre=list_ll[1]->code;

	other=list_ll[pos]->code;

	if (pos < list_ll.size() -1) next=list_ll[pos+1]->code;
	else {
	  if (list_ll.size() >1) next=list_ll[list_ll.size()-1]->code;
	  else next=list_ll[1]->code;
	}
     
	if (half < pre) ddd= get_dist(half + pre);
	else ddd= get_dist(pre + half);

	if (half < other) d= get_dist(half + other);
	else d= get_dist(other + half);

	if (half < next) dd= get_dist(half + next);
	else dd= get_dist(next + half);
      }

      //cout << "pos is: " << pos << endl;
      if (cost+d <= list_ll[pos]->finish_time) {
	if (cost+d < list_ll[pos]->start_time) d=list_ll[pos]->start_time;
	else d= cost+d;
      }else d=-1;
      //cout << "d: " << d << endl;
      if (pos+1 < list_ll.size() -1) {
	if (cost + dd <= list_ll[pos +1]->finish_time) {
	  if (cost+ dd < list_ll[pos +1]->start_time) dd=list_ll[pos +1]->start_time;
	  else dd= cost+ dd;
	}else dd=-1;
      }else dd=-1;
      // cout << "dd: " << dd << endl;
      if (pos > 1) {
	if (cost + ddd <= list_ll[pos -1]->finish_time) {
	  if (cost+ ddd < list_ll[pos -1]->start_time) ddd=list_ll[pos -1]->start_time;
	  else ddd= cost+ ddd;
	}else ddd=-1;
      }else ddd=-1;
      // cout << "ddd: " << ddd << endl;
      if ( (d<0) && (dd<0) && (ddd<0) ) {
	//cout << "failed to find eligible customer" << endl;
	pos=-1;
	return pos;
      }else if ( (d<0) && (dd<0) && (ddd>0) ) {
	//cout << "returning pos-1" << endl;
	return pos-1;
      }else if ( (d<0) && (dd>0) && (ddd<0) ) {
	//cout << "returning pos+1" << endl;
	return pos+1;
      }else if ( (d>0) && (dd<0) && (ddd<0) ) {
	// cout << "returning pos" << endl;
	return pos;
      }else if ( (d>0) && (dd>0) && (ddd<0) ) {
	if (d<=dd) {
	  // cout << "pos better than pos+1" << endl;
	  return pos;
	}else {
	  // cout << "pos+1 better than pos" << endl;
	  return pos+1;
	}
      }else if ( (d>0) && (dd<0) && (ddd>0) ) {
	if (d<=ddd) {
	  // cout << "pos better than pos-1" << endl;
	  return pos;
	}else {
	  //cout << "pos-1 better than pos" << endl;
	  return pos-1;
	}
      }else if ( (d<0) && (dd>0) && (ddd>0) ) {
	if (dd<=ddd) {
	  // cout << "pos+1 better than pos-1" << endl;
	  return pos+1;
	}else {
	  //cout << "pos-1 better than pos+1" << endl;
	  return pos-1;
	}
      }else if ( (d>0) && (dd>0) && (ddd>0) ) {
	if ( (d<=dd) && (d<=ddd) ) {
	  //cout << "pos best" << endl;
	  return pos;
	}else if ( (dd<d) && (dd<=ddd) ) {
	  //cout << "pos+1 best" << endl;
	  return pos+1;
	}else if ( (ddd<=d) && (ddd<dd) ) {
	  // cout << "pos-1 best" << endl;
	  return pos-1;
	}else  cout << "NO IDEA" << endl;
      }
  }
}

LinehaulCustomer* LinehaulCustomer::random_sel(int& pos) {
    //cout << " list_l size : " << list_l.size() << endl;
    //cout << "list_ll size : " << list_ll.size() << endl;
    if ( list_l.size() > 1 ) {                                 // the depot will (should) never be considered, therefore 1
	pos = irand()%list_l.size();
	while  ( list_l[pos]->get_serviced() ) {              // assumption: depot will always be the first input "customer"
	    pos=irand()%list_l.size();                           // cnt: apparently depot is not eligible for selection
	}
	// cout <<  "1-Linehaul customer selected : " << list_l[pos]->get_code() << " at position : " << pos << endl;
	return list_l[pos];
    } else {
	//cout << "NULL" << endl;
	set_allserviced();
	return NULL;
    }
}


void LinehaulCustomer::erase_c(int random) {
    /* cout << "1-Linehaul customer : " << list_l[random]->get_code() << 
       " is being erased from position " << random << " of " << 
       list_l.size() << " element list" << endl; */
    list_ll.push_back(list_l[random]);
    list_l.erase(list_l.begin() + random);
    /* cout << "1-Now list contains : " << list_l.size() << 
       " elements." << endl; */
}


double LinehaulCustomer::get_dist(string pair) {
  return dist_l[pair];

  /*  map < string, double >::iterator iter= dist_l.find(pair);
  if (iter != dist_l.end() ) return iter->second;
  else return 1000000.0;
  */
}
 

void LinehaulCustomer::compute_dist() {
  map < string, Position* >::iterator top;
  map < string, Position* >::iterator nested;
  map < string, Position* >::iterator next;
  for (top=coord_l.begin(); top!=coord_l.end(); top++) {
    //    cout << top->first << endl;
    next=top;
    next++;
    for (nested=next; nested!=coord_l.end(); nested++) {
      double temp = sqrt( pow( nested->second->get_x() - top->second->get_x(),2 ) + pow( nested->second->get_y() - top->second->get_y(),2 ) );
      dist_l.insert(make_pair(top->first + nested->first, temp));
      // cout << "The distance between " << top->first << " and " << nested->first << " linehaul participators is : " << LinehaulCustomer::get_dist(top->first + nested->first) << " units" <<  endl;
    }
  }
}

void LinehaulCustomer::sort_list(int i) {
  //  cout << "sorting now" << endl;
  vector < LinehaulCustomer* >::iterator start;
  vector < LinehaulCustomer* >::iterator finish;
  if (i%2==0) {
    start=list_l.begin() +1;
    finish=list_l.end() -1;
    // cout << "start is: " << (*start)->code << endl;
    // cout << "finish is: " << (*finish)->code << endl;
    random_quicksort( i, start, finish);
  } else {
    start=list_ll.begin() +1;
    finish=list_ll.end() -1;
    random_quicksort( i, start, finish);
  }
  // cout << "code: " << (*(list_l.begin()))->code << ", start_time: " << start_travel << endl;
  int number=0;
  for (; start!=finish +1; start++) {
    // cout << "code: " << (*start)->code << ", start time: " << (*start)->start_time << endl;
    number++;
  }
  // cout << "customers ordered: " << number << endl;
}

void LinehaulCustomer::random_quicksort( int i, vector < LinehaulCustomer* >::iterator start, vector < LinehaulCustomer* >::iterator finish) {
  // cout << "random quicksort" << endl;
  vector < LinehaulCustomer* >::iterator position; 
  if (start < finish) {
    position=randomized_partition( i, start, finish);
    random_quicksort(i, start, position -1);
    random_quicksort(i, position +1, finish);
  }
}

vector < LinehaulCustomer* >::iterator LinehaulCustomer::randomized_partition( int i, vector < LinehaulCustomer* >::iterator start, vector < LinehaulCustomer* >::iterator finish) {
  // cout << "randomized partition" << endl;
  vector < LinehaulCustomer* >::iterator position;
  LinehaulCustomer* l;
  if (i%2==0) {
    position= start + irand()%list_l.size();
    while ( (position > finish) || (position < start) ) {
      position= start + irand()%list_l.size(); 
    }
    // cout << "position is: " << (*position)->code << endl;
    l=(*position);
    (*position)=(*finish);
    (*finish)=l;
  } else {
    position= list_ll.begin() + irand()%list_ll.size();
    while ( (position > finish) || (position < start) ) {
      position= list_ll.begin() + irand()%list_ll.size(); 
    }
    l=(*position);
    (*position)=(*finish);
    (*finish)=l;
  }
  return partition(start, finish);
}

vector < LinehaulCustomer* >::iterator LinehaulCustomer::partition(vector < LinehaulCustomer* >::iterator start, vector < LinehaulCustomer* >::iterator finish) {
  // cout << "partition" << endl;
  LinehaulCustomer* l;
  LinehaulCustomer* exchange;
  vector < LinehaulCustomer* >::iterator h,j;
  // cout << "finish is: " << (*finish)->code << endl;
  l = (*finish);
  h = start -1;
  for (j=start; j<= finish -1; j++) {
    if ( (*j)->start_time <= l->start_time )   {
      h++;
      // cout << "h is: " << (*h)->code << endl;
      exchange=(*h);
      (*h)=(*j);
      (*j)=exchange;
    }else ;  // cout << "not in if" << endl;
  }
  exchange=(*(h+1));
  (*(h+1))=(*finish);
  (*finish)=exchange;
  // cout << "after loop, exchanged: " << (*finish)->code << " with " << (*(h+1))->code << endl; 
  return h+1;
}
