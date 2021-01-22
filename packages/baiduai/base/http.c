#include <rtthread.h>
#include <webclient.h>
#include <cJSON.h>
#include "http.h"

#ifdef DRV_DEBUG
#define DBG_LVL               DBG_LOG
#else
#define DBG_LVL               DBG_INFO
#endif
#define LOG_TAG             "baiduai_http"
#include <drv_log.h>

/* get token */
int get_ai_token(const char *uri, unsigned char *token)
{
    char *request = RT_NULL;
    int token_len = 0, index = 0;

    cJSON* cjson_parse = RT_NULL;
    cJSON* cjson_token = RT_NULL;

    if (webclient_request(uri, RT_NULL, RT_NULL, (unsigned char **)&request) < 0)
    {
        rt_kprintf("webclient send get request failed.");
        return -RT_ERROR;
    }

    rt_kprintf("webclient send get request by simplify request interface.\n");
    rt_kprintf("webclient get response data: \n");

    for (index = 0; index < rt_strlen(request); index++)
    {
        rt_kprintf("%c", request[index]);
    }
    rt_kprintf("\n");

    cjson_parse = cJSON_Parse(request);
    if(cjson_parse == RT_NULL)
    {
        LOG_E("parse fail.\n");
        goto __exit;
    }

    cjson_token = cJSON_GetObjectItem(cjson_parse, "access_token");
    if (cjson_token == RT_NULL)
    {
        LOG_E("get onject 'access_token' item fail.\n");
        goto __exit;
    }

    LOG_D("get_token: %s\n", cjson_token->valuestring);
    token_len = rt_strlen(cjson_token->valuestring);
    rt_memcpy(token, cjson_token->valuestring, token_len);

__exit:

    if (cjson_parse)
    {
        cJSON_Delete(cjson_parse);
        cjson_parse = RT_NULL;
        cjson_token = RT_NULL;
    }

    if (request)
    {
        web_free(request);
    }

    return token_len;
}

int get_ai_result(const char *uri, const char *post_data, int post_data_size)
{
    struct webclient_session* session = RT_NULL;
    unsigned char *buffer = RT_NULL;
    int index, result = 0, resp_status, bytes_read;

    buffer = (unsigned char *)web_malloc(POST_RESP_BUFSZ);
    if (buffer == RT_NULL)
    {
        rt_kprintf("no memory for receive response buffer.\n");
        result = -RT_ENOMEM;
        goto __exit;
    }

    /* create webclient session and set header response size */
    session = webclient_session_create(POST_HEADER_BUFSZ);
    if (session == RT_NULL)
    {
        result = -RT_ENOMEM;
        goto __exit;
    }

    /*  add http header */
    webclient_header_fields_add(session, "Content-Length: %d\r\n", post_data_size);
    webclient_header_fields_add(session, "Content-Type: application/x-www-form-urlencoded\r\n");

    /* send POST request by default header */
    if ((resp_status = webclient_post(session, uri, (const char *)post_data)) != 200)
    {
        LOG_E("webclient POST request failed, response(%d) error.\n", resp_status);
        result = -RT_ERROR;
        goto __exit;
    }

    rt_kprintf("webclient post response data: \n");
    do
    {
        bytes_read = webclient_read(session, buffer, POST_RESP_BUFSZ);
        if (bytes_read <= 0)
        {
            break;
        }
        for (index = 0; index < bytes_read; index++)
        {
            rt_kprintf("%c", buffer[index]);
        }

    } while (1);

    rt_kprintf("\n");

__exit:
    if (session)
    {
        webclient_close(session);
    }

    if (buffer)
    {
        web_free(buffer);
    }

    return result;  
}
