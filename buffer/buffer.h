#ifndef BUFFER_H
#define BUFFER_H

#include <iostream>
#include <sts/uio.h> //readv writev
#include <unistd>
#include <vector>
#include <assert.h>
#include <algorithm>

//用户缓冲区，不支持线程安全

class Buffer{
private:
    const int READ_TEMP_BUFF_MAX = 65535;

    std::vector<char> buffer_;
    int readPos, writePos;

    char* BeginPtr_();
    const char* BeginPtr_() const;
    void MakeSpace(size_t len);

public:
    explicit Buffer(int initBuffSize = 1024);
    ~Buffer() = default;

    size_t WritableBytes() const;
    size_t ReadableBytes() const;
    size_t PrependableBytes() const;

    const char* Peek() const;
    void EnsureWriteable(size_t len);
    void HasWritten(size_t len);

    void Retrieve(size_t len);
    void RetrieveUntil(const char* end);

    void RetrieveAll();
    std::string RetrieveAllToStr();

    const char* BeginWriteConst() const;
    char* BeginWrite();

    void Append(const std::string& str);
    void Append(const char* str, size_t len);
    void Append(const void* data, size_t len);
    void Append(const Buffer& buff);

    ssize_t ReadFd(int fd, int* Errno);
    ssize_t WriteFd(int fd, int* Errno);

    string getnextline();
};

#endif
