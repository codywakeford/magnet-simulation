#include <vector>
// #include <cuda_runtime.h>

struct Particle {
    float2 position;  // Use float2 for simplicity
};

// GPU kernel for assigning particles to grid
__global__
void assignParticlesKernel(Particle* particles, int numParticles, int* cellCounts, Particle** cellData, 
                           int nColumns, int nRows, float cellSize) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx >= numParticles) return;

    Particle& particle = particles[idx];
    int col = static_cast<int>(particle.position.x / cellSize);
    int row = static_cast<int>(particle.position.y / cellSize);

    if (col >= 0 && col < nColumns && row >= 0 && row < nRows) {
        int cellIdx = row * nColumns + col;

        // Atomic operation to add particle to cell
        int pos = atomicAdd(&cellCounts[cellIdx], 1);
        cellData[cellIdx][pos] = particle;
    }
}

// Wrapper function
void assignParticlesToGrid(std::vector<Particle>& particles, 
                           std::vector<std::vector<std::vector<Particle>>>& cells,
                           int nColumns, int nRows, float cellSize) {
    int numParticles = particles.size();
    int numCells = nColumns * nRows;

    // Flatten cells data structure for GPU use
    std::vector<int> cellCounts(numCells, 0);
    std::vector<Particle*> cellData(numCells, nullptr);

    // Allocate memory on GPU
    Particle* d_particles;
    int* d_cellCounts;
    Particle** d_cellData;

    cudaMalloc(&d_particles, numParticles * sizeof(Particle));
    cudaMalloc(&d_cellCounts, numCells * sizeof(int));
    cudaMalloc(&d_cellData, numCells * sizeof(Particle*));

    // Allocate memory for individual cell particle pointers
    for (int i = 0; i < numCells; ++i) {
        cudaMalloc(&cellData[i], particles.size() * sizeof(Particle)); // Over-allocate for simplicity
    }

    // Copy data to GPU
    cudaMemcpy(d_particles, particles.data(), numParticles * sizeof(Particle), cudaMemcpyHostToDevice);
    cudaMemcpy(d_cellCounts, cellCounts.data(), numCells * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_cellData, cellData.data(), numCells * sizeof(Particle*), cudaMemcpyHostToDevice);

    // Launch kernel
    int threadsPerBlock = 256;
    int blocksPerGrid = (numParticles + threadsPerBlock - 1) / threadsPerBlock;
    assignParticlesKernel<<<blocksPerGrid, threadsPerBlock>>>(d_particles, numParticles, d_cellCounts, d_cellData,
                                                              nColumns, nRows, cellSize);

    // Copy results back to host
    cudaMemcpy(cellCounts.data(), d_cellCounts, numCells * sizeof(int), cudaMemcpyDeviceToHost);

    for (int i = 0; i < numCells; ++i) {
        cudaMemcpy(cells[i / nColumns][i % nColumns].data(), cellData[i], cellCounts[i] * sizeof(Particle), cudaMemcpyDeviceToHost);
    }

    // Free GPU memory
    cudaFree(d_particles);
    cudaFree(d_cellCounts);
    for (int i = 0; i < numCells; ++i) {
        cudaFree(cellData[i]);
    }
    cudaFree(d_cellData);
}
