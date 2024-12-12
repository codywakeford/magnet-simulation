#include <SFML/Graphics.hpp>
#include <iostream>
#include "Simulation.hpp"
#include "Solver.hpp"
#include "WindowManager.hpp"
#include "Renderer.hpp"
#include "InputManger.hpp"
#include "Config.hpp"
#include "Particle.hpp"
#include "CollisionGrid.hpp"     


Config config; // Stores Globals

Solver solver;
InputManager inputManager;

WindowManager windowManger;
Renderer renderer;

CollisionGrid collisionGrid;
GravityGrid gravityGrid;

Simulation simulation;


int main() {

    while (window.isOpen()) {
        inputManager.handle_inputs();
        simulation.update(window, config.dt);
        renderer.render(window);
    }

    return 0;
}