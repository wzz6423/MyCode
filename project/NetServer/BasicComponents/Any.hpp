#pragma once

#include <iostream>
#include <optional>
#include <algorithm>
#include <typeinfo>
#include <unistd.h>

// Any 类不能是模板类, 否则编译的时候 Any<int> a,  Any<float> b, 需要传类型作为模板参数, 即在使用的时候就要确定其类型
// 不可以, 因为保存在 Content 中的协议上下文在定义 Any 对象时是不知道他们的协议类型, 无法传递类型作为模板参数
// 因此考虑 Any 内部设计一个模板容器 placeholder 类, 可以保存各种类型数据
// 而因为在 Any类 中无法定义这个 placeholder 对象或指针，因为 Any 也不知道这个类要保存什么类型的数据, 因此无法传递类型参数
// 所以, 定义一个基类 holder, 让 placeholder 继承于 holder,  Any 类保存父类指针即可
// 当需要保存数据时, 则 new 一个带有模板参数的子类 placeholder 对象出来保存数据, 然后让  Any类中的父类指针, 指向这个子类对象

// 通用类型
class Any
{
public:
    Any()
        : _content(nullptr)
    {
    }

    template <typename T>
    Any(const T &val)
        : _content(new placeholder<T>(val))
    {
    }

    Any(const Any &other)
        : _content(other._content ? other._content->Clone() : nullptr)
    {
    }

    Any &Swap(Any &other)
    {
        std::swap(_content, other._content);
        return *this;
    }

    // 这三个函数用于优化
    Any(Any &&other) noexcept
        : _content(other._content)
    {
        other._content = nullptr;
    }
    template <typename T, typename... Args>
    void emplace(Args &&...args)
    {
        delete _content;
        _content = new placeholder<T>(std::forward<Args>(args)...);
    }
    Any &operator=(Any &&other) noexcept
    {
        if (this != &other)
        {
            delete _content;
            _content = other._content;
            other._content = nullptr;
        }
        return *this;
    }

    // 返回子类对象保存数据的指针
    template <typename T>
    std::optional<T*> Get()
    {
        // 想要获取的数据类型必须和保存的数据类型一致
        if (_content && typeid(T) == _content->Type())
        {
            return &static_cast<placeholder<T> *>(_content)->_val;
        }
        return std::nullopt;
    }

    // 赋值运算符重载
    template <typename T>
    Any &operator=(const T &val)
    {
        // 为 val 构造一个临时通用容器, 然后与当前容器自身指针交换, 临时对象释放时就释放掉原先资源
        Any(val).Swap(*this);
        return *this;
    }

    Any &operator=(const Any &other)
    {
        Any(other).Swap(*this);
        return *this;
    }

    ~Any()
    {
        delete _content;
    }

private:
    class holder
    {
    public:
        virtual ~holder() {}
        virtual const std::type_info &Type() = 0;
        virtual holder *Clone() = 0;
    };

    template <typename T>
    class placeholder : public holder
    {
    public:
        placeholder(const T &val)
            : _val(val)
        {
        }

        // 获取子类对象保存的数据类型
        virtual const std::type_info &Type()
        {
            return typeid(T);
        }

        // 针对当前对象克隆出一个新的子类对象
        virtual holder *Clone()
        {
            return new placeholder(_val);
        }

    public:
        T _val;
    };

private:
    holder *_content;
};