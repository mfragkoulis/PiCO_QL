/*
 *   Implement the member methods for Position class.
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

#include <math.h> // cmath would fail for pow(int, int).
#include "Position.h"


Position::Position () {
  x = 0;
  y = 0;
}

Position::Position (int xx, int yy) {
  x = xx;
  y = yy;
}


int Position::get_x() {
  return x;
}


int Position::get_y() {
  return y;
}


// Calculates the distance between two positions.
double Position::distance( Position* pos) {
 return sqrt(pow(pos->get_x() - this->get_x(), 2) + 
	     pow(pos->get_y() - this->get_y(), 2));
}
