// Copyright 2024 KernelTurtle
#include <iostream>
#include <cstdint>

int64_t UniquePath(int m, int n) {
    int N = n + m -2;
    int r = std::min(m - 1, n - 1);
    int64_t res = 1;

    for (int i = 1; i <= r; i++) {
        res *= (N - r + i);
        res /= i;
    }
    return static_cast<int>(res);
}

int main() {
    int totalCount = UniquePath(3, 7);
    std::cout << "The total number of Unique Paths are " << totalCount << std::endl;
}
