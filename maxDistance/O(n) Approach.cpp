// Copyright 2024 KernelTurtle
#include <iostream>
#include <vector>

int maxDistance(std::vector<std::vector<int>> arr) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(NULL), std::cout.tie(NULL);
    int maxV = arr[0].front(), minV = arr[0].back(), distMax = 0;

    for (int i = 0; i < static_cast<int>(arr.size()); ++i) {
        distMax = std::max(distMax, abs(arr[i].back() - minV)),
        distMax = std::max(distMax, abs(maxV -  arr[i].front()));
        minV = std::min(minV, arr[i].front()), maxV = std::max(maxV, arr[i].back());
    }
    return distMax;
}

int main() {
    std::vector<std::vector<int>> arr = {{1, 2, 3}, {4, 5}, {6, 7, 8}};
    std::cout << maxDistance(arr) << '\n';
    return 0;
}
