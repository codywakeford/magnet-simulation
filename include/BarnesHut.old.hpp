#pragma once

#include <array>
#include <vector>
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <limits>
#include "Particle.hpp"
#include <cmath>
// create tree
// insert all particles into tree
// calculate gravity



// Struct representing a 2D vector
struct Vec2 {
    float x, y;

    Vec2 operator*(float scalar) const {
        return {x * scalar, y * scalar};
    }

    Vec2 operator+(const Vec2& other) const {
        return {x + other.x, y + other.y};
    }
};



// Class representing a Quad
class Quad {
public:
    Vec2 center;
    float size;

    // Constructor for Quad containing a set of bodies
    static Quad newContaining(const std::vector<Particle>& particles) {
        float min_x = std::numeric_limits<float>::max();
        float min_y = std::numeric_limits<float>::max();
        float max_x = std::numeric_limits<float>::lowest();
        float max_y = std::numeric_limits<float>::lowest();

        for (const Particle& particle : particles) {
            min_x = std::min(min_x, particle.position.x);
            min_y = std::min(min_y, particle.position.y);
            max_x = std::max(max_x, particle.position.x);
            max_y = std::max(max_y, particle.position.y);
        }

        Vec2 center = Vec2{min_x + max_x, min_y + max_y} * 0.5f;
        float size = std::max(max_x - min_x, max_y - min_y);

        return Quad{center, size};
    }

    // Find the quadrant for a given position
    int findQuadrant(const Vec2& pos) const {
        return ((pos.y > center.y) << 1) | (pos.x > center.x);
    }

    // Create a new Quad in a specific quadrant
    Quad intoQuadrant(int quadrant) const {
        float new_size = size * 0.5f;
        Vec2 new_center = center;
        new_center.x += ((quadrant & 1) - 0.5f) * new_size;
        new_center.y += ((quadrant >> 1) - 0.5f) * new_size;
        return Quad{new_center, new_size};
    }

    // Subdivide the Quad into four smaller Quads
    std::array<Quad, 4> subdivide() const {
        return {intoQuadrant(0), intoQuadrant(1), intoQuadrant(2), intoQuadrant(3)};
    }
};



struct Node {
    std::size_t children; // Equivalent to Rust's usize
    std::size_t next;
    Vec2 pos;
    float mass;
    Quad quad;

    // Constructor to initialize a new Node
    Node(std::size_t next, const Quad& quad)
        : children(0), next(next), pos(Vec2::zero()), mass(0.0f), quad(quad) {}


    // creates a new node
    void create() {};

    bool isLeaf() {
        return children == 0;
    }

    bool isBranch() { 
        return children != 0;
    }

    bool isEmpty() {
        return mass == 0.0f;
    }

};


class Quadtree {
public:
    static constexpr std::size_t ROOT = 0;

    // Public members
    float t_sq; // theta squared
    float e_sq; // epsilon squared
    std::vector<Node> nodes;
    std::vector<std::size_t> parents;

    // Constructor
    Quadtree(float theta, float epsilon)
        : t_sq(theta * theta), e_sq(epsilon * epsilon) {}

    // Clear the Quadtree and initialize the root node
    void clear(const Quad& quad) {
        nodes.clear();
        parents.clear();
        nodes.emplace_back(Node(0, quad)); // Add the root node
    }

    // Subdivide a node into four children
    std::size_t subdivide(std::size_t node) {
        // Register parent node
        parents.push_back(node);

        // Determine the children indices
        std::size_t children = nodes.size();
        nodes[node].children = children;

        // Calculate the `next` indices for the children
        std::size_t nexts[4] = {
            children + 1,
            children + 2,
            children + 3,
            nodes[node].next
        };

        // Subdivide the node's quad
        Quad quads[4];
        nodes[node].quad.subdivide(quads); // Assuming `Quad::subdivide()` populates an array of 4 Quads

        // Add the child nodes
        for (int i = 0; i < 4; ++i) {
            nodes.emplace_back(Node(nexts[i], quads[i]));
        }

        return children;
    }

    void insert(const Vec2& pos, float mass) {
        std::size_t node = ROOT;

        // Traverse until a leaf node is found
        while (nodes[node].is_branch()) {
            std::size_t quadrant = nodes[node].quad.find_quadrant(pos);
            node = nodes[node].children + quadrant;
        }

        // If the node is empty, initialize it with the new position and mass
        if (nodes[node].is_empty()) {
            nodes[node].pos = pos;
            nodes[node].mass = mass;
            return;
        }

        // Handle case where the position is the same
        const Vec2& p = nodes[node].pos;
        float m = nodes[node].mass;

        if (pos == p) {
            nodes[node].mass += mass;
            return;
        }

        // Handle subdivision and inserting into correct quadrants
        while (true) {
            std::size_t children = subdivide(node);

            std::size_t q1 = nodes[node].quad.find_quadrant(p);
            std::size_t q2 = nodes[node].quad.find_quadrant(pos);

            if (q1 == q2) {
                // Continue inserting into the same quadrant
                node = children + q1;
            } else {
                // Assign positions and masses to the respective quadrants
                std::size_t n1 = children + q1;
                std::size_t n2 = children + q2;

                nodes[n1].pos = p;
                nodes[n1].mass = m;
                nodes[n2].pos = pos;
                nodes[n2].mass = mass;
                return;
            }
        }
    }

    void propagate() {
        // Iterate through the parents in reverse order
        for (auto it = parents.rbegin(); it != parents.rend(); ++it) {
            std::size_t node = *it;
            std::size_t i = nodes[node].children;

            // Calculate the weighted positions of the child nodes
            nodes[node].pos = nodes[i].pos * nodes[i].mass
                            + nodes[i + 1].pos * nodes[i + 1].mass
                            + nodes[i + 2].pos * nodes[i + 2].mass
                            + nodes[i + 3].pos * nodes[i + 3].mass;

            // Sum up the masses of the child nodes
            nodes[node].mass = nodes[i].mass
                            + nodes[i + 1].mass
                            + nodes[i + 2].mass
                            + nodes[i + 3].mass;

            // Normalize the position by dividing by the total mass
            float mass = nodes[node].mass;
            nodes[node].pos /= mass;
        }
    }

    Vec2 acc(const Vec2& pos) const {
        Vec2 acc = Vec2::zero(); // Initialize acceleration to zero

        std::size_t node = ROOT;
        while (true) {
            const Node& n = nodes[node]; // Reference to the current node

            Vec2 d = n.pos - pos;       // Vector from `pos` to the node's position
            float d_sq = d.mag_sq();    // Squared magnitude of the distance vector

            // Check if the node is a leaf or satisfies the threshold condition
            if (n.is_leaf() || n.quad.size * n.quad.size < d_sq * t_sq) {
                float denom = (d_sq + e_sq) * std::sqrt(d_sq); // Calculate the denominator
                acc += d * std::min(n.mass / denom, std::numeric_limits<float>::max());

                // If there are no more nodes to process, exit the loop
                if (n.next == 0) {
                    break;
                }
                node = n.next; // Move to the next node
            } else {
                node = n.children; // Move to the child nodes
            }
        }

        return acc;
    }

};
