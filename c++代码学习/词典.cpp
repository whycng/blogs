#include <iostream>
#include <fstream>
#include <string>
#include <map>

// 读取词典文件
std::map<std::string, std::string> loadDict(const std::string& filename) {
    //文件存储模式为 object=对象 ，xx=yy 这种 ，一行一个
    std::ifstream fin(filename); 
    std::map<std::string, std::string> dict;//词典

    if (fin) {//打开
        std::string line;
        while (std::getline(fin, line)) {//行读取到line
            int pos = line.find('=');
            if (pos != -1) {//找到位置，解析键值对
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                dict[key] = value;
            }
        }
    }
    else
    {
        std::cout << " 未能打开词典文件\n"; 
    }
    return dict;
}

// 保存词典到文件
void saveDict(const std::string& filename, const std::map<std::string, std::string>& dict) {
    std::ofstream fout(filename);
    for (const auto& [key, val] : dict) {
        fout << key << "=" << val << "\n";//存储模式  xx=yy
    }
}

int main() {
    const std::string dictFile = "dict.txt";//字典文件
    std::map<std::string, std::string> dict = loadDict(dictFile);//加载

    // 如果文件为空，添加默认词典
    if (dict.empty()) {
        dict = {
            {"variable", "变量"},
            {"function", "函数"},
            {"class", "类"},
            {"object", "对象"},
            {"array", "数组"},
            {"pointer", "指针"},
            {"loop", "循环"},
            {"condition", "条件"},
            {"compile", "编译"},
            {"debug", "调试"},
            {"变量", "variable"},
            {"函数", "function"},
            {"类", "class"},
            {"对象", "object"},
            {"数组", "array"},
            {"指针", "pointer"},
            {"循环", "loop"},
            {"条件", "condition"},
            {"编译", "compile"},
            {"调试", "debug"}
        };
        saveDict(dictFile, dict);
    }

    // 查询词典
    std::string word;
    std::cout << "请输入英文或中文单词，输入 exit 退出：" << std::endl;
    while (std::cin >> word) {
        if (word == "exit") break;//退出

        auto it = dict.find(word);//查找
        if (it != dict.end()) {//找到
            std::cout << "翻译：" << it->second << std::endl;
        } else {//未找到
            std::cout << "未收录该词，是否添加？(y/n): ";
            char choice;
            std::cin >> choice;
            if (choice == 'y' || choice == 'Y') {
                std::cout << "请输入翻译：";
                std::string trans;
                std::cin >> trans;
                dict[word] = trans;//存储该键值对
                saveDict(dictFile, dict);//保存到本地
                std::cout << "已添加并保存\n";
            }
        }
        std::cout << "\n请输入英文或中文单词,输入 exit 退出：" << std::endl;
    }

    return 0;
}
