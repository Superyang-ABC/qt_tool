#ifndef HTTP_H_
#define HTTP_H_
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include "curl/curl.h"
struct memory
{
   char *response;
   size_t size;
};

#define INVILLED_FORM_DATA_TYPE 1
#define CACERT_FILE_PATH "/part/lib/openssl/ssl/certs/cacert.pem"
#define HTTP_TIME_OUT_MS 3000 //ms

typedef struct formdata_t FormData;
struct formdata_t
{
   int type;
   char key[64];
   char value[256];
};
#define lsizeof(array) ((sizeof(array) / sizeof(array[0])) - 1) //��ȡ�����С -1
int http_post(const char * url,const struct curl_slist *plist, const char *data,struct memory * response_data);
int http_get(const char *url, struct memory *data);
int http_upload(const char *url, FormData *form_data, struct memory *respone_data);
#endif
