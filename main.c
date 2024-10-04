#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <locale.h>
#include "functions.h"

char forest[SIZE][SIZE];
pthread_mutex_t forest_mutex[SIZE][SIZE];
pthread_cond_t fire_cond[SIZE][SIZE];
pthread_mutex_t control_mutex = PTHREAD_MUTEX_INITIALIZER;

int main() {

    setlocale(LC_ALL, "Portuguese");
    srand(time(NULL));

    // Inicializar a floresta
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            forest[i][j] = (rand() % 3 == 0) ? SENSOR : FREE;
            pthread_mutex_init(&forest_mutex[i][j], NULL);
            pthread_cond_init(&fire_cond[i][j], NULL);
        }
    }

    print_forest();

    // Criar threads dos sensores
    pthread_t sensor_threads[SIZE][SIZE];
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (forest[i][j] == SENSOR) {
                Sensor* sensor = (Sensor*)malloc(sizeof(Sensor));
                sensor->x = i;
                sensor->y = j;
                pthread_create(&sensor_threads[i][j], NULL, sensor_node, (void*)sensor);
            }
        }
    }

    // Criar thread para geração de incêndios
    pthread_t fire_gen_thread;
    pthread_create(&fire_gen_thread, NULL, fire_generator, NULL);

    // Criar thread para a central de controle
    pthread_t control_thread;
    pthread_create(&control_thread, NULL, control_center, NULL);

    // Esperar que as threads terminem (não terminarão até que o programa seja encerrado manualmente)
    pthread_join(fire_gen_thread, NULL);
    pthread_join(control_thread, NULL);

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (forest[i][j] == SENSOR) {
                pthread_join(sensor_threads[i][j], NULL);
            }
        }
    }

    return 0;
}
