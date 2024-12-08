#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include "Config.hpp"
#include "WindowManager.hpp"

struct Particle {
    static std::vector<Particle> particles;

    sf::CircleShape shape;
    sf::Vector2f position = {0.0f, 0.0f};
    sf::Vector2f velocity = {0.0f, 0.0f};
    sf::Vector2f force = {0.0f, 0.0f};

    float radius;
    float mass;

    Particle(sf::Vector2f position, float radius, sf::Vector2f velocity) 
        : shape(radius), position(position), velocity(velocity) {

        shape.setRadius(radius);
        shape.setPosition(position);
        shape.setFillColor(sf::Color::Green);

        mass = 3.14159f * radius * radius;
    }
    
    void update() {
        sf::Vector2f acceleration = force / mass;
            
        velocity += acceleration;
        position += velocity;
    }

    static void renderAll() {
        for (Particle particle : particles) {
            particle.shape.setPosition(particle.position);
            windowManger.window.draw(particle.shape);
        }
    }

    void render(sf::RenderWindow& window) {
        shape.setPosition(position);
        shape.setFillColor(sf::Color::White);
        windowManger.window.draw(shape);
    }

    static void add(std::vector<Particle> particlesToAdd) {
        particles.insert(particles.end(), particlesToAdd.begin(), particlesToAdd.end());
    }

    static void calculate_gravity() {

    }

    void calculate_gravitational_force(Particle& particle1, Particle& particle2) {
        float forceX = 0.0f;
        float forceY = 0.0f;

        if (&particle1 == &particle2) return;
        
        int massProduct = particle1.mass * particle2.mass;

        float dX = particle2.position.x - particle1.position.x;
        float dY = particle2.position.y - particle1.position.y;
        
        float distanceSquared = dX * dX + dY * dY;
        float distance = std::sqrt(distanceSquared);


        if (distance < config.minDistance) {
            float forceMagnitude = (config.gravitational_constant * particle1.mass * particle2.mass) / (distanceSquared + config.minDistance);
            dX *= config.dampingFactor;
            dY *= config.dampingFactor;
            particle1.force.x += forceMagnitude * (dX / distance);
            particle1.force.y += forceMagnitude * (dY / distance);
            return; 
        }

        float forceMagnitude = ( config.gravitational_constant * massProduct) / distanceSquared;

        forceX += forceMagnitude * (dX / distance);
        forceY += forceMagnitude * (dY / distance);
    }    

};

#endif