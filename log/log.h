#ifndef LOG_H
#define LOG_H

#include <mutex>
#include <string>
#include <thread>
#include <time.h>
#include <utility>
#include <stdarg.h>
#include <assert.h>
#include <sys/stat.h>
#include "blockqueue.h"
#include "../buffer/buffer.h"

class Log{
public:
    void init(int level, const char* path = "./log",
              const char* suffix = ".log",
              int maxQueueCapacity = 1024);

    static Log* Instance();
    static void FlushLogThread();

    void write(int level, const char* format, ...);
    void flush();

    int GerLevel();
    void SetLevel(int level);
    bool IsOpen(){
        return isOpen;
    }

private:
    //这里存放单例模式需要的几个函数
    //当让将析构和构造放在这里也是为了单例模式
    Log();
    void AppendLogLevelTitle(int level);
    ~Log();
    void AsyncWrite();

private:
    static const int LOG_PATH_LEN = 256;
    static const int LOG_NAME_LEN = 256;
    static const int MAX_LINES = 50000;

    const char* path;
    const char* suffix;
    
    int MAX_LINES;

    int lineCount;
    int toDay;

    bool isOpen;

    Buffer buff;
    int level;
    bool isAsync;

    FILE* fp;
    std::unique_ptr<BlockDeque<std::string>> deque;
    std::unique_ptr<std::thread> writeThread;
    std::mutex mtx;
};

//用inline替换原来的define，使用完美转发转发可变参数，按照effective的条款认真执行。
template <typename... Args>
inline void LOG_BASE(int level, const char* format, Args&&... args){
    Log* log = Log::Instance();
    if(log->IsOpen() && log->GetLevel() <= level){
        log->write(level, format, std::forward<Args>(args)...);
        log->flush();
    }
}

template <typename... Args>
inline void LOG_DEBUG(const char* format, Args&&... args){
    LOG_BASE(0, format, std::forward<Args>(args)...);
}

template <typename... Args>
inline void LOG_INFO(const char* format, Args&&... args){
    LOG_BASE(1, format, std::forward<Args>(args)...);
}

template <typename... Args>
inline void LOG_WARN(const char* format, Args&&... args){
    LOG_BASE(2, format, std::forward<Args>(args)...);
}

template <typename... Args>
inline void LOG_ERROR(const char* format, Args&&... args){
    LOG_BASE(3, format, std::forward<Args>(args)...);
}

#endif
