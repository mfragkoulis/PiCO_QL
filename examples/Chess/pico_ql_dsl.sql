#include "ChessPiece.h"
#include <vector>
;

CREATE STRUCT VIEW ChessPiece (
       name STRING FROM get_name(),
       color STRING FROM get_color());

CREATE VIRTUAL TABLE ChessDB.ChessRow 
USING STRUCT VIEW ChessPiece
WITH REGISTERED C TYPE vector<ChessPiece>;
// Chessboard description
CREATE STRUCT VIEW ChessBoard (
       FOREIGN KEY(row_id) FROM self REFERENCES ChessRow);

CREATE VIRTUAL TABLE ChessDB.ChessBoard 
USING STRUCT VIEW ChessBoard
WITH REGISTERED C NAME board 
WITH REGISTERED C TYPE vector<vector<ChessPiece> >;
