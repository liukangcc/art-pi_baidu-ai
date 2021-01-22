#include <rtthread.h>
#include <rtdevice.h>
#include "drv_common.h"
#include <webclient.h>
#include <cJSON.h>
#include <dfs_posix.h>
#include <http.h>

#ifdef DRV_DEBUG
#define DBG_LVL               DBG_LOG
#else
#define DBG_LVL               DBG_INFO
#endif
#define LOG_TAG             "baiduai_image"
#include <drv_log.h>

//#define BD_AI_BAIKE

#define MAX_ENCODE_IMAGE_SIZE  6144
#define MAX_IMAGE_SIZE         4096

const char *access_token = "https://aip.baidubce.com/oauth/2.0/token?grant_type=client_credentials&client_id=your api key&client_secret=your Secret Key";
 
const char *BAIDU_AI_API[] = {
    "https://aip.baidubce.com/rest/2.0/image-classify/v2/advanced_general", /* advanced General Url */
    "https://aip.baidubce.com/rest/2.0/image-classify/v2/dish",             /* dish Detect Url*/
    "https://aip.baidubce.com/rest/2.0/image-classify/v1/car",              /* car Detect Url */
    "https://aip.baidubce.com/rest/2.0/image-classify/v1/vehicle_detect",   /* vehicle Detect Url */
    "https://aip.baidubce.com/rest/2.0/image-classify/v1/vehicle_damage",
    "https://aip.baidubce.com/rest/2.0/image-classify/v2/logo",
    "https://aip.baidubce.com/rest/2.0/realtime_search/v1/logo/add",
    "https://aip.baidubce.com/rest/2.0/realtime_search/v1/logo/delete",
    "https://aip.baidubce.com/rest/2.0/image-classify/v1/animal",
    "https://aip.baidubce.com/rest/2.0/image-classify/v1/plant",
    "https://aip.baidubce.com/rest/2.0/image-classify/v1/object_detect",
    "https://aip.baidubce.com/rest/2.0/image-classify/v1/landmark",
    "https://aip.baidubce.com/rest/2.0/image-classify/v1/flower",
    "https://aip.baidubce.com/rest/2.0/image-classify/v1/classify/ingredient",
    "https://aip.baidubce.com/rest/2.0/image-classify/v1/redwine",
    "https://aip.baidubce.com/rest/2.0/image-classify/v1/currency",
    "https://aip.baidubce.com/rest/2.0/image-classify/v1/realtime_search/dish/add",
    "https://aip.baidubce.com/rest/2.0/image-classify/v1/realtime_search/dish/search",
    "https://aip.baidubce.com/rest/2.0/image-classify/v1/realtime_search/dish/delete",
    "https://aip.baidubce.com/rest/2.0/image-classify/v1/multi_object_detect",
    "https://aip.baidubce.com/api/v1/solution/direct/imagerecognition/combination"
};

rt_err_t baidu_ai_sample(char *filename)
{
    unsigned char *ai_token = RT_NULL;
    int ai_token_len = 0;
    rt_err_t result = RT_EOK;
    int fd;
    unsigned char *buff_ptr = RT_NULL;
    unsigned char *encode_ptr = RT_NULL;

    rt_size_t total_length, encode_size, post_uri_size;

    rt_uint8_t *post_uri = RT_NULL;

    const char image_arg[] = "image=";
    rt_uint8_t image_arg_len = rt_strlen(image_arg);
    const char token_arg[] = "?access_token=";
    rt_uint8_t token_arg_len = rt_strlen(token_arg);
#ifdef BD_AI_BAIKE
    const char baike_args[] = "&baike_num=5";
    rt_uint8_t baike_args_len = rt_strlen(baike_args);
#endif

    rt_uint8_t *post_data = RT_NULL;
    rt_size_t post_data_size = 0, percent_len = 0;

    static int index = 0;

    /* step1: get token */
    ai_token = rt_malloc(256);
    if (ai_token == RT_NULL)
    {
        result = -RT_ENOMEM;
        goto __exit;
    }
    LOG_D("start to get baidu ai token");
    ai_token_len = get_ai_token(access_token, ai_token);
    LOG_D("token : %s", ai_token);
    if (ai_token_len <= 0)
    {
        result = -RT_ENOMEM;
        goto __exit;
    }
    LOG_D("get baidu ai token success");

    /* step2: get picture data */
    LOG_D("open filename : %s", filename);

    fd = open(filename, 0, O_RDONLY);
    if (fd < 0)
    {
        rt_kprintf("open file: %s failed\n", filename);
        result = -RT_ENOMEM;
        goto __exit;
    }

    buff_ptr = rt_malloc(MAX_IMAGE_SIZE);
    if (buff_ptr == RT_NULL)
    {
        rt_kprintf("no memory\n");
        result = -RT_ENOMEM;
        close(fd);
        goto __exit;
    }

    total_length = read(fd, buff_ptr, MAX_IMAGE_SIZE);
    if (total_length > MAX_IMAGE_SIZE)
    {
        LOG_E("image size too big.\n");
        result = -RT_ERROR;
        goto __exit;
    }
    LOG_D("open file success.");
    /* close file and release memory */
    close(fd);

    /* step3: encode image */
    LOG_D("encode image start.");
    encode_ptr = rt_malloc(MAX_ENCODE_IMAGE_SIZE);
    encode_size = base64_encode(buff_ptr, encode_ptr, total_length);

    /* step4: get post uri */
    index = strlen(BAIDU_AI_API[1]);
    post_uri_size = index;
    post_uri = rt_malloc(256);

    rt_memcpy(post_uri, BAIDU_AI_API[1], post_uri_size);
    rt_memcpy(&post_uri[post_uri_size], token_arg, token_arg_len);
    post_uri_size += token_arg_len;
    rt_memcpy(&post_uri[post_uri_size], ai_token, ai_token_len);
    post_uri_size += ai_token_len;

    LOG_D("post uri: %s", post_uri);

    /* step5: percentage code post data */
#ifdef BD_AI_BAIKE
    post_data = rt_malloc(encode_size * 3 + image_arg_len + baike_args_len);
#else
    post_data = rt_malloc(encode_size * 3 + image_arg_len);
#endif

    if (post_data == RT_NULL)
    {
        LOG_E("no memory.\n");
        result = -RT_ERROR;
        goto __exit;
    }
    rt_memcpy(post_data, image_arg, image_arg_len);

    LOG_D("percentage http data.");
    percent_len = http_percentage_coding(encode_ptr, &post_data[image_arg_len], encode_size);

    post_data_size = percent_len + image_arg_len;

#ifdef BD_AI_BAIKE
    if (percent_len < encode_size * 3)
    {
        rt_memcpy(&post_data[post_data_size], baike_args, baike_args_len);
        post_data_size += baike_args_len;
    }
#endif
    LOG_D("post data: %s", post_data);
    /* step: upload image to baidu ai */
    get_ai_result((const char *)post_uri, (const char *)post_data, post_data_size);

__exit:

    if(ai_token)
    {
        rt_free(ai_token);
    }

    if (buff_ptr)
    {
        rt_free(buff_ptr);
    }

    if (encode_ptr)
    {
        rt_free(encode_ptr);
    }

    if (post_data)
    {
        rt_free(post_data);
    }
    
    return result;

}

static int baidu_ai(int argc, char **argv)
{
    if (argc == 2)
    {
        baidu_ai_sample(argv[1]);
    }
    else
    {
        rt_kprintf("baidu_ai xxx.jpg\n");
    }
    
    return RT_EOK;
}
MSH_CMD_EXPORT(baidu_ai, baidu_ai sample);
