#ifndef CONFIG_HPP
#define CONFIG_HPP 

#include <string>

struct Config {
    const int FPS = 60;
    const float dt = 1.0f / FPS;
    const float gravitational_constant = 0.8f;
    const float gravitational_softening = 0.5f; // for use at small distances
    const int gravityGridCellSize = 500;

    // for collision dampening
    const float COLLISION_DAMPENING = 0.2f; 
    const float minDistance = 50.0f;
    const float dampingFactor = 0.2f;
    //////////////////////////////////
   
    const int windowHeight = 1080;
    const int windowWidth = 1920;

    const int particleSize = 2;
};

extern Config config;

#endif