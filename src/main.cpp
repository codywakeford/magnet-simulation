#include <SFML/Graphics.hpp>
#include "Renderer.hpp"
#include "WindowManager.hpp"
#include "Simulation.hpp"
#include "Config.hpp"
#include <iostream>

Config config;
WindowManager windowManger;
Renderer renderer;
Simulation simulation;

int main() {

    while (windowManger.window.isOpen()) {
        simulation.handle_inputs(windowManger.window);
        simulation.update(windowManger.window);
        renderer.render(windowManger.window);
    }

    return 0;
}