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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#if defined (WIN32)
#  include <winsock2.h>
#  define CLOSE(soc)        closesocket(soc)
#else
#  include <unistd.h>
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <netinet/in.h>
#  define SOCKET            int
#  define INVALID_SOCKET    (-1)
#  define SOCKET_ERROR      (-1)
#  define SD_RECEIVE        SHUT_RD
#  define SD_SEND           SHUT_WR
#  define CLOSE(soc)        close(soc)
#endif

#include "httpsvr.h"


#define HTTPSVR_CONTENT_LENGTH_STR      "Content-Length:"
#define HTTPSVR_CONTENT_PLACE_HOLDER    "      0"

typedef struct {
    char                   *ext;
    httpsvr_file_handler    handler;
} httpsvr_file_handler_struct;

typedef struct {
    char                   *name;
    httpsvr_file_handler    handler;
} httpsvr_page_handler_struct;


typedef struct {
    SOCKET  listen_soc;
    SOCKET  recv_soc;
    char   *recv_data;
    int     recv_data_max_len;
    int     recv_data_len;
    char   *send_data;
    int     send_data_max_len;
    int     send_data_len;
    char   *req_method;
    char   *req_path;
    char   *req_params;
    char   *req_ver;
    char   *user_agent;
    int     user_agent_max_len;
    char   *file_path;
    char   *file_root_path;
    int     file_path_max_len;
    httpsvr_file_handler_struct *file_handlers;
    int     file_handlers_max_len;
    int     file_handlers_len;
    httpsvr_page_handler_struct *page_handlers;
    int     page_handlers_max_len;
    int     page_handlers_len;
} httpsvr_struct;


void httpsvr_init_struct(httpsvr_struct *hss) {
    hss->listen_soc             = INVALID_SOCKET;
    hss->recv_soc               = INVALID_SOCKET;
    hss->recv_data              = NULL;
    hss->recv_data_max_len      = 0;
    hss->recv_data_len          = 0;
    hss->send_data              = NULL;
    hss->send_data_max_len      = 0;
    hss->send_data_len          = 0;
    hss->req_method             = NULL;
    hss->req_path               = NULL;
    hss->req_params             = NULL;
    hss->req_ver                = NULL;
    hss->user_agent             = NULL;
    hss->user_agent_max_len     = 0;
    hss->file_path              = NULL;
    hss->file_root_path         = NULL;
    hss->file_path_max_len      = 0;
    hss->file_handlers          = NULL;
    hss->file_handlers_max_len  = 0;
    hss->file_handlers_len      = 0;
    hss->page_handlers          = NULL;
    hss->page_handlers_max_len  = 0;
    hss->page_handlers_len      = 0;
}


