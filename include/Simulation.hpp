#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <SFML/Graphics.hpp>
#include "Solver.hpp" 
#include "InputManger.hpp"
#include "CollisionGrid.hpp"
#include "GravityGrid.hpp"

struct Simulation {
    bool isPaused = false;

    void update(sf::RenderWindow& window, float dt) {


        inputManager.update();
        collisionGrid.assignParticlesToGrid(Particle::particles);
        collisionGrid.checkCollisionsInGrid();

        gravityGrid.assignParticlesToGrid(Particle::particles);
        gravityGrid.calculateGravity();

        // Solver::calculate_gravity(Particle::particles);

        Particle::updateAll(dt);
    }
};

extern Simulation simulation;

#endif