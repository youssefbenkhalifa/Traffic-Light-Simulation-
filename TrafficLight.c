#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

typedef enum {
    RED,
    GREEN
} Color;

// Structure for the traffic light simulation
typedef struct {
    Color currentColor;
    pthread_mutex_t colorLock;
    pthread_cond_t waitForGreen;
    pthread_mutex_t printLock;  // Mutex for synchronized printing
    int nextCarToPrint;         // To ensure cars print in order
    pthread_cond_t printOrder;  // Condition variable for printing order
} TrafficLightSimulation;

void* trafficLightThread(void* arg);
void* carThread(void* arg);

// the traffic light simulation
void initSimulation(TrafficLightSimulation* sim) {
    sim->currentColor = RED;
    sim->nextCarToPrint = 0;
    pthread_mutex_init(&sim->colorLock, NULL);
    pthread_cond_init(&sim->waitForGreen, NULL);
    pthread_mutex_init(&sim->printLock, NULL);
    pthread_cond_init(&sim->printOrder, NULL);
}

// Clean up traffic light simulation
void cleanupSimulation(TrafficLightSimulation* sim) {
    pthread_mutex_destroy(&sim->colorLock);
    pthread_cond_destroy(&sim->waitForGreen);
    pthread_mutex_destroy(&sim->printLock);
    pthread_cond_destroy(&sim->printOrder);
}

void* trafficLightThread(void* arg) {
    TrafficLightSimulation* sim = (TrafficLightSimulation*)arg;
    
    // Wait 3 seconds and change to green
    sleep(3);
    
    pthread_mutex_lock(&sim->colorLock);
    sim->currentColor = GREEN;
    pthread_mutex_unlock(&sim->colorLock);
    
    pthread_mutex_lock(&sim->printLock);
    printf("[traffic light] changed color to green\n");
    pthread_mutex_unlock(&sim->printLock);
    
    // Notify all waiting cars
    pthread_cond_broadcast(&sim->waitForGreen);
    
    // Wait 3 more seconds and change back to red
    sleep(3);
    
    pthread_mutex_lock(&sim->colorLock);
    sim->currentColor = RED;
    pthread_mutex_unlock(&sim->colorLock);
    
    pthread_mutex_lock(&sim->printLock);
    printf("[traffic light] changed color to red\n");
    pthread_mutex_unlock(&sim->printLock);
    
    return NULL;
}

// Car thread function
void* carThread(void* arg) {
    // Extract simulation and car ID from the argument
    TrafficLightSimulation* sim = ((void**)arg)[0];
    int carId = (int)(long)((void**)arg)[1];
    
    pthread_mutex_lock(&sim->colorLock);
    
    // If the light is red, print that the car is waiting and wait for green
    if (sim->currentColor == RED) {
        pthread_mutex_lock(&sim->printLock);
        printf("[car %d] arrives at red light, waiting\n", carId);
        pthread_mutex_unlock(&sim->printLock);
        
        while (sim->currentColor == RED) {
            pthread_cond_wait(&sim->waitForGreen, &sim->colorLock);
        }
    }
    
    pthread_mutex_unlock(&sim->colorLock);
    
    // Waiting for our turn to print the "continuing" message
    pthread_mutex_lock(&sim->printLock);
    while (carId != sim->nextCarToPrint) {
        pthread_cond_wait(&sim->printOrder, &sim->printLock);
    }
    // Car's turn
    printf("[car %d] sees green light, continuing\n", carId);
    sim->nextCarToPrint++;
    
    // Signal to wake up the next car waiting to print
    pthread_cond_broadcast(&sim->printOrder);
    pthread_mutex_unlock(&sim->printLock);
    
    return NULL;
}

// Run the traffic light simulation
void runSimulation() {
    TrafficLightSimulation sim;
    initSimulation(&sim);
    
    printf("[main thread] light starts off as red\n");
    
    // Create threads for cars and traffic light
    pthread_t lightThread;
    pthread_t carThreads[6];
    void* carArgs[6][2];
    
    // Start the traffic light thread
    pthread_create(&lightThread, NULL, trafficLightThread, &sim);
    
    // Start 6 car threads, one second apart
    for (int i = 0; i < 6; i++) {
        carArgs[i][0] = &sim;
        carArgs[i][1] = (void*)(long)i;
        pthread_create(&carThreads[i], NULL, carThread, carArgs[i]);
        
        // Wait a second before the next car arrives
        sleep(1);
    }
    
    // Wait for all threads to finish
    pthread_join(lightThread, NULL);
    for (int i = 0; i < 6; i++) {
        pthread_join(carThreads[i], NULL);
    }
    
    // Clean up
    cleanupSimulation(&sim);
}

int main() {
    runSimulation();
    return 0;
}