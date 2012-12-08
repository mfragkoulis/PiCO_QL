#ifndef MONETARY_SYSTEM_H
#define MONETARY_SYSTEM_H

#include "Money.h"

typedef struct {

  int nCurrency;
  struct Money *root;

} Monetary_System;

#endif
