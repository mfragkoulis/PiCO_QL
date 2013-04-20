#include <stdlib.h>
#include <stdio.h>
#include "denseMatrix.h"
#include "pico_ql_search.h"


int main() {
  struct denseMatrix dm;
  dm.size = 6;
  dm.me = (struct matrixElement *)malloc(sizeof(struct matrixElement) * dm.size);
  dm.me[0].row = 0;
  dm.me[0].col = 0;
  dm.me[0].val = 1;

  dm.me[1].row = 0;
  dm.me[1].col = 1;
  dm.me[1].val = 2;

  dm.me[2].row = 1;
  dm.me[2].col = 1;
  dm.me[2].val = 3;

  dm.me[3].row = 1;
  dm.me[3].col = 2;
  dm.me[3].val = 9;

  dm.me[4].row = 2;
  dm.me[4].col = 1;
  dm.me[4].val = 1;

  dm.me[5].row = 2;
  dm.me[5].col = 2;
  dm.me[5].val = 4;

  pico_ql_register(&dm, "denseMatrix");
  pico_ql_serve(8080);

  int r = 0, c = 0;
  int rows = 3;
  int cols = 4;
//  int **matrix = (int **)malloc(rows * (sizeof(int) * cols));
  int matrix[3][4] = {{1, 2, 0, 0},
                      {0, 3, 9, 0},
                      {0, 1, 4, 0}};

  for (r = 0; r < rows; r++) {
    for (c = 0; c < cols; c++)
      printf("matrix[%i][%i] = %i\n", r, c, matrix[r][c]);
    printf("\n");
  }
  return 0;
}


