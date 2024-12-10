#include <SFML/Graphics.hpp>

#ifndef WINDOW_MANAGER_HPP
#define WINDOW_MANAGER_HPP

struct WindowManager {
    static sf::RenderWindow window;

    WindowManager() {
        window.setFramerateLimit(165);
    }
};

sf::RenderWindow WindowManager::window(sf::VideoMode(1920, 1080), "");
sf::RenderWindow& window = WindowManager::window;

extern WindowManager windowManger;

#endif