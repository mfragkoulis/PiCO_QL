#include <math.h>
#include "Position.h"

using namespace std;

Position::Position (int xx, int yy) {
  x=xx;
  y=yy;
}

int Position::get_x() {
  return x;
}

int Position::get_y() {
  return y;
}

double Position::distance( Position* pos) {
 return sqrt( pow( pos->get_x() - this->get_x(),2 ) + pow( pos->get_y() - this->get_y(),2 ) );
}
