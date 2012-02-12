#include "ChessPiece.h"
#include "Position.h"
#include <vector>
#include <iostream>

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
    vector<vector<ChessPiece> >::iterator iter;
    vector<ChessPiece>::iterator it;
    iter = board.begin();

    iter = board.insert(iter, vector<ChessPiece>());
    it = (*iter).begin();
    it = (*iter).insert(it, ChessPiece("rook", "white"));
    it = (*iter).insert(++it, ChessPiece("knight", "white"));
    it = (*iter).insert(++it, ChessPiece("bishop", "white"));
    it = (*iter).insert(++it, ChessPiece("king", "white"));
    it = (*iter).insert(++it, ChessPiece("queen", "white"));
    it = (*iter).insert(++it, ChessPiece("bishop", "white"));
    it = (*iter).insert(++it, ChessPiece("knight", "white"));
    it = (*iter).insert(++it, ChessPiece("rook", "white"));

    iter = board.insert(++iter, vector<ChessPiece>());
    it = (*iter).begin();
    it = (*iter).insert(it, ChessPiece("pawn", "white"));
    for (int i=1; i<8; i++)
	it = (*iter).insert(++it, ChessPiece("pawn", "white"));

    iter = board.insert(++iter, vector<ChessPiece>());
    it = (*iter).begin();
    it = (*iter).insert(it, ChessPiece("", ""));
    for (int i=1; i<8; i++)
	it = (*iter).insert(++it, ChessPiece("", ""));

    iter = board.insert(++iter, vector<ChessPiece>());
    it = (*iter).begin();
    it = (*iter).insert(it, ChessPiece("", ""));
    for (int i=0; i<8; i++)
	it = (*iter).insert(++it, ChessPiece("", ""));

    iter = board.insert(++iter, vector<ChessPiece>());
    it = (*iter).begin();
    it = (*iter).insert(it, ChessPiece("", ""));
    for (int i=0; i<8; i++)
	it = (*iter).insert(++it, ChessPiece("", ""));

    iter = board.insert(++iter, vector<ChessPiece>());
    it = (*iter).begin();
    it = (*iter).insert(it, ChessPiece("", ""));
    for (int i=0; i<8; i++)
	it = (*iter).insert(++it, ChessPiece("", ""));
    iter = board.insert(++iter, vector<ChessPiece>());

    it = (*iter).begin();
    it = (*iter).insert(it, ChessPiece("pawn", "black"));
    for (int i=0; i<8; i++)
	it = (*iter).insert(++it, ChessPiece("pawn", "black"));

    iter = board.insert(++iter, vector<ChessPiece>());
    it = (*iter).begin();
    it = (*iter).insert(it, ChessPiece("rook", "black"));
    it = (*iter).insert(++it, ChessPiece("knight", "black"));
    it = (*iter).insert(++it, ChessPiece("bishop", "black"));
    it = (*iter).insert(++it, ChessPiece("king", "black"));
    it = (*iter).insert(++it, ChessPiece("queen", "black"));
    it = (*iter).insert(++it, ChessPiece("bishop", "black"));
    it = (*iter).insert(++it, ChessPiece("knight", "black"));
    it = (*iter).insert(++it, ChessPiece("rook", "black"));

/*
    for (iter=board.begin(); iter!=board.end(); iter++) {
	cout << "Row " << iter + 1 - board.begin() << ":" << endl << endl;
	for (it=(*iter).begin(); it!=(*iter).end(); it++) {
	    cout << "Column " << ((char)(it - (*iter).begin() + 'a')) << ": " << (*it).get_color() << " " << (*it).get_name() << endl;
	}
	cout << endl << "---------------------------" << endl << endl;
    }
*/
}

int ChessPiece::move(Position from, Position to) {

}
