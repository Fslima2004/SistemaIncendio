#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#define SIZE 30

#define FREE '-'
#define SENSOR 'T'
#define FIRE '@'
#define BURNED '/'

typedef struct {
    int x;
    int y;
} Sensor;

void print_forest();
void* sensor_node(void* arg);
void* fire_generator(void* arg);
void* control_center(void* arg);

#endif
