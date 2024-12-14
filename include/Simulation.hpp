#pragma once

#include <SFML/Graphics.hpp>
#include "Solver.hpp" 
#include "CollisionGrid.hpp"
#include "TextManager.hpp"

struct Simulation {
    static bool isPaused;


    static int fps;

    static sf::Clock frameTimer;
    static int simulationTimeUs;
    static int frameCount;
    static int totalSimulationTimeUs;



    static void update(float dt) {
        frameTimer.restart();

        if (isPaused) {
            TextManager::update();
            return;
        }

        collisionGrid.assignParticlesToGrid(Particle::particles);
        collisionGrid.checkCollisionsInGrid();

        Solver::calculateGravity(Particle::particles);
        Particle::updateAll(dt);

        // frameCount++;
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


int Simulation::simulationTimeUs = 0;
int Simulation::frameCount = 0;
int Simulation::totalSimulationTimeUs = 0;
sf::Clock Simulation::frameTimer;
