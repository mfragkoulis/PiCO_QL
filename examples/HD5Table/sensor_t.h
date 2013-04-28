#ifndef SENSOR_T_H
#define SENSOR_T_H

typedef struct {
    int     serial_no;
    char    *location;
    double  temperature;
    double  pressure;
} sensor_t;                                 /* Compound type */

#endif
