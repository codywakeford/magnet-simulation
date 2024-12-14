#include <SFML/Graphics.hpp>
#include "Config.hpp"
#include "InputManger.hpp"
#include "LiveText.hpp"

struct Renderer {
    static sf::Clock clock;
    
    static int counter;
    static int ms;

    static void render() {
        awaitFrame();
        window.clear(sf::Color::Black);

        InputManager::renderAll(window);
        Particle::renderAll();
        TextManager::render(window);

        window.display();
    }

    static int getRenderDuration() {
        if (counter < 10) {
            counter++;
            return ms;
        }
        counter = 0;
        ms = static_cast<int>(clock.getElapsedTime().asMilliseconds());
        return ms;
    }

    static void awaitFrame() {
        sf::Time deltaTime = clock.restart();
        if (deltaTime.asSeconds() < config.dt) {
            sf::sleep(sf::seconds(config.dt - deltaTime.asSeconds()));
        }
        clock.restart();
    }
};

int Renderer::counter = 0;
int Renderer::ms = 0;

sf::Clock Renderer::clock;