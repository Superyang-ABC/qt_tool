
#include <string>
#include <list>
extern "C"
{
  #include "http.h"
}

#pragma once;
using namespace std;
class HttpCpp
{
public:
    HttpCpp();
    ~HttpCpp();
public:
    int HttpPost(string url,string& respone);
    int HttpPost(string url,list<string> &header,string& respone);
    int HttpPost(string url,list<string> &header,string&body ,string& respone);
    int HttpGet(string url,string& respone);
private:
    void Init();
    // int ModifyFormData(const char *file_path, UPLOAD_FILE_INFO *upload_file_info, FormData *form_data);
private:
    struct memory post_respone;
    struct memory get_respone;   
};