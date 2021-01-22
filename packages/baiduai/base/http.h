#ifndef __HTTP_H
#define __HTTP_H

#ifndef POST_RESP_BUFSZ
#define POST_RESP_BUFSZ                10240
#endif

#ifndef POST_HEADER_BUFSZ
#define POST_HEADER_BUFSZ              1024
#endif

extern int get_ai_token(const char *uri, unsigned char *token);
extern int get_ai_result(const char *uri, const char *post_data, int post_data_size);

extern int http_percentage_coding(unsigned char *org_data, unsigned char *new_data, int len);
extern int base64_encode(unsigned char * bytes_to_encode, unsigned char *encode, int bytes_len);

#endif /* HTTP_H */
