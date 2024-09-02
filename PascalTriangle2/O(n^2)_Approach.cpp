// Copyright 2024 KernelTurtle
#include <iostream>
#include <vector>
#include <cstdint>

std::vector<int> getRow(int rowIndex) {
    std::vector<int> ans(rowIndex+1, 1);

    for (int i = 2; i <= rowIndex; ++i)
        for (int j = 1; j < i; ++j)
            ans[i - j] += ans[i - j - 1];
    return ans;
}

int main() {
    int rowIndex = 3;
    std::vector<int> result = getRow(rowIndex);
    for (int i = 0; i < static_cast<int>(result.size()); i++) {
        std::cout << result[i] << " ";
    }
    std::cout << std::endl;
    return 0;
}
