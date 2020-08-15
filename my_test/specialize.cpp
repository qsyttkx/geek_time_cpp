#include <iostream>

using std::cout;
using std::endl;

// 通用版本
template<typename T>
T Add(const T& a, const T& b)
{
    return a + b;
}

struct Comp
{
    double r;
    double m;
};

std::ostream& operator<<(std::ostream& out, const Comp& c)
{
    out << c.r << ", " << c.m;
    return out;
}

// 然而Comp不支持+，如果我不给Comp重载+的话可以
template<>
Comp Add(const Comp& a, const Comp& b)
{
    cout << "template spec" << endl;
    return {a.r + b.r, a.m + b.m};
}

// 当然上面的模板函数也可以用重载（实际上函数优先用重载比较好，两者都存在时也是优先用重载）
Comp Add(const Comp& a, const Comp& b)
{
    cout << "Func override" << endl;
    return {a.r + b.r, a.m + b.m};
}

int main() {
    cout << Add(1, 3) << endl;
    cout << Add(Comp{1, 0}, Comp{2, 1}) << endl;
}