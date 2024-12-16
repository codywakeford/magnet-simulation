#pragma once

#include <SFML/Graphics.hpp>
#include "Solver.hpp" 
#include "CollisionGrid.hpp"
#include "TextManager.hpp"
#include "BarnesHut.hpp"

struct Simulation {
    static bool isPaused;


    static int fps;

    static sf::Clock frameTimer;
    
    static sf::Clock collisionTimer;
    static int collisionTime;

    static sf::Clock gravityTimer;
    static int gravityTime;

    static int simulationTimeUs;
    static int frameCount;
    static int totalSimulationTimeUs;


    static void update(float dt) {
        frameTimer.restart();
        

        if (isPaused) {
            TextManager::update();
            return;
        }

        gravityTimer.restart();
        quadTree.update();
        if (frameCount == 1) gravityTime = gravityTimer.getElapsedTime().asMicroseconds();

        collisionTimer.restart();
        CollisionGrid::update(Particle::particles);
        if (frameCount == 1) collisionTime = collisionTimer.getElapsedTime().asMicroseconds();

        Particle::updateAll(dt);
        TextManager::update();

        handleTimer();
    }

    static void handleTimer() {
        int currentFrameTime = frameTimer.getElapsedTime().asMicroseconds();
        totalSimulationTimeUs += currentFrameTime;
        frameCount++;

        // Update every N frames (e.g., every 60 frames)
        if (frameCount == 60) {
            simulationTimeUs = totalSimulationTimeUs / frameCount;
            totalSimulationTimeUs = 0; // Reset for next period
            frameCount = 0;
        }
    } 

};

int Simulation::fps = 60;
bool Simulation::isPaused = false;

sf::Clock Simulation::frameTimer;
int Simulation::simulationTimeUs = 0;
int Simulation::frameCount = 0;
int Simulation::totalSimulationTimeUs = 0;

sf::Clock Simulation::collisionTimer;
int Simulation::collisionTime = 0;

sf::Clock Simulation::gravityTimer;
int Simulation::gravityTime = 0;
