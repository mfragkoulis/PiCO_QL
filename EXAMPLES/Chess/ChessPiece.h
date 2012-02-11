#ifndef CHESSPIECE_H
#define CHESSPIECE_H

#include <string>
#include <vector>
#include "Position.h"

using namespace std;

class ChessPiece {

 private:
  string name;
  string color;

 public:
  ChessPiece(string n, string c);
  static vector<vector<ChessPiece> > board;
  static void setup_board();
  static int move(Position from, Position to);
  string get_name();
  string get_color();

};

#endif
