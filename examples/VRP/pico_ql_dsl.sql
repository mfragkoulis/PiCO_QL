#include <string>
#include <vector>
#include <map>

#include "Truck.h"
#include "Customer.h"
$

CREATE STRUCT VIEW Truck (
      FOREIGN KEY(customers_id) FROM get_Customers() REFERENCES Customer POINTER,
      cost DOUBLE FROM get_cost(),
      delcapacity INT FROM get_delcapacity(),
      delcapacity_root DOUBLE FROM get_delcapacity_math_root(this.get_delcapacity())
)$

CREATE VIRTUAL TABLE Truck
USING STRUCT VIEW Truck
WITH REGISTERED C NAME vehicles 
WITH REGISTERED C TYPE vector<Truck*>*$

CREATE STRUCT VIEW Customer (
      demand INT FROM get_demand(),  // Demand column 
      code STRING FROM get_code(),
      serviced INT FROM get_serviced(),
      x_coord INT FROM get_pos()->get_x(),
      y_coord INT FROM get_pos()->get_y()
)$

// Customer description
CREATE VIRTUAL TABLE Customer
USING STRUCT VIEW Customer
WITH REGISTERED C TYPE vector<Customer*>$

CREATE STRUCT VIEW MapIndex (
      map_index INT FROM first,
      INHERITS STRUCT VIEW Customer FROM second POINTER
)$

CREATE VIRTUAL TABLE MapIndex 
USING STRUCT VIEW MapIndex
WITH REGISTERED C NAME test 
WITH REGISTERED C TYPE map<int, Customer*>$

CREATE VIEW MyTrucks AS
SELECT rownum, cost, delcapacity
FROM Truck
WHERE cost>400$
