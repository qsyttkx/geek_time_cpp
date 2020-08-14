#include "shape.h"  // shape/shape_type/create_shape

class shape_wrapper {
public:
    explicit shape_wrapper(shape* ptr = nullptr)
        : ptr_(ptr) {}
    ~shape_wrapper()
    {
        delete ptr_;
    }
    shape* get() const { return ptr_; }

private:
    shape* ptr_;
};

void foo()
{
    // 用栈上对象的生命周期来括住堆里的对象，使得new出来的对象在函数结束的时候释放掉
    shape_wrapper ptr_wrapper(create_shape(shape_type::circle));
}

int main()
{
    foo();
}

// shape_wrapper的问题：拷贝或者赋值会引起多次析构导致崩溃