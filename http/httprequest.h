#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <regex>
#include <errno.h>

#include "../buffer/buffer.h"
#include "../log/log.h"

//跟原版比起来我重构了逻辑，添加了一些函数，并且使用map来保存函数指针来完成我们想要的操作。
//我决定用三个类来处理这个逻辑，request和reponse就用这个项目原来自带的，稍作修改，让这两个类只承担分析
//我再添加一个httpwork类来做工作

class HttpRequest{
    bool ParseRequestLine_(const std::string& line);
    void ParseHeader_(const std::string& line);
    void ParseBody_(const std::string& line);

    void splice_data_();
    std::string url_decode_(const std::string& src);

    PARSE_STATE state_;
    std::string method_, path_, version_, body_;
    std::unordered_map<std::string, std::string> header_;
    std::unordered_map<std::string, std::string> post_;

    static const std::unordered_set<std::string> DEFAULT_HTML;
    static const std::string URLHEXMAP;

public:
    enum PARSE_STATE {
        REQUEST_LINE,
        HEADERS,
        BODY,
        FINISH,        
    };
    
    HttpRequest() { Init(); }
    ~HttpRequest() = default;

    void Init();
    bool parse(Buffer& buff);

    std::string path() const;
    std::string path();
    std::string method() const;
    std::string version() const;
    std::string GetBody_v(const std::string& key) const;
    std::string GerHead_v(const std::string& key) const;

    //这下面的函数用来返回HEAD中的内容
    bool IsKeepAlive() const;
};

#endif
