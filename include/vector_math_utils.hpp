#include <cmath>
#include <SFML/Graphics.hpp>

#ifndef VECTOR_MATH_UTILS_HPP
#define VECTOR_MATH_UTILS_HPP

sf::Vector2f& squareVec(sf::Vector2f& vector) {
    vector.x *= vector.x;
    vector.y *= vector.y;

    return vector;
}

sf::Vector2f& scaleVec(sf::Vector2f& vector, float scaler) {
    vector.x *= scaler;
    vector.y *= scaler;

    return vector;
}

sf::Vector2f divideVec(const sf::Vector2f vector1, const sf::Vector2f vector2) {
    return sf::Vector2f(vector1.x / vector2.x, vector1.y / vector2.y);
}

sf::Vector2f multiplyVec(const sf::Vector2f vector1, const sf::Vector2f vector2) {
    return sf::Vector2f(vector1.x * vector2.x, vector1.y * vector2.y);
}

float dotProduct(const sf::Vector2f& vector1, const sf::Vector2f& vector2) {
    return vector1.x * vector2.x + vector1.y * vector2.y;
}

#endif