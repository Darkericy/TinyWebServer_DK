#include "log.h"

using namespace std;

Log::Log(){
    lineCount = 0;
    isAsync = false;
    writeThread = nullptr;
    deque = nullptr;
    toDay = 0;
    fp = nullptr;
}

//析构函数对线程和文件描述符两个资源的处理。
//这个类应该不需要特意声明将拷贝构造函数和复制构造函数删除
//因为有unique_ptr存在，编译器应当不会合成
Log::~Log(){
    if(writeThread && writeThread->joinable()){
        while(!deque->empty()){
            deque->flush();
        };
        deque->Close();
        writeThread->join();
    }
    if(fp){
        lock_guard<mutex> locker(mtx);
        flush();
        fclose(fp);
    }
}

int Log::GetLevel(){
    lock_guard<mutex> locker(mtx);
    return level;
}

void Log::SetLevel(int level){
    lock_guard<mutex> locker(mtx);
    this->level = level;
}

void Log::init(int level = 1, const char* path, const char* suffix, int maxQueueSize){
    //这里我把上锁提前了，如果要保证线程安全，那美在修改类变量的时候就应当上锁了。
    //原版这里在后面上锁给我一种保护了但是没有完全保护的感觉。
    lock_guard<mutex> locker(mtx);

    isOpen = true;
    this->level = level;
    if(maxQueueSize > 0){
        isAsync = true;
        if(!deque){
            //遵从条款，使用make_unique
            unique_ptr<BlockDeque<std::string>> dequetemp = make_unique<BlockDeque<std::string>>(maxQueueSize);
            deque = move(dequetemp);

            unique_ptr<std::thread> threadtemp = make_unique<std::thread>(FlushLogThread);
            writeThread = move(threadtemp);
        }
    }else{
        isAsync = false;
    }

    lineCount = 0;

    time_t timer = time(nullptr);
    struct tm t = *localtime(&timer);
    this->path = path;
    this->suffix = suffix;
    char fileName[LOG_NAME_LEN] = {0};
    snprintf(fileName, LOG_NAME_LEN - 1, "%s/%04d_%02d_%02d%s", 
            path, t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, suffix);
    toDay = t.tm_mday;
    buff.RetrieveAll();
    if(fp){
        flush();
        fclose(fp);
    }

    fp = fopen(fileName, "a");
    if(fp == nullptr){
        mkdir(path, 0777);
        fp = fopen(fileName, "a");
    }
    assert(fp != nullptr);
}

void Log::write(int level, const char* format, ...){
    time_t timer = time(nullptr);
    struct tm t = *localtime(&timer);
    va_list vaList;

    //既然要检查成员变量的值那就应该上锁
    unique_lock<mutex> locker(mtx);
    if(toDay != t.tm_mday || (lineCount && (lineCount % MAX_LINES == 0))){
        locker.unlock();

        char newFile[LOG_NAME_LEN];
        char tail[36] = {0};
        snprintf(tail, 36, "%04d_%02d_%02d", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);

        if(toDay != t.tm_mday){
            snprintf(newFile, LOG_NAME_LEN - 72, "%s/%s%s", path, tail, suffix);
            toDay = t.tm_mday;
            lineCount = 0;
        }else{
            snprintf(newFile, LOG_NAME_LEN - 72, "%s/%s-%d%s", path, tail, (lineCount  / MAX_LINES), suffix);
        }
        locker.lock();
        flush();
        fclose(fp);
        fp = fopen(newFile, "a");
        assert(fp != nullptr);
    }
    
    ++lineCount;
    //原版这里的128有越界风险,所以我认为在输入前提供一个ensure会更好
    buff.EnsureWriteable(128);
    int n = snprintf(buff.BeginWrite(), 128, "%d-%02d-%02d %02d:%02d:%02d",
                    t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
                    t.tm_hour, t.tm_min, t.tm_sec);
    buff.HasWritten(n);
    AppendLogLevelTitle(level);

    va_start(vaList, format);
    int m = vsnprintf(buff.BeginWrite(), buff.WritableBytes(), format, vaList);
    va_end(vaList);

    buff.HasWritten(m);
    buff.Append("\n\0", 2);

    //这里不应该也不用判断生产着队列是否满了
    if(isAsync && deque){
        deque->push_back(buff.RetrieveAllToStr());
    }else{
        fputs(buff.Peek(), fp);
        buff.RetrieveAll();
    }
}

void Log::AppendLogLevelTitle(int level){
    //这里不用拿锁，因为是由write函数调用的，此时是持有锁的状态，否则会死锁。
    switch(level) {
    case 0:
        buff.Append("[debug]: ", 9);
        break;
    case 1:
        buff.Append("[info] : ", 9);
        break;
    case 2:
        buff.Append("[warn] : ", 9);
        break;
    case 3:
        buff.Append("[error]: ", 9);
        break;
    default:
        buff.Append("[info] : ", 9);
        break;
    }
}

void Log::flush(){
    if(isAsync){
        deque->flush();
    }
    fflush(fp);
}

void Log::AsyncWrite(){
    string str = "";
    while(deque->pop(str)){
        lock_guard<mutex> locker(mtx);
        fputs(str.c_str(), fp);
    }
}

Log* Log::Instance(){
    static Log inst;
    return &inst;
}

void Log::FlushLogThread(){
    Log::Instance()->AsyncWrite();
}
