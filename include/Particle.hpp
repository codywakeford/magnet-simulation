#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "WindowManager.hpp"
#include "Config.hpp"
#include "iostream"
#include <cmath>
#include <random>
#include <algorithm>

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

    // Updates particle position based on applied force.
    void update(float dt) {
        sf::Vector2f acceleration = force / mass; // a = F / m
            
        velocity += acceleration * dt;
        position += velocity;

        // std::cout << particles.size() << std::endl;
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

            if (isOutOfBounds(particle)) {
                it = particles.erase(it);
                continue;
            } else {
                particle.update(dt);
                it++;
            }
        }
    }

    static void renderAll() {
        sf::VertexArray particlesArray(sf::Points, particles.size());

        for (size_t i = 0; i < particles.size(); ++i) {
            Particle& particle = particles[i];

            particlesArray[i].position = particle.position;
            particlesArray[i].color = sf::Color::Green;
        }

        window.draw(particlesArray);
    }

    void render() {
        shape.setPosition(position);  

        window.draw(shape); 
    }


    static void add(std::vector<Particle> particlesToAdd, sf::Vector2f velocity) {
        for (Particle& particle : particlesToAdd) {
            particle.velocity += velocity;
        }
        
        particles.insert(particles.end(), particlesToAdd.begin(), particlesToAdd.end());
    }


    static void uniform_disc(int n) {
        std::mt19937 rng(0); // Seed for deterministic results
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * M_PI);

        float inner_radius = 5.0f;
        float outer_radius = sqrt(static_cast<float>(n)) * 10.0f;

        std::vector<Particle> particleList;
        float centralMass = 5.0f; // Mass of the central particle
        
        // Create the central particle (massive center object)
        Particle center(sf::Vector2f(config.windowWidth / 2, config.windowHeight / 2),
                        inner_radius, sf::Vector2f(0.0f, 0.0f));
        center.mass = centralMass;
        particleList.push_back(center);
        
        // Generate the remaining particles
        while (particleList.size() < n) {
            float angle = angleDist(rng);
            float radius = dist(rng) * (outer_radius - inner_radius) + inner_radius;

            // Calculate position
            sf::Vector2f pos = sf::Vector2f(std::cos(angle), std::sin(angle)) * radius;
            pos += sf::Vector2f(config.windowWidth / 2, config.windowHeight / 2);

            // Calculate orbital velocity
            float r = radius;
            float v = std::sqrt((config.gravitational_constant * centralMass) / r); // Orbital velocity

            // Velocity is perpendicular to the radial vector
            sf::Vector2f vel = sf::Vector2f(std::sin(angle), -std::cos(angle)) * (v + 0.80f) ;

            // Create the particle
            float mass = 1.0f; // Mass of individual particle
            float particleRadius = cbrt(mass); // Adjust radius based on mass
            Particle p(pos, particleRadius, vel);
            p.mass = mass;

            particleList.push_back(p);
        }

        // Add particles to the main list
        for (Particle& particle : particleList) {
            particles.push_back(particle);
        }
    }

};

std::vector<Particle> Particle::particles;


