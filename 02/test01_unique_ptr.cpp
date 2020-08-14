#include <utility>  // std::swap/move
#include "shape.h"  // shape/shape_type/create_shape

template <typename T>
class smart_ptr {
public:
    explicit smart_ptr(T* ptr = nullptr)
        : ptr_(ptr) {}
    ~smart_ptr()
    {
        delete ptr_;
    }
    smart_ptr(smart_ptr&& other)
    {
        ptr_ = other.release();
    }

    // 值传递可以触发一次构造函数（而且左值就是拷贝构造，右值就是移动构造）
    // 以便于函数退出时析构自动释放老的资源
    smart_ptr& operator=(smart_ptr rhs)
    {
        rhs.swap(*this);
        return *this;
    }

    // 用户定义析构函数、拷贝构造、拷贝赋值时，存在阻止移动构造、移动赋值的隐式定义

    T* get() const { return ptr_; }
    T& operator*() const { return *ptr_; }
    T* operator->() const { return ptr_; }
    operator bool() const { return ptr_; }
    T* release()
    {
        T* ptr = ptr_;
        ptr_ = nullptr;
        return ptr;
    }
    void swap(smart_ptr& rhs)
    {
        using std::swap;
        swap(ptr_, rhs.ptr_);
    }

private:
    T* ptr_;
};

int main()
{
    smart_ptr<shape> ptr1{create_shape(shape_type::circle)};
    // smart_ptr<shape> ptr2{ptr1};             // Cannot compile
    smart_ptr<shape> ptr3;
    // ptr3 = ptr1;                             // Cannot compile
    ptr3 = std::move(ptr1);                  // OK
    smart_ptr<shape> ptr4{std::move(ptr3)};  // OK
}
