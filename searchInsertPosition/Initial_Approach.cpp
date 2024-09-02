// Copyright 2024 KernelTurtle
#include <iostream>
#include <vector>

int searchInsert(std::vector<int> nums, int target) {
    int low = 0, high = nums.size() - 1;
    while (high < static_cast<int>(nums.size()) && static_cast<int>(nums[high]) < target)
        low = high, high = std::min(high * 2, static_cast<int>(nums.size()) - 1);
    while (low <= high) {
        int mid = (low + high) / 2;
        (nums[mid] >= target) ? (high = --mid) : (low = ++mid);
    }
    return low;
}

int main() {
    std::vector<int> nums = {1, 3, 5, 6};
    int target = 5;
    std::cout << searchInsert(nums, target) << "\n";
    return 0;
}
