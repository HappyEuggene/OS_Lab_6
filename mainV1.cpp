#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <random>
#include <chrono>

const int ROWS = 3;
const int COMMON_SIZE = 4;
const int COLS = 2;

std::vector<std::vector<int>> matrixA(ROWS, std::vector<int>(COMMON_SIZE));
std::vector<std::vector<int>> matrixB(COMMON_SIZE, std::vector<int>(COLS));
std::vector<std::vector<int>> matrixC(ROWS, std::vector<int>(COLS));

std::mutex output_mutex;

void initializeMatrixWithRandomNumbers(std::vector<std::vector<int>>& mat) {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(1, 10);

    for (auto& row : mat) {
        for (auto& element : row) {
            element = distribution(generator);
        }
    }
}

void calculateElement(int row, int col) {
    int sum = 0;
    for (int i = 0; i < COMMON_SIZE; ++i) {
        sum += matrixA[row][i] * matrixB[i][col];
    }

    {
        std::lock_guard<std::mutex> lock(output_mutex);
        std::cout << "Element [" << row << "," << col << "] = " << sum << std::endl;
    }

    matrixC[row][col] = sum;
}

void multiplyMatrices() {
    std::vector<std::thread> workerThreads;
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            workerThreads.emplace_back(calculateElement, row, col);
        }
    }

    for (auto& thread : workerThreads) {
        thread.join();
    }
}

int main() {
    initializeMatrixWithRandomNumbers(matrixA);
    initializeMatrixWithRandomNumbers(matrixB);

    auto startTime = std::chrono::high_resolution_clock::now();
    multiplyMatrices();
    auto endTime = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> timeElapsed = endTime - startTime;
    std::cout << "Time elapsed: " << timeElapsed.count() << " ms\n";

    return 0;
}
