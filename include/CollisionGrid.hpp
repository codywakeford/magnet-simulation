#pragma once
#include "Config.hpp"
#include <vector>
#include "Particle.hpp"
#include "Solver.hpp"
#include <thread>

struct CollisionGrid {
    constexpr static int cellSize = Config::particleSize; 
    constexpr static int nColumns = Config::windowWidth / cellSize;
    constexpr static int nRows = Config::windowHeight / cellSize;

    static std::vector<std::vector<std::vector<Particle*>>> cells;

    static void initialize() {
        cells.resize(nColumns, std::vector<std::vector<Particle*>>(nRows));
    }

    static void update(std::vector<Particle>& particles) {
        assignParticlesToGrid(particles);
        checkCollisionsInGrid();
    }

    static void assignParticlesToGrid(std::vector<Particle>& particles) {
        // Clear all cells
        for (auto& col : cells) {
            for (auto& cell : col) {
                cell.clear();
            }
        }

        // Assign particles to their respective cells
        for (Particle& particle : particles) {
            int col = static_cast<int>(particle.position.x / cellSize);
            int row = static_cast<int>(particle.position.y / cellSize);

            if (col >= 0 && col < nColumns && row >= 0 && row < nRows) {
                cells[col][row].push_back(&particle);
            }
        }
    }

    static void _checkCollisionsInGrid() {
        const int dx[9] = {-1, -1, -1,  0, 0, 0,  1, 1, 1};
        const int dy[9] = {-1,  0,  1, -1, 0, 1, -1, 0, 1};

        for (int col = 0; col < nColumns; ++col) {
            for (int row = 0; row < nRows; ++row) {
                for (Particle* particle1 : cells[col][row]) {
                    for (int dir = 0; dir < 9; ++dir) {
                        int adjCol = col + dx[dir];
                        int adjRow = row + dy[dir];

                        if (adjCol >= 0 && adjCol < nColumns &&
                            adjRow >= 0 && adjRow < nRows) {
                            for (Particle* particle2 : cells[adjCol][adjRow]) {
                                if (particle1 != particle2) {
                                    Solver::resolve_collision(*particle1, *particle2);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // Threaded
    static void checkCollisionsInGrid() {
        const int dx[9] = {-1, -1, -1,  0, 0, 0,  1, 1, 1};
        const int dy[9] = {-1,  0,  1, -1, 0, 1, -1, 0, 1};

        auto collisionCheck = [&](int rowStart, int rowEnd) {
            for (int col = 0; col < nColumns; ++col) {
                for (int row = rowStart; row < rowEnd; ++row) {
                    
                    for (Particle* particle1 : cells[col][row]) {
                        for (int dir = 0; dir < 9; ++dir) {
                            int adjCol = col + dx[dir];
                            int adjRow = row + dy[dir];

                            if (adjCol >= 0 && adjCol < nColumns &&
                                adjRow >= 0 && adjRow < nRows) {
                                for (Particle* particle2 : cells[adjCol][adjRow]) {
                                    if (particle1 != particle2) {
                                        Solver::resolve_collision(*particle1, *particle2);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        };

        const int numThreads = std::thread::hardware_concurrency();; 
        std::vector<std::thread> threads;
        int rowsPerThread = nRows / numThreads;

        for (int i = 0; i < numThreads; ++i) {
            int rowStart = i * rowsPerThread;
            int rowEnd = (i == numThreads - 1) ? nRows : (i + 1) * rowsPerThread;
            threads.emplace_back(collisionCheck, rowStart, rowEnd);
        }

        for (auto& t : threads) {
            t.join();
        }
    }
};

std::vector<std::vector<std::vector<Particle*>>> CollisionGrid::cells;

// constexpr int CollisionGrid::cellSize = Config::particleSize;
extern CollisionGrid collisionGrid;