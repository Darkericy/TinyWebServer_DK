#ifndef MISSION_H
#define MISSION_H

#include <unordered_map>
#include <string>
#include <iostream>

#include "../pool/sqlconnpool.h"
#include "../pool/sqlconnRAII.h"
using namespace std;

class Mission{
    using Action = void (Mission::*)(const string&);
    using Way = void(Mission::*)();
    using Dynamic = void (Mission::*)(const string& cgiargs, string& ret);
    
    void Get(const string& uri);
	void Head(const string& uri);
	void Post(const string& uri);
	void Put(const string& uri);
	void Delete(const string& uri);
	void Connect(const string& uri);
	void Options(const string& uri);
	void Trace(const string& uri);

    void Register();
    void Enter();

    bool parse_url(const string& uri, string& name, string& cgiargs);

    unordered_map<string, Action> method;
    unordered_map<string, Way> post_way;
    unordered_map<string, Dynamic> dynamic;

    unordered_map<string, string> post;
    string url, strFile_, method_, path_;
    bool isStatic;
    int code;

public:
    Mission():isStatic(true), code()
	{
		method["GET"] = &Mission::Get;
    	method["HEAD"] = &Mission::Head;
    	method["POST"] = &Mission::Post;
    	method["PUT"] = &Mission::Put;
    	method["DELETE"] = &Mission::Delete;
    	method["CONNECT"] = &Mission::Connect;
    	method["OPTIONS"] = &Mission::Options;
        method["TRACE"] = &Mission::Trace;
        post_way["/register"] = &Mission::Register;
        post_way["/login"] = &Mission::Enter;
	};

    ~Mission() = default;

    void Init(const string& method, const string& url, int code, const unordered_map<string, string>& POST);

    void start();

    string GetPath() const;
    string GetStrFile() const;
    bool GetStatic() const;
    int GetCode() const;
};

#endif
