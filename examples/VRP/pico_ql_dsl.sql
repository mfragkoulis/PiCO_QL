#include <string>
#include <vector>
#include <map>

#include "Truck.h"
#include "Customer.h"
;

CREATE ELEMENT TABLE Trucks (
      truck_ptr INT FROM TABLE Truck WITH BASE=self
);

CREATE TABLE mydb.Trucks WITH BASE=vehicles AS SELECT * FROM vector<Truck*>*;

CREATE ELEMENT TABLE Truck (
      customers_ptr INT FROM TABLE Customers WITH BASE=get_Customers(),
      cost DOUBLE FROM get_cost(),
      delcapacity INT FROM get_delcapacity(),
);

CREATE TABLE mydb.Truck AS SELECT * FROM Truck;

CREATE ELEMENT TABLE Customers (
      customer_ptr INT FROM TABLE Customer WITH BASE =self
);

CREATE TABLE mydb.Customers AS SELECT * FROM vector<Customer*>;

CREATE ELEMENT TABLE Customer (
      position_ptr INT FROM TABLE Position WITH BASE= get_pos(),
      demand INT FROM get_demand(),  // Demand column 
      code STRING FROM get_code(),
      serviced INT FROM get_serviced(),
);

// Customer description
CREATE TABLE mydb.Customer AS SELECT * FROM Customer;

CREATE ELEMENT TABLE Position (
      x_coord INT FROM get_x(),
      y_coord INT FROM get_y()
);

CREATE TABLE mydb.Position AS SELECT * FROM Position*;

CREATE ELEMENT TABLE MapIndex (
      map_index INT FROM first,
      customer_ptr INT FROM TABLE Customer WITH BASE = second
);

CREATE TABLE mydb.MapIndex WITH BASE =test AS SELECT * FROM map<int, Customer*>;
