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


Config config; // Stores Globals
CollisionGrid collisionGrid;

int main() {

    LiveText liveText({10.0f, 10.0f}, []() -> std::string {
        return "Particles: " + std::to_string(Particle::particles.size());
    });

    LiveText simulationTime({10.0f, 40.0f}, []() -> std::string {
        if (Simulation::simulationTimeUs > 1000) {
            return "Simulation Time: " + std::to_string(Simulation::simulationTimeUs / 1000) + "ms";
        }

        else {
            return "Simulation Time: " + std::to_string(Simulation::simulationTimeUs) + "us";
        }
    });

    LiveText renderingTime({10.0f, 70.0f}, [&]() -> std::string {
        return "Rendering Time: " + std::to_string(Renderer::renderTimeUs) + "us";
    });

    LiveText inputHandlingTime({10.0f, 100.0f}, [&]() -> std::string {
        return "Input Handling Time: " + std::to_string(InputManager::timeUs) + "us";
    });

    LiveText fps({10.0f, 130.0f}, []() -> std::string {
        return "FPS: " + std::to_string(Simulation::fps);
    });

    LiveText paused({900.0f, 10.0f}, []() -> std::string {
        if (Simulation::isPaused) return "PAUSED";
        return "";
    });

    TextManager::textObjects.push_back(liveText);
    TextManager::textObjects.push_back(simulationTime);
    TextManager::textObjects.push_back(renderingTime);
    TextManager::textObjects.push_back(fps);
    TextManager::textObjects.push_back(inputHandlingTime);
    TextManager::textObjects.push_back(paused);

    Particle::uniform_disc(2500);


    while (window.isOpen()) {
        Simulation::update(config.dt);

        InputManager::handle_inputs();

        Renderer::render();

        WindowManager::awaitFrame();
    }

    return 0;
}