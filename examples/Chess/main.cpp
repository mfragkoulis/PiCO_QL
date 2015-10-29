/*
 *   Implement the main method for Chess.
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
 *  permissions and limitations under the License.
 */

#include "ChessPiece.h"
#include "Position.h"
#include <stdio.h>
/* PiCO_QL header */
#ifndef PICO_QL_SINGLE_THREADED
#include <pthread.h>
#endif
#include "pico_ql_search.h"
/*-------------*/

using namespace picoQL;

int main() {
    vector<vector<ChessPiece> > board;
    pico_ql_register((const void *)&board, "board");
    vector<vector<ChessPiece> >::iterator iter;
    vector<ChessPiece>::iterator it;

    for (int i = 0; i < 8; i++) /*Set up the chess board.*/
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

#ifndef PICO_QL_SINGLE_THREADED
    void *exit_status = NULL;
    pthread_t t;
    pico_ql_serve(8082, &t);
#else
    pico_ql_serve(8082, NULL);
#endif
//    move(Position(1, 'c'), Position(2, 'c'));

/*                                                                              
    for (iter = board.begin(); iter != board.end(); iter++) {                       
    cout << "Row " << iter + 1 - board.begin() << ":" << 
    endl << endl;      
        for (it = (*iter).begin(); it != (*iter).end(); it++) {                     
	cout << "Column " << ((char)(it - 
	(*iter).begin() + 'a')) << ": " << 
	(*it).get_color() << " " << (*it).get_name() << 
	endl;                         
        } 
	cout << endl << "---------------------------" << 
	endl << endl;          
    }
*/
#ifndef PICO_QL_SINGLE_THREADED
    pthread_join(t, &exit_status);
#endif
    return 0;
}

int move(Position from, Position to) {return 0;}


