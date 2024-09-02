// Copyright 2024 KernelTurtle
#include <iostream>
#include <vector>
#include <algorithm>

void rotate90Degrees(std::vector<std::vector<int>> matrix) {
    int n = matrix.size();
    // Transpose
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < i; j++) {
            std::swap(matrix[i][j], matrix[j][i]);
        }
    }

    // Reverse
    for (int i = 0; i < n; i++) {
        std::reverse(matrix[i].begin(), matrix[i].end());
    }
}

int main() {
    std::vector<std::vector<int>> arr;
    arr =  {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    rotate90Degrees(arr);
    std::cout << "Rotated Image" << std::endl;
    for (int i = 0; i < static_cast<int>(arr.size()); i++) {
        for (int j = 0; j < static_cast<int>(arr[0].size()); j++) {
            std::cout << arr[i][j] << " ";
        }
        std::cout << "\n";
    }
}
