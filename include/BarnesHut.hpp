#pragma once

#include <SFML/Graphics.hpp>
#include <array>
#include "Particle.hpp"
#include "WindowManager.hpp"
#include "Solver.hpp"

#include <thread>
#include <vector>
#include <functional> // For std::bind

// Node struct
struct Node {
    sf::Vector2f position;     // Top-left corner of the node
    sf::Vector2f centerOfMass; // Center of mass for the node
    float size;                // Size of the node (width and height)
    float totalMass = 0.0f;    // Total mass in the node
    bool isLeaf = true;        // Leaf status
    Particle* particle = nullptr; // Pointer to a particle (if any)
    std::array<Node*, 4> children = {nullptr, nullptr, nullptr, nullptr};

    // Rendering //
    sf::RectangleShape rectangle;

    Node(const sf::Vector2f& position, float size)
        : position(position), size(size), centerOfMass({0.0f, 0.0f}) {
            
            rectangle.setPosition(position);
            rectangle.setOutlineThickness(0.1f);
            rectangle.setFillColor(sf::Color::Transparent);
            rectangle.setSize({size, size});
        }

    ~Node() {
        for (auto child : children) {
            delete child;
        }
    }
    
    void render() {
        if (!isLeaf) {
            for (auto& child : children) {
                child->render();
            }
            return;
        }

        rectangle.setSize({size, size});
        window.draw(rectangle);
    }
    
    void clear() {
        for (auto& child : children) {
            if (child) {
                child->clear();
                delete child;
                child = nullptr;
            }
        }
        isLeaf = true;
        particle = nullptr;
        totalMass = 0.0f;
        centerOfMass = {0.0f, 0.0f};
    }

    // Check if a particle is within the node boundaries
    bool contains(const Particle& particle) const {
        return particle.position.x >= position.x &&
               particle.position.x < position.x + size &&
               particle.position.y >= position.y &&
               particle.position.y < position.y + size;
    }

    // Subdivide the node into four quadrants
    void subdivide() {
        float halfSize = size / 2.0f;
        children[0] = new Node(position, halfSize);                            // NW
        children[1] = new Node({position.x + halfSize, position.y}, halfSize); // NE
        children[2] = new Node({position.x + halfSize, position.y + halfSize}, halfSize); // SE
        children[3] = new Node({position.x, position.y + halfSize}, halfSize); // SW
        isLeaf = false;
    }


    // Insert a particle into the tree
    void insert(Particle& particle) {
        if (!contains(particle)) {
            return; // Particle is not within this node
        }

        if (isLeaf) {
            if (!this->particle) {
                // Node is empty; store the particle here
                this->particle = &particle;
                centerOfMass = particle.position;
                totalMass = particle.mass;
                return;
            }

            // Node already has a particle; subdivide and redistribute
            subdivide();

            // Reinsert the existing particle
            Particle* existingParticle = this->particle;
            this->particle = nullptr;
            for (auto& child : children) {
                child->insert(*existingParticle);
            }

            // Insert the new particle
            for (auto& child : children) {
                child->insert(particle);
            }
        } else {
            // Pass the particle to the appropriate child
            for (auto& child : children) {
                child->insert(particle);
            }
        }

        // Update the center of mass and total mass
        totalMass += particle.mass;
        centerOfMass.x = (centerOfMass.x * (totalMass - particle.mass) + particle.position.x * particle.mass) / totalMass;
        centerOfMass.y = (centerOfMass.y * (totalMass - particle.mass) + particle.position.y * particle.mass) / totalMass;
    }

    // Compute center of mass using DFS
    void computeMassDistribution() {
        if (isLeaf) {
            // Leaf node: Center of mass and total mass are already set
            return;
        }

        // Reset total mass and center of mass for recomputation
        totalMass = 0.0f;
        centerOfMass = {0.0f, 0.0f};

        // Iterate through children to compute center of mass and total mass
        for (auto& child : children) {
            if (child) {
                child->computeMassDistribution();

                // Accumulate total mass
                totalMass += child->totalMass;

                // Accumulate weighted center of mass
                centerOfMass.x += child->centerOfMass.x * child->totalMass;
                centerOfMass.y += child->centerOfMass.y * child->totalMass;
            }
        }

        if (totalMass > 0) {
            // Finalize center of mass calculation
            centerOfMass.x /= totalMass;
            centerOfMass.y /= totalMass;
        }
    }

