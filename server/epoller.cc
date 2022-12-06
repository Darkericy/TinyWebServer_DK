#include "epoller.h"

//这里epoll_creat参数512我不明白有没有什么特殊的意义
Epoller::Epoller(int maxEvent): epollFd(epoll_creat(512)), events(maxEvent){
    assert(epollFd >= 0 && events.size() > 0);
}

Epoller::~Epoller(){
    close(epollFd);
}

bool Epoller::AddFd(int fd, uint32_t events){
    if(fd < 0){
        return false;
    }
    epoll_event ev = {0};
    ev.data.fd = fd;
    ev.events = evenets;
    return 0 == epoll_cnl(epoolfd, EPOLL_CTL_ADD, fd, &ev);
}

bool Epoller::ModFd(int fd, unint32_t events){
    if(fd < 0) return false;
    epoll_event ev = {0};
    ev.data.fd = fd;
    ev.events = events;
    return 0 == epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
}

bool Epoller::DelFd(int fd) {
    if(fd < 0) return false;
    epoll_event ev = {0};
    return 0 == epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, &ev);
}

int Epoller::Wait(int timeoutMs) {
    return epoll_wait(epollFd, &events_[0], static_cast<int>(events_.size()), timeoutMs);
}

int Epoller::GetEventFd(size_t i) const {
    assert(i < events_.size() && i >= 0);
    return events_[i].data.fd;
}

uint32_t Epoller::GetEvents(size_t i) const {
    assert(i < events_.size() && i >= 0);
    return events_[i].events;
}
