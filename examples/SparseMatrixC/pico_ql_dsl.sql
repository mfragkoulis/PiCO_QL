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
USING LOOP for(tuple_iter = &base->me[record]; record != base->size; Matrix_advance(tuple_iter,base->me,++record))$

CREATE VIEW RowWiseA AS 
SELECT mval, mcol 
FROM Matrix 
WHERE mval != 0 
ORDER BY mrow, mcol$

CREATE VIEW RowWiseB AS 
SELECT mrow, COUNT(mrow) AS countRows
FROM Matrix 
WHERE mval != 0 
GROUP BY mrow 
ORDER BY mrow$

CREATE VIEW RowWiseC AS
SELECT B1.mrow, IFNULL(SUM(B2.countRows), 0) + 1 as IA 
FROM RowWiseB B1 
LEFT OUTER JOIN RowWiseB B2 
ON B2.mrow < B1.mrow 
GROUP BY B1.mrow$
