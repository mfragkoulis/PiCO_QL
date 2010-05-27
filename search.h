#ifndef SEARCH_H
#define SEARCH_H

#include "bridge.h"
#include "sqlite3.h"

void search(int * resultset, void * data, int iColumn, char *op, sqlite3_value *val);

#endif
