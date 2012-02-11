#include "ChessPiece.h"
#include "Position.h"


int main() {
    ChessPiece::setup_board();
    ChessPiece::move(Position(1, 'c'), Position(2, 'c'));

}
