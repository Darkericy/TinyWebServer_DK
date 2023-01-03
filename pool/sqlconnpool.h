#ifndef SQLCONNPOOL_H
#define SQLCONNPOOL_H

#include <mysql/mysql.h>
#include <mutex>
#include "../log/log.h"
#include "../log/blockqueue.h"
//我放弃了原版的操作，使用另一个文件中的阻塞队列实现
//毕竟模板让我物尽其用一下

class SqlConnPool{
    SqlConnPool();
    ~SqlConnPool();

    BlockDeque<MYSQL*> connQue;
public:
    static SqlConnPool *Instance();

    MYSQL* GetConn();
    void FreeCon(MYSQL* conn);
    int GetFreeConnCount();

    void Init(const char* host, int port,
              const char* user, const char* pwd,
              const char* dbName, int connSize);
    void ClosePool();
};

#endif
