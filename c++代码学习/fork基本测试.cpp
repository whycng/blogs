#include <iostream>
#include <unistd.h>
#include <vector>

int main() {
    pid_t pid = fork();

    std::cout << "我是PID: " << getpid() << ", 我的父亲是: " << getppid() << ", fork返回的是: " << pid << std::endl;


    if (pid < 0) {
        std::cerr << "ERROR !" << std::endl;
        return 1;
    } else if (pid == 0) {
        // 子进程 
        std::cout << "子进程 pid:" << getpid() << std::endl;
        std::vector<int> v = {1};
        std::cout << "子进程 输出: " << v.size() << std::endl;
    } else {
        // 父进程
        std::cout << "父进程 pid:" << getpid() << " 子进程 pid:" << pid << std::endl;
        std::vector<int> v = {1,1,1,1,1,1,1};
        std::cout << "父进程 输出: " << v.size() << std::endl;
    }

    return 0;
}
