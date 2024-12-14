#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include <string>
#include <iostream> 

const int FONT_SIZE = 20;

struct LiveText {
    sf::Text text;
    sf::Font font;
    std::function<std::string()> valueProvider;

    LiveText(const sf::Vector2f& position, std::function<std::string()> provider) {

        if (!font.loadFromFile("assets/font.ttf")) {
            throw std::runtime_error("Font loading failed");
        }

        text.setFont(font);
        text.setPosition(position);
        text.setFillColor(sf::Color::White);
        text.setCharacterSize(FONT_SIZE);
        text.setString("Default Text");

        valueProvider = provider;

        
    }

    void update() {
        if (valueProvider) {
            text.setString(valueProvider());
        } else {
            std::cerr << "valueProvider is null!" << std::endl;
        }
    }


    void render(sf::RenderWindow& window) {
        std::string textStr = text.getString().toAnsiString();

        // Ensure the font is set and the text string is not empty
        if (!textStr.empty() && text.getFont() != nullptr) {
            window.draw(text);
        }
    }



};
