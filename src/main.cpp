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
#include "TextManager.hpp"
#include "BarnesHut.cpp"
#include "Text.cpp"


Config config; // Stores Globals

int main() {
    initText();

    Particle::uniform_disc(1000);
    CollisionGrid::initialize();
    Node::initializeNodePool(10000);

    while (window.isOpen()) {
        Simulation::update(config.dt);
        InputManager::handle_inputs();
        Renderer::render();
        WindowManager::awaitFrame();
    }

    return 0;
}