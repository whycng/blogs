#include <iostream> 
#include <deque>

class Stack {
private:
    std::deque<int> dq;
public:
    void push(int x) {
        dq.push_back(x);
    }
    void pop() {
        if (!dq.empty()) dq.pop_back();
    }
    int top() {
        if (!dq.empty()) return dq.back();
        throw std::out_of_range("Stack is empty");
    }
    bool empty() {
        return dq.empty();
    }
};

int main()
{
    Stack s;
    s.push(1);
    s.push(2);
    std::cout << s.top() << std::endl;  
    s.pop();
    std::cout << s.top() << std::endl;  
    s.pop();
    std::cout << s.empty() << std::endl;  
}
