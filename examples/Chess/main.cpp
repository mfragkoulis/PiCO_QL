#include "ChessPiece.h"
#include "Position.h"
#include <stdio.h>
/* SQTL header */
#include "stl_search.h"
/*-------------*/

/* At global scope to be accessible by SQTL */
vector<vector<ChessPiece> > *chessBoard;
/*------------------------------------------*/

int main() {
    vector<vector<ChessPiece> > board;
    chessBoard = &board;
    vector<vector<ChessPiece> >::iterator iter;
    vector<ChessPiece>::iterator it;

    for (int i = 0; i < 8; i++)             // Set up the chess board.
	board.push_back(vector<ChessPiece>());
    iter = board.begin();
    it = (*iter).begin();
    (*iter).push_back(ChessPiece("rook", "white"));
    (*iter).push_back(ChessPiece("knight", "white"));
    (*iter).push_back(ChessPiece("bishop", "white"));
    (*iter).push_back(ChessPiece("king", "white"));
    (*iter).push_back(ChessPiece("queen", "white"));
    (*iter).push_back(ChessPiece("bishop", "white"));
    (*iter).push_back(ChessPiece("knight", "white"));
    (*iter).push_back(ChessPiece("rook", "white"));

    iter++;
    it = (*iter).begin();
    (*iter).push_back(ChessPiece("pawn", "white"));
    for (int i = 1; i < 8; i++)
        (*iter).push_back(ChessPiece("pawn", "white"));

    iter++;
    it = (*iter).begin();
    (*iter).push_back(ChessPiece("", ""));
    for (int i = 1; i < 8; i++)
        (*iter).push_back(ChessPiece("", ""));

    iter++;
    it = (*iter).begin();
    (*iter).push_back(ChessPiece("", ""));
    for (int i = 0; i < 8; i++)
	(*iter).push_back(ChessPiece("", ""));

    iter++;
    it = (*iter).begin();
    (*iter).push_back(ChessPiece("", ""));
    for (int i = 0; i < 8; i++)
        (*iter).push_back(ChessPiece("", ""));

    iter++;
    it = (*iter).begin();
    (*iter).push_back(ChessPiece("", ""));
    for (int i = 0; i < 8; i++)
        (*iter).push_back(ChessPiece("", ""));
    iter++;

    it = (*iter).begin();
    (*iter).push_back(ChessPiece("pawn", "black"));
    for (int i = 0; i < 8; i++)
        (*iter).push_back(ChessPiece("pawn", "black"));

    iter++;
    it = (*iter).begin();
    (*iter).push_back(ChessPiece("rook", "black"));
    (*iter).push_back(ChessPiece("knight", "black"));
    (*iter).push_back(ChessPiece("bishop", "black"));
    (*iter).push_back(ChessPiece("king", "black"));
    (*iter).push_back(ChessPiece("queen", "black"));
    (*iter).push_back(ChessPiece("bishop", "black"));
    (*iter).push_back(ChessPiece("knight", "black"));
    (*iter).push_back(ChessPiece("rook", "black"));

    int re_sqtl = call_sqtl();
    printf("Thread sqlite returned %i\n", re_sqtl);
//    move(Position(1, 'c'), Position(2, 'c'));

/*                                                                              
    for (iter = board.begin(); iter != board.end(); iter++) {                       
    cout << "Row " << iter + 1 - board.begin() << ":" << endl << endl;      
        for (it = (*iter).begin(); it != (*iter).end(); it++) {                     
	cout << "Column " << ((char)(it - (*iter).begin() + 'a')) << ": " <\
	< (*it).get_color() << " " << (*it).get_name() << endl;                         
        }                                                                       
        cout << endl << "---------------------------" << endl << endl;          
    }                                                                           
*/
}

int move(Position from, Position to) {

}


