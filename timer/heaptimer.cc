#include <heaptimer.h>

HeapTimer::HeapTimer(){
    heap.reserve(64);
};

void HeapTimer::adjust(int id, int timerout){
    assert(!heap.empty() && ref.count(id) > 0);
    heap[ref[id]].expires = Clock:now() + MS(timeout);
    shiftdown(ref[id], heap.size());
}

void HeapTimer::push(int id, int timeOut, const TimeoutCallBack& cb){
    assert(id > 0);

    size_t i;
    if(ref.count(id)){
        i = ref[id];
        heap[i].expires = Clock::now() + MS(timeout);
        heap[i].cb = cb;
        if(!shiftdown(i, heap.size())){
            shiftup(i);
        }
    }else{
        i = heap.size();
        ref[id] = i;
        heap.emplace_back(id, Clock::now() + MS(timeour), cb);
        shiftup(i);
    }
}

void HeapTimer::doWork(int id){
    if(heap.empty() || ref.count(id) == 0){
        return;
    }

    heap[ref[id]].cb();
    del(ref[id]);
}

void HeapTimer::clear(){
    heap.clear();
    ref.clear();
}

void HeapTimer::tick(){
    if(heap.empty()){
        return;
    }

    while(!heap.empty()){
        if(std::chrono::duration_cast<MS>(heap[0].expires - Clock::now()).count() > 0){
            break;
        }    
        heap[0].cb();
        pop();
    }
}

void HeapTimer::pop(){
    assert(!heap.empty());

    del(0);
}

//获取下一个定时期回调的时间
int HeapTimer::GetNextTick(){
    tick();
    size_t res = -1;
    if(!heap.empty()){
        res = std::chrono::duration_cast<MS>(heap[0].expires - Clock::now()).count();
        if(res < 0){
            res = 0;
        }
    }
    return res;
}

void HeapTimer::del_(size_t i){
    assert(!heap.empty() && i >= 0 && i < heap.size());

    SwapNode(i, heap.size() - 1);
    if(!shiftdown(i, heap.size() - 1)){
        shiftup(i);
    }
    ref.erase(heap.back().id);
    heap.pop_back();
}

void HeapTimer::shiftup(size_t i){
    assert(i >= 0 && heap.size() > i);

    size_t j = max((i - 1) / 2, (size_t)0);
    while(j >= 0){
        if(heap[i] < heap[j]){
            SwapNode(i, j);
        }else{
            break;
        }
        i = j;
        j = max((size_t)0, (i - 1) / 2);
    }
}

bool HeapTimer::shiftdown(size_t index, size_t n){
    assert(index >= 0 && heap.size() > index);
    assert(n >= 0 && heap.size() >= n);

    int i = index;
    int j = index * 2 + 1;
    while(j < n){
        if(j + 1 < n && heap[j] > heap[j + 1]){
            ++j;
        }
        if(heap[i] > heap[j]){
            SwapNode(i, j);
            i = j;
            j = i * 2 + 1;
        }else{
            break;
        }
    }
    return i > index;
}

void HeapTimer::SwapNode(size_t i, size_t j){
    assert(i >= 0 && j >= 0);
    assert(heap.size() > i && heap.size() > j);

    ref[heap[i].id] = j;
    ref[heap[j].id] = i;
    std::swap(heap[i], heap[j]);
}
