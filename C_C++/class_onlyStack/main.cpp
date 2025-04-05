#include <iostream>
using namespace std;

// 设计一个类，在类外面只能在栈上创建对象
// 设计一个类，在类外面只能在堆上创建对象
class A
{
public:
    static A GetStackObj(int a1, int a2)
    {
        A aa(a1, a2);
        return aa;
    }

    static A* GetHeapObj()
    {
        return new A;
    }

    void Print(){
        cout << _a1 << " " << _a2 << endl;
    }
private:
    A(int a1 = 1, int a2 = 2)
    :_a1(a1)
    ,_a2(a2)
    {}

private:
    int _a1;
    int _a2;
};

int main()
{
    //static A aa1;   //  静态区
    //A aa2;          //  栈
    //A* ptr = new A; //  堆

    //A::GetHeapObj(); // 只能是堆
    A a1 = A::GetStackObj(10, 20); // 只能是栈
    a1.Print();

    return 0;
}