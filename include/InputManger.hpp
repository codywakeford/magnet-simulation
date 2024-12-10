#ifndef INPUT_MANAGER_HPP
#define INPUT_MANAGER_HPP

#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include "Particle.hpp"
#include "Config.hpp"

class InputManager {
public:
    // Holds all of the particles under the cursor; 
    static std::vector<Particle> particles;

    sf::Vector2f mousePosF = {0.0f, 0.0f};
    sf::Vector2i mousePosI = {0, 0};

    int nParticles = 100;
    int radius = config.particleSize;
    sf::Vector2f particleVelocity = {0.0f, 0.0f};
    
    bool isDragging = false;
    sf::Vector2f dragStart;
    sf::Vector2f dragEnd;

public:

    void handle_inputs() {
        sf::Event event;

        while (windowManger.window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                windowManger.window.close();
            }


            switch (event.type) {

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
    }

    void updateParticleCount(sf::Event event) {
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

    void update() {
        mousePosI = sf::Mouse::getPosition(windowManger.window);
        mousePosF = sf::Vector2f(mousePosI);

        const float spiralGrowthRate = 0.2f; 
        const float angleStep = 3.0f;
        float currentAngle = 0.0f;

        while (particles.size() < nParticles) {
            // Calculate the current distance from the center using the spiral formula
            float r = spiralGrowthRate * sqrt(currentAngle);

            // Convert polar to Cartesian coordinates
            float x = mousePosF.x + r * cos(currentAngle);
            float y = mousePosF.y + r * sin(currentAngle);

            Particle newParticle(mousePosF, 2, {0.0f,0.0f});
            newParticle.positionOffset = sf::Vector2f(x - mousePosF.x, y - mousePosF.y);
            particles.push_back(newParticle);

            std::cout << x << y << std::endl;
            // Increment the angle to "unwrap" the spiral
            currentAngle += angleStep;
        }
        // std::cout << particles.size() << std::endl;
        // Ensure the particles don't exceed the max number of particles
        while (particles.size() > nParticles) {
            particles.pop_back();
        }

        for (Particle& particle : particles) {

            particle.position = mousePosF += particle.positionOffset;
        }

        resolve_collisions();
    }
    void startDrag(sf::RenderWindow& window) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        dragStart = mousePosF;
        isDragging = true;
    }

    void endDrag(sf::RenderWindow& window) {
        // float vX = dragStart.x - dragEnd.x;
        // float vY = dragStart.y - dragEnd.y;

        sf::Vector2f velocity = dragStart - mousePosF;
        // sf::Vector2f velocity(vX, vY);

        Particle::add(particles, velocity);
        // createSatellite(window, velocity * VELOCITY_MULTIPLYER, gameObjects);

        isDragging = false;
    }


    
    static void resolve_collisions() {  
        const float EPSILON = 1e-6f;

        for (size_t i = 0; i < particles.size(); i++) {
            for (size_t j = i + 1; j < particles.size(); j++) {
                Particle& particle1 = particles[i];
                Particle& particle2 = particles[j];

                Solver::resolve_collision(particle1, particle2);
            }
        }
    }

 
    static void renderAll(sf::RenderWindow& window, InputManager& inputManager) {
        for (Particle& particle : particles) {
            particle.render(window);
        }

        if (inputManager.isDragging) {
            sf::VertexArray line(sf::Lines, 2);
            line[0].position = inputManager.dragStart;
            line[0].color = sf::Color::White;
            line[1].position = inputManager.dragEnd;
            line[1].color = sf::Color::White;
            window.draw(line);
        }   
    }

};

std::vector<Particle> InputManager::particles;
extern InputManager inputManager;

#endif