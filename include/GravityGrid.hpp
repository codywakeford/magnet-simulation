#pragma once
#include "Config.hpp"
#include <vector>
#include "Particle.hpp"
#include "Solver.hpp"



struct GravityGridCell {
    float mass = 0.0f;
    sf::Vector2f position = {0.0f, 0.0f};
    std::vector<Particle> particles; 
};

struct GravityGrid {
    int cellSize = config.gravityGridCellSize;
    int nColumns = config.windowWidth / static_cast<int>(cellSize);
    int nRows = config.windowHeight / static_cast<int>(cellSize);

    std::vector<std::vector<GravityGridCell>> cells;

    GravityGrid() {
        // Resize the grid to the required number of rows and columns
        cells.resize(nColumns);
        for (auto& col : cells) {
            col.resize(nRows);
        }

        for (int col = 0; col < nColumns; ++col) {
            for (int row = 0; row < nRows; ++row) {
                GravityGridCell& cell = cells[col][row];

                cell.position = sf::Vector2f(
                    (col * cellSize) + cellSize / 2.0f,  // X position (center of the cell)
                    (row * cellSize) + cellSize / 2.0f   // Y position (center of the cell)
                );
            }
        }
    }

    void assignParticlesToGrid(std::vector<Particle>& particles) {
        for (auto& col : cells) {
            for (auto& cell: col) {
                cell.mass = 0.0f;
                cell.particles.clear();
            }
        }

        for (Particle& particle : particles) {
            int col = static_cast<int>(particle.position.x / cellSize);
            int row = static_cast<int>(particle.position.y / cellSize);

            if (col >= 0 && col < nColumns && row >= 0 && row < nRows) {
                GravityGridCell& cell = cells[col][row];
                cell.particles.push_back(particle);
                cell.mass += particle.mass;
            }
        }
    }

    /*

    e : is used to dampen gravity at small distances. 

            m1 * m2
    F = G -----------
           r^2 - e^2
    
    */

    void calculateGravity() {
        for (int col = 0; col < nColumns; ++col) {
            for (int row = 0; row < nRows; ++row) {
                GravityGridCell& cell = cells[col][row];

                // Skip empty cells
                if (cell.particles.empty()) continue;

                // Interact with neighbors and itself
                for (int dCol = -1; dCol <= 1; ++dCol) {
                    for (int dRow = -1; dRow <= 1; ++dRow) {
                        int neighborCol = col + dCol;
                        int neighborRow = row + dRow;

                        // Ensure neighbor indices are within bounds
                        if (neighborCol < 0 || neighborCol >= nColumns ||
                            neighborRow < 0 || neighborRow >= nRows) {
                            continue;
                        }

                        GravityGridCell& neighborCell = cells[neighborCol][neighborRow];

                        if (neighborCell.particles.empty()) continue;

                        // Particle-to-particle interaction
                        for (Particle& particle1 : cell.particles) {
                            for (Particle& particle2 : neighborCell.particles) {
                                if (&particle1 != &particle2) {
                                    sf::Vector2f force =
                                        Solver::calculateGravitationalForce(
                                            particle1.mass, particle1.position,
                                            particle2.mass, particle2.position);

                                    particle1.force += force;
                                }
                            }
                        }
                    }
                }
            }
        }
    }



    void calculateGravityOld() {
        for (Particle& particle1 : Particle::particles) {
            
            particle1.force = {0.0f, 0.0f};

            for (auto& col : cells) {
                for (auto& cell: col ) {

                    if (cell.mass == 0.0f && cell.particles.empty()) {
                        continue;
                    }
            
                    int massProduct = particle1.mass * cell.mass;

                    float dX = cell.position.x - particle1.position.x;
                    float dY = cell.position.y - particle1.position.y;

                    float gravitationalSofteningSquared =
                        config.gravitational_softening *
                        config.gravitational_softening;

                    // pythagarus
                    float distanceSquared = dX * dX + dY * dY + gravitationalSofteningSquared;
                    float distance = std::sqrt(distanceSquared);


                    if (distance < config.minDistance) {
                        for (Particle& particle2 : cell.particles) {
                            float dXP = particle2.position.x - particle1.position.x;
                            float dYP = particle2.position.y - particle1.position.y;

                            float distanceSquaredP = dXP * dXP + dYP * dYP + gravitationalSofteningSquared;
                            float distanceP = std::sqrt(distanceSquaredP);

                            if (distanceP < config.minDistance) {
                                // Calculate the gravitational force between particle1 and particle2
                                float forceMagnitude = (config.gravitational_constant * particle1.mass * particle2.mass) / (distanceSquaredP + config.minDistance);
                                particle1.force.x += forceMagnitude * (dXP / distanceP);
                                particle1.force.y += forceMagnitude * (dYP / distanceP);
                            }
                        }
                    } else {
                        // If the distance is far, calculate the force between particle1 and the grid cell mass
                        float forceMagnitude = (config.gravitational_constant * particle1.mass * cell.mass) / distanceSquared;

                        dX *= config.dampingFactor;
                        dY *= config.dampingFactor;
                        particle1.force.x += forceMagnitude * (dX / distance);
                        particle1.force.y += forceMagnitude * (dY / distance);
                    }
                }
            } 
        }
        
    }
};

extern GravityGrid gravityGrid;