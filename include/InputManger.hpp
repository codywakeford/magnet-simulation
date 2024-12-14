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

    static sf::Clock clock;
    static int counter;
    static int ms;

    static void handle_inputs() {
        clock.restart();
        sf::Event event;

        while (windowManger.window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                windowManger.window.close();
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
                    updateParticleCount(event);
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
    }

    static int getInputHandlingDuration() {
        if (counter < 10) {
            counter++;
            return ms;
        }
        counter = 0;
        ms = static_cast<int>(clock.getElapsedTime().asMilliseconds());
        clock.restart();
        return ms;
    }

    static void updateParticleCount(sf::Event event) {
        if (event.type == sf::Event::MouseWheelScrolled) {
            if (event.mouseWheelScroll.delta > 0) {
                nParticles += 3;
            } else {
                if (nParticles > 3) {
                    nParticles -= 3;
                }
            }
        }
    }

    static void update() {
        mousePosI = sf::Mouse::getPosition(windowManger.window);
        mousePosF = sf::Vector2f(mousePosI);

        const float particleSpacing = config.particleSize + 1.25f;
        int gridSide = static_cast<int>(std::sqrt(nParticles));  

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
                float x = mousePosF.x + (col - gridSide / 2.0f) * particleSpacing;
                float y = mousePosF.y + (row - gridSide / 2.0f) * particleSpacing;

                particles[particleIndex].position = {x, y};
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
    
 
    static void renderAll(sf::RenderWindow& window) {
        for (Particle& particle : particles) {
            particle.render(window);
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

int InputManager::nParticles = 50;
int InputManager::radius = config.particleSize;
sf::Vector2f InputManager::particleVelocity = {0.0f, 0.0f};

bool InputManager::isDragging = false;
sf::Vector2f InputManager::dragStart;
sf::Vector2f InputManager::dragEnd;

sf::Clock InputManager::clock;
int InputManager::counter = 0;
int InputManager::ms = 0;