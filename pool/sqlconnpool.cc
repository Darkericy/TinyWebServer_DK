//#include "sqlconnpool.h"
////这里的namespace可能应该只是个摆设，所以我先把他注释掉
////using namespace std;
//
//SqlConnPool::SqlConnPool():connQue(10){}
//
//SqlConnPool* SqlConnPool::Instance(){
//    static SqlConnPool connPool;
//    return &connPool;
//}
//
//void SqlConnPool::Init(const char* host, int port,
//                       const char* user, const char* pwd, const char* dbName,
//                       int connSize = 10){
//    assert(connSize > 0);
//    auto que = Instance();
//    que->connQue.reserve(connSize);
//    for(int i = 0; i < connSize; ++i){
//        MYSQL* sql = nullptr;
//        sql = mysql_init(sql);
//        if (!sql) {
//            LOG_ERROR("MySql init error!");
//            assert(sql);
//        }
//        sql = mysql_real_connect(sql, host,
//                                 user, pwd,
//                                 dbName, port, nullptr, 0);
//        if (!sql) {
//            LOG_ERROR("MySql Connect error!");
//        }
//        que->connQue.push_back(sql);
//    }
//}
//
//MYSQL* SqlConnPool::GetConn(){
//    MYSQL* sql = nullptr;
//    auto que = Instance();
//    if(que->connQue.empty()){
//        LOG_WARN("SqlConnPool busy!");
//        return nullptr;
//    }
//
//    if(!que->connQue.pop(sql)){
//        std::cout << "是这里吗" << std::endl;
//        return nullptr;
//    }
//    if(!sql){
//        std::cout << "为什么这里不行" << std::endl;
//    }
//    return sql;
//}
//
//void SqlConnPool::FreeConn(MYSQL* sql){
//    assert(sql);
//    Instance()->connQue.push_back(sql);
//}
//
//void SqlConnPool::ClosePool(){
//    auto que = Instance();
//    MYSQL* item;
//    while(que->connQue.pop(item, 1) > 0){
//        mysql_close(item);
//    }
//    mysql_library_end();
//}
//
//int SqlConnPool::GetFreeConnCount(){
//    return Instance()->connQue.size();
//}
//
//SqlConnPool::~SqlConnPool(){
//    ClosePool();
//}

#include "sqlconnpool.h"
#include <iostream>
using namespace std;

SqlConnPool::SqlConnPool() {
    useCount_ = 0;
    freeCount_ = 0;
}

//单例模式
SqlConnPool* SqlConnPool::Instance() {
    static SqlConnPool connPool;
    return &connPool;
}

void SqlConnPool::Init(const char* host, int port,
    const char* user, const char* pwd, const char* dbName,
    int connSize = 10) {
    assert(connSize > 0);
    for (int i = 0; i < connSize; i++) {
        MYSQL* sql = nullptr;
        sql = mysql_init(sql);
        if (!sql) {
            LOG_ERROR("MySql init error!");
            assert(sql);
        }
        sql = mysql_real_connect(sql, host,
            user, pwd,
            dbName, port, nullptr, 0);
        if (!sql) {
            std::cout << "这里的问题" << std::endl;
            LOG_ERROR("MySql Connect error!");
        }
        connQue_.push(sql);
    }
    MAX_CONN_ = connSize;
    //信号量操作，有什么是不能用互斥锁替代的吗？
    //懂了他是用来控制保证队列长度的，那我用生产者消费者模型不能解决吗？
    sem_init(&semId_, 0, MAX_CONN_);
}

MYSQL* SqlConnPool::GetConn() {
    MYSQL* sql = nullptr;
    if (connQue_.empty()) {
        LOG_WARN("SqlConnPool busy!");
        return nullptr;
    }
    //用信号量保证队列的长度不超多或不少于一定的范围
    //锁用来却保对于队列的操作是线程安全的。
    sem_wait(&semId_);
    {
        lock_guard<mutex> locker(mtx_);
        sql = connQue_.front();
        connQue_.pop();
    }
    return sql;
}

void SqlConnPool::FreeConn(MYSQL* sql) {
    assert(sql);
    lock_guard<mutex> locker(mtx_);
    connQue_.push(sql);
    sem_post(&semId_);
}

void SqlConnPool::ClosePool() {
    lock_guard<mutex> locker(mtx_);
    while (!connQue_.empty()) {
        auto item = connQue_.front();
        connQue_.pop();
        mysql_close(item);
    }
    mysql_library_end();
}

int SqlConnPool::GetFreeConnCount() {
    lock_guard<mutex> locker(mtx_);
    return connQue_.size();
}

SqlConnPool::~SqlConnPool() {
    ClosePool();
}
