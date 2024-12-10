#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <SFML/Graphics.hpp>
#include "Solver.hpp" 
#include "InputManger.hpp"
#include "Grid.hpp"
struct Simulation {
    bool isPaused = false;

    void update(sf::RenderWindow& window) {
        if (isPaused) return;


        inputManager.update();
        grid.assignParticlesToGrid(Particle::particles);
        grid.checkCollisionsInGrid();
        Solver::calculate_gravity(Particle::particles);

        Particle::updateAll();
    }
};

extern Simulation simulation;

#endif