httpsvr_handle httpsvr_init(unsigned short port,
                            int recv_buffer_len,
                            int send_buffer_len,
                            int file_path_len,
                            int num_file_handlers,
                            int num_page_handlers) {
    int i = 0;
    struct sockaddr addr;
    int addrlen = sizeof(addr);
    httpsvr_struct *hss = NULL;
    hss = malloc(sizeof(httpsvr_struct));
    
    if (hss != NULL) {
        httpsvr_init_struct(hss);
        hss->recv_data_max_len  = recv_buffer_len;
        hss->recv_data          = malloc(hss->recv_data_max_len);
        hss->send_data_max_len  = send_buffer_len;
        hss->send_data          = malloc(hss->send_data_max_len);
        hss->user_agent_max_len = file_path_len;
        hss->user_agent         = malloc(hss->file_path_max_len);
        hss->file_path_max_len  = file_path_len;
        hss->file_path          = malloc(hss->file_path_max_len);
        hss->file_root_path     = malloc(hss->file_path_max_len);
        hss->file_handlers_max_len = num_file_handlers;
        hss->file_handlers      = malloc(hss->file_handlers_max_len * sizeof(httpsvr_file_handler_struct));
        hss->page_handlers_max_len = num_page_handlers;
        hss->page_handlers      = malloc(hss->page_handlers_max_len * sizeof(httpsvr_page_handler_struct));
        if ((hss->recv_data         == NULL) ||
            (hss->send_data         == NULL) ||
            (hss->user_agent        == NULL) ||
            (hss->file_path         == NULL) ||
            (hss->file_root_path    == NULL) ||
            (hss->file_handlers     == NULL) ||
            (hss->page_handlers     == NULL)) {
            if (hss->page_handlers != NULL) {
                free(hss->page_handlers);
            }
            if (hss->file_handlers != NULL) {
                free(hss->file_handlers);
            }
            if (hss->file_root_path != NULL) {
                free(hss->file_root_path);
            }
            if (hss->file_path != NULL) {
                free(hss->file_path);
            }
            if (hss->user_agent != NULL) {
                free(hss->user_agent);
            }
            if (hss->send_data != NULL) {
                free(hss->send_data);
            }
            if (hss->recv_data != NULL) {
                free(hss->recv_data);
            }
            free(hss);
            hss = NULL;
        } else {
            for (i = 0; i < hss->page_handlers_max_len; i++) {
                hss->page_handlers[i].name    = NULL;
                hss->page_handlers[i].handler = NULL;
            }
            for (i = 0; i < hss->file_handlers_max_len; i++) {
                hss->file_handlers[i].ext     = NULL;
                hss->file_handlers[i].handler = NULL;
            }
            strncpy(hss->file_root_path, ".", hss->file_path_max_len);
            strncpy(hss->user_agent, HTTPSVR_USER_AGENT, hss->user_agent_max_len);
            hss->recv_soc = INVALID_SOCKET;
            hss->listen_soc = socket(PF_INET, SOCK_STREAM, 0);
            if (hss->listen_soc == INVALID_SOCKET) {
                free(hss->page_handlers);
                free(hss->file_handlers);
                free(hss->file_root_path);
                free(hss->file_path);
                free(hss->send_data);
                free(hss->recv_data);
                free(hss);
                hss = NULL;
            } else {
                
                /* bind to local receive port */
                memset(&addr, 0, addrlen);
                struct sockaddr_in *config = (struct sockaddr_in *) &addr;
                config->sin_family      = AF_INET;
                config->sin_addr.s_addr = htonl(INADDR_ANY);  /* listen to anyone */
                config->sin_port        = htons(port);
                if (bind(hss->listen_soc, &addr, addrlen) == SOCKET_ERROR) {
                    CLOSE(hss->listen_soc);
                    free(hss->page_handlers);
                    free(hss->file_handlers);
                    free(hss->file_root_path);
                    free(hss->file_path);
                    free(hss->send_data);
                    free(hss->recv_data);
                    free(hss);
                    hss = NULL;
                }
            }
        }
    }
    
    return hss;
}


int httpsvr_add_file_handler(httpsvr_handle handle,
                             const char *file_extension,
                             httpsvr_file_handler file_handler) {
    int rc = -1;
    
    httpsvr_struct *hss = handle;
    if (hss != NULL) {
        
        /* check if file extension is already in the list */
        int i = 0;
        for (i = 0; i < hss->file_handlers_len; i++) {
            if (hss->file_handlers[i].ext != NULL) {
                int n = strcmp(file_extension,
                               hss->file_handlers[i].ext);
                if (n == 0) {
                    break;
                }
            }
        }
        
        if (i < hss->file_handlers_len) {
            
            /* replace existing file handler */
            hss->file_handlers[i].handler = file_handler;
            
        } else if (i < hss->file_handlers_max_len) {
            
            /* add new file handler */
            int n = strlen(file_extension) + 1;
            hss->file_handlers[i].ext = malloc(n);
            memset(hss->file_handlers[i].ext, 0, n);
            strncpy(hss->file_handlers[i].ext, file_extension, n - 1);
            hss->file_handlers[i].handler = file_handler;
            hss->file_handlers_len++;
        }
    }
    
    return rc;
}


int httpsvr_add_page_handler(httpsvr_handle handle,
                             const char *page_name,
                             httpsvr_page_handler page_handler) {
    int rc = -1;
    
    httpsvr_struct *hss = handle;
    if (hss != NULL) {
        
        /* check if page name is already in the list */
        int i = 0;
        for (i = 0; i < hss->page_handlers_len; i++) {
            if (hss->page_handlers[i].name != NULL) {
                int n = strcmp(page_name,
                               hss->page_handlers[i].name);
                if (n == 0) {
                    break;
                }
            }
        }
        
        if (i < hss->page_handlers_len) {
            
            /* replace existing page handler */
            hss->page_handlers[i].handler = page_handler;
            
        } else if (i < hss->file_handlers_max_len) {
            
            /* add new page handler */
            int n = strlen(page_name) + 1;
            hss->page_handlers[i].name = malloc(n);
            memset(hss->page_handlers[i].name, 0, n);
            strncpy(hss->page_handlers[i].name, page_name, n - 1);
            hss->page_handlers[i].handler = page_handler;
            hss->page_handlers_len++;
        }
    }
    
    return rc;
}