    void calculateForce(Particle& particle, const Node* node) {
        // Avoid self-interaction
        if (node->particle == &particle && node->isLeaf) {
            return;
        }

        sf::Vector2f direction = node->centerOfMass - particle.position;
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y) + config.epsilon;
        
        if (distance < config.particleSize) {
            return;
        }

        // Check if we can approximate
        if (node->totalMass < 100.0f) return;
        if (node->isLeaf || (node->size / distance < config.theta)) {
            float force = 
                (config.gravitational_constant * particle.mass * node->totalMass) / 
                ((distance * distance) + config.gravitationalSoftening);

            sf::Vector2f forceVector = (force / distance) * direction;
            particle.force += forceVector;

        } else {
            // Recurse into child nodes
            for (auto& child : node->children) {
                if (child) {
                    calculateForce(particle, child);
                }
            }
        }
    };

};


class QuadTree {
public:
    Node* root;
    float theta = 0.5f; // appoximation threshold


    QuadTree(const sf::Vector2f& position, float size) {
        root = new Node(position, size);
    }

    ~QuadTree() {
        clear();
    }

    void update() {
        reset();
        insert(Particle::particles);
        computeMassDistribution();
        calculateForces(Particle::particles);
    }

    void insert(std::vector<Particle>& particles) {
        for (auto& particle : particles) {

            root->insert(particle);
        }
    }

    void render() {
        root->render();   
    }

    // TODO: dfs to calculate center of mass
    void computeMassDistribution() {
        root->computeMassDistribution();
    }
    // Clear the entire quadtree
    void clear() {
        if (root) {
            root->clear();  // Clear the root node recursively
            delete root;    // Free the root node memory
            root = nullptr; // Reset the root pointer
        }
    }

    // Reinitialize the QuadTree
    void reset() {
        clear(); // Clear the existing tree
        root = new Node({0.0f, 0.0f}, config.windowWidth);
    }


    void _calculateForces(std::vector<Particle>& particles) {
        for (auto& particle : particles) {
            root->calculateForce(particle, root);
        }
    }


    void calculateForces(std::vector<Particle>& particles) {
        const size_t numThreads = 6;
        const size_t numParticles = particles.size();
        const size_t chunkSize = (numParticles + numThreads - 1) / numThreads; 

        std::vector<std::thread> threads;

        // Function to calculate forces for a chunk of particles
        auto calculateChunk = [&](size_t start, size_t end) {
            for (size_t i = start; i < end; ++i) {
                root->calculateForce(particles[i], root);
            }
        };

        // Launch threads
        for (size_t t = 0; t < numThreads; ++t) {
            size_t start = t * chunkSize;
            size_t end = std::min(start + chunkSize, numParticles); // Ensure end does not exceed particles.size()
            if (start < end) { // Only create a thread if there's work to do
                threads.emplace_back(calculateChunk, start, end);
            }
        }

        // Join threads
        for (auto& thread : threads) {
            thread.join();
        }
    }


};
/*
Idea:

The further away a particle is from another the more an approximation can be used. 
By splitting the scene into quadrants and calculating every quadrants center of mass we can 
massivly reduce the amount of checks done. The further a particle is from a quadrant the more we can approximate.

Tasks:

1. Assign every particle a unique quadrant in the scene.
2. Compute the center of mass' for every quadrant using dfs.
3. iterate over each particle and compare the accuracy with theta. Recursivly go down the tree the more precision is needed.


Tree:
    The tree will store the root node.

    insert() will insert a body recursively into the tree until its in its own quad
    compute mass() will compute the centers of mass for each quad using dfs

Node:
    Node will potentially contain 4 child nodes.

    subdivide() will add 4 child nodes and append nodes into them

Quad:

*/

extern QuadTree quadTree;