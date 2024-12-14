#pragma once

#include <SFML/Graphics.hpp>


struct WindowManager {
    static sf::RenderWindow window;

    WindowManager() {
        window.setFramerateLimit(60);
    }
};

sf::RenderWindow WindowManager::window(sf::VideoMode(1920, 1080), "");
sf::RenderWindow& window = WindowManager::window;

extern WindowManager windowManger;
