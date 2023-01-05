#include "buffer.h"

Buffer::Buffer(int initBuffSize): buffer_(initBuffSize), readPos(0), writePos(0) {};

size_t Buffer::WritableBytes() const{
    return buffer_.size() - writePos; 
}

size_t Buffer::ReadableBytes() const{
    return writePos - readPos;
}

size_t Buffer::PrependableBytes() const{
    return readPos;
}

const char* Buffer::Peek() const{
    return BeginPtr_() + readPos;
}

void Buffer::EnsureWriteable(size_t len){
    if(WritableBytes() < len){
        MakeSpace(len);
    }
    assert(WritableBytes() >= len);
}

void Buffer::HasWritten(size_t len){
    //这行在原项目中没有，但我个人认为有必要判断输入的合法性
    //毕竟这是个public函数，客户的调用可能是错误的。
    assert(WritableBytes() >= len);
    writePos += len;
}

void Buffer::Retrieve(size_t len){
    assert(len <= ReadableBytes());
    readPos += len;
}

void Buffer::RetrieveUntil(const char* end){
    assert(end >= Peek());
    Retrieve(end - Peek());
}

void Buffer::RetrieveAll(){
    //这里和参考项目不一样，更新读写指针即可，不需要清空vector
    readPos = 0, writePos = 0;
}

std::string Buffer::RetrieveAllToStr(){
    //这里不适宜使用ROV优化
    //除了生成字符串的操作，还有一个清零操作，如果使用ROV优化，还要付出清零的成本。
    //相比起来付出一个string的构造析构成本说得上是划算
    string ret(Peek(), ReadableBytes());
    RetrieveAll();
    return ret;
}

const char* Buffer::BeginWriteConst() const{
    return BeginPtr_() + writePos;
}

char* Buffer::BeginWrite(){
    return BeginPtr_() + writePos;
}

void Buffer::Append(const std::string& str){
    Append(str.c_str(), str.size());
}

void Buffer::Append(const char* str, size_t len){
    assert(str);
    EnsureWriteable(len);
    //用copy比自己写要好得多，可以看stl源码剖析中的详解
    std::copy(str, str + len, BeginWrite());
    HasWritten(len);
}

void Buffer::Append(const void* data, size_t len){
    assert(data);
    Append(static_cast<const char*>(data), len);
}

void Buffer::Append(const Buffer& buff){
    Append(buff.Peek(), buff.ReadableBytes());
}

ssize_t Buffer::ReadFd(int fd, int* Errno){
    struct iovec input[2];
    char buff[READ_TEMP_BUFF_MAX];
    const size_t writable = WritableBytes();    

    input[0].iov_base = BeginWrite();
    input[0].iov_len = writable;
    input[1].iov_base = buff;
    input[1].iov_len = READ_TEMP_BUFF_MAX;

    const ssize_t len = readv(fd, input, 2);
    if(len < 0){
        *Errno = len;
    }else if(len <= static_cast<ssize_t>(writable)){
        HasWritten(len);
    }else{
        writePos = buffer_.size();
        Append(buff, len - writable);
    }
    return len;
}

ssize_t Buffer::WriteFd(int fd, int* Errno){
    int ret = 0;
    int temp = write(fd, Peek(), ReadableBytes());
    while(temp > 0){
        if(temp < 0){
            *Errno = temp;
            ret = temp;
            break;
        }
        ret += temp;
        Retrieve(temp);
    }
    return ret;
}

char* Buffer::BeginPtr_(){
    return &*buffer_.begin();
}

const char* Buffer::BeginPtr_() const{
    return &*buffer_.cbegin();
}

void Buffer::MakeSpace(size_t len){
    size_t step = PrependableBytes() + WritableBytes();
    if(step < len){
        buffer_.resize(buffer_.size() + len);
    }else{
        size_t readable = ReadableBytes();
        std::copy(BeginPtr_() + readPos, BeginWrite(), BeginPtr_());
        readPos = 0;
        writePos = readPos + readable;
        assert(readable == ReadableBytes());
    }
}

std::string Buffer::getnextline(){
    char cur = buffer_[readPos];
    int point = readPos;

    while(readPos < writePos && cur != '\n'){
        buffer_[readPos];
        ++readPos;
    }
    int step = readPos;
    if(step < writePos){
        step -= 2;
    }
    return std::string(BeginPtr_() + point, BeginPtr_() + step);
}
