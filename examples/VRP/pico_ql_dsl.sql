#include <string>
#include <vector>
#include <map>

#include "Truck.h"
#include "Customer.h"
;

CREATE STRUCT VIEW Trucks (
      FOREIGN KEY (truck_id) FROM self REFERENCES Truck POINTER
);

CREATE VIRTUAL TABLE mydb.Trucks 
USING STRUCT VIEW Trucks
WITH REGISTERED C NAME vehicles 
WITH REGISTERED C TYPE vector<Truck*>*;

CREATE STRUCT VIEW Truck (
      FOREIGN KEY(customers_id) FROM get_Customers() REFERENCES Customers POINTER,
      cost DOUBLE FROM get_cost(),
      delcapacity INT FROM get_delcapacity(),
);

CREATE VIRTUAL TABLE mydb.Truck 
USING STRUCT VIEW Truck
WITH REGISTERED C TYPE Truck;

CREATE STRUCT VIEW Customers (
      FOREIGN KEY(customer_id) FROM self REFERENCES Customer POINTER
);

CREATE VIRTUAL TABLE mydb.Customers 
USING STRUCT VIEW Customers
WITH REGISTERED C TYPE vector<Customer*>;

CREATE STRUCT VIEW Customer (
      FOREIGN KEY(position_id) FROM get_pos() REFERENCES Position POINTER,
      demand INT FROM get_demand(),  // Demand column 
      code STRING FROM get_code(),
      serviced INT FROM get_serviced(),
);

// Customer description
CREATE VIRTUAL TABLE mydb.Customer 
USING STRUCT VIEW Customer
WITH REGISTERED C TYPE Customer;

CREATE STRUCT VIEW Position (
      x_coord INT FROM get_x(),
      y_coord INT FROM get_y()
);

CREATE VIRTUAL TABLE mydb.Position 
USING STRUCT VIEW Position
WITH REGISTERED C TYPE Position;

CREATE STRUCT VIEW MapIndex (
      map_index INT FROM first,
      FOREIGN KEY(customer_id) FROM second REFERENCES Customer POINTER
);

CREATE VIRTUAL TABLE mydb.MapIndex 
USING STRUCT VIEW MapIndex
WITH REGISTERED C NAME test 
WITH REGISTERED C TYPE map<int, Customer*>;
