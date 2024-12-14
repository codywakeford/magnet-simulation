#pragma once 

#include <string>

struct Config {
    float epsilon = 1e-6f;

    const int FPS = 60;
    const float dt = 1.0f / FPS;

    // Gravity //
    const float gravitational_constant = 250.0f;
    const float gravitationalSoftening = 100000.0f; // for use at small distances
    /////////////

    // Collision //
    const float COLLISION_DAMPENING = 0.2f;
    ///////////////
   
    const int windowHeight = 1080;
    const int windowWidth = 1920;

    const int particleSize = 2; // particles are rendered at size - 1 to show gaps.
};

extern Config config;
