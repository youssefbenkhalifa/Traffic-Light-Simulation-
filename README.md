# Traffic Light Simulation

A simple C program demonstrating thread synchronization using condition variables to simulate cars waiting at a traffic light.
### Inspired from the Stanford CS110 assignment on Traffic Lights
## Description

This project simulates a scenario where multiple cars arrive at a traffic light and need to wait for it to turn green before proceeding. It demonstrates fundamental concepts of multithreaded programming in C, including:

- Thread creation and management using POSIX threads (pthreads)
- Synchronization with mutexes and condition variables
- Concurrent access to shared resources
- Ordered execution of concurrent operations

The simulation follows a simple pattern:

1. The traffic light starts red
2. Cars arrive at 1-second intervals
3. The light turns green after 3 seconds
4. Cars proceed through the intersection in order
5. The light turns red again after another 3 seconds

## Features

- Proper synchronization to prevent race conditions
- Controlled access to shared state (traffic light color)
- Ordered output to demonstrate deterministic behavior
- Clean implementation of condition variables
