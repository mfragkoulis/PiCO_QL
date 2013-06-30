#include "sensor_t.h"
#define DIM0 4
#define DIM1 3
#define DIM2 2
#define Sensor_decl(X) sensor_t *X; int d2 = 0; int d1 = 0;int d0 = 0
#define Sensor_advance(X,Y,V,W,Z) X = Y[V][mod(W,DIM1)][mod(Z,DIM2)]
#define div(X,Y) (X/Y)
#define mod(X,Y) (X%Y)
int check_terminate(int *d0, int *d1, int *d2) {
  if (*d0 < DIM0) {
    (*d2)++;
    *d1 = div(*d2,DIM2);
    *d0 = div(*d1, DIM1);
    return 1;
  } else
    return 0;
}
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
WITH REGISTERED C TYPE sensor_t (*)[DIM0][DIM1][DIM2]:sensor_t *
USING LOOP for(iter = base[0][0][0]; check_terminate(&d0, &d1, &d2); Sensor_advance(iter, base, d0, d1, d2))$

#endif
