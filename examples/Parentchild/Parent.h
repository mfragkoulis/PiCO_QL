#ifndef PARENT_H
#define PARENT_H

#include <list>
#include "Child.h"

using namespace std;

typedef struct Parent
{
	const char *m_data;
	list<Child> m_children;
} Parent;

#endif
