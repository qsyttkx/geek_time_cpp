# 松湖培训第一天

讲师：

吴咏炜  
wuyongwei@gmail.com  
wuyongwei1@huawei.com  

能用栈的尽量用栈

用好`}`（析构）

RAII：Resource Acquisition Is Initialization  
名字起的不好，这个技巧的重点是析构行为，一般实现一个帮助类来管理目标类（或者资源）

enum class比enum的好处是引用的时候必须是`枚举类名::枚举名`，避免名字冲突

三法则：如果需要用户定义的`析构函数`、用户定义的`拷贝构造`、用户定义的`拷贝赋值`，则它几乎三者同时都需要。

不推荐使用const string&，如果确实不需要修改，推荐使用string_view或者const char*。因为使用const string&的话如果调用者传入的是字符串字面量，会导致一次构造和析构而这也许是不必要的。（那如果我函数里面要调用string的方法呢）

array作为C数组的替代品，是在栈上分配内存的（所以性能比vector好）

在线尝试cling： https://mybinder.org/v2/gh/jupyter-xeus/xeus-cling/stable?filepath=notebooks/xcpp.ipynb

对于异常，现代C++编译器不关注函数抛出的异常是什么，只关注函数抛出异常还是确保不抛出异常。（那么上层函数怎么知道调用的函数抛啥呢，文档约定？）

用户自定义的字面量（待补充）