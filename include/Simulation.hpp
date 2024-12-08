#include "Config.hpp"
#include "InputManger.hpp"

// Handles inputs and updates game objects.
struct Simulation {

    void update(sf::RenderWindow& window) {
        inputManager.update();
    }
};