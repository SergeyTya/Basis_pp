#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H


#define HTTP_PORT 80
#define HTTP_BUFF_SIZE 1400

void http_server_init(void);

struct http_state
{
    size_t file_offset; // текущая позиция в отправке
    size_t content_len;
    size_t html_len;

    size_t dyn_data_ofst; 

    uint16_t adr_cnt;
    uint8_t done;

    size_t total_records;

    uint8_t page_to_send;


    char buff[HTTP_BUFF_SIZE];
};


#endif
