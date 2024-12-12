#pragma once
#include "Config.hpp"
#include <vector>
#include "Particle.hpp"
#include "Solver.hpp"

struct FakeGravityParticle {
    float mass;
    sf::Vector2f position;
};

struct GravityGrid {
    int cellSize = config.gravityGridCellSize;
    int nColumns = config.windowWidth / static_cast<int>(cellSize);
    int nRows = config.windowHeight / static_cast<int>(cellSize);

    std::vector<std::vector<FakeGravityParticle>> cellMasses;

    GravityGrid() {
        // Resize the grid to the required number of rows and columns
        cellMasses.resize(nColumns);
        for (auto& col : cellMasses) {
            col.resize(nRows);
        }

        // Initialize each FakeGravityParticle
        for (int col = 0; col < nColumns; ++col) {
            for (int row = 0; row < nRows; ++row) {
                FakeGravityParticle& gravityParticle = cellMasses[col][row];
                
                // Set default mass to 0
                gravityParticle.mass = 0.0f;

                // Set the position to the center of the grid cell
                gravityParticle.position = sf::Vector2f(
                    (col * cellSize) + cellSize / 2.0f,  // X position (center of the cell)
                    (row * cellSize) + cellSize / 2.0f   // Y position (center of the cell)
                );
            }
        }
    }

    void assignParticlesToGrid(std::vector<Particle>& particles) {
        for (auto& col : cellMasses) {
            for (auto& gravityPartcle : col) {
                gravityPartcle.mass = 0.0f;
            }
        }

        for (Particle& particle : particles) {
            int col = static_cast<int>(particle.position.x / cellSize);
            int row = static_cast<int>(particle.position.y / cellSize);

            if (col >= 0 && col < nColumns && row >= 0 && row < nRows) {
                cellMasses[col][row].mass += particle.mass;
            }
        }
    }

    void calculateGravity() {
        for (Particle& particle1 : Particle::particles) {
            
            particle1.force = {0.0f, 0.0f};

            for (auto& col : cellMasses) {
                for (auto& massParticle : col ) {

                    if (massParticle.mass == 0.0f) continue;
        
                    float forceX = 0.0f;
                    float forceY = 0.0f;
                    
                    int massProduct = particle1.mass * massParticle.mass;

                    float dX = massParticle.position.x - particle1.position.x;
                    float dY = massParticle.position.y - particle1.position.y;

                    float gravitationalSoftningSquared =
                        config.gravitational_softening *
                        config.gravitational_softening;

                    // pythagarus
                    float distanceSquared = dX * dX + dY * dY + gravitationalSoftningSquared;
                    float distance = std::sqrt(distanceSquared);


                    if (distance < config.minDistance) {
                        float forceMagnitude = (config.gravitational_constant * particle1.mass * massParticle.mass) / (distanceSquared + config.minDistance);
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
        
    }
};

extern GravityGrid gravityGrid;