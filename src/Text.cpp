#include <SFML/Graphics.hpp>
#include <iostream>

#include "Simulation.hpp"
#include "Renderer.hpp"
#include "InputManger.hpp"
#include "Particle.hpp"
#include "TextManager.hpp"



LiveText liveText({10.0f, 10.0f}, []() -> std::string {
    return "Particles: " + std::to_string(Particle::particles.size());
});


LiveText renderingTime({10.0f, 40.0f}, []() -> std::string {
    return "Rendering Time: " + std::to_string(Renderer::renderTimeUs) + "us";
});

LiveText inputHandlingTime({10.0f, 70.0f}, []() -> std::string {
    return "Input Handling Time: " + std::to_string(InputManager::timeUs) + "us";
});

LiveText fps({10.0f, 100.0f}, []() -> std::string {
    return "FPS: " + std::to_string(Simulation::fps);
});

LiveText paused({900.0f, 10.0f}, []() -> std::string {
    if (Simulation::isPaused) return "PAUSED";
    return "";
});

LiveText simulationTime({10.0f, 220.0f}, []() -> std::string {
    if (Simulation::simulationTimeUs > 1000) {
        return "Simulation Time: " + std::to_string(Simulation::simulationTimeUs / 1000) + "ms";
    }

    else {
        return "Simulation Time: " + std::to_string(Simulation::simulationTimeUs) + "us";
    }
});

LiveText collisionTime({10.0f, 160.0f}, []() -> std::string {
    if (Simulation::collisionTime > 1000) {
        return "Collision Time: " + std::to_string(Simulation::collisionTime / 1000) + "ms";
    }

    else {
        return "Collision Time: " + std::to_string(Simulation::collisionTime) + "us";
    }
});

LiveText gravityTime({10.0f, 190.0f}, []() -> std::string {
    if (Simulation::gravityTime > 1000) {
        return "Gravity Time: " + std::to_string(Simulation::gravityTime / 1000) + "ms";
    }

    else {
        return "Gravity Time: " + std::to_string(Simulation::gravityTime) + "us";
    }
});

void initText() {
    TextManager::textObjects.push_back(liveText);
    TextManager::textObjects.push_back(renderingTime);
    TextManager::textObjects.push_back(fps);
    TextManager::textObjects.push_back(inputHandlingTime);
    TextManager::textObjects.push_back(paused);

    TextManager::textObjects.push_back(simulationTime);
    TextManager::textObjects.push_back(gravityTime);
    TextManager::textObjects.push_back(collisionTime);
}