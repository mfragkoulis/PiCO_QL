#include "ChessPiece.h"
#include "Position.h"
#include "stl_search.h"

vector<vector<ChessPiece> > *chessBoard;

int main() {
    ChessPiece::setup_board();
    chessBoard = &ChessPiece::board;
    int re_sqtl = call_sqtl();
    printf("Thread sqlite returned %i\n", re_sqtl);
//    ChessPiece::move(Position(1, 'c'), Position(2, 'c'));

}
