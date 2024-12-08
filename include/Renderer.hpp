#include <SFML/Graphics.hpp>
#include "Config.hpp"

class Renderer {
public:
    sf::Clock clock;

    void render(sf::RenderWindow& window) {
        window.clear();

        await_frame();
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