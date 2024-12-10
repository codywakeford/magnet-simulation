#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include "WindowManager.hpp"
#include "Config.hpp"

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
    
    void update() {
        sf::Vector2f acceleration = force / mass; // a = F / m
            
        velocity += acceleration;
        position += velocity;
    }

    static void updateAll() {
        for (Particle& particle : particles) {
            particle.update();
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

