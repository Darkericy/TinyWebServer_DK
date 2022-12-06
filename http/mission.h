#ifndef MISSION_H
#define MISSION_H

#include <unordered_map>

#include "../pool/sqlconnpool.h"
#include "../pool/sqlconnRAII.h"
using namespace std;

class Mission{
    using Action = void (Mission::*)(const string&);
    using Way = void(Mission::*)();
    
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

    bool parse_uri(const string& uri, string& filename, string& cgiargs);

    unordered_map<string, Action> method;
    unordered_map<string, Way> post_way;
    string url, strFile, method;
    bool isStatic;
    int code;

public:
    Mission(int connect, MYSQL* s_connect):path(), strFile(), isStatic(true), code()
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

    void Init(const string& method, const string& url, int code);

    void start();
};

#endif
