#pragma once

#include <SFML/Graphics.hpp>
#include <array>
#include "Particle.hpp"
#include "WindowManager.hpp"
#include "Solver.hpp"

#include <thread>
#include <vector>
#include <functional>

using namespace std;

// Node struct
struct Node {
    sf::Vector2f position;     // Top-left corner of the node
    sf::Vector2f centerOfMass; // Center of mass for the node
    float size;                // Size of the node (width and height)
    float totalMass = 0.0f;    // Total mass in the node
    bool isLeaf = true;        // Leaf status
    Particle* particle = nullptr; // Pointer to a particle (if any)
    array<Node*, 4> children = {nullptr, nullptr, nullptr, nullptr};

    mutex massMutex;
    mutex particleMutex;
    mutex nodeMutex;
    
    // Rendering //
    // sf::RectangleShape rectangle;

    static vector<Node*> nodePool;
    static mutex poolMutex;

    Node(const sf::Vector2f& position, float size)
        : position(position), size(size), centerOfMass({0.0f, 0.0f}) {
            
            // rectangle.setPosition(position);
            // rectangle.setOutlineThickness(0.1f);
            // rectangle.setFillColor(sf::Color::Transparent);
            // rectangle.setSize({size, size});
        }

    ~Node() {
        for (auto child : children) {
            delete child;
        }
    }

    static Node* acquireNode() {
        lock_guard<mutex> lock(poolMutex);
        if (!nodePool.empty()) {
            Node* node = nodePool.back();
            nodePool.pop_back();
            return node;
        }
        
        return new Node({0.0f, 0.0f}, 0.0f);
    }

    static void releaseNode(Node* node) {
        if (node) {
            node->resetNode(); 
            lock_guard<mutex> lock(poolMutex);
            nodePool.push_back(node);
        }
    }


    static void initializeNodePool(int nodeCount) {
        lock_guard<mutex> lock(poolMutex);
        for (int i = 0; i < nodeCount; i++) {
            nodePool.push_back(new Node({0.0f, 0.0f}, 0.0f));
        }
    }


    void resetNode() {
        position = {0.0f, 0.0f};
        centerOfMass = {0.0f, 0.0f};
        size = 0.0f;
        totalMass = 0.0f;
        isLeaf = true;
        particle = nullptr;

        // Reset child pointers
        for (auto& child : children) {
            if (child) {
                releaseNode(child);
                child = nullptr;
            }
        }
    }
    
    // void render() {
    //     if (!isLeaf) {
    //         for (auto& child : children) {
    //             child->render();
    //         }
    //         return;
    //     }

    //     rectangle.setSize({size, size});
    //     window.draw(rectangle);
    // }
    
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

    bool contains(const Particle& particle) const {
        return particle.position.x >= position.x &&
               particle.position.x < position.x + size &&
               particle.position.y >= position.y &&
               particle.position.y < position.y + size;
    }

    void subdivide() {
        float halfSize = size / 2.0f;

        // Acquire nodes from the pool and initialize them
        children[0] = Node::acquireNode(); // NW
        children[0]->position = position;
        children[0]->size = halfSize;

        children[1] = Node::acquireNode(); // NE
        children[1]->position = {position.x + halfSize, position.y};
        children[1]->size = halfSize;

        children[2] = Node::acquireNode(); // SE
        children[2]->position = {position.x + halfSize, position.y + halfSize};
        children[2]->size = halfSize;

        children[3] = Node::acquireNode(); // SW
        children[3]->position = {position.x, position.y + halfSize};
        children[3]->size = halfSize;

        isLeaf = false;
    }


    void _insert(vector<Particle>& particles) {
        if (particles.empty()) return;

        const int numThreads = min(thread::hardware_concurrency(), static_cast<unsigned int>(particles.size()));
        vector<thread> threads;
        int particlesPerThread = particles.size() / numThreads;

        for (int i = 0; i < numThreads; i++) {
            int particleStart = i * particlesPerThread;
            int particleEnd = (i == numThreads - 1) ? particles.size() : particleStart + particlesPerThread;

            threads.emplace_back([this, &particles, particleStart, particleEnd]() {
                for (int j = particleStart; j < particleEnd; j++) {
                    this->insert(particles[j]);
                }
            });
        }

        for (auto& t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }
    }

    void insert(Particle& particle) {
        if (!contains(particle)) return;

        if (isLeaf) {
            lock_guard<mutex> lock(particleMutex);
            if (!this->particle) {
                this->particle = &particle;
                centerOfMass = particle.position;
                totalMass = particle.mass;
                return;
            }

            subdivide();

            Particle* existingParticle = this->particle;
            this->particle = nullptr;

            for (auto& child : children) {
                if (child->contains(*existingParticle)) {
                    child->insert(*existingParticle);
                    break;
                }
            }

            for (auto& child : children) {
                if (child->contains(particle)) {
                    child->insert(particle);
                    break;
                }
            }
        } else {
            for (auto& child : children) {
                if (child->contains(particle)) {
                    child->insert(particle);
                    break;
                }
            }
        }

        // Thread-safe update of total mass and center of mass
        {
            lock_guard<mutex> lock(massMutex);
            totalMass += particle.mass;
            centerOfMass.x = (centerOfMass.x * (totalMass - particle.mass) + particle.position.x * particle.mass) / totalMass;
            centerOfMass.y = (centerOfMass.y * (totalMass - particle.mass) + particle.position.y * particle.mass) / totalMass;
        }
    }



