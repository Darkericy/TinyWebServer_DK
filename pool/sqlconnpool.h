//#ifndef SQLCONNPOOL_H
//#define SQLCONNPOOL_H
//
//#include <iostream>
//#include <mysql/mysql.h>
//#include <mutex>
//#include "../log/log.h"
//#include "../log/blockqueue.h"
////我放弃了原版的操作，使用另一个文件中的阻塞队列实现
////毕竟模板让我物尽其用一下
//
//class SqlConnPool{
//    SqlConnPool();
//    ~SqlConnPool();
//
//    BlockDeque<MYSQL*> connQue;
//public:
//    static SqlConnPool *Instance();
//
//    MYSQL* GetConn();
//    void FreeConn(MYSQL* conn);
//    int GetFreeConnCount();
//
//    void Init(const char* host, int port,
//              const char* user, const char* pwd,
//              const char* dbName, int connSize);
//    void ClosePool();
//};
//
//#endif

#ifndef SQLCONNPOOL_H
#define SQLCONNPOOL_H

#include <mysql/mysql.h>
#include <string>
#include <queue>
#include <mutex>
#include <semaphore.h>
#include <thread>
#include "../log/log.h" //解决
//数据库的链接池比我想象的还要简单，而且在我看来这里的实现有些略显多余，
//我认为可以直接使用已经写好的blockqueue类模板。
//如果要使用类模板的话，静态生产函数中就需要外加一个对象了呢。

class SqlConnPool {
public:
    static SqlConnPool* Instance();

    MYSQL* GetConn();
    void FreeConn(MYSQL* conn);
    int GetFreeConnCount();

    void Init(const char* host, int port,
        const char* user, const char* pwd,
        const char* dbName, int connSize);
    void ClosePool();

private:
    //构造和析构申明为private成员后客户就不能自己构造对象了
    //也不用担心继承等问题
    SqlConnPool();
    ~SqlConnPool();

    int MAX_CONN_;
    int useCount_;
    int freeCount_;

    //MySQL链接队列
    std::queue<MYSQL*> connQue_;
    //互斥锁
    std::mutex mtx_;
    //型号量？
    sem_t semId_;
};


#endif // SQLCONNPOOL_H
