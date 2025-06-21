#include <iostream>
#include <vector>
#include <array>

int main() { 
    std::vector<int> nums = {1, 2, 3 };
    std::array<int,3> arr = {3,2,1};

    auto insertPos = nums.begin() + 1;  

    //插入
    nums.insert(insertPos, 9);
    nums.insert(nums.begin() + 2, 99);
 
    for (int n : nums) {
        std::cout << n << " ";
    } 
    for( const auto& v : arr)
    {
        std::cout << v << " ";
    }

    // 使用迭代器遍历 std::vector
    for (auto it = nums.begin(); it != nums.end(); ++it) {
        std::cout << *it << " ";
    } 

    // 使用迭代器遍历 std::array
    for (auto it = arr.begin(); it != arr.end(); ++it) {
        std::cout << *it << " ";
    }

    return 0;
}
