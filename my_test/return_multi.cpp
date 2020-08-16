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
