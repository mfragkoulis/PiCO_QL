#include "sensor_t.h"
#define DIM0 4
#define DIM1 3
#define DIM2 2
#define Sensor_decl(X) sensor_t *X;int i = 0, jj = 0, k = 0
#define foreach(sensor_t *iter, sensor_t ***base, int i, int j, int k) \
  if (++k == DIM2) {j++;k=0;}
  if (j == DIM1) {i++;j=0;}
  if (i == DIM0) break;
  iter = &base[i][j][k];
}  
  for(;i < DIM0; i++) {
    for(;j < DIM1; j++) {
#define Sensor_advance(X,Y,Z,W,V) X = &Y[Z][W][V]
#define Sensor_advanceD1(X,Y,Z,W,V) (W == DIM1 ? Sensor_advance(X,Y,++Z,W=0,V) : X = &Y[Z][W][V])
#define Sensor_advanceD2(X,Y,Z,W,V) (V == DIM2 ? Sensor_advanceD1(X,Y,Z,++W,V=0) : X = &Y[Z][W][V])
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
WITH REGISTERED C TYPE sensor_t ***
USING LOOP foreach(iter, base, i, jj, k)$
USING LOOP for(iter = &base[i][jj][k]; i < DIM0; Sensor_advanceD2(iter, base, i, jj, ++k))$

#endif
