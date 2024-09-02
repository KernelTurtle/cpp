// Copyright 2024 KernelTurtle
#include <algorithm>
#include <iostream>
#include <vector>

std::vector<std::vector<int>> threeSum(std::vector<int> nums) {
    std::vector<std::vector<int>> ans;
    sort(nums.begin(), nums.end());
    for  (int i = 0; i < static_cast<int>(nums.size()); i++) {
        if (i != 0 && nums[i] == nums[i - 1]) continue;

        int j = i + 1;
        int k = nums.size() - 1;
        while (j < k) {
            int sum = nums[i] + nums[j] + nums[k];
            if (sum < 0) {
                j++;
            } else if (sum > 0) {
                k--;
            } else {
                std::vector<int> tmp = {nums[i], nums[j], nums[k]};
                ans.push_back(tmp);
                j++;
                k--;

                while (j < k && nums[j] == nums[j - 1]) j++;
                while (j < k && nums[k] == nums[k + 1]) k--;
            }
        }
    }
    return ans;
}

int main() {
    std::vector<int> arr = {0, 0, 0};
    std::vector<std::vector<int>> ans = threeSum(arr);
    for (auto it : ans) {
        std::cout << "[";
        for (auto i : it) {
            std::cout << i << " ";
        }
        std::cout << "] ";
    }
    std::cout << "\n";
    return 0;
}

