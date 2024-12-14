#pragma once

#include <array>
#include <vector>
#include <SFML/Graphics.hpp>

// create tree
// insert all particles into tree
// calculate gravity

struct QuadTree {
    std::vector<Node> nodes;

    void clear() {
        nodes.clear();

        Node node;
        nodes.push_back(node);
    };

    void subdivide(Node node) { 
        for (int i = 0; i < 4; i++) {
            Node newNode;
            nodes.push_back(newNode);
        }
    };

    void insert(sf::Vector2f position, float mass) {

    };
};

struct Quadrant {
    sf::Vector2f center = {0.0f, 0.0f};

    void quadrant(sf::Vector2f position) {
        
    }

};

struct Node {
    std::array<Node, 4> children;
    Quadrant quad;
    sf::Vector2f position = {0.0f, 0.0f};

    float mass = 0;

    // creates a new node
    void create() {};

    bool isBranch() { 
        return children.size() != 0;
    }

    bool isEmpty() {
        return mass == 0.0f;
    }

};