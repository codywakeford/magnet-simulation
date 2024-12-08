#include <SFML/Graphics.hpp>

#ifndef WINDOW_MANAGER_HPP
#define WINDOW_MANAGER_HPP

class WindowManager {
public:
    sf::RenderWindow window;

    WindowManager() : window(sf::VideoMode(1920, 1080), "Maxwell") {
        window.setFramerateLimit(165);
    }
};
extern WindowManager windowManger;

#endif