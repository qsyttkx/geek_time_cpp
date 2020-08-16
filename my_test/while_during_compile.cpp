// 编译期编程

#include <iostream>
#include <type_traits>

using std::integral_constant; // 类型推导工具

// 编译器循环语句
template<bool condition, typename Body>
struct WhileLoop;

template <typename Body>
struct WhileLoop<true, Body> {
    typedef typename WhileLoop<Body::cond_value, typename Body::next_type>::type type;
};

template <typename Body>
struct WhileLoop<false, Body> {
    typedef typename Body::res_type type;
};

template <typename Body>
struct While {
    typedef typename WhileLoop<Body::cond_value, Body>::type type;
};

// 使用编译期循环
template <int result, int n>
struct SumLoop {
    static const bool cond_value = n != 0;
    static const int res_value = result;
    typedef integral_constant<int, res_value> res_type;
    typedef SumLoop<result + n, n - 1> next_type;
};

template <int n>
struct Sum {
    typedef SumLoop<0, n> type;
};

int main()
{
    // 编译期间完成了1到10的累加
    // 0x37
    int a = While<Sum<10>::type>::type::value;
    printf("%d\n", a);
}
