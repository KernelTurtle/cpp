// Copyright 2024 KernelTurtle
#include <iostream>
#include <vector>

std::vector<int> spiralOrder(std::vector<std::vector<int>>& matrix) {
    int top = 0, bottom = matrix.size();
    int  left = 0, right = matrix[0].size();
    std::vector<int> res;
    
    while (left < right && top < bottom) {
        // left to right
        for (int i =  left; i < right; ++i) {
            res.push_back(matrix[top][i]);
        }
        ++top;

        // top to bottom
        for (int i = top; i < bottom; ++i) {
            res.push_back(matrix[i][ right - 1]);
        }
        --right;

        // row not same
        if (!(top < bottom) || !( left <  right)) break;

        // right to left
        for (int i =  right - 1; i >=  left; --i) {
            res.push_back(matrix[bottom - 1][i]);
        }
        --bottom;

        // different column
        for (int i = bottom - 1; i >= top; --i) {
            res.push_back(matrix[i][ left]);
        }
        ++left;
    }
    return res;
}

int main() {
    std::vector<std::vector<int>> matrix = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };

    std::vector<int> result = spiralOrder(matrix);

    for (int num : result) {
        std::cout << num << " ";
    }

    return 0;
}
