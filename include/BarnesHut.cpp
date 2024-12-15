#include "BarnesHut.hpp"
#include "Config.hpp"
#include "WindowManager.hpp"

sf::Vector2u windowSize = window.getSize(); 
// float windowWidth = static_cast<float>(windowSize.x);
// float windowHeight = static_cast<float>(windowSize.y);

QuadTree quadTree({0.0f, 0.0f}, static_cast<float>(windowSize.x));