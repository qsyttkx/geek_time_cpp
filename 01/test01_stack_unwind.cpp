#include <stdio.h>  // puts

class Obj {
public:
    Obj() { puts("Obj()"); }
    ~Obj() { puts("~Obj()"); }
};

void foo(int n)
{
    Obj obj;
    if (n == 42) {
        throw "life, the universe and everything";
    }
}

int main()
{
    try {
        foo(41);
        foo(42);
    }
    catch (const char* s) {
        puts(s);
    }
}

/*
输出：

Obj()
~Obj()
Obj()
~Obj()
life, the universe and everything

构造函数中抛出异常，但是在try块的结尾处调用了析构函数，然后进入了catch块。
*/