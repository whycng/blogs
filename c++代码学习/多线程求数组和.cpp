#include <iostream>
#include <vector>
#include <thread>
#include <cstdlib>
#include <ctime>
#include <chrono>

const int N = 1000000; // 数组大小
const int tCOUNTs = 4; // 线程数量

std::vector<int> data(N); // 全局数组
int psuns[tCOUNTs]; // 每个线程自己的和

// 每个线程计算一段数据和
void compute_sum(int thread_id, int start, int end) {
    int sum = 0;
    for (int i = start; i < end; ++i) {
        sum += data[i];
    }
    psuns[thread_id] = sum;   
}

int main() {
    // 生成随机数据
    std::srand((unsigned)std::time(0));
    for (int i = 0; i < N; ++i) {
        data[i] = std::rand() % 20 - 10 ;   
    }

    std::vector<std::thread> threads;//线程
    int plen = N / tCOUNTs;//每一块数组大小

    auto start_time = std::chrono::steady_clock::now();//计时

    // 启动多个线程
    for (int i = 0; i < tCOUNTs; ++i) {
        int start = i * plen;
        int end = start + plen;
        if (i == tCOUNTs - 1) end = N;
        threads.push_back(std::thread(compute_sum, i, start, end));
    }

    // 等待线程结束
    for (int i = 0; i < tCOUNTs; ++i) {
        threads[i].join();//等待对应线程运行完毕
    }

    // 汇总所有部分和
    int all_sum = 0;
    for (int i = 0; i < tCOUNTs; ++i) {
        all_sum += psuns[i];
    }

    auto end_time = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end_time - start_time;

    //单线程计时
    auto start_time2 = std::chrono::steady_clock::now();//计时
    int all_sum2 = 0;
    for(int i=0;i<data.size();++i)
    {
        all_sum2 += data[i];
    }
    auto end_time2 = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> elapsed2 = end_time2 - start_time2;

    std::cout << "数组总和: " << all_sum << std::endl;
    std::cout << "线程数量: " << tCOUNTs << std::endl;
    std::cout << "多线程耗时(ms): " << elapsed.count() << " ms" << std::endl;
    std::cout << "单线程耗时(ms): " << elapsed2.count() << " ms" << std::endl;

    return 0;
}
