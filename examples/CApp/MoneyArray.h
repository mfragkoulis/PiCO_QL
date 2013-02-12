#ifndef MONEYARRAY_H
#define MONEYARRAY_H

#include "Money.h"

typedef struct {

  struct Money **mArray;
  int mArraySize;

} MoneyArray;

#endif
