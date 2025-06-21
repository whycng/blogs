#include <iostream>
#include <string>
#include <map>
 
int fun(const std::string& inputStr) { 
    std::map<char, int> charCounts; 
 
    for (char c : inputStr) { 
        charCounts[c]++;
    } 
    return charCounts.size();
}
 
int main() {
     
    std::string testStr1 = "yayyayaysandjnjkasdkandsadsd";
    std::string testStr2 = "jsdhiud aui diuwad uiwdnsndw  wd";
    std::string testStr3 = "C++ STL xuexi";
    std::string testStr4 = ""; 
 
    std::cout << "字符串: " << testStr1 << "" << std::endl;
    std::cout << "不同字符的个数: " << fun(testStr1) << std::endl; 

    std::cout << "字符串: " << testStr2 << "" << std::endl;
    std::cout << "不同字符的个数: " << fun(testStr2) << std::endl; 

    std::cout << "字符串: " << testStr3 << "" << std::endl;
    std::cout << "不同字符的个数: " << fun(testStr3) << std::endl; 

    std::cout << "字符串: " << testStr4 << "" << std::endl;
    std::cout << "不同字符的个数: " << fun(testStr4) << std::endl; 

    return 0;
}
