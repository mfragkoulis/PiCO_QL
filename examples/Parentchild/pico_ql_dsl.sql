#include <list>
#include <vector>
#include "Child.h"
#include "Parent.h"

using namespace std;
$

CREATE STRUCT VIEW Child
( 
	data TEXT FROM m_data
)$

CREATE VIRTUAL TABLE child
USING STRUCT VIEW Child
WITH REGISTERED C TYPE list<Child>$

CREATE STRUCT VIEW Parent
( 
	data TEXT FROM m_data,
	FOREIGN KEY( child_id) FROM m_children REFERENCES child
)$

CREATE VIRTUAL TABLE parent
USING STRUCT VIEW Parent
WITH REGISTERED C NAME parent
WITH REGISTERED C TYPE vector<Parent>$
