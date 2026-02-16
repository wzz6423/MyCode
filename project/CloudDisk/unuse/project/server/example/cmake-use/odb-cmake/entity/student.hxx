#pragma once
#include <string>
#include <cstddef>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <odb/nullable.hxx>
#include <odb/core.hxx>

#pragma db object
class Student
{
public:
    Student() = default;
    Student(const unsigned long &sn, const std::string &name, const unsigned short age, const unsigned long class_id)
        : _sn(sn), _name(name), _age(age), _class_id(class_id) {}

    void set_sn(const unsigned long &sn) { _sn = sn; }
    unsigned long sn() const { return _sn; }

    void set_name(const std::string &name) { _name = name; }
    const std::string &name() const { return _name; }

    void set_age(const unsigned short &age) { _age = age; }
    odb::nullable<unsigned short> age() const { return _age; }

    void set_class_id(const unsigned long &class_id) { _class_id = class_id; }
    unsigned long class_id() const { return _class_id; }

private:
    friend class odb::access; // 让 ODB 访问私有成员
#pragma db id auto            // 使用 auto 让 ODB 自动生成 ID
    unsigned long _id;
#pragma db unique
    unsigned long _sn;
    std::string _name;
    odb::nullable<unsigned short> _age; // 使用 nullable 来表示年龄可能为空, 获取值使用 * (类似 std::optional)
#pragma db index
    unsigned long _class_id;
};

#pragma db object
class Classes
{
public:
    Classes() = default;
    Classes(const std::string &name)
        :_name(name) {}

    void set_id(const unsigned long &id) { _id = id; }
    unsigned long id() const { return _id; }

    void set_name(const std::string &name) { _name = name; }
    const std::string &name() const { return _name; }

private:
    friend class odb::access; // 让 ODB 访问私有成员
#pragma db id auto            // 使用 auto 让 ODB 自动生成 ID
    unsigned long _id;
    std::string _name;
};

// 查询所有学生信息, 并显示班级名称
// 起别名(Classes = classes)
#pragma db view object(Student)                                    \
    object(Classes = classes : Student::_class_id == classes::_id) \
        query((?))
struct classes_student
{
#pragma db column(Student::_id)
    unsigned long student_id;
#pragma db column(Student::_sn)
    unsigned long student_sn;
#pragma db column(Student::_name)
    std::string student_name;
#pragma db column(Student::_age)
    odb::nullable<unsigned short> student_age;
#pragma db column(classes::_name)
    std::string classes_name;
};

// 只查询学生姓名, (?)表示外部调用时传入的过滤筛选条件
#pragma db view query("select name from Student" + (?))
struct student_name
{
    std::string name;
};

// odb -d mysql --std c++11 --generate-query --generate-schema --profile boost/date-time student.hxx