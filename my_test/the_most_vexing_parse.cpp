#include <iostream>

struct MyClass {
    MyClass() = default;
    MyClass(int v) : v(v) {}
    int v = 3;
};

struct MyTest {
    MyTest(MyClass c):c(c) {}
    MyClass c;
};

MyTest my_test0(MyClass());

int b = 6;
// 这个会解析为一个函数声明...
MyTest my_test1(MyClass(b));

// 这个会解析为一个对象
MyTest my_test2(MyClass(2));

// 这个会解析为一个对象
MyTest my_test3{MyClass(1)};

int main() {
    // std::cout << my_test0.c.v << std::endl;  // Compile error
    // std::cout << my_test1.c.v << std::endl;  // Compile error
    std::cout << my_test2.c.v << std::endl;
    std::cout << my_test3.c.v << std::endl;
}