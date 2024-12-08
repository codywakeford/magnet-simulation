#include <SFML/Graphics.hpp>

class WindowManager {
public:
    sf::RenderWindow window;

    WindowManager() : window(sf::VideoMode(1920, 1080), "Maxwell") {
        window.setFramerateLimit(165);
    }
};