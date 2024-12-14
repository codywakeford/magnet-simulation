#pragma once

#include "vector_math_utils.hpp"
#include "Particle.hpp"
#include "Config.hpp"
#include <vector>

struct Solver {
    static void resolve_collision(Particle& body1, Particle& body2) {
        const float EPSILON = 1e-6f;
        
        float dX = body1.position.x - body2.position.x;
        float dY = body1.position.y - body2.position.y;
        float normalMagnitude = std::sqrt(dX * dX + dY * dY);

        if (normalMagnitude < EPSILON) return;

        // get the normal force
        sf::Vector2f normalVector((dX / normalMagnitude), (dY / normalMagnitude));

        // r1 + r2
        float sumOfRadii = config.particleSize * 2;
        if (normalMagnitude + EPSILON >= sumOfRadii) return;

        // v2 - v1
        sf::Vector2f velocityDifference = body1.velocity - body2.velocity;

        // (2 x m2)
        // ---------
        //  m1 + m2

        // (v2 - v1) * (x2 - x1)
        float dotProductResult = dotProduct(velocityDifference, normalVector);
        if (dotProductResult > 0) return;

        float massScaler1 = (2.0f * body2.mass) / (body1.mass + body2.mass);
        float massScaler2 = (2.0f * body1.mass) / (body1.mass + body2.mass);

        sf::Vector2f impulse1 = normalVector * dotProductResult * massScaler1;
        sf::Vector2f impulse2 = normalVector * dotProductResult * massScaler2;

        body1.velocity -= scaleVec(impulse1, config.COLLISION_DAMPENING);
        body2.velocity += scaleVec(impulse2, config.COLLISION_DAMPENING);

        // Positional correction to prevent overlap
        float penetrationDepth = sumOfRadii - normalMagnitude;
        sf::Vector2f correctionVector = normalVector * (penetrationDepth / 2.0f);
        body1.position += correctionVector;
        body2.position -= correctionVector;
    }

    // Brute Force O(n*n)
    static void calculateGravity(std::vector<Particle>& particles) {
        for (Particle& particle1 : particles) {
            particle1.force = {0.0f, 0.0f};

            for (Particle& particle2 : particles) {
                if (&particle1 == &particle2) continue;

                particle1.force += calculateGravitationalForce(
                    particle1.mass, particle1.position, particle2.mass,
                    particle2.position);
            }
        }
    }

    /*
          G * m1 * m2
    F =  --------------
            r*r + e*e


    F = force
    G = Gravitational Constant
    m1 = Mass 1
    m2 = Mass 2
    r = Distance between bodies
    e = Softening factor - This reduces forces at small distances.

    */

    static sf::Vector2f calculateGravitationalForce(
        float mass1, sf::Vector2f position1, float mass2, sf::Vector2f position2) {
        
        sf::Vector2f force = {0.0f, 0.0f};          
        sf::Vector2f dPos = position2 - position1;

        // Pythagoras for distance.       
        float distanceSquared = (dPos.x * dPos.x) + (dPos.y * dPos.y);
        float distance = std::sqrt(distanceSquared + config.epsilon);

        float forceMagnitude = 
            (config.gravitational_constant * mass1 * mass2) / 
            (distanceSquared + config.gravitationalSoftening);

        // Direction of force
        sf::Vector2f normalizedDisplacement = {dPos.x / distance, dPos.y / distance};

        force.x = forceMagnitude * normalizedDisplacement.x;
        force.y = forceMagnitude * normalizedDisplacement.y;

        return force;
    }


};


