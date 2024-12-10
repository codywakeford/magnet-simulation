#include <SFML/Graphics.hpp>
#include "Config.hpp"
#include "InputManger.hpp"

class Renderer {
public:
    sf::Clock clock;

    void render(sf::RenderWindow& window) {
        await_frame();
        window.clear(sf::Color::Black);

        InputManager::renderAll(window, inputManager);
        Particle::renderAll();

        window.display();
    }

    void await_frame() {
        sf::Time deltaTime = clock.restart();
        if (deltaTime.asSeconds() < config.FRAME_TIME) {
            sf::sleep(sf::seconds(config.FRAME_TIME - deltaTime.asSeconds()));
        }
        clock.restart();
    }
};