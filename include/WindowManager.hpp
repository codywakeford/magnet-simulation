#pragma once

#include "Config.hpp"
#include <SFML/Graphics.hpp>

sf::ContextSettings settings;

struct WindowManager {
    static sf::RenderWindow window;
    static sf::Clock frameClock;

    WindowManager() {
        window.setFramerateLimit(60);
        settings.antialiasingLevel = 0;
        window.setVerticalSyncEnabled(false);

    }

    static void awaitFrame() {
        sf::Time deltaTime = frameClock.restart();
        if (deltaTime.asSeconds() < config.dt) {
            sf::sleep(sf::seconds(config.dt - deltaTime.asSeconds()));
        }
    }
};

sf::RenderWindow WindowManager::window(sf::VideoMode(1920, 1080), "");
sf::RenderWindow& window = WindowManager::window;
sf::Clock WindowManager::frameClock;

