#pragma once

// #include "Grid.hpp"
#include "vector_math_utils.hpp"
#include "Particle.hpp"
#include "Config.hpp"
#include <vector>

struct Solver {
    static void resolve_collision(Particle& body1, Particle& body2) {
        const float EPSILON = 1e-6f;
        const float COLLISION_DAMPENING = 0.1f;
        float dX = body1.position.x - body2.position.x;
        float dY = body1.position.y - body2.position.y;
        float normalMagnitude = std::sqrt(dX * dX + dY * dY);

        if (normalMagnitude < EPSILON) return;

        // get the normal force
        sf::Vector2f normalVector((dX / normalMagnitude), (dY / normalMagnitude));

        // r1 + r2
        float sumOfRadii = body1.shape.getRadius() + body2.shape.getRadius();
        if (normalMagnitude + EPSILON >= sumOfRadii) return;

        // v2 - v1
        sf::Vector2f velocityDifference = body1.velocity - body2.velocity;

        // (2 x m2)
        // ---------
        //  m1 + m2
        float massScaler = (2 * body2.mass) / (body1.mass + body2.mass);

        // (v2 - v1) * (x2 - x1)
        float dotProductResult = dotProduct(velocityDifference, normalVector);
        if (dotProductResult > 0) return;

        float massScaler1 = (2.0f * body2.mass) / (body1.mass + body2.mass);
        float massScaler2 = (2.0f * body1.mass) / (body1.mass + body2.mass);

        sf::Vector2f impulse1 = normalVector * dotProductResult * massScaler1;
        sf::Vector2f impulse2 = normalVector * dotProductResult * massScaler2;

        body1.velocity -= scaleVec(impulse1, COLLISION_DAMPENING);
        body2.velocity += scaleVec(impulse2, COLLISION_DAMPENING);

        // Positional correction to prevent overlap
        float penetrationDepth = sumOfRadii - normalMagnitude;
        sf::Vector2f correctionVector = normalVector * (penetrationDepth / 2.0f);
        body1.position += correctionVector;
        body2.position -= correctionVector;
    }

    static void calculate_gravity(std::vector<Particle>& particles) {
        for (Particle& particle1 : particles) {
            particle1.force = {0.0f, 0.0f};
            float forceX = 0.0f;
            float forceY = 0.0f;
            
            for (Particle& particle2 : particles) {
                if (&particle1 == &particle2) continue;
                
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
                    continue; 
                }

                float forceMagnitude = ( config.gravitational_constant * massProduct) / distanceSquared;

                forceX += forceMagnitude * (dX / distance);
                forceY += forceMagnitude * (dY / distance);

                particle1.force.x = forceX;
                particle1.force.y = forceY;
            }
        }
    }

    // static void calculate_gravitational_force(Particle& particle1, Particle& particle2) {
    // }

    // static void resolve_collisions() {
    //     grid.assignParticlesToGrid(Particle::particles);
    //     grid.checkCollisionsInGrid();
    // }
};


