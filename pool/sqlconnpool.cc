#include "sqlconnpool.h"
//这里的namespace可能应该只是个摆设，所以我先把他注释掉
//using namespace std;

SqlConnPool::SqlConnPool():connQue(10){}

SqlConnPool* SqlConnPool::Instance(){
    static SqlConnPool connPool;
    return &connPool;
}

void SqlConnPool::Init(const char* host, int port,
                       const char* user, const char* pwd, const char* dbName,
                       int connSize = 10){
    assert(connSize > 0);
    auto que = Instance();
    que->connQue.reserve(connSize);
    for(int i = 0; i < connSize; ++i){
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
            LOG_ERROR("MySql Connect error!");
        }
        que->connQue.push_back(sql);
    }
}

MYSQL* SqlConnPool::GetConn(){
    MYSQL* sql = nullptr;
    auto que = Instance();
    if(que->connQue.empty()){
        LOG_WARN("SqlConnPool busy!");
        return nullptr;
    }

    if(!que->connQue.pop(sql)){
        std::cout << "是这里吗" << std::endl;
        return nullptr;
    }
    if(!sql){
        std::cout << "为什么这里不行" << std::endl;
    }
    return sql;
}

void SqlConnPool::FreeConn(MYSQL* sql){
    assert(sql);
    Instance()->connQue.push_back(sql);
}

void SqlConnPool::ClosePool(){
    auto que = Instance();
    MYSQL* item;
    while(que->connQue.pop(item, 1) > 0){
        mysql_close(item);
    }
    mysql_library_end();
}

int SqlConnPool::GetFreeConnCount(){
    return Instance()->connQue.size();
}

SqlConnPool::~SqlConnPool(){
    ClosePool();
}
