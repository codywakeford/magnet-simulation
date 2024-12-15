#include <SFML/Graphics.hpp>
#include "Config.hpp"
#include "InputManger.hpp"
#include "LiveText.hpp"

struct Renderer {
    static sf::Clock frameTimer;
    static int renderTimeUs;
    static int frameCount;
    static int totalRenderTimeUs;

    static void render() {
        frameTimer.restart();
        window.clear(sf::Color::Black);

        // quadTree.render();
        InputManager::renderAll();
        Particle::renderAll();
        TextManager::render();



        window.display();
        handleTimer();
    }

    static void handleTimer() {
        int currentFrameTime = frameTimer.getElapsedTime().asMicroseconds();
        totalRenderTimeUs += currentFrameTime;
        frameCount++;

        // Update every N frames (e.g., every 60 frames)
        if (frameCount == 60) {
            renderTimeUs = totalRenderTimeUs / frameCount;
            totalRenderTimeUs = 0; // Reset for next period
            frameCount = 0;
        }
    } 
};

int Renderer::renderTimeUs = 0;
int Renderer::frameCount = 0;
int Renderer::totalRenderTimeUs = 0;
sf::Clock Renderer::frameTimer;