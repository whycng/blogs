//线程池示例
#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

class ThreadPool {
public:
    ThreadPool(int maxThreads = 5, int maxQueue = 20)
        : MmaxThreads(maxThreads), MmaxQueue(maxQueue), stop(false) {}

    // 启动线程池
    void start() {
        for (int i = 0; i < MmaxThreads; ++i) { 
            //thread 传入 函数指针、以及对象指针;此处是 workerThreadFunc函数和this对象指针
            workers.emplace_back(&ThreadPool::workerThreadFunc, this); 
        }
    }

    // 提交任务（函数），阻塞直到有位置
    void submit(std::function<void()> task) {
        //加锁，unique_lock 离开作用域时自动释放，保护task压入任务
        std::unique_lock<std::mutex> lock(mtx);
        while (tasks.size() >= MmaxQueue) {//如果满了，主线程进行等待
            std::cout << "[等待中] 任务队列已满，等待...\n";
            cond_full.wait(lock); // 释放锁并阻塞主线程，直到被唤醒 
        }
        tasks.push(task);//压入任务队列
        cond.notify_one(); // 通知线程去工作
    }

    // 停止线程池并回收资源
    void shutdown() {
        {
            std::unique_lock<std::mutex> lock(mtx);
            stop = true;
        }
        cond.notify_all(); // 通知所有线程退出
        for (auto& t : workers) {
            if (t.joinable()) t.join();//线程回收
        }
    }

private:
    const int MmaxThreads;
    const int MmaxQueue;
    std::vector<std::thread> workers;  // 工作线程
    std::queue<std::function<void()>> tasks; // 任务队列
    std::mutex mtx;//锁
    std::condition_variable cond;//条件变量， 任务队列非空 
    std::condition_variable cond_full; // 任务队列非满 
    bool stop;

    void workerThreadFunc() {
        while (true) {
            std::function<void()> task;

            {//锁作用于该作用域，任务执行已经不需要加锁
                std::unique_lock<std::mutex> lock(mtx);
                //如果没有任务，则等待通知
                cond.wait(lock, [this] {//等价于先判断条件是否为真，为真跳过，假则阻塞等待
                    return stop || !tasks.empty(); 
                });
                
                //如果收到停止信号，或者任务队列为空，则退出
                if (stop && tasks.empty()) return;

                //从任务队列中取任务
                task = tasks.front();
                //任务队列弹出
                tasks.pop();
            }
            // 执行任务
            task();
            // 通知主线程：队列空出一个位置                          
            cond_full.notify_one();//唤醒一个         
        }
    }

};

 
int main() {

    std::cout << "主线程：" << std::this_thread::get_id() << std::endl;

    ThreadPool pool(5, 20);
    pool.start(); // 启动线程池

    // 提交30个任务 
    for (int i = 1; i <= 30; ++i) {
        pool.submit([i]() {
            std::cout << "任务 " << i << " 正在运行于线程 " << std::this_thread::get_id() << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        });
    }

    std::this_thread::sleep_for(std::chrono::seconds(5));
    pool.shutdown(); // 关闭线程池
    std::cout << "所有任务完成，线程池已关闭" << std::endl;
    return 0;
}
