#include "httpcpp.h"

HttpCpp::HttpCpp()
{
    Init();
}
HttpCpp::~HttpCpp()
{
    if(post_respone.response != NULL)
    {
        free(post_respone.response);
        post_respone.response = NULL;
        post_respone.size = 0;
    }
    if(get_respone.response != NULL)
    {
        free(get_respone.response);
        get_respone.response = NULL;
        get_respone.size = 0;
    }

}
void HttpCpp::Init()
{
    memset(&post_respone,0,sizeof(post_respone));
    memset(&get_respone,0,sizeof(get_respone));
}

int HttpCpp::HttpPost(string url,string& respone)
{
    int ret = -1;
    ret = http_post(url.c_str(),NULL,"",&post_respone);
    respone.clear();
    if(post_respone.size)
    {
        respone.append(post_respone.response);
    } 
    return ret;
}

int HttpCpp::HttpPost(string url,list<string> &header,string& respone)
{
    int ret = -1;
    struct curl_slist *list = NULL;
    for(auto it = header.begin();it != header.end();it++)
    {
        list = curl_slist_append(list, (*it).c_str());
    }
    ret = http_post(url.c_str(),list,"",&post_respone);
    respone.clear();
    if(post_respone.size)
    {
        respone.append(post_respone.response);
    } 
    if(list)
    {
        curl_slist_free_all(list); /* free the list */
    }
    return ret;
}

int HttpCpp::HttpPost(string url,list<string> &header,string&body ,string& respone)
{
    int ret = -1;
    struct curl_slist *list = NULL;
    for(auto it = header.begin();it != header.end();it++)
    {
        list = curl_slist_append(list, (*it).c_str());
    }
    ret = http_post(url.c_str(),list,body.c_str(),&post_respone);
    respone.clear();
    if(post_respone.size)
    {
        respone.append(post_respone.response);
    } 
    if(list)
    {
        curl_slist_free_all(list); /* free the list */
    }
    return ret;

}


int HttpCpp::HttpGet(string url,string &respone)
{   int ret = -1;
    ret = http_get(url.c_str(),&get_respone);
    respone.clear();
    if(get_respone.size)
    {
        respone.append(get_respone.response);
    }
    return ret;
}

// int HttpCpp::ModifyFormData(const char *file_path, UPLOAD_FILE_INFO *upload_file_info, FormData *form_data)
// {
//     cJSON *usr = NULL;
//     if (!file_path)
//     {
//         fprintf(stderr, "CreateFileInfo file_path error\r\n");
//         return 0; //URL error;
//     }
//     if (strlen(file_path) < 5)
//     {
//         fprintf(stderr, "CreateFileInfo file_path len error\r\n");
//         return 0; //URL error;
//     }
//     if (!upload_file_info)
//     {
//         fprintf(stderr, "CreateFileInfo upload_file_info error\r\n");
//         return 0; //URL error;
//     }

//     usr = cJSON_CreateObject();                                               //创建根数据对象
//     cJSON_AddStringToObject(usr, "time", upload_file_info->time);             //加入键值，加字符串
//     cJSON_AddNumberToObject(usr, "event_type", upload_file_info->event_type); //加入键值，加整型
//     cJSON_AddStringToObject(usr, "camera_id", upload_file_info->camera_id);
//     cJSON_AddStringToObject(usr, "id", upload_file_info->id);
//     cJSON_AddStringToObject(usr, "event_description", upload_file_info->event_description);
//     cJSON_AddStringToObject(usr, "upload_filename", upload_file_info->upload_file_name);

//     char *out = cJSON_Print(usr); //将json形式转换字符串形式

//     for (int i = 0; form_data != NULL && form_data[i].type != INVILLED_FORM_DATA_TYPE; i++)
//     {
//         if (form_data[i].type == CURLFORM_COPYCONTENTS)
//         {
//             memset(form_data[i].value, 0, sizeof(form_data[i].value));
//             strncpy(form_data[i].value, out, lsizeof(form_data[i].value));
//         }
//         else if (form_data[i].type == CURLFORM_FILE)
//         {
//             memset(form_data[i].value, 0, sizeof(form_data[i].value));
//             strncpy(form_data[i].value, file_path, lsizeof(form_data[i].value));
//             break;
//         }
//     }
//     // 释放内存
//     cJSON_Delete(usr);
//     return 1;
// }