void httpsvr_print(const char *data, int data_len) {
    int i = 0;
    
    if (data != NULL) {
        for (i = 0; i < data_len; i++) {
            if (isprint(data[i])) {
                putchar(data[i]);
            } else {
                printf("[%02hX]", (0x00FF) & data[i]);
                if (data[i] == '\n') {
                    putchar('\n');
                }
            }
        }
        putchar('\n');
    }
}


void httpsvr_print_recv(httpsvr_handle handle) {
    httpsvr_struct *hss = handle;
    if (hss != NULL) {
        httpsvr_print(hss->recv_data, hss->recv_data_len);
    }
}


void httpsvr_print_send(httpsvr_handle handle) {
    httpsvr_struct *hss = handle;
    if (hss != NULL) {
        httpsvr_print(hss->send_data, hss->send_data_len);
    }
}


void httpsvr_print_req(httpsvr_handle handle) {
    httpsvr_struct *hss = handle;
    if (hss != NULL) {
        putchar('\n');
        printf("Method: %s\n", (hss->req_method == NULL) ? "NULL" : hss->req_method);
        printf("Path:   %s\n", (hss->req_path   == NULL) ? "NULL" : hss->req_path);
        printf("Params: %s\n", (hss->req_params == NULL) ? "NULL" : hss->req_params);
        printf("Ver:    %s\n", (hss->req_ver    == NULL) ? "NULL" : hss->req_ver);
    }
}


void httpsvr_append_send(httpsvr_handle handle, const char *s) {
    httpsvr_struct *hss = handle;
    if (hss != NULL) {
        int len = strlen(s);
        int max_len = hss->send_data_max_len - hss->send_data_len;
        strncpy(&hss->send_data[hss->send_data_len], s, max_len);
        hss->send_data_len += len;
        if (hss->send_data_len > hss->send_data_max_len) {
            hss->send_data_len = hss->send_data_max_len;
        }
        
        /* make sure send buffer remains null terminated */
        hss->send_data[hss->send_data_max_len - 1] = '\0';
    }
}


void httpsvr_send(httpsvr_handle handle) {
    httpsvr_struct *hss = handle;
    if (hss != NULL) {
        
        /* find start of content */
        int content_len = 0;
        const char *content = strstr(hss->send_data, "\r\n\r\n");
        if (content != NULL) {
            content += 4;  /* advance past blank line */
            int header_len = content - hss->send_data;
            content_len = hss->send_data_len - header_len;
            
            /* now fill in content length */
            char *s = strstr(hss->send_data, HTTPSVR_CONTENT_LENGTH_STR);
            if (s != NULL) {
                if ((s - hss->send_data) < header_len) {
                    char s2[16];
                    sprintf(s2, "%7d", content_len);
                    strncpy(s + 15, s2, 7);
                }
            }
        }
        send(hss->recv_soc, hss->send_data, hss->send_data_len, 0);
        httpsvr_print_send(handle);
    }
}


void httpsvr_echo_req(httpsvr_handle handle) {
    httpsvr_struct *hss = handle;
    if (hss != NULL) {
        hss->send_data_len = 0;
        memcpy(&hss->send_data[hss->send_data_len], hss->recv_data, hss->recv_data_len);
        hss->send_data_len += hss->recv_data_len;
        httpsvr_send(handle);
    }
}


void httpsvr_ok_resp(httpsvr_handle handle) {
    httpsvr_struct *hss = handle;
    if (hss != NULL) {
        hss->send_data_len = 0;
        httpsvr_append_send(handle, hss->req_ver);
        httpsvr_append_send(handle, " 200 OK\r\n");
        if (hss->user_agent[0] != '\0') {
            httpsvr_append_send(handle, "User-Agent: ");
            httpsvr_append_send(handle, hss->user_agent);
            httpsvr_append_send(handle, "\r\n");
        }
        
        /* content length place holder, to be filled in by httpsvr_send */
        httpsvr_append_send(handle, HTTPSVR_CONTENT_LENGTH_STR);
        httpsvr_append_send(handle, HTTPSVR_CONTENT_PLACE_HOLDER);
        httpsvr_append_send(handle, "\r\n");
    }
}


