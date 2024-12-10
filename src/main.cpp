#include <SFML/Graphics.hpp>
#include <iostream>
#include "Simulation.hpp"
#include "Solver.hpp"
#include "WindowManager.hpp"
#include "Renderer.hpp"
#include "InputManger.hpp"
#include "Config.hpp"
#include "Particle.hpp"
#include "Grid.hpp"     


Config config; // Stores Globals

Solver solver;
InputManager inputManager;

WindowManager windowManger;
Renderer renderer;
Grid grid;

Simulation simulation;


int main() {

    while (window.isOpen()) {
        inputManager.handle_inputs();
        simulation.update(window);
        renderer.render(window);
    }

    return 0;
}