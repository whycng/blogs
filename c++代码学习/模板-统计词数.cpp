#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <list> 

//使用模板来实现
template <typename Container>
int funT(const Container& input) {
    std::map<typename Container::value_type, int> counts;

    for (const auto& elem : input) {  
        counts[elem]++;
    }

    return counts.size();
}

int main() { 

    std::string str = "xiaomixunlianying";
    std::vector<int> nums = {1, 2, 2, 3, 4, 5};
    std::list<char> chars = {'x', 'm', 'y', 'x', 'z', ' '};
    std::vector<std::string> words = {"water", "drink", "stom", "water"};

    std::cout << "不同字符个数 : " << funT(str) << std::endl;
    std::cout << "不同整数个数: " << funT(nums) << std::endl;
    std::cout << "不同字符个数 : " << funT(chars) << std::endl;
    std::cout << "不同字符串个数: " << funT(words) << std::endl;

    return 0;
}

