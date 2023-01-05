#include "httprequest.h"
using namespace std;

//#define DEBUG

const unordered_set<string> HttpRequest::DEFAULT_HTML{
            "/index", "/register", "/login",
             "/welcome", "/video", "/picture", };

const string HttpRequest::URLHEXMAP = "0123456789ABCDEF";

void HttpRequest::Init() {
    method_ = path_ = version_ = body_ = "";
    state_ = REQUEST_LINE;
    header_.clear();
    post_.clear();
}

bool HttpRequest::IsKeepAlive() const {
    if(header_.count("Connection") == 1) {
        return header_.at("Connection") == "keep-alive" && version_ == "1.1";
    }
    return false;
}

bool HttpRequest::parse(Buffer& buff) {
#ifdef DEBUG
    LOG_INFO("调用httprequest的parse");
#endif
    if(buff.ReadableBytes() <= 0) {
        return false;
    }
    
    while(buff.ReadableBytes() && state_ != FINISH) {
        std::string line = buff.getnextline();
#ifdef DEBUG
        LOG_INFO("line的长度为:%d", line.size());
#endif
        switch(state_)
        {
        case REQUEST_LINE:
            //std::cout << "开始解析头部" << line << endl;
            if(!ParseRequestLine_(line)) {
                return false;
            }
            break;
        case HEADERS:
            ParseHeader_(line);
            if(buff.ReadableBytes() <= 2) {
                state_ = FINISH;
            }
            break;
        case BODY:
            ParseBody_(line);
            break;
        default:
            break;
        }
    }
    LOG_DEBUG("[%s], [%s], [%s]", method_.c_str(), path_.c_str(), version_.c_str());
    return true;
}

bool HttpRequest::ParseRequestLine_(const string& line) {
    regex patten("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");
    smatch subMatch;
    if(regex_search(line, subMatch, patten)) {
        //这里的用法，可以根据c++ prime P654进行改写i
        //std::cout << "成功匹配请求" << std::endl;
        method_ = subMatch.str(1);
        path_ = subMatch.str(2);
        version_ = subMatch.str(3);
        state_ = HEADERS;
        return true;
    }
    LOG_ERROR("RequestLine Error");
    return false;
}

void HttpRequest::ParseHeader_(const string& line) {
    regex patten("^([^:]*): ?(.*)$");
    smatch subMatch;
    if(regex_match(line, subMatch, patten)) {
        header_[subMatch[1]] = subMatch[2];
    }
    else {
        state_ = BODY;
    }
}

void HttpRequest::ParseBody_(const string& line) {
    body_ = line;
    splice_data_();
    state_ = FINISH;
    LOG_DEBUG("Body:%s, len:%d", line.c_str(), line.size());
}

std::string HttpRequest::path() const{
    return path_;
}

std::string HttpRequest::path() {
    return path_;
}

std::string HttpRequest::method() const{
    return method_;
}

std::string HttpRequest::version() const{
    return version_;
}

std::string HttpRequest::GetHead_v(const std::string& key) const{
    assert(key != "");
    if(header_.count(key) != 0){
        return header_.at(key);
    }
    return string();
}

std::string HttpRequest::GetBody_v(const std::string& key) const{
    assert(key != "");

    if(post_.count(key) != 0){
        return post_.at(key);
    }
    return string();
}

void HttpRequest::splice_data_(){
    assert(body_.size() > 0);

    for(auto& c: body_){
        if(c == '&'){
            c = ' ';
        }
    }
    istringstream input(body_);
    string temp;
    while(input >> temp){
        auto index = temp.find("=");
        string key = temp.substr(index + 1);
        key = url_decode_(key);
        string value = temp.substr(0, index);
        value = url_decode_(value);
        post_[key] = value;
    }
}
std::string HttpRequest::url_decode_(const std::string& src){
    string dst;
	string::size_type size  = src.size();
	string::size_type index = 0;
	unsigned char  index_c1 = 0;
	unsigned char  index_c2 = 0;

	for(index = 0; index < size; index++)
	{
		if('%' == src[index] && (index+2) < size && isxdigit(src[index+1]) && isxdigit(src[index+2]))
		{
			index_c1 = URLHEXMAP.find(src[++index]);
			index_c2 = URLHEXMAP.find(src[++index]);
			dst.push_back(index_c1 << 4 | index_c2);
		}
		else if('+' == src[index])
		{
			dst.push_back(' ');
		}
		else
		{
			dst.push_back(src[index]);
		}
	}
	return dst;
}

std::unordered_map<std::string, std::string> HttpRequest::GetPost() const{
    return post_;
}
