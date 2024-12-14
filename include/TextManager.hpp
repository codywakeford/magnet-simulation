#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include <iostream>
#include <string>
#include "LiveText.hpp"

struct TextManager {
    static std::vector<LiveText> textObjects;

    static void render() {
        if (textObjects.size() == 0) {
            std::cerr << "No LiveText objects to render!" << std::endl;
            return;
        }
 
        for (LiveText& textObject : textObjects) {
            textObject.render(window);
        }
    };

    static void update() {
        // std::cout << textObjects.size() << std::endl;
        for (LiveText& textObject : textObjects) {
            textObject.update();
        }
    };
    
    static void create(std::function<std::string()> valueProvider) {
        float yPos = textObjects.size() + 1 * 30.0f + 10.0f;
        LiveText newText({ 10.0f, 40.0f }, valueProvider);
        textObjects.push_back(newText);
    }
    
};


std::vector<LiveText> TextManager::textObjects;