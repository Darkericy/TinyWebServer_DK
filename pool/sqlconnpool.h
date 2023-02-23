#ifndef sqlconnpool_h
#define sqlconnpool_h

#include <iostream>
#include <mysql/mysql.h>
#include <mutex>
#include "../log/log.h"
#include "../log/blockqueue.h"
//我放弃了原版的操作，使用另一个文件中的阻塞队列实现
//毕竟模板让我物尽其用一下

class sqlconnpool{
    sqlconnpool();
    ~sqlconnpool();

    blockdeque<mysql*> connque;
public:
    static sqlconnpool *instance();

    mysql* getconn();
    void freeconn(mysql* conn);
    int getfreeconncount();

    void init(const char* host, int port,
              const char* user, const char* pwd,
              const char* dbname, int connsize);
    void closepool();
};

#endif