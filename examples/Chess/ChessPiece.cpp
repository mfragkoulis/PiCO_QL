#include "ChessPiece.h"
#include "Position.h"
#include <vector>
#include <iostream>

using namespace std;

ChessPiece::ChessPiece(string n, string c) {
    name.assign(n);
    color.assign(c);
}

string ChessPiece::get_name() {
    return name;
}

string ChessPiece::get_color() {
    return color;
}
