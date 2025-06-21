
// 并发集合
// 当多个线程同时访问一个容器，若不加锁，则出现数据竞争
//shared_lock 共享锁，读锁，同时读是可以，其他线程不能写
//unique_lock 独占锁，写锁，写时候独占，其他线程不能读也不能写
 
#include <iostream>
#include <unordered_set>
#include <shared_mutex>
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>

// 线程安全集合 
template <typename T>
class ThSet {
public:
    // 插入元素（写锁）
    bool insert(const T& value) {
        std::unique_lock lock(m_mutex);
        return m_set.insert(value).second;
    }

    // 删除元素（写锁）
    bool erase(const T& value) {
        std::unique_lock lock(m_mutex);
        return m_set.erase(value) > 0;
    }

    // 查找元素（读锁）
    bool contains(const T& value) const {//const保证不修改函数内元素值，但锁可被更改，mutable
        std::shared_lock lock(m_mutex);
        return m_set.find(value) != m_set.end();
    }

    // 获取集合大小（读锁）
    int size() const {// 
        std::shared_lock lock(m_mutex);
        return m_set.size();
    }

private:
    mutable std::shared_mutex m_mutex; // 可读写共享的锁，锁在const函数内可更改
    std::unordered_set<T> m_set; // 内部哈希集合
};

// 主函数：多线程测试
int main() {
    ThSet<int> thset;

    const int tcounts = 4;//线程数
    const int nums = 9999; 

    std::vector<std::thread> threads;

    // 多线程并发插入
    for (int i = 0; i < tcounts; ++i) {
        threads.emplace_back([&thset, i]() {//lambda，&引用传值；emplace_back 原地构造，压入线程函数
            for (int j = 0; j < nums; ++j) {
                thset.insert(i * nums + j);
            }
        });
    }

    for (auto& t : threads) t.join();//主线程阻塞

    std::cout << "集合大小: " << thset.size() << std::endl;

    // 多线程并发查找
    std::atomic<int> hit_count{0};//计数；单纯使用int同样考虑要加锁
    threads.clear();

    for (int i = 0; i < tcounts; ++i) {
        threads.emplace_back([&thset, &hit_count, i]() {//同样的，原地构造，线程函数并压入
            for (int j = 0; j < nums; ++j) {
                if (thset.contains(i * nums + j)) {
                    hit_count++;
                }
            }
        });
    }

    for (auto& t : threads) t.join();//主线程阻塞

    std::cout << "成功找到元素数量: " << hit_count.load() << std::endl;

    return 0;
}
