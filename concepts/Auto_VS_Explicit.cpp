// Copyright 2024 KernelTurtle
#include <iostream>
#include <vector>
#include <string>
#include <chrono> // NOLINT

int main() {
    std::vector<std::string> words = {"This", "is", "a", "test", "vector"};

    // Auto Benchmarking
    std::cout << "Using auto: " << std::endl;
    auto start_auto = std::chrono::high_resolution_clock::now();
    for (auto& word : words) {
        std::cout << word << " ";
    }
    auto end_auto = std::chrono::high_resolution_clock::now();
    auto elapsed_auto = std::chrono::duration_cast<std::chrono::nanoseconds>(end_auto - start_auto);
    std::cout << std::endl << "Elapsed time using auto: " << elapsed_auto.count() << "ns" << std::endl << std::endl;

    // Explicit Benchmarking
    std::cout << "Using Explicit: " << std::endl;
    auto start_explicit = std::chrono::high_resolution_clock::now();
    for (std::string& word : words) {
        std::cout << word << " ";
    }
    auto end_explicit = std::chrono::high_resolution_clock::now();
    auto elapsed_explicit = std::chrono::duration_cast<std::chrono::nanoseconds>(end_explicit - start_explicit);
    std::cout << std::endl << "Elapsed time using Explicit: " << elapsed_explicit.count() << "ns" << std::endl;
    return 0;
}
