#include "sql_method.h"

M_sql::M_sql(){
    link = SqlConnPool::Instance()->GerConn();
}

M_sql::~M_sql(){
    if(link){
        SqlConnPool::Instance()->FreeCon(link);
    }
}


