#ifndef MONEYARRAY_H
#define MONEYARRAY_H

#include "Money.h"

typedef struct {

  int *intArray;
  int iArraySize;
  struct Money **mArray;
  int mArraySize;

} MoneyArray;

#endif
