#ifndef SQLCONNRAII_H
#define SQLCONNRAII_H

#include "sqlconnpool.h"
#include <string>
//我使用我自己定义的sql连接管理类
//他当然实现了对于sql连接的管理，和原版差别较大，那是因为原版的错误比较明显。
//原版的RAII只是表面功夫而已，没有任何实际用途

using namespace std;

class M_sql{
private:
    //数据库连接
    MYSQL* link;

public:
    M_sql();
    ~M_sql();
};

#endif
