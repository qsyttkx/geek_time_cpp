模板：“鸭子类型”，不关注继承关系，只关心有相同的动作

对于个别没有通用的动作的类，可以使用“特化”来提供一个特别的版本（相关： https://harttle.land/2015/10/03/cpp-template.html）

折叠表达式：
```c++
template <typename... Args>
void print(Args... args)
{
    (std::cout << ... << args) << std::endl;
}
```


http://godbolt.org/z/oKc1YT