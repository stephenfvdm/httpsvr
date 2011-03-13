/*
 * Copyright (c) 2011, Jim Hollinger
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Jim Hollinger nor the names of its contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef HTTPSVR_H_
#define HTTPSVR_H_


#define HTTPSVR_USER_AGENT                  "httpsvr/1.0"
#define HTTPSVR_WILDCARD                    "*"


enum HTTPSVR_STATUS_CODES {
    HTTPSVR_STATUS_OK           = 200,
    HTTPSVR_STATUS_NO_CONTENT   = 204,
    HTTPSVR_STATUS_MOVED        = 301,
    HTTPSVR_STATUS_BAD_REQUEST  = 400,
    HTTPSVR_STATUS_NOT_FOUND    = 404,
};
    

typedef void *httpsvr_handle;

typedef int (*httpsvr_file_handler)(const char *file_path,
                                    const char *parameters,
                                    char *buffer,
                                    int buffer_len);

typedef int (*httpsvr_page_handler)(const char *page_name,
                                    const char *parameters,
                                    char *buffer,
                                    int buffer_len);


httpsvr_handle httpsvr_init(unsigned short port,
                            int recv_buffer_len,
                            int send_buffer_len,
                            int file_path_len,
                            int num_file_handlers,
                            int num_page_handlers);

int  httpsvr_set_user_agent(httpsvr_handle handle,
                            const char *user_agent);

int  httpsvr_add_file_handler(httpsvr_handle handle,
                              const char *file_extension,
                              httpsvr_file_handler file_handler);

int  httpsvr_add_page_handler(httpsvr_handle handle,
                              const char *page_name,
                              httpsvr_file_handler page_handler);

void httpsvr_receive(httpsvr_handle handle);

int httpsvr_redirect_to_index_html(const char *path,
                                   const char *parameters,
                                   char *buffer,
                                   int buffer_len);
    
int  httpsvr_append_content_type(char *buffer,
                                 int buffer_len,
                                 const char *content_type);

int  httpsvr_append(char *buffer,
                    int buffer_len,
                    const char *text);

#endif  /* HTTPSVR_H_ */
