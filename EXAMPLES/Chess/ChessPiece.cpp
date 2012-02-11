#include "ChessPiece.h"
#include "Position.h"
#include <vector>

using namespace std;

ChessPiece::ChessPiece(string n, string c) {
    name = n;
    color = c;
}

string ChessPiece::get_name() {
    return name;
}

string ChessPiece::get_color() {
    return color;
}

vector<vector<ChessPiece> > ChessPiece::board;

void ChessPiece::setup_board() {
    board.push_back(vector<ChessPiece>());


}

int ChessPiece::move(Position from, Position to) {

}
