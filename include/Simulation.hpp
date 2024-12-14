#pragma once

#include <SFML/Graphics.hpp>
#include "Solver.hpp" 
#include "CollisionGrid.hpp"
#include "TextManager.hpp"

struct Simulation {
    static bool isPaused;
    static sf::Clock clock;
    static sf::Clock frameClock;

    static int counter;
    static int ms;
    static int frameCount;
    static int fps;

    static void update(float dt) {
        if (isPaused) {
            TextManager::update();
            return;
        }

        clock.restart();

        collisionGrid.assignParticlesToGrid(Particle::particles);
        collisionGrid.checkCollisionsInGrid();

        Solver::calculateGravity(Particle::particles);
        Particle::updateAll(dt);

        frameCount++;
        TextManager::update();
        
    }

    static void calculateFPS() {
        // Calculate FPS every second
        if (frameClock.getElapsedTime().asSeconds() >= 1.0f) {
            fps = frameCount;
            frameCount = 0;
            frameClock.restart();
        }
    }

    static int getFps() {
        if (counter % 5 != 0) {
            return fps;
        }

        calculateFPS();
        return fps;
    }

    static int getSimulationDuration() {
        if (isPaused) return 0;
        

        if (counter < 10) {
            counter++;
            return ms;
        }

        counter = 0;
        ms = static_cast<int>(clock.getElapsedTime().asMilliseconds());
        return ms;
    }
};

int Simulation::ms = 0;
int Simulation::counter = 0;
int Simulation::fps = 60;
int Simulation::frameCount = 0;
bool Simulation::isPaused = false;

sf::Clock Simulation::frameClock;
sf::Clock Simulation::clock;
