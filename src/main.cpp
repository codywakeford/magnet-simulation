#include <SFML/Graphics.hpp>
#include "Renderer.hpp"
#include "WindowManager.hpp"
#include "Simulation.hpp"
#include "Config.hpp"
#include <iostream>
#include "InputManger.hpp"

Config config;

InputManager inputManager;
std::vector<Particle> InputManager::particles;

WindowManager windowManger;
Renderer renderer;

Simulation simulation;
std::vector<Particle> Particle::particles;


int main() {

    while (windowManger.window.isOpen()) {
        inputManager.handle_inputs();
        simulation.update(windowManger.window);
        renderer.render(windowManger.window);
    }

    return 0;
}