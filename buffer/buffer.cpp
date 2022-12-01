#include <buffer.h>

Buffer::Buffer(int initBuffSize = 1024): buffer_(1024), readPos(0), writePos(0) {};

size_t Buffer::WritableBytes() const{
    return buffer_,size() - writePos;
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

void EnsureWriteable(size_t len){
    if(WritableBytes() < len){
        MakeSpace(len);
    }
    assert(WritableBytes() >= len);
}

void HasWritten(size_t len){
    //这行在原项目中没有，但我个人认为有必要判断输入的合法性
    //毕竟这是个public函数，客户的调用可能是错误的。
    assert(WritableBytes() >= len);
    WritePos += len;
}

void Buffer::Retrieve(size_t len){
    assert(len <= ReadableBytes());
    readPos_ += len;
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

char* Buffer::BeginWriteConst(){
    return BeginPtr() + writePos;
}

void Append(const std::string& str){
    Append(str.c_str(), str.size());
}

void Append(const char* str, size_t len){
    assert(str);
    EnsureWriteable(len);
    //用copy比自己写要好得多，可以看stl源码剖析中的详解
    std::copy(str, str + len, BeginWrite());
    HasWritten(len);
}

void Append(const void* data, size_t len){
    assert(data);
    Append(static_cast<const char*>(data), len);
}

void Append(const Buffer& buff){
    Append(buff.Peek(), buff.ReadableBytes());
}

ssize_t ReadFd(int fd, int* Errno){
    struct iovec input[2];
    char buff[READ_TEMP_BUF_MAX];
    const size_t writable = WritableBytes();    

    input[0].iov_base = BeginWrite();
    input[0].iov_len = writable;
    input[1].iov_base = buff;
    input[1].iov_len = READ_TEMP_BUFF_MAX;

    cosnt ssize_t readv(fd, input, 2);
    if(len < 0){
        *Errno = len;
    }else if(len <= writable){
        HasWritten(len);
    }else{
        writePos = buffer_.size();
        Append(buff, len - writable);
    }
    return len;
}

ssize_t WriteFd(int fd, int* Errno){
    int ret = 0;
    int temp;
    while(temp = write(fd, Peek(), ReadableBytes()) > 0){
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

char* BeginPtr_(){
    return &*buffer.begin();
}

const char* BeginPtr_() const{
    return &*buffer.cbegin();
}

void MakeSpace(size_t len){
    int step = PrependableBytes() + WritableBytes();
    if(step < len){
        buffer.resize(buffer.size() + len);
    }else{
        size_t readable = ReadableBytes();
        copy(BeginPtr_() + readPos, BeginWrite(), BeginPtr_());
        readPos = 0;
        writePos = readPos + readable;
        assert(readable == ReadableBytes());
    }
}
