#ifndef POSITION_H
#define POSITION_H

class Position {

 private:
  int row;
  char column;

 public:
  Position(int r, char c);
  int get_row();
  char get_column();

};

#endif
