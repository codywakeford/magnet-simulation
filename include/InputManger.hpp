#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include "Particle.hpp"
#include "Simulation.hpp"
#include "Config.hpp"

struct InputManager {
    // Holds all of the particles under the cursor; 
    static std::vector<Particle> particles;

    static sf::Vector2f mousePosF;
    static sf::Vector2i mousePosI;

    static int nParticles;
    static int radius;
    static sf::Vector2f particleVelocity;
    
    static bool isDragging;
    static sf::Vector2f dragStart;
    static sf::Vector2f dragEnd;

    static sf::Clock frameTimer;
    static int timeUs;
    static int frameCount;
    static int totalTimeUs;

    static float particleSpacing;

    static void handle_inputs() {
        frameTimer.restart();

        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }


            switch (event.type) {

                case sf::Event::KeyPressed:
                    if (event.key.code == sf::Keyboard::R) {
                        Particle::particles.clear();
                    }
                    else if (event.key.code == sf::Keyboard::Space) {
                        Simulation::isPaused = !Simulation::isPaused;
                    }

                case sf::Event::MouseWheelScrolled:
                    if (event.key.code == sf::Keyboard::LControl) {
                        changeParticleSpacing(event);
                        break;
                    }

                    else {
                        updateParticleCount(event);
                    }
                    break;


                // Click and drag to add velocity to new object. // 
                case sf::Event::MouseButtonPressed:
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        startDrag(window);
                    }
                    break;

                // Create satellite on release //
                case sf::Event::MouseButtonReleased:
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        endDrag(window);
                        
                    }

                    break;
                default:
                    break;
                
            }
        }

        update();
        handleTimer();
    }

    static void handleTimer() {
        int currentFrameTime = frameTimer.getElapsedTime().asMicroseconds();
        totalTimeUs += currentFrameTime;
        frameCount++;

        // Update every N frames (e.g., every 60 frames)
        if (frameCount == 60) {
            timeUs = totalTimeUs / frameCount;
            totalTimeUs = 0; // Reset for next period
            frameCount = 0;
        }
    } 

    static void updateParticleCount(sf::Event& event) {
        

        if (event.mouseWheelScroll.delta > 0 && nParticles < 1000) {
            nParticles += 20;
        } 
        
        else if (nParticles > 20) {
            nParticles -= 20;
            
        }
    }

    static void changeParticleSpacing(sf::Event& event) {
        if (event.mouseWheelScroll.delta > 0) {
            particleSpacing += 0.5f;
        } 

        else if (particleSpacing > 1.0f) {
            particleSpacing -= 0.5f;
        }
    }

    static void update() {
        mousePosI = sf::Mouse::getPosition(window);
        mousePosF = sf::Vector2f(mousePosI);

        int gridSide = static_cast<int>(std::sqrt(nParticles));
        size_t particleCount = static_cast<std::vector<Particle>::size_type>(nParticles);

        if (particles.size() != particleCount) {
            particles.resize(particleCount, Particle({0.0f, 0.0f}, config.particleSize - 1, {0.0f, 0.0f}));
        }

        while (particles.size() < static_cast<std::vector<Particle>::size_type>(nParticles)) {
            Particle newParticle({0.0f, 0.0f}, config.particleSize - 1, {0.0f, 0.0f});
            particles.push_back(newParticle);
        }
        while (particles.size() > static_cast<std::vector<Particle>::size_type>(nParticles)) {
            particles.pop_back();
        }

        int particleIndex = 0;
        for (int row = 0; row < gridSide && particleIndex < nParticles; ++row) {
            for (int col = 0; col < gridSide && particleIndex < nParticles; ++col) {
                // Avoid recomputing the same values every iteration
                float offsetX = (col - gridSide / 2.0f) * particleSpacing;
                float offsetY = (row - gridSide / 2.0f) * particleSpacing;

                particles[particleIndex].position = {mousePosF.x + offsetX, mousePosF.y + offsetY};
                ++particleIndex;
            }
        }

    }


    static void startDrag(sf::RenderWindow& window) {
        dragStart = mousePosF;
        isDragging = true;
    }

    static void endDrag(sf::RenderWindow& window) {
        sf::Vector2f velocity = dragStart - mousePosF;
        std::cout << velocity.x << velocity.y << std::endl;
        Particle::add(particles, (velocity * 0.2f));
        InputManager::particles.clear();

        isDragging = false;
    }
    
 
    static void renderAll() {
        for (Particle& particle : particles) {
            particle.render();
        }

        if (InputManager::isDragging) {
            sf::VertexArray line(sf::Lines, 2);
            line[0].position = InputManager::dragStart;
            line[0].color = sf::Color::White;
            line[1].position = InputManager::mousePosF;
            line[1].color = sf::Color::White;
            window.draw(line);
        }   
    }

};

std::vector<Particle> InputManager::particles;

sf::Vector2f InputManager::mousePosF = {0.0f, 0.0f};
sf::Vector2i InputManager::mousePosI = {0, 0};

int InputManager::nParticles = 20;
int InputManager::radius = config.particleSize;
sf::Vector2f InputManager::particleVelocity = {0.0f, 0.0f};

bool InputManager::isDragging = false;
sf::Vector2f InputManager::dragStart;
sf::Vector2f InputManager::dragEnd;

float InputManager::particleSpacing = 4.0f;


sf::Clock InputManager::frameTimer;
int InputManager::timeUs = 0;
int InputManager::frameCount = 0;
int InputManager::totalTimeUs = 0;