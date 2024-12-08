#ifndef INPUT_MANAGER_HPP
#define INPUT_MANAGER_HPP

#include <SFML/Graphics.hpp>
#include "Particle.hpp"
#include <iostream>
#include <string>
#include "WindowManager.hpp"
#include "collision_math.hpp"

class InputManager {
public:
    // Holds all of the particles under the cursor; 
    static std::vector<Particle> particles;

    sf::Vector2f mousePosF = {0.0f, 0.0f};
    sf::Vector2i mousePosI = {0, 0};

    int nParticles = 5;
    int radius = 10;
    sf::Vector2f particleVelocity = {0.0f, 0.0f};


public:

    void handle_inputs() {
        sf::Event event;

        while (windowManger.window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                windowManger.window.close();
            }

        }

        switch (event.type) {
            case sf::Event::MouseButtonPressed:
                Particle::add(particles);
            }
        
    }

    void update() {
        mousePosI = sf::Mouse::getPosition(windowManger.window);
        mousePosF = sf::Vector2f(mousePosI);

        while (particles.size() < nParticles) {
            Particle particle(mousePosF, radius, particleVelocity);
            particles.push_back(particle);
        };

        while (particles.size() > nParticles) {
            particles.pop_back();
        }

        for (Particle particle : particles) {
            std::cout << particle.position.x << particle.position.y << std::endl;
            particle.position = mousePosF; 
        }

    }

    

    void resolve_collisions() {  
        const float EPSILON = 1e-6f;

        for (size_t i = 0; i < particles.size(); i++) {
            for (size_t j = i + 1; j < particles.size(); j++) {
                Particle& particle1 = particles[i];
                Particle& particle2 = particles[j];

                resolve_collision(particle1, particle2);
            }
        }
    }

 
    static void renderAll(sf::RenderWindow& window) {
        for (Particle& particle : particles) {
            particle.render(window);
        }
    }

};

extern InputManager inputManager;

#endif