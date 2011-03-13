#include <stdio.h>

#include "httpsvr.h"
#include "httpsvr_file.h"



int httpsvr_wildcard_page(const char *path,
                          const char *parameters,
                          char *buffer,
                          int buffer_len) {
    int len = 0;
    int n = 0;
    
    n = httpsvr_append_content_type(buffer, buffer_len, "text/html");
    len += n;
    buffer_len -= n;
    n = httpsvr_append(&buffer[len], buffer_len, "<html><head><title>*</title></head>");
    len += n;
    buffer_len -= n;
    n = httpsvr_append(&buffer[len], buffer_len, "<body><h1>*</h1></body></html>");
    len += n;
    buffer_len -= n;
    
    return len;
}


int main (int argc, const char * argv[]) {
    unsigned short port     = 18080;
    int recv_buffer_len     = 1024;
    int send_buffer_len     = 8192;
    int file_path_len       = recv_buffer_len;
    int num_file_handlers   = 32;
    int num_page_handlers   = 32;
    httpsvr_handle handle   = NULL;
    handle = httpsvr_init(port,
                          recv_buffer_len,
                          send_buffer_len,
                          file_path_len,
                          num_file_handlers,
                          num_page_handlers);
    if (handle == NULL) {
        fprintf(stderr, "Failed to start httpsvr on port %hu\n", port);
    } else {
        httpsvr_add_file_handler(handle, "html", httpsvr_html_file_handler);
        httpsvr_add_file_handler(handle, "htm",  httpsvr_html_file_handler);
        httpsvr_add_file_handler(handle, "css",  httpsvr_css_file_handler);
        httpsvr_add_file_handler(handle, "ico",  httpsvr_html_file_handler);
        httpsvr_add_file_handler(handle, "png",  httpsvr_png_file_handler);
        httpsvr_add_file_handler(handle, "jpg",  httpsvr_jpeg_file_handler);
        httpsvr_add_file_handler(handle, "jpeg", httpsvr_jpeg_file_handler);
        httpsvr_add_file_handler(handle, "txt",  httpsvr_plain_file_handler);
        httpsvr_add_file_handler(handle, "text", httpsvr_plain_file_handler);
        httpsvr_add_file_handler(handle, "*",    httpsvr_html_file_handler);

        httpsvr_add_page_handler(handle, "/",    httpsvr_redirect_to_index_html);
        httpsvr_add_page_handler(handle, "*",    httpsvr_wildcard_page);
        while (1) {
            httpsvr_receive(handle);
        }
    }
    
    return 0;
}
