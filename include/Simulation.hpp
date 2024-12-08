#include "Config.hpp"

// Handles inputs and updates game objects.
struct Simulation {
    sf::Clock clock;

    void handle_inputs(sf::RenderWindow& window) {
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
    }

    void update(sf::RenderWindow& window) {
        config.checkForUpdates();
    }


};