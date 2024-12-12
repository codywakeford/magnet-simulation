#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include "WindowManager.hpp"
#include "Config.hpp"
#include "iostream"

// An optimisation for collision detection.


struct Particle {
    static std::vector<Particle> particles;

    sf::CircleShape shape;
    sf::Vector2f position = {0.0f, 0.0f};
    sf::Vector2f positionOffset = {0.0f, 0.0f};
    sf::Vector2f velocity = {0.0f, 0.0f};
    sf::Vector2f force = {0.0f, 0.0f};

    float radius;
    float mass;

    Particle(sf::Vector2f position, float radius, sf::Vector2f velocity) 
        : position(position), velocity(velocity) {

        shape.setRadius(radius);
        shape.setPosition(position);
        shape.setFillColor(sf::Color::Green);

        mass = 3.14159f * radius * radius;
    }
    
    void update(float dt) {
        sf::Vector2f acceleration = force / mass; // a = F / m
            
        velocity += acceleration * dt;
        position += velocity;

        std::cout << particles.size() << std::endl;
    }

    static bool isOutOfBounds(Particle& particle) {
        if (
            particle.position.x > config.windowWidth || 
            particle.position.x < 0 ||
            particle.position.y > config.windowHeight ||
            particle.position.y < 0
        ) {
            return true;
        }

        return false;
    }

    // Removes out of bound particles and moves the particles
    // based on the force applied.
    static void updateAll(float dt) {
        for (auto it = particles.begin(); it != particles.end();) {
            Particle& particle = *it; 

            if (Particle::isOutOfBounds(particle)) {
                it = particles.erase(it);
                continue;
            } else {
                particle.update(dt);
                it++;
            }
        }
    }

    static void renderAll() {
        for (Particle& particle : particles) {
            particle.shape.setPosition(particle.position);
            WindowManager::window.draw(particle.shape);
        }
    }

    void render(sf::RenderWindow& window) {
        shape.setPosition(position);
        shape.setFillColor(sf::Color::White);
        WindowManager::window.draw(shape);
    }

    static void add(std::vector<Particle> particlesToAdd, sf::Vector2f velocity) {
        for (Particle& particle : particlesToAdd) {
            particle.velocity += velocity;
        }
        
        particles.insert(particles.end(), particlesToAdd.begin(), particlesToAdd.end());
    }


};

std::vector<Particle> Particle::particles;

#endif

