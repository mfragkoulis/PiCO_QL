#include "sensor_t.h"
#define DIM0 4
#define Sensor_decl(X) sensor_t *X;int i = 0
#define Sensor_advance(X,Y,Z) X = &Y[Z]
$

CREATE STRUCT VIEW Sensor (
    serial_no INT FROM serial_no,
    location TEXT FROM location,
    temperature DOUBLE FROM temperature,
    pressure DOUBLE FROM pressure
)$

CREATE VIRTUAL TABLE Sensor
USING STRUCT VIEW Sensor
WITH REGISTERED C NAME hd5_sensors
WITH REGISTERED C TYPE sensor_t *
USING LOOP for(iter = &base[i]; i < DIM0; Sensor_advance(iter, base, ++i))$

#endif
