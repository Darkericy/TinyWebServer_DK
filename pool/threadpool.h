#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>
#include <functional>
class ThreadPool{
    //这种设计让我觉得和pimpl比较像，但是这里又不是，我不知道这样设计是不是有什么优点。
    //保留问题
    struct Pool{
        std::mutex mtx;
        std::condition_variable cond;
        bool isClosed;
        std::queue<std::function<void()>> tasks;
    }
    std::shared_ptr<Pool> pool_;

public:
    explicit ThreadPool(size_t threadCount = 8):pool_(std::make_shared<Pool>()){
        for(int i = 0; i < threadCount; ++i){
        std::thread([pool = pool_]{
                std::unique_lock<std::mutex> locker(pool->mtx);
                while(true){
                    if(!pool->tasks.empty()){
                    //因为是用完美转发填充进去的，用move效果会更好
                        auto task = std::move(pool->tasks.front());
                        pool->tasks.pop();
                        locker.unlock();
                        task();
                        locker.lock();
                        }else if(pool->isClosed){
                        break;
                        }else{
                        pool->cond.wait(locker);
                        }
                }
            }).detach();
        }
    }

    ThreadPool() = default;

    ThreadPool(ThreadPool&&) = default;

    ~ThreadPool(){
        //智能指针自己封装了向bool类型的隐形转换
        if(pool_){
            {
                std::lock_guard<std::mutex> locker(pool_->mtx);
                pool->isClosed = true;
            }
            pool_->cond.notify_all();
        }
    }

    template<typename F>
        void AddTask(F&& task){
            {
                std::lock_guard<std::mutex> locker(mtx);
                pool->tasks.emplace(std::forward<F>(task));
            }
            pool->cond.notify_one();
        }
};

#endif
