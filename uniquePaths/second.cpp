// Copyright 2024 KernelTurtle
#include <iostream>
#include <vector>

int countPath(int i, int j, int m, int n, std::vector<std::vector<int>> dp) {
    if (i == (m-1) && j == (n-1) )
        return 1;
    if (i >= m || j >= n)
        return 0;
    if (dp[i][j] != -1)
        return dp[i][j];
    else
        return dp[i][j] = countPath(i+1, j, m, n, dp) + countPath(i, j+1, m, n, dp);
}

int uniquePath(int m, int n) {
    std::vector<std::vector<int>> dp(m, std::vector<int>(n, -1));

    return countPath(0, 0, m, n, dp);;
}

int main() {
    int totalCount = uniquePath(3, 7);
    std::cout << "The total number of Unique Paths are " << totalCount << std::endl;
}
