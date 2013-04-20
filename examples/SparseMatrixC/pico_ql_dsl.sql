#include "denseMatrix.h"
#define Matrix_decl(X) struct matrixElement *X;int record = 0
#define Matrix_advance(X,Y,Z) X = &Y[Z]
$

CREATE STRUCT VIEW Matrix (
	mrow INT FROM row,
	mcol INT FROM col,
	mval DOUBLE FROM val
)$

CREATE VIRTUAL TABLE Matrix
USING STRUCT VIEW Matrix
WITH REGISTERED C NAME denseMatrix
WITH REGISTERED C TYPE struct denseMatrix:struct matrixElement *
USING LOOP for(iter = &base->me[record]; record != base->size; Matrix_advance(iter,base->me,++record))$
