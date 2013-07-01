#include "sensor_t.h"
#define DIM0 4
#define DIM1 3
#define DIM2 2
#define Sensor_decl(X) sensor_t *X; int d0 = 0; int d1 = 0; int d2 = 0 
#define Sensor_advance(X,Y,V,W,Z) X = &((*Y)[V][W][Z])
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
WITH REGISTERED C TYPE sensor_t (*) [DIM0][DIM1][DIM2]:sensor_t *
USING LOOP for(iter = &((*base)[0][0][0]); d0 < DIM0; d0++) {
           for(d1 = 0; d1 < DIM1; d1++) {
           for(d2 = 0; d2 < DIM2; Sensor_advance(iter, base, d0, d1, ++d2))$

#endif
