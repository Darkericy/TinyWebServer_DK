#include "mission.h"

void Mission::Init(const string& method, const string& url, int code){
    this.url = url;
    this,method = method;
    isStatic = true;
    this.code = code;
}

void Mission::start(){
    (this->*method[me])(uri);
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


