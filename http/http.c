#include <stdio.h>

// #include <json/json.h>
#include "http.h"

static size_t get_write_callback(void *contents,size_t size,size_t nmemb, void *userp)
{
    struct memory * data = userp;
    if(!data)
    {
        return 0;
    }
    size_t realsize = size * nmemb;
    data->response = (char *)realloc(data->response,data->size+realsize + 1);
    if(!data->response)
    {
        fprintf(stderr,"post realloc fail\r\n");
        return 0;
    }
    
    memcpy(&data->response[data->size],(char *) contents,realsize);
    data->size += realsize;
    data->response[data->size]  = 0;
    
    return realsize;
}
static size_t post_write_callback(void *contents,size_t size,size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct memory * data = userp;
    data->response = (char *)realloc(data->response,data->size+realsize + 1);
    if(!data->response)
    {
        fprintf(stderr,"post realloc fail\r\n");
        return 0;
    }
    
    memcpy(&data->response[data->size],(char *) contents,realsize);
    data->size += realsize;
    data->response[data->size]  = 0;
    return realsize;
}

int http_upload(const char *url,FormData *form_data,struct memory * response_data)
{
    CURL* hCurl = NULL;
    struct curl_slist* pOptionList = NULL;
    struct curl_http_post* pFormPost = NULL;
    struct curl_http_post* pLastElem = NULL;
    int i = 0;
    pOptionList = curl_slist_append(pOptionList, "Expect:");
    if(pOptionList == NULL)
    {
        fprintf(stderr, "HttpUpload curl_slist_append  Error\r\n");
        return 0;
    }
    for(i = 0;(form_data != NULL && form_data[i].type !=  INVILLED_FORM_DATA_TYPE &&  form_data[i].key != NULL &&  form_data[i].value != NULL); i++ )
    {
        switch(form_data[i].type)
        {
            case CURLFORM_FILE:
                if(curl_formadd(&pFormPost, &pLastElem, CURLFORM_COPYNAME, form_data[i].key, CURLFORM_FILE, form_data[i].value, CURLFORM_END) != 0)
    {
                    fprintf(stderr, "HttpUpload curl_formadd  Error %s\r\n",form_data[i].key);
        curl_formfree(pFormPost);
        return 0;
                }
                break;
            case CURLFORM_COPYCONTENTS:
                if(curl_formadd(&pFormPost, &pLastElem, CURLFORM_COPYNAME, form_data[i].key, CURLFORM_COPYCONTENTS, form_data[i].value, CURLFORM_END) != 0)
                {
                    fprintf(stderr, "HttpUpload curl_formadd  Error %s\r\n",form_data[i].key);
                    curl_formfree(pFormPost);
                    return 0;
                }
                break;
            default:
                fprintf(stderr, "HttpUpload curl_formadd  Error %s\r\n",form_data[i].key);
                curl_formfree(pFormPost);
                return 0;
        }
    }
    hCurl = curl_easy_init();
    if(hCurl == NULL)
    {
        curl_formfree(pFormPost);
        curl_slist_free_all(pOptionList);
        fprintf(stderr, "HttpUpload curl  Error\r\n");
        return 0;
    }
    if(hCurl)
    {
        curl_easy_setopt(hCurl, CURLOPT_HTTPHEADER, pOptionList);

        //上传文件，指定本地文件完整路径
        curl_easy_setopt(hCurl, CURLOPT_HTTPPOST, pFormPost);
        curl_easy_setopt(hCurl, CURLOPT_URL,url);
        curl_easy_setopt(hCurl, CURLOPT_WRITEFUNCTION,post_write_callback);
        curl_easy_setopt(hCurl, CURLOPT_WRITEDATA,(void *)response_data);
        CURLcode res = curl_easy_perform(hCurl);
        if(res != CURLE_OK)
        {
            fprintf(stderr, " HttpUpload perform  Error:%d\r\n",res);
            curl_formfree(pFormPost);
            curl_slist_free_all(pOptionList);
            curl_easy_cleanup(hCurl);
            return 0;
        }
        if(response_data->size == 0 )
        {
            fprintf(stderr,"HttpUpload response data_len inlegal\r\n ");
            curl_formfree(pFormPost);
            curl_slist_free_all(pOptionList);
            curl_easy_cleanup(hCurl);
            return 0;
        }
        curl_formfree(pFormPost);
        curl_slist_free_all(pOptionList);
        curl_easy_cleanup(hCurl);
        return 1;
    }
    return 0;
}
//返回0失败，返回其他未http状态码
int http_post(const char * url,const struct curl_slist *plist, const char *data,struct memory * response_data)
{
    CURL *curl;
    CURLcode res;
    /* In windows, this will init the winsock stuff */ 
    /* get a curl handle */
    if(data == NULL)
    {
        fprintf(stderr, "http_post data is empty");
        return 0;
    }
   
    curl = curl_easy_init();
    if(curl == NULL)
    {
        fprintf(stderr, "http_post url is null");
        return 0;
    }
    if(curl) 
    {
        /* First set the URL that is about to receive our POST. This URL can
           just as well be a https:// URL if that is what should receive the
           data. */ 
	curl_easy_setopt(curl, CURLOPT_URL, url);
	/* Now specify the POST data */ 
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    if(plist)
    {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, plist);  //json的说明加入消息头中
    }
    curl_easy_setopt(curl, CURLOPT_CAINFO, CACERT_FILE_PATH );
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS , HTTP_TIME_OUT_MS);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT , HTTP_TIME_OUT_MS);
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,post_write_callback);
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,response_data);
        /* Perform the request, res will get the return code */ 
       
	res = curl_easy_perform(curl);
        /* Check for errors */ 
    if(res != CURLE_OK)
    {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        return 0;
    }
        /* always cleanup */ 
        long response_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        curl_easy_cleanup(curl);
    	return response_code;
	}
    return 0;
}

int http_get(const char * url,struct memory *response_data)
{
    CURL *curl_handle;
    CURLcode res;
    curl_handle = curl_easy_init();
    if(curl_handle == NULL)
    {
        fprintf(stderr, "http_get url is null");
        return 0;
    }
    if(curl_handle) 
    {   
         /* send all src_data to this function  */
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, get_write_callback);
         /* we pass our 'chunk' struct to the callback function */
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)response_data);
        curl_easy_setopt(curl_handle, CURLOPT_CAINFO, CACERT_FILE_PATH );
        curl_easy_setopt(curl_handle, CURLOPT_URL, url);
        curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT_MS , HTTP_TIME_OUT_MS);
        curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT , HTTP_TIME_OUT_MS);
        /* Perform the request, res will get the return code */ 
        res = curl_easy_perform(curl_handle);
        /* Check for errors */ 
        if(res != CURLE_OK)
        {
            fprintf(stderr, "http_get failed: %s\n",curl_easy_strerror(res));
            curl_easy_cleanup(curl_handle);
            return 0;
        }
        else
        {
            long response_code;
            curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &response_code);
            curl_easy_cleanup(curl_handle);
            return response_code;
        }
    }
    return 0;
}
