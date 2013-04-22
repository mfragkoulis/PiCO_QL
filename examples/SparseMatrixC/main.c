#include <stdlib.h>
#include <stdio.h>
#include "denseMatrix.h"
#include "pico_ql_search.h"


int main() {
  struct denseMatrix dm;
  int r = 0, c = 0, n = 0;
  int rows = 3;
  int cols = 4;
  int matrix[3][4] = {{1, 2, 0, 0},
                      {0, 3, 9, 0},
                      {0, 1, 4, 0}};
  dm.size = 0;

  for (r = 0; r < rows; r++) {
    for (c = 0; c < cols; c++) {
      if (matrix[r][c] > 0)
        dm.size++;
      printf("matrix[%i][%i] = %i\n", r, c, matrix[r][c]);
    }
    printf("\n");
  }
  dm.me = (struct matrixElement *)malloc(sizeof(struct matrixElement) * dm.size);
  for (r = 0; r < rows; r++) {
    for (c = 0; c < cols; c++) {
      if (matrix[r][c] > 0) {
        dm.me[n].row = r + 1;
        dm.me[n].col = c + 1;
        dm.me[n].val = matrix[r][c];
        n++;
      }
    }
  }

  pico_ql_register(&dm, "denseMatrix");
  pico_ql_serve(8080);

  return 0;
}


