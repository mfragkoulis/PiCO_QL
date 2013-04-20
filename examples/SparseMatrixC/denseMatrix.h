#ifndef DENSEMATRIX_H
#define DENSEMATRIX_H

struct matrixElement {
  int row, col;
  double val;
};

struct denseMatrix {
  struct matrixElement *me;
  int size;
};

#endif
