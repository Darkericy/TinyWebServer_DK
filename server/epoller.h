#ifndef EPOLLER_H
#define EPOLLER_H

#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <vector>
#include <errno.h>

class Epoller{
    int epollFd;

    std::vector<struct epoll_event> events;

public:
    explicit Epoller(int maxEvent = 1024);

    ~Epoller();

    bool AddFd(int fd, unit32_t events);

    bool ModFd(int fd, unit32_t events);

    bool DefFd(int fd);

    int Wait(int timeoutMS = -1);

    int GetEventFd(size_t i) const;

    unint32_t GetEvents(size_t i) const;
};

#endif
