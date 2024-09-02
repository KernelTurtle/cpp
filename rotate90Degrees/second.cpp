// Copyright 2024 KernelTurtle
#include <iostream>
#include <vector>

void rotate90Degrees(std::vector<std::vector<int>> matrix) {
    int n = matrix.size();

    for (int i = 0; i < n / 2; ++i) {
        for (int j = i; j < n - i - 1; ++j) {
            int temp = matrix[i][j];

            matrix[i][j] = matrix[n - j - 1][i];
            matrix[n - j - 1][i] = matrix[n - i - 1][n - j - 1];
            matrix[n - i - 1][n - j - 1] = matrix[j][n - i - 1];
            matrix[j][n - i - 1] = temp;
        }
    }
}

int main() {
    std::vector < std::vector < int >> arr;
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