void httpsvr_no_content_resp(httpsvr_handle handle) {
    httpsvr_struct *hss = handle;
    if (hss != NULL) {
        hss->send_data_len = 0;
        httpsvr_append_send(handle, hss->req_ver);
        httpsvr_append_send(handle, " 204 No content\r\n");
        if (hss->user_agent[0] != '\0') {
            httpsvr_append_send(handle, "User-Agent: ");
            httpsvr_append_send(handle, hss->user_agent);
            httpsvr_append_send(handle, "\r\n");
        }
        httpsvr_send(handle);
    }
}


void httpsvr_bad_request_resp(httpsvr_handle handle) {
    httpsvr_struct *hss = handle;
    if (hss != NULL) {
        hss->send_data_len = 0;
        httpsvr_append_send(handle, hss->req_ver);
        httpsvr_append_send(handle, " 400 Bad request\r\n");
        if (hss->user_agent[0] != '\0') {
            httpsvr_append_send(handle, "User-Agent: ");
            httpsvr_append_send(handle, hss->user_agent);
            httpsvr_append_send(handle, "\r\n");
        }
        httpsvr_send(handle);
    }
}


void httpsvr_not_found_resp(httpsvr_handle handle) {
    httpsvr_struct *hss = handle;
    if (hss != NULL) {
        hss->send_data_len = 0;
        httpsvr_append_send(handle, hss->req_ver);
        httpsvr_append_send(handle, " 404 Not found\r\n");
        if (hss->user_agent[0] != '\0') {
            httpsvr_append_send(handle, "User-Agent: ");
            httpsvr_append_send(handle, hss->user_agent);
            httpsvr_append_send(handle, "\r\n");
        }
        httpsvr_append_send(handle, HTTPSVR_CONTENT_LENGTH_STR);
        httpsvr_append_send(handle, HTTPSVR_CONTENT_PLACE_HOLDER);
        httpsvr_append_send(handle, "\r\n");
        httpsvr_append_send(handle, "\r\nCould not find object\r\n");
        httpsvr_send(handle);
    }
}


int httpsvr_strncmp(const char *s1, int len1, const char *s2, int len2) {
    int rc = -1;
    
    if (len2 >= len1) {
        if (strncmp(s1, s2, len1) == 0) {
            rc = 1;
        }
    }
    
    return rc;
}


void httpsvr_parse_req(httpsvr_handle handle) {
    int i = 0;
    int n = 0;

    httpsvr_struct *hss = handle;
    if (hss != NULL) {
        hss->req_method = NULL;
        hss->req_path   = NULL;
        hss->req_params = NULL;
        hss->req_ver    = NULL;
        
        /* verify request method */
        if (httpsvr_strncmp("GET ", 4, hss->recv_data, hss->recv_data_len)) {
            n = 3;
            hss->recv_data[n++] = '\0';
            hss->req_method = hss->recv_data;
        } else if (httpsvr_strncmp("POST ", 5, hss->recv_data, hss->recv_data_len)) {
            n = 4;
            hss->recv_data[n++] = '\0';
            hss->req_method = hss->recv_data;
        } else if (httpsvr_strncmp("HEAD ", 5, hss->recv_data, hss->recv_data_len)) {
            n = 4;
            hss->recv_data[n++] = '\0';
            hss->req_method = hss->recv_data;
        }
        if (hss->req_method != NULL) {
            
            /* find request path */
            for (i = n; i < hss->recv_data_len; i++) {
                if (hss->recv_data[i] == ' ') {
                    hss->recv_data[i] = '\0';
                    hss->req_path = &hss->recv_data[n];
                    n = i + 1;
                    break;
                }
            }
            if (hss->req_path != NULL) {
                
                /* get parameters at the end of the path */
                hss->req_params = strchr(hss->req_path, '?');
                if (hss->req_params != NULL) {
                    hss->req_params[0] = '\0';
                    hss->req_params++;
                }
                
                /* determine request version */
                if (httpsvr_strncmp("HTTP", 4, &hss->recv_data[n], hss->recv_data_len - n)) {
                    for (i = n + 4; i < hss->recv_data_len; i++) {
                        if ((hss->recv_data[i] == '\r') || (hss->recv_data[i] == '\n')) {
                            hss->recv_data[i] = '\0';
                            hss->req_ver = &hss->recv_data[n];
                            break;
                        }
                    }
                }
            }
        }
    }
}


