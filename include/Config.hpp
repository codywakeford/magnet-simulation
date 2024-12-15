#pragma once 

#include <string>

struct Config {
    // Epsilon value, typically small and constant
    constexpr static float epsilon = 1e-6f;

    // FPS and delta time (dt)
    constexpr static int FPS = 60;
    constexpr static float dt = 1.0f / FPS;

    // Gravity
    // Gravity constant can be made constexpr if it remains constant throughout the program
    constexpr static float gravitational_constant = 10.0f;
    constexpr static float gravitationalSoftening = 10000000.0f;

    // Collision settings
    constexpr static float COLLISION_DAMPENING = 0.1f;

    // Window dimensions
    constexpr static int windowHeight = 1200;
    constexpr static int windowWidth = 1200;

    // Particle size
    constexpr static int particleSize = 2;


    // Barnes Hut
    constexpr static float theta = 0.50f; // gravity approximation threshold
};

extern Config config;