    // Compute center of mass using DFS
    void computeMassDistribution() {
        if (isLeaf) return;

        totalMass = 0.0f;
        centerOfMass = {0.0f, 0.0f};

        for (auto& child : children) {
            if (child) {
                child->computeMassDistribution();

                totalMass += child->totalMass;

                centerOfMass.x += child->centerOfMass.x * child->totalMass;
                centerOfMass.y += child->centerOfMass.y * child->totalMass;
            }
        }

        if (totalMass > 0) {
            centerOfMass.x /= totalMass;
            centerOfMass.y /= totalMass;
        }
    }

    void calculateForce(Particle& particle, const Node* node) {
        if (node->particle == &particle && node->isLeaf) {
            return;
        }

        sf::Vector2f direction = node->centerOfMass - particle.position;
        float distance = sqrt(direction.x * direction.x + direction.y * direction.y);
        
        if (distance < config.particleSize) {
            return;
        }

        // if (node->totalMass < 50.0f) return;

        // Here we check if the we meet the approximation criteria, if so use that 
        // data, if not recurse into children to get a more accurate force.
        if (node->isLeaf || (node->size / distance < config.theta)) {
            float force = 
                (config.gravitational_constant * particle.mass * node->totalMass) / 
                ((distance * distance) + config.gravitationalSoftening);

            sf::Vector2f forceVector = (force / distance) * direction;
            particle.force += forceVector;

        } else {
            for (auto& child : node->children) {
                if (child) {
                    calculateForce(particle, child);
                }
            }
        }
    };

};

std::vector<Node*> Node::nodePool;
std::mutex Node::poolMutex;

class QuadTree {
public:
    Node* root;

    QuadTree(const sf::Vector2f& position, float size) {
        root = new Node(position, size);
    }

    ~QuadTree() {
        clear();
    }

    void _update() {
        reset();
        insert(Particle::particles);
        computeMassDistribution();
        calculateForces(Particle::particles);
    }

    void update() {
        sf::Clock clock; // Create a clock for timing
        
        clock.restart();
        reset();
        cout << "reset() took: " << clock.getElapsedTime().asMicroseconds() << " us" << endl;

        clock.restart();
        insert(Particle::particles);
        cout << "insert() took: " << clock.getElapsedTime().asMicroseconds() << " us" << endl;

        clock.restart();
        computeMassDistribution();
        cout << "computeMassDistribution() took: " << clock.getElapsedTime().asMicroseconds() << " us" << endl;

        clock.restart();
        calculateForces(Particle::particles);
        cout << "calculateForces() took: " << clock.getElapsedTime().asMicroseconds() << " us" << endl;
    }
    
    void insert(vector<Particle>& particles) {
        root->_insert(particles);
    }

    // void render() {
    //     root->render();   
    // }

    void computeMassDistribution() {
        root->computeMassDistribution();
    }
    
    void clear() {
        if (root) {
            root->clear();
            delete root;
            root = nullptr;
        }
    }
    
    void reset() {
        if (root) {
            for (auto& child : root->children) {
                if (child) {
                    Node::releaseNode(child);
                    child = nullptr;
                }
            }

            root->centerOfMass = {0.0f, 0.0f}; 
            root->totalMass = 0.0f;
            root->isLeaf = true;
            root->particle = nullptr;
            root->children.fill(nullptr);
        }
    }


    void _calculateForces(vector<Particle>& particles) {
        for (auto& particle : particles) {
            root->calculateForce(particle, root);
        }
    }

    void calculateForces(vector<Particle>& particles) {
        const size_t numThreads = thread::hardware_concurrency();
        const size_t numParticles = particles.size();
        const size_t chunkSize = (numParticles + numThreads - 1) / numThreads; 

        vector<thread> threads;

        auto calculateChunk = [&](size_t start, size_t end) {
            for (size_t i = start; i < end; ++i) {
                root->calculateForce(particles[i], root);
            }
        };

        for (size_t t = 0; t < numThreads; ++t) {
            size_t start = t * chunkSize;
            size_t end = min(start + chunkSize, numParticles); 
            if (start < end) {
                threads.emplace_back(calculateChunk, start, end);
            }
        }

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

1. Assign every particle a unique quadrant in the scene. Dividing a node recursivly if necessary.
2. Compute the center of mass' for every quadrant using dfs.
3. iterate over each particle and compare the accuracy with theta. Recursivly go down the tree the more precision is needed.


Tree:
    The tree will store the root node.

    insert() will insert a body recursively into the tree until its in its own quad
    compute mass() will compute the centers of mass for each quad using dfs

Node:
    Node will potentially contain 4 child nodes.

    subdivide() will add 4 child nodes and append nodes into them

*/

extern QuadTree quadTree;