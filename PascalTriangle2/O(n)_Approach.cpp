// Copyright 2024 KernelTurtle
#include <iostream>
#include <vector>
#include <cstdint>

std::vector<int> getRow(int rowIndex) {
    std::vector<int> ans(rowIndex + 1, 1);

    int64_t cur = 1;
    for (int i = 1; i <= rowIndex/2; ++i) {
        cur *= (rowIndex - i + 1);
        cur /= i;
        ans[i] = ans[rowIndex - i] = cur;
    }

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
