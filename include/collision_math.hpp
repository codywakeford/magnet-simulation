#include <cmath>
#include <SFML/Graphics.hpp>
#include "Particle.hpp"
#include "vector_math_utils.hpp"

#ifndef COLLISION_MATH_HPP
#define COLLISION_MATH_HPP

/**

v - velocity
m - mass
x - normal
v1f - velocity final

Equation: 

            2 * m2      (v2 - v1) * (x2 - x1)
v1f = v1 + -------- *  --------------------- * (x2 - x1)
            m1 + m2         (x1 - x2)^2

*/

void resolve_collision(Particle& body1, Particle& body2) {
    const float EPSILON = 1e-6f;
    const float COLLISION_DAMPENING = 0.9f;
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

#endif
