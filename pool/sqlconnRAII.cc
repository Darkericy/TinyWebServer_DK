#include "sqlconnRAII.h"

M_sql::M_sql(){
    link = SqlConnPool::Instance()->GetConn();
}

M_sql::~M_sql(){
    if(link){
        SqlConnPool::Instance()->FreeConn(link);
    }
}

bool M_sql::userAdd(const string& name, const string& pwd)
{
    //cout << "来到useradd" << " " << name << " " << pwd << endl;
		string sql = "insert into usrinfo(name, pwd) values ('" + name + "','" + pwd + "')";

        if(!link){
            cout << "数据库连接有误" << endl;
        }
		int ret = mysql_query(link, sql.c_str());
        LOG_INFO("新用户：\"%s\"加入", name);
		if (ret < 0)
		{
			return false;
		}
		else
		{
			return true;
		}
}

bool M_sql::userVerify(const string& name, const string& pwd)
{
    string sql = "select name, pwd from usrinfo where name = '" + name + "'";
    int k;
    if((k = mysql_query(link, sql.c_str()))){
        LOG_ERROR("数据库查询失败");
        return false;
    }

    MYSQL_RES* result;
    if(!(result = mysql_store_result(link))){
        LOG_ERROR("数据库结果获取失败");
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
	if (row)
	{
        if (row[0] == name && row[1] == pwd){
            LOG_INFO("登陆成功");
            return true;
        }
        mysql_free_result(result);
		return false;
	}
	else
	{
        LOG_INFO("查无此人");
        mysql_free_result(result);
		return false;
	}
}
