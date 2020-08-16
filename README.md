2020年8月14、15两天报名参加一个松山湖的C++培训，主要讲一些现代C++的概念、技巧，以及一些工具。讲师是吴咏炜（wuyongwei@gmail.com）。

以下是一些总结：

- [技巧](#技巧)
  - [优先使用栈内存](#优先使用栈内存)
  - [RAII：自动管理资源](#raii自动管理资源)
  - [放心使用+来连接字符串](#放心使用来连接字符串)
  - [不推荐使用const std::string&作为函数入参](#不推荐使用const-stdstring作为函数入参)
  - [自定义字面量](#自定义字面量)
  - [编译期编程](#编译期编程)
  - [函数返回多参数](#函数返回多参数)
  - [奇怪的语法解析](#奇怪的语法解析)
- [工具](#工具)
  - [Clang-Tidy](#clang-tidy)
  - [xeus-cling](#xeus-cling)
  - [Compiler Explorer](#compiler-explorer)
  - [Valgrind](#valgrind)

# 技巧

讲师讲了一些C++技巧，有些是C++98就能做到的，有些是C++11甚至17才有的特性。

## 优先使用栈内存

因为堆内存的申请、释放背后操作系统需要进行比栈内存复杂的多的处理，使用堆内存相对而言是较慢的。所以只要允许我们应该尽可能优先使用栈内存，除非对象非常大或者不能确定其大小等情况。

```c++
int main()
{
    array arr { 1, 2, 3, 4, 5 };    // arr在栈上，arr里的元素也在栈上
    vector vec { 2, 3, 4, 5, 6 };   // vec本身在栈上，但vec里的元素是申请在堆上的
    // 对于array和vector都能实现的功能，用array实现通常可以获得更高性能。
}
```

`★ C++17中允许定义变量时省略模板参数`

## RAII：自动管理资源

RAII（Resource Acquisition Is Initialization）广泛用在管理资源上。这个名字虽然是获得即初始化，但这个技巧的**重点是析构**行为。

讲师说C++有一个很重要的特性就是  
<font size="8">}</font>  
意思是在对象在其作用域末尾时会被析构。所以我们可以利用这个特性来做一些有意思的事情。

例如加锁，按C的风格可能会写成：

```c++
std::mutex mtx;

void MyThread()
{
    mtx.lock();
    if (xxx) {
        mtx.unlock();   // 在每个返回分支都要解锁
        return;
    }
    mtx.unlock();
}
```

每次新增返回分支都需要进行加锁，如果忘记就容易写出BUG。使用RAII的方式则可以：

```c++
std::mutex mtx;

void MyThread()
{
    std::lock_guard<std::mutex> autoLock(mtx);
    if (xxx) {
        return;
    }
}
```

`autoLock`在构造的时候锁住mtx，析构时释放mtx。无论在哪个分支返回，都会触发它的析构。于是就不必再在每个分支都做解锁的处理。

很自然的我们可能也想搞点别的用途比如管理文件、网络连接、线程等等。但根据我司的编程规范，由于构造函数没有返回值，构造函数中不允许执行可能失败的操作。那么在构造函数中打开文件、连接、创建线程之类的可能会有规范问题。这个规则看似有道理但实际上执行起来有很大难度，因为严格来说像给字符串赋值这种操作也是可能导致失败的：

```c++
MyClass(const char* s) : str(s) {}
```

当然我们也有规避的方式，要么提供一个检查函数（如`ifstream`），要么把构造过程拆分为几乎不做任何事的构造函数+初始化函数的形式。

好消息随着编译器的发展C++异常的性能问题逐渐得到优化，我们逐渐放开了使用C++异常机制的限制。一旦资源获取失败我们可以在构造函数中抛出异常，但我们需要确保我们的代码满足**异常安全**，使得构造失败时也能让程序恢复构造前的状态。关于异常安全可以参考这篇博文：[C++异常安全的思考](https://www.cnblogs.com/zgfLawliet/p/3417308.html)。

这些都要求在析构函数中保证，释放没有成功获取到的资源也是合法的（如`delete nullptr`）。

## 放心使用+来连接字符串

在C++98的年代，如果写出

```c++
std::string s = std::string("Hello ") + name + "!";
```

这样的代码是会被大佬喷的，因为这句代码虽然符合直觉，但实际上是非常低效的：

1. 构造出一个string对象，然后拷贝"Hello "到字符串的内存；
2. 上一个字符串与`name`加运算，构造出一个新的string对象，然后把这两个的内存拷贝到了新字符串的内存里；
3. 上一个字符串与`"!"`加运算，构造出一个新的string对象，然后把这两个的内存拷贝到了新字符串的内存里；
4. 把等号右边的值拷贝到左边的字符串`s`里（又产生了一次拷贝）。

字符串每加一次就构造一个新对象然后整个拷贝一次内存，因此不得不换成这种丑陋的写法：

```c++
std::string s("Hello ");
s.append(name);
s.append("!");
```

以上代码只构造了一个string对象，性能较高。

C++11引入了新的构造类型：移动构造。并对std::string做了改造，在字符串相加时不再是调用赋值构造而是移动构造，存放string数据的内存不再需要每次都分配、复制。因此**在现代C++中字符串相加的效率和append是基本类似的**。

## 不推荐使用const std::string&作为函数入参

我们经常这么用，是希望告诉调用者此函数不会修改这个字符串的内容，同时想要避免内存拷贝等低效的问题。但这个写法有时候并不能提高效率。

```c++
void MyFunc(const std::string& str);
```

调用`MyFunc`时，如果我们传入的参数就是一个`std::string`那其实没什么问题。但如果我们传入的参数是字符串字面量，或者是字符指针，那就会出发一次隐式的类型转换，还是会导致不必要的构造和内存拷贝：

```c++
std::string a("abc");
const char* b = "bcd";

MyFunc(a);      // 没有问题
MyFunc(b);      // 触发类型转换，构造了一个string类型的将亡值并拷贝了b指向的内存
MyFunc("cde");  // 触发类型转换，构造了一个string类型的将亡值并拷贝了"cde"指向的内存
```

**替代方案1**：使用`const char *`作为入参类型：

```c++
void MyFunc(const char* str);
```

但这样我们如果要在MyFunc里做一堆字符串操作的话难免要在MyFunc的内部再把它实例化为string。

**替代方案2**：使用std::string_view作为入参类型：

```c++
void MyFunc(std::string_view str);
```

C++17引入了string_view。正如其名，这个类用来对字符串进行观察。它有部分类似string的方法，但它不拷贝字符串数据，占用体积极小（仅16字节）因此值传递即可。

## 自定义字面量

C++11引入的特性。我们可以在字面量上加入自定义的后缀，虽然没啥实际作用但是感觉有点意思，例子：

```c++
class Length {
    Length(double m) : val(m) {}
    friend Length operator""_m(long double v);
    friend Length operator""_cm(long double v);
    friend Length operator""_mm(long double v);
    friend Length operator""_km(long double v);
    friend ostream& operator<<(ostream& o, const Length& l);
private:
    double val;
};

Length operator""_m(long double v) {return Length(v);}
Length operator""_cm(long double v) {return Length(v/100);}
Length operator""_mm(long double v) {return Length(v/1000);}
Length operator""_km(long double v) {return Length(v*1000);}
ostream& operator<<(ostream& o, const Length& l) {return o << l.val << "m";}

int main()
{
    cout << 10.0_m << endl;
    cout << 100.0_cm << endl;
    cout << 1.0_km << endl;
    cout << 1000.0_mm << endl;
}
```

运行得到输出：

```
10m
1m
1000m
1m
```

## 编译期编程

有时候我们的程序可能会需要一些初始化一些变量的值，但在之后我们都不再修改它们。在C++17中引入的`constexpr`可以把这部分计算放到编译期间，从而让你的程序运行起来更快一些：

```c++
#include <cstdio>

constexpr int Fab(int a)
{
    if (a <= 2) {
        return 1;
    }
    return Fab(a - 1) + Fab(a - 2);
};

int main()
{
    // 我们想要获取斐波那契数列的某几个值
    constexpr int a[] = { Fab(3), Fab(10), Fab(15), Fab(20) };
    for (auto i : a) {
        printf("%x ", i);
    }
    return 0;
}
```

使用VisualStudio 2017编译以上代码（设置语言标准为C++17），反汇编后可以看到a[]在声明的时候右边的函数返回值都被替换为了立即数：

```
00364348  mov         ecx,offset _1BF4426A_main@cpp (036B004h)  
0036434D  call        @__CheckForDebuggerJustMyCode@4 (0361212h)  
    constexpr int a[] = { Fab(3), Fab(10), Fab(15), Fab(20) };
00364352  mov         dword ptr [a],2  
00364359  mov         dword ptr [ebp-14h],37h  
00364360  mov         dword ptr [ebp-10h],262h  
00364367  mov         dword ptr [ebp-0Ch],1A6Dh  
    for (auto i : a) {
0036436E  lea         eax,[a]  
00364371  mov         dword ptr [ebp-24h],eax  
```

fab(3)、fab(10)、fab(15)、fab(20)这四个函数返回值被优化成了0x37、0x262、0x1a6d。也就是说程序在运行时直接拿来就用，不再需要计算。

当然不是C++17也是可以做编译期编程的，只不过要麻烦一些，我们可以用模板展开来实现。感兴趣可以参考[while_during_compile.cpp](my_test/while_during_compile.cpp)里的代码。

## 函数返回多参数

在C++17中，我们可以更灵活的使用tuple：

```c++
#include <iostream>
#include <tuple>

using namespace std;

auto MyFunc()
{
    return tuple{1, true, 5.0};
}

int main()
{
    auto [a0, a1, a2] = MyFunc();
    cout << a0 << a1 << a2 << endl;
}
```

## 奇怪的语法解析

C++在解析一条语句时，如果它可以被解析为函数声明，那它就会优先被解析为函数声明。例子如下：

```c++
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

// 这个会解析为一个函数声明...
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
```

# 工具

## Clang-Tidy

一款格式检查工具（类似我们用的CheckStyle）。

## xeus-cling

一款C++的Jupyter内核，可以让你在Jupyter Notebook上像脚本一样写C++（拿来测试一些想法还是挺便利的）。

[点我在线尝试](https://mybinder.org/v2/gh/jupyter-xeus/xeus-cling/stable?filepath=notebooks/xcpp.ipynb)

## Compiler Explorer

https://godbolt.org/z/oKc1YT

一个在线反编译网站，我觉得在内网用这个有BCG风险。

## Valgrind

可以检查内存泄漏、越界访问、野指针等问题（确实好用，就是运行慢）。