#include <unistd.h>
#include <thread>
#include <iostream>
#include "server/webserver.h"

using namespace std;

int main() {
    /* 守护进程 后台运行 */
    //daemon(1, 0); 

    WebServer server(
        2403, 3, 60000, false,             /* 端口 ET模式 timeoutMs 优雅退出  */
        3306, "root", "240307", "webserver", /* Mysql配置 */
        12, 8, true, 0, 1024);             /* 连接池数量 线程池数量 日志开关 日志等级 日志异步队列容量 */
    
    std::thread([&](){
                     string str;
                     cin >> str;
                     while(str != "quit"){
                        cin >> str;
                     };
                     server.~WebServer();
                     }).detach();

    server.Start();
} 