void httpsvr_process_file(httpsvr_handle handle) {
    int processed_flag = 0;
    int n = 0;
    httpsvr_struct *hss = handle;
    if (hss != NULL) {
        
        /* append resp path to root path */
        strncpy(hss->file_path, hss->file_root_path, hss->file_path_max_len);
        hss->file_path[hss->file_path_max_len - 1] = '\0';
        n = strlen(hss->file_path);
        strncpy(&hss->file_path[n], hss->req_path, hss->file_path_max_len - n);
        
        /* get file extension */
        const char *file_extension = strrchr(hss->req_path, '.');
        if (file_extension != NULL) {
            file_extension++;
        
            /* find matching file extension */
            int i = 0;
            for (i = 0; i < hss->file_handlers_len; i++) {
                n = strcmp(file_extension,
                           hss->file_handlers[i].ext);
                if (n == 0) {
                    break;
                }
            }
            
            /* if match was not found, then check if last extension is a wildcard */
            if (i == hss->file_handlers_len) {
                if (i > 0) {
                    n = strcmp(HTTPSVR_WILDCARD,
                               hss->file_handlers[i - 1].ext);
                    if (n == 0) {
                        i--;
                    }
                }
            }
            
            /* check if handler is valid */
            if (i < hss->file_handlers_len) {
                if (hss->file_handlers[i].handler != NULL) {
                    httpsvr_ok_resp(handle);
                    
                    /* call handler */
                    n = hss->file_handlers[i].handler(hss->file_path,
                                                          hss->req_params,
                                                          &hss->send_data[hss->send_data_len],
                                                          hss->send_data_max_len - hss->send_data_len);
                    /* check return status */
                    if (n > 0) {  /* ok */
                        if ((hss->send_data_len + n) > hss->send_data_max_len) {
                            hss->send_data_len = hss->send_data_max_len;
                        } else {
                            hss->send_data_len += n;
                        }
                        
                        /* send response */
                        httpsvr_send(handle);
                        processed_flag = 1;
                        
                    } else if (n == -HTTPSVR_STATUS_NO_CONTENT) {  /* no content */
                        httpsvr_no_content_resp(handle);
                        processed_flag = 1;
                        
                    } else if (n == -HTTPSVR_STATUS_MOVED) {  /* redirect */
                        
                        /* over write ok status with moved status */
                        char *s = strstr(hss->send_data, "200");
                        if (s != NULL) {
                            memcpy(s, "301", 3);
                        }
                        hss->send_data_len = strlen(hss->send_data);
                        
                        /* send response */
                        httpsvr_send(handle);
                        processed_flag = 1;
                    }
                }
            }
        }
        
        /* if no match, then respond with not found error */
        if (!processed_flag) {
            httpsvr_not_found_resp(handle);
        }
    }
}


