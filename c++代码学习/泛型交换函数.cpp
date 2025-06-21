
#include <iostream>
#include <vector>
 

// 泛型交换函数
template<typename T>
void mySwap(T& a, T& b) {
    T temp = a;
    a = b;
    b = temp;
}

int main() {
    int x = 1, y = 2;
    std::cout << " x=" << x << ", y=" << y << std::endl;
    mySwap(x, y);
    std::cout << " x=" << x << ", y=" << y << std::endl;

    double xd = 1.2, yd = 2.2;
    std::cout << " xd=" << xd << ", yd=" << yd << std::endl;
    mySwap(xd, yd);
    std::cout << " xd=" << xd << ", yd=" << yd << std::endl;

    std::vector<int> v = {1, 2, 3};
    std::vector<int> w = {9, 10, 11};
    std::cout << " v[0]=" << v[0] << ", w[0]=" << w[0] << std::endl;
    mySwap(v, w);
    std::cout << " v[0]=" << v[0] << ", w[0]=" << w[0] << std::endl;

    return 0;
}
