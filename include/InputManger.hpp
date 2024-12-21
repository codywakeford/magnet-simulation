#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include "Particle.hpp"
#include "Simulation.hpp"
#include "Config.hpp"

using namespace std;


struct InputManager {
    // Holds all of the particles under the cursor; 
    static vector<Particle> particles;

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
                        break;
                    }
                    else if (event.key.code == sf::Keyboard::Space) {
                        Simulation::isPaused = !Simulation::isPaused;
                        break;
                    }
                    else if (event.key.code == sf::Keyboard::Z) {
                        if (particleSpacing <= config.particleSize + 1.0f) return;
                        particleSpacing -= 1.0f;
                        break;
                    }
                    else if (event.key.code == sf::Keyboard::X) {
                        particleSpacing +=  1.0f;
                        break;
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
        if (event.mouseWheelScroll.delta > 0 && (nParticles * 2) <= 4048) {
            nParticles *= 2;
        }
        
        else if (event.mouseWheelScroll.delta < 0 && nParticles > 1) {
            nParticles *= 0.5f;
        }
    }

    static void update() {
        mousePosI = sf::Mouse::getPosition(window);
        mousePosF = sf::Vector2f(mousePosI);

        size_t particleCount = static_cast<std::vector<Particle>::size_type>(nParticles);

        // Ensure the particle vector has the correct size
        if (particles.size() != particleCount) {
            particles.resize(particleCount, Particle(mousePosF, config.particleSize - 1, {0.0f, 0.0f}));
        }

        while (particles.size() < particleCount) {
            Particle newParticle(mousePosF, config.particleSize - 1, {0.0f, 0.0f});
            particles.push_back(newParticle);
        }

        while (particles.size() > particleCount) {
            particles.pop_back();
        }

        float particleDiameter = config.particleSize * 1.1f; 
        float currentRadius = particleDiameter;          
        size_t currentParticle = 0;

        for (size_t i = 0; currentParticle < particles.size(); ++i) {
            size_t particlesInRing = static_cast<size_t>(floor(2 * M_PI * currentRadius / particleDiameter));

            for (size_t j = 0; j < particlesInRing && currentParticle < particles.size(); ++j) {
                float angle = static_cast<float>(j) / particlesInRing * 2.0f * M_PI;

                float offsetX = currentRadius * cos(angle);
                float offsetY = currentRadius * sin(angle);
                particles[currentParticle].position = {mousePosF.x + offsetX, mousePosF.y + offsetY};

                ++currentParticle;
            }

            currentRadius += particleDiameter;
        }
    }


    static void startDrag(sf::RenderWindow& window) {
        dragStart = mousePosF;
        isDragging = true;
    }

    static void endDrag(sf::RenderWindow& window) {
        sf::Vector2f velocity = dragStart - mousePosF;
        Particle::add(particles, (velocity * 0.1f));
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

vector<Particle> InputManager::particles;

sf::Vector2f InputManager::mousePosF = {0.0f, 0.0f};
sf::Vector2i InputManager::mousePosI = {0, 0};

int InputManager::nParticles = 1;
int InputManager::radius = config.particleSize;
sf::Vector2f InputManager::particleVelocity = {0.0f, 0.0f};

bool InputManager::isDragging = false;
sf::Vector2f InputManager::dragStart;
sf::Vector2f InputManager::dragEnd;

float InputManager::particleSpacing = 1.0f;


sf::Clock InputManager::frameTimer;
int InputManager::timeUs = 0;
int InputManager::frameCount = 0;
int InputManager::totalTimeUs = 0;