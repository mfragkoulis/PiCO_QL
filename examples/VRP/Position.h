#ifndef POSITION_H
#define POSITION_H

class Position {

 private:
  int x;
  int y;

 public:
  Position(int xx, int yy);
  int get_x();
  int get_y();
  double distance(Position* pos);
};


#endif
