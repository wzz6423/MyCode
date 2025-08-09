#include <memory>
#include <iostream>

#include <odb/database.hxx>
#include <odb/mysql/database.hxx>
#include <gflags/gflags.h>

#include "student.hxx"
#include "student-odb.hxx"

DEFINE_string(host, "127.0.0.1", "MySQL host");
DEFINE_int32(port, 0, "MySQL port");
DEFINE_string(db, "TestDB", "MySQL datebase default name");
DEFINE_string(user, "wzz", "MySQL user");
DEFINE_string(password, "passwd", "MySQL user password");
DEFINE_string(charset, "utf8", "MySQL charset");
DEFINE_int32(max_poll_number, 3, "MySQL max connection pool size");

auto insert_class(odb::mysql::database &db) -> void
{
    try
    {
        // 1.获取事务操作对象, 开启事务
        odb::transaction t(db.begin()); // 自动开启事务
                                        // std::unique_ptr<odb::mysql::transaction_impl> t(db.begin()); // 手动开启事务

        // 错误, 要求是非常量引用, 不能直接传递临时对象
        // db.persist(Classes("Class 1"));
        // db.persist(Classes("Class 2"));

        Classes c1("Class 1");
        Classes c2("Class 2");
        db.persist(c1);
        db.persist(c2);

        // 2.提交事务
        t.commit();
    }
    catch (const std::exception &e)
    {
        std::cerr << "insert class error: " << e.what() << std::endl;
    }
}

auto insert_student(odb::mysql::database &db) -> void
{
    try
    {
        // 1.获取事务操作对象, 开启事务
        odb::transaction t(db.begin()); // 自动开启事务
        // std::unique_ptr<odb::mysql::transaction_impl> t(db.begin()); // 手动开启事务

        // 错误, 要求是非常量引用, 不能直接传递临时对象
        // db.persist(Student(1001, "Alice", 20, 1));
        // db.persist(Student(1002, "Bob", 22, 2));
        // db.persist(Student(1003, "Charlie", 21, 1));
        // db.persist(Student(1004, "David", 23, 2));

        Student s1(1001, "Alice", 20, 1);
        Student s2(1002, "Bob", 22, 2);
        Student s3(1003, "Charlie", 21, 1);
        Student s4(1004, "David", 23, 2);
        db.persist(s1);
        db.persist(s2);
        db.persist(s3);
        db.persist(s4);

        // 2.提交事务
        t.commit();
    }
    catch (const std::exception &e)
    {
        std::cerr << "insert student error: " << e.what() << std::endl;
    }
}

auto select_student(odb::mysql::database &db) -> Student
{
    Student res;
    try
    {
        // 1.获取事务操作对象, 开启事务
        odb::transaction t(db.begin()); // 自动开启事务

        using query = odb::query<Student>;
        using result = odb::result<Student>;

        result r(db.query<Student>(query::name == "Alice"));

        if (r.size() != 1)
        {
            std::cerr << "select student number error" << std::endl;
            return res;
        }
        res = *r.begin();

        // 2.提交事务
        t.commit();
    }
    catch (const std::exception &e)
    {
        std::cerr << "select student error: " << e.what() << std::endl;
    }
    return res;
}

auto update_student(odb::mysql::database &db, Student &stu) -> void
{
    try
    {
        // 1.获取事务操作对象, 开启事务
        odb::transaction t(db.begin()); // 自动开启事务

        db.update(stu);

        // 2.提交事务
        t.commit();
    }
    catch (const std::exception &e)
    {
        std::cerr << "update student error: " << e.what() << std::endl;
    }
}

auto remove_student1(odb::mysql::database &db) -> void
{
    try
    {
        // 1.获取事务操作对象, 开启事务
        odb::transaction t(db.begin()); // 自动开启事务

        using query = odb::query<Student>;
        db.erase_query<Student>(query::name == "new name");

        // 2.提交事务
        t.commit();
    }
    catch (const std::exception &e)
    {
        std::cerr << "remove student error: " << e.what() << std::endl;
    }
}

auto remove_student2(odb::mysql::database &db) -> void
{
    try
    {
        // 1.获取事务操作对象, 开启事务
        odb::transaction t(db.begin()); // 自动开启事务

        using query = odb::query<Student>;
        db.erase_query<Student>(query::class_id == 2);

        // 2.提交事务
        t.commit();
    }
    catch (const std::exception &e)
    {
        std::cerr << "remove student error: " << e.what() << std::endl;
    }
}

auto class_student(odb::mysql::database &db) -> void
{
    try
    {
        // 1.获取事务操作对象, 开启事务
        odb::transaction t(db.begin()); // 自动开启事务

        using query = odb::query<classes_student>;
        using result = odb::result<classes_student>;

        result r(db.query<classes_student>(query::classes::id == 1));

        for (auto &s : r)
        {
            std::cout << s.student_name << " : " << s.classes_name << " :" << s.student_id << " : " << *s.student_age << " : " << s.student_sn << std::endl;
        }

        // 2.提交事务
        t.commit();
    }
    catch (const std::exception &e)
    {
        std::cerr << "select student error: " << e.what() << std::endl;
    }
}

auto student_name(odb::mysql::database &db) -> void
{
    try
    {
        // 1.获取事务操作对象, 开启事务
        odb::transaction t(db.begin()); // 自动开启事务

        // 不加 struct 关键字会报错:"student_name 不是类型名" --> 与函数名重复(就近)
        using query = odb::query<struct student_name>;
        // using query = odb::query<Student>;
        using result = odb::result<struct student_name>;

        result r(db.query<struct student_name>());
        // result r(db.query<struct student_name>(query::id == 7));

        for (auto &s : r)
        {
            std::cout << s.name << std::endl;
        }

        // 2.提交事务
        t.commit();
    }
    catch (const std::exception &e)
    {
        std::cerr << "select all student error: " << e.what() << std::endl;
    }
}

auto main(int argc, char *argv[]) -> int
{
    google::ParseCommandLineFlags(&argc, &argv, true);

    // 1.构造连接池工厂配置对象
    std::unique_ptr<odb::mysql::connection_pool_factory> cpf(std::make_unique<odb::mysql::connection_pool_factory>(FLAGS_max_poll_number, 0));
    // 只能用 unique_ptr

    // 2.构造数据库操作对象
    odb::mysql::database db(FLAGS_user, FLAGS_password, FLAGS_db, FLAGS_host, FLAGS_port, "", FLAGS_charset, 0, std::move(cpf));
    // cpf 必须 std::move, 直接写会报错 -- 此处必须是右值

    // 3.数据库事务操作
    // insert_class(db);
    // insert_student(db);

    // 先查询后更新
    // auto student = select_student(db);
    // student.set_name("new name");
    // update_student(db, student);

    // remove_student1(db);
    // remove_student2(db);

    // class_student(db);

    // student_name(db);

    return 0;
}