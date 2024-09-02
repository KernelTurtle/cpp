// Copyright 2024 KernelTurtle
#include <iostream>
#include <vector>

std::vector<std::vector<int>> spiralMatrixIII(int rows, int cols, int startRow, int startCol) {
    std::vector<std::vector<int>> res;
    int total = rows * cols;
    res.push_back({startRow, startCol});

    if (total == 1)
        return res;

    for (int si = 1; ; si += 2) {
        // directions are needed              r  c  step
        std::vector<std::vector<int>> dir = {{0,  1,   si},   // east
                                             {1,  0,   si},   // south
                                             {0, -1, si+1},   // west
                                             {-1, 0, si+1}};  // north

        // Go through each directions
        for (auto& dire : dir) {
            // Direction alloc
            int rowInc = dire[0], colInc = dire[1], steps = dire[2];

            // Move the number of steps in current
            while (steps-- > 0) {
                // Update start Position
                startRow += rowInc;
                startCol += colInc;

                // Check if the new position is in bounds
                if (startRow >= 0 && startRow < rows && startCol >= 0 && startCol < cols) {
                    res.push_back({startRow, startCol});
                    // if everything is exist; return
                    if (static_cast<int>(res.size()) == total)
                        return res;
                }
            }
        }
    }
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
