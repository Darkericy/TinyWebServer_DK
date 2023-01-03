#ifndef HEAP_TIMER_H
#define HEAP_TIMER_H

#include <unordered_map>
#include <algorithm>
#include <functional>
#include <assert.h>
#include <chrono>
#include <../log/log.h>

using TimeoutCallBack = std::function<void()>;
//这里使用steadyclock也是和原版的小区别，这是遵从了文档描述的建议
using Clock = std::chrono::steady_clock;
using MS = std::chrono::milliseconds;
using TimeStamp = Clock::time_point;

struct TimerNode{
    int id;
    TimeStamp expires;
    TimeoutCallBack cb;
    bool operator < (const TimerNode& rhs){
        return expires < rhs.expires;
    }
    bool operator > (const TimerNode& rhs){
        return expires > rhs.expires;
    }
};

class HeapTimer{
    void del(size_t i);

    void shiftup(size_t i);

    bool shiftdown(size_t index, size_t n);

    void SwapNode(size_t i, size_t j);
    
    std::vector<TimerNode> heap;
    //这里装着文件描述符和节点下标的映射
    std::unordered_map<int, size_t> ref;

public:
    HeapTimer();

    ~HeapTimer() = default;

    void adjust(int id, int newExpires);

    void push(int id, int timeOut, const TimeoutCallBack& cb);

    void doWork(int id);

    void clear();

    void tick();

    void pop();

    int GetNextTick();
};

#endif
