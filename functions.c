#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "functions.h"

#define RESET_COLOR "\x1b[0m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_RED "\x1b[31m"
#define COLOR_GRAY "\x1b[90m"
#define COLOR_BLUE "\e[0;34m"

extern char forest[SIZE][SIZE];
extern pthread_mutex_t forest_mutex[SIZE][SIZE];
extern pthread_cond_t fire_cond[SIZE][SIZE];
extern pthread_mutex_t control_mutex;

void print_forest() {
    pthread_mutex_lock(&control_mutex);
    printf("Estado atual da floresta:\n");
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            char cell = forest[i][j];
            switch (cell) {
                case FREE:
                    printf(COLOR_GRAY "%c " RESET_COLOR, cell);
                    break;
                case SENSOR:
                    printf(COLOR_GREEN "%c " RESET_COLOR, cell);
                    break;
                case FIRE:
                    printf(COLOR_RED "%c " RESET_COLOR, cell);
                    break;
                case BURNED:
                    printf(COLOR_BLUE "%c " RESET_COLOR, cell);
                    break;
                default:
                    printf("%c ", cell);
            }
        }
        printf("\n");
    }
    printf("\n");
    pthread_mutex_unlock(&control_mutex);
}


void* sensor_node(void* arg) {
    Sensor* sensor = (Sensor*)arg;

    while (1) {
        pthread_mutex_lock(&forest_mutex[sensor->x][sensor->y]);
        if (forest[sensor->x][sensor->y] == FIRE) {
            printf("Sensor [%d, %d] detectou incêndio!\n", sensor->x, sensor->y);

            if (sensor->x > 0) {
                pthread_mutex_lock(&forest_mutex[sensor->x - 1][sensor->y]);
                if (forest[sensor->x - 1][sensor->y] == SENSOR) {
                    forest[sensor->x - 1][sensor->y] = FIRE;
                }
                pthread_mutex_unlock(&forest_mutex[sensor->x - 1][sensor->y]);
            }
            if (sensor->x < SIZE - 1) {
                pthread_mutex_lock(&forest_mutex[sensor->x + 1][sensor->y]);
                if (forest[sensor->x + 1][sensor->y] == SENSOR) {
                    forest[sensor->x + 1][sensor->y] = FIRE;
                }
                pthread_mutex_unlock(&forest_mutex[sensor->x + 1][sensor->y]);
            }
            if (sensor->y > 0) {
                pthread_mutex_lock(&forest_mutex[sensor->x][sensor->y - 1]);
                if (forest[sensor->x][sensor->y - 1] == SENSOR) {
                    forest[sensor->x][sensor->y - 1] = FIRE;
                }
                pthread_mutex_unlock(&forest_mutex[sensor->x][sensor->y - 1]);
            }
            if (sensor->y < SIZE - 1) {
                pthread_mutex_lock(&forest_mutex[sensor->x][sensor->y + 1]);
                if (forest[sensor->x][sensor->y + 1] == SENSOR) {
                    forest[sensor->x][sensor->y + 1] = FIRE;
                }
                pthread_mutex_unlock(&forest_mutex[sensor->x][sensor->y + 1]);
            }

            pthread_cond_signal(&fire_cond[sensor->x][sensor->y]);
        }
        pthread_mutex_unlock(&forest_mutex[sensor->x][sensor->y]);
        usleep(700000);
    }

    return NULL;
}

void* fire_generator(void* arg) {
    while (1) {
        int x = rand() % SIZE;
        int y = rand() % SIZE;

        pthread_mutex_lock(&forest_mutex[x][y]);
        if (forest[x][y] == SENSOR) {
            forest[x][y] = FIRE;
            printf("Fogo iniciado em [%d, %d]!\n", x, y);
            print_forest();
        }
        pthread_mutex_unlock(&forest_mutex[x][y]);

        sleep(2);
    }
    return NULL;
}

void* control_center(void* arg) {
    while (1) {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                pthread_mutex_lock(&forest_mutex[i][j]);
                if (forest[i][j] == FIRE) {
                    printf("Central de Controle: Combate ao fogo iniciado em [%d, %d]\n", i, j);

                    if (i > 0 && forest[i - 1][j] == FIRE) {
                        forest[i - 1][j] = BURNED;
                    }
                    if (i < SIZE - 1 && forest[i + 1][j] == FIRE) {
                        forest[i + 1][j] = BURNED;
                    }
                    if (j > 0 && forest[i][j - 1] == FIRE) {
                        forest[i][j - 1] = BURNED;
                    }
                    if (j < SIZE - 1 && forest[i][j + 1] == FIRE) {
                        forest[i][j + 1] = BURNED;
                    }

                    forest[i][j] = BURNED;
                    print_forest();
                }
                pthread_mutex_unlock(&forest_mutex[i][j]);
            }
        }
        sleep(1);
    }
    return NULL;
}
