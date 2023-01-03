#include "sqlconnRAII.h"

M_sql::M_sql(){
    link = SqlConnPool::Instance()->GetConn();
}

M_sql::~M_sql(){
    if(link){
        SqlConnPool::Instance()->FreeCon(link);
    }
}