void httpsvr_process_page(httpsvr_handle handle) {
    int processed_flag = 0;
    int n = 0;
    httpsvr_struct *hss = handle;
    if (hss != NULL) {
        
        /* skip leading slash */
        if ((hss->req_path[0] == '/') && (hss->req_path[1] != '\0')) {
            hss->req_path++;
        }
            
        /* find matching page */
        int i = 0;
        for (i = 0; i < hss->page_handlers_len; i++) {
            n = strcmp(hss->req_path,
                       hss->page_handlers[i].name);
            if (n == 0) {
                break;
            }
        }
        
        /* if match was not found, then check if last path name is a wildcard */
        if (i == hss->page_handlers_len) {
            if (i > 0) {
                n = strcmp(HTTPSVR_WILDCARD,
                           hss->page_handlers[i - 1].name);
                if (n == 0) {
                    i--;
                }
            }
        }
        
        /* check if handler is valid */
        if (i < hss->page_handlers_len) {
            if (hss->page_handlers[i].handler != NULL) {
                httpsvr_ok_resp(handle);
                
                /* call handler */
                n = hss->page_handlers[i].handler(hss->req_path,
                                                  hss->req_params,
                                                  &hss->send_data[hss->send_data_len],
                                                  hss->send_data_max_len - hss->send_data_len);
                /* check return status */
                if (n >= 0) {  /* ok */
                    if ((hss->send_data_len + n) > hss->send_data_max_len) {
                        hss->send_data_len = hss->send_data_max_len;
                    } else {
                        hss->send_data_len += n;
                    }
                    
                    /* send response */
                    httpsvr_send(handle);
                    processed_flag = 1;
                    
                } else if (n == -HTTPSVR_STATUS_NO_CONTENT) {  /* no content */
                    httpsvr_no_content_resp(handle);
                    processed_flag = 1;
                    
                } else if (n == -HTTPSVR_STATUS_MOVED) {  /* redirect */
                    
                    /* over write ok status with moved status */
                    char *s = strstr(hss->send_data, "200");
                    if (s != NULL) {
                        memcpy(s, "301", 3);
                    }
                    hss->send_data_len = strlen(hss->send_data);
                    
                    /* send response */
                    httpsvr_send(handle);
                    processed_flag = 1;
                }
            }
        }
    }
    
    /* if no match, then respond with not found error */
    if (!processed_flag) {
        httpsvr_not_found_resp(handle);
    }
}


void httpsvr_process_req(httpsvr_handle handle) {
    httpsvr_struct *hss = handle;
    if (hss != NULL) {
        hss->send_data_len = 0;

        /* special check for echo request */
        if (strncmp("GET /echo ", hss->recv_data, 10) == 0) {
            httpsvr_echo_req(handle);
        } else {
            httpsvr_parse_req(handle);
            httpsvr_print_req(handle);
            
            if (hss->req_path != NULL) {

                /* check path for bad characters */
                if (strrchr(hss->req_path, '~') != NULL) {
                    httpsvr_bad_request_resp(handle);
                
                /* check if requested path is a file (has a '.') */
                } else if (strrchr(hss->req_path, '.') != NULL) {
                    httpsvr_process_file(handle);
                    
                    /* else it must be a special page */
                } else {
                    httpsvr_process_page(handle);
                }
            } else {
                httpsvr_not_found_resp(handle);
            }

        }
    }
}


void httpsvr_receive(httpsvr_handle handle) {
    int n = 0;
    httpsvr_struct *hss = handle;
    if (hss != NULL) {
        if (listen(hss->listen_soc, 6) == 0) {
            hss->recv_soc = accept(hss->listen_soc, NULL, 0);
            if (hss->recv_soc != INVALID_SOCKET) {
                n = hss->recv_data_max_len - hss->recv_data_len;
                n = recv(hss->recv_soc, hss->recv_data, n, 0);
                if (n > 0) {
                    hss->recv_data_len += n;
                    httpsvr_process_req(handle);
                }
                shutdown(hss->recv_soc, SD_SEND | SD_RECEIVE);
                CLOSE(hss->recv_soc);
                hss->recv_data_len = 0;
            }
        }
    }
}


int httpsvr_redirect_to_index_html(const char *path,
                                   const char *parameters,
                                   char *buffer,
                                   int buffer_len) {
    int len = 0;
    int n = 0;
    
    n = httpsvr_append(buffer, buffer_len, "Location: /index.html\r\n");
    len += n;
    buffer_len -= n;
    
    return -HTTPSVR_STATUS_MOVED;
}


int httpsvr_append(char *buffer,
                   int buffer_len,
                   const char *text) {
    int len = strlen(text);

    strncpy(buffer, text, buffer_len);
    if (len > buffer_len) {
        len = buffer_len;
    }
    
    /* make sure send buffer remains null terminated */
    buffer[buffer_len - 1] = '\0';
    
    return len;
}


int httpsvr_append_content_type(char *buffer,
                                int buffer_len,
                                const char *content_type) {
    int len = 0;
    int n = 0;
    
    n = httpsvr_append(buffer, buffer_len, "Content-Type: ");
    len += n;
    buffer_len -= n;
    n = httpsvr_append(&buffer[len], buffer_len, content_type);
    len += n;
    buffer_len -= n;
    n = httpsvr_append(&buffer[len], buffer_len, "\r\n\r\n");
    len += n;
    buffer_len -= n;

    return len;
}
