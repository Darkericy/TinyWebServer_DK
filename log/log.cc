#include "log.h"

using namespace std;

Log::Log(){
    lineCount = 0;
    isAsync = false;
    writeThread = nullptr;
    deque = nullptr;
    today = 0;
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
    this.level = level;
}

void Log::init(int level = 1, const char* path, const char* suffix, int maxQueueSize){
    isOpen = true;
    this.level = level;
    if(maxQueueSize > 0){
        isAsync = true;
        if(!deque){
            //遵从条款，使用make_unique
            unique_ptr<BlockDeque<std::string>> dequetemp = make_unique<BlockDeque<std::string>>(maxQueueSize);
            deque = move(dequetemp);

            unique_ptr<std::thread> threadtemp = make_unique<std::thread>(FLushLogThread);
            writeThread = move(threadtemp);
        }
    }else{
        isAsync = false;
    }

    lineCount = 0;

    time_t timer = time(nullptr);
    struct tm = *localtime(&timer);
    this.path = path;
    this.suffix = suffix;
    char fileName[LOG_NAME_LEN] = {0};
}
