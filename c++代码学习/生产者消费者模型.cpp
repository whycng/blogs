// 生产者消费者模型
#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

/*
    std::future; promise; 
*/

const int BufferSize = 10; // 仓库最多能放 10 件产品
const int AllProd = 20; // 本次总共生产 20 件

std::queue<int> buffer; // 仓库容量
std::mutex mtx; // 互斥量，保护 buffer
std::condition_variable cv; // 条件变量，用于等待/通知
bool done = false;  // 生产者结束标记

// 生产者：进行生产，把20 个整数放进仓库
void producer() {
    for (int i = 0; i < AllProd; ++i) {
        std::unique_lock<std::mutex> lock(mtx);

        // 仓库满了就等待消费者消费,条件为真则跳过，条件为假则wait阻塞
        cv.wait(lock, [] { 
            return buffer.size() < BufferSize;
        });

        buffer.push(i);//生产
        std::cout << "生产者进行生产:" << i << std::endl;

        // 通知消费者  
        cv.notify_one();   
    }

    // 所有产品都生产完毕 
    {
        std::lock_guard<std::mutex> lock(mtx);
        done = true;
    }
    cv.notify_all();//通知消费者
}

// 消费者：一直消费，直到确定生产者完工且仓库为空
void consumer() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);

        // 仓库非空跳过，仓库空则进行阻塞等待，等待生产者进行生产
        cv.wait(lock, [] { return !buffer.empty() || done; });

        if (buffer.empty() && done)//仓库空并且生产者生产完毕，则退出 
        {
            break;  
        }

        int i = buffer.front();//取货消费
        buffer.pop();//仓库弹货
        std::cout << "消费者进行消费:" << i << std::endl;

        //通知生产者
        cv.notify_one();  
    }
}

int main() {
    std::thread prod(producer);
    std::thread cons(consumer);

    prod.join();
    cons.join();

    std::cout << "全部结束" << std::endl;
    return 0;
}
