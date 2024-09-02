// Copyright 2024 KernelTurtle
#include <iostream>

int countPath(int i, int j, int m, int n) {
    if (i == (m-1) && j == (n-1) )
        return 1;
    if (i >= m || j >= n)
        return 0;
    return countPath(i+1, j, m, n) + countPath(i, j+1, m, n);
}

int uniquePath(int m, int n) {
    return countPath(0, 0, m, n);
}

int main() {
    int totalCount = uniquePath(3, 7);
    std::cout << "The total number of Unique Paths are " << totalCount << std::endl;
}
