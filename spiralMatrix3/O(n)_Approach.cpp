// Copyright 2024 KernelTurtle
#include <iostream>
#include <vector>

std::vector<std::vector<int>> spiralMatrixIII(int rows, int cols, int startRow, int startCol) {
    std::vector<std::vector<int>> res;
    int total = rows * cols;
    res.reserve(total);      // Reserve space to avoid reallocations
    res.push_back({startRow, startCol});

    if (total == 1) {
        return res;
    }

    // Directions: right, down, left, up
    int directionRow[] = {0, 1, 0, -1};
    int directionCol[] = {1, 0, -1, 0};

    int step = 1;            // Steps to move in a given direction
    int directionIndex = 0;  // Start with the "right" direction

    while (static_cast<int>(res.size()) < total) {
        for (int i = 0; i < 2; ++i) {     // Two movements per cycle: horizontal and vertical
            for (int j = 0; j < step; ++j) {
                startRow += directionRow[directionIndex];
                startCol += directionCol[directionIndex];

                if (startRow >= 0 && startRow < rows && startCol >= 0 && startCol < cols) {
                    res.push_back({startRow, startCol});
                    if (static_cast<int>(res.size()) == total) {
                        return res;
                    }
                }
            }
            // Change direction
            directionIndex = (directionIndex + 1) % 4;
        }
        step++;   // Increase the step size after completing the cycle
    }

    return res;
}

int main() {
    int rows = 5, cols = 6;
    int startRow = 1, startCol = 4;

    std::vector<std::vector<int>> result = spiralMatrixIII(rows, cols, startRow, startCol);

    for (const auto& position : result) {
        std::cout << "[" << position[0] << "," << position[1] << "] ";
    }

    return 0;
}
