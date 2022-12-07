#include "mission.h"

void Mission::Init(const string& method, const string& url, int code, 
                   const unordered_map<string, string> POST){
    this.url = url;
    this,method = method;
    isStatic = true;
    this.code = code;
    post = POST;
}

void Mission::start(){
    if(code != 200){
        return;
    }

    (this->*method[me])(url);
}

void Mission::Head(const string& uri){
    code = 501;
    path = "";
}

void Mission::Put(const string& uri){
    code = 501;
    path = "";
}

void Mission::Delete(const string& uri){
    code = 501;
    path = "";
}

void Mission::Connect(const string& uri){
    code = 501;
    path = "";
}

void Mission::Options(const string& uri){
    code = 501;
    path = "";
}

void Mission::Trace(const string& uri){
    code = 501;
    path = "";
}

void Mission::Get(const string& url){
    string cgiargs;
    isStatic = parse_url(url, path, cgiargs);
    if(!isStatic){
       (this->*dynamic[path])(cgiargs, strFile); 
    }
}

void Mission::Post(const string& url){
    (this->*post_way[url])();
}

void Mission::Register(){
    M_sql mysql;
    string name = post["username"];
    string pwd = post["password"];

    if(mysql.userAdd(name, pwd)){
        path = "/welcome.html";
    }else{
        path = "/error.html"
    }
    isStatic = true;
}

void Mission::Enter(){
    M_sql mysql;
    string name = post["username"];
    string pwd = post["password"];

    if(mysql.userVerify(name, pwd)){
        path = "/welcome.html";
    }else{
        path = "/error.html";
    }
    isStatic = true;
}

bool Mission::parse_uri(const string& uri, string& filename, string& cgiargs){
	if(uri.find("cgi_bin") == string::npos){
        cgiargs = "";
        filename = "." + uri;
        if(uri.size() == 1 && uri[0] == '/'){
            filename += "index.html";        
        }
        return true;
    }else{
        auto ptr = uri.find('?');
        if(ptr != string::npos){
            cgiargs = uri.substr(ptr + 1);
        }else{
            cgiargs = "";
            ptr = uri.size();
        }
        filename = "." + uri.substr(0, ptr);
        return false;
    }
}

string Mission::GetPath() const{
    return path;
}

string Mission::GetStrFile() const{
    return strFile;
}

bool Mission::GetStatic() const{
    return isStatic;
}

int Mission::GetCode() const{
    return code;
}