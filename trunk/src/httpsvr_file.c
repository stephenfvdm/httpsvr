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

#include "httpsvr.h"


int httpsvr_generic_file_handler(const char *content_type,
                                 const char *path,
                                 const char *parameters,
                                 char *buffer,
                                 int buffer_len) {
    int len = 0;
    int n = 0;
    
    if ((path != NULL) && (buffer != NULL)) {
        FILE *fp = fopen(path, "r");
        if (fp != NULL) {
            n = httpsvr_append_content_type(buffer, buffer_len, content_type);
            len += n;
            buffer_len -= n;
            n = fread(&buffer[len], 1, buffer_len, fp);
            if (n > 0) {
                len += n;
                buffer_len -= n;
            }
        }
    }
    
    return len;
}


int httpsvr_css_file_handler(const char *path,
                             const char *parameters,
                             char *buffer,
                             int buffer_len) {
    return httpsvr_generic_file_handler("text/css",
                                        path,
                                        parameters,
                                        buffer,
                                        buffer_len);
}


int httpsvr_csv_file_handler(const char *path,
                             const char *parameters,
                             char *buffer,
                             int buffer_len) {
    return httpsvr_generic_file_handler("text/csv",
                                        path,
                                        parameters,
                                        buffer,
                                        buffer_len);
}


int httpsvr_html_file_handler(const char *path,
                              const char *parameters,
                              char *buffer,
                              int buffer_len) {
    return httpsvr_generic_file_handler("text/html",
                                        path,
                                        parameters,
                                        buffer,
                                        buffer_len);
}


int httpsvr_plain_file_handler(const char *path,
                               const char *parameters,
                               char *buffer,
                               int buffer_len) {
    return httpsvr_generic_file_handler("text/plain",
                                        path,
                                        parameters,
                                        buffer,
                                        buffer_len);
}


int httpsvr_xml_file_handler(const char *path,
                             const char *parameters,
                             char *buffer,
                             int buffer_len) {
    return httpsvr_generic_file_handler("text/xml",
                                        path,
                                        parameters,
                                        buffer,
                                        buffer_len);
}


int httpsvr_gif_file_handler(const char *path,
                             const char *parameters,
                             char *buffer,
                             int buffer_len) {
    return httpsvr_generic_file_handler("text/gif",
                                        path,
                                        parameters,
                                        buffer,
                                        buffer_len);
}


int httpsvr_jpeg_file_handler(const char *path,
                              const char *parameters,
                              char *buffer,
                              int buffer_len) {
    return httpsvr_generic_file_handler("text/jpeg",
                                        path,
                                        parameters,
                                        buffer,
                                        buffer_len);
}


int httpsvr_png_file_handler(const char *path,
                             const char *parameters,
                             char *buffer,
                             int buffer_len) {
    return httpsvr_generic_file_handler("text/png",
                                        path,
                                        parameters,
                                        buffer,
                                        buffer_len);
}


int httpsvr_svg_file_handler(const char *path,
                             const char *parameters,
                             char *buffer,
                             int buffer_len) {
    return httpsvr_generic_file_handler("text/svg+xml",
                                        path,
                                        parameters,
                                        buffer,
                                        buffer_len);
}


int httpsvr_tiff_file_handler(const char *path,
                              const char *parameters,
                              char *buffer,
                              int buffer_len) {
    return httpsvr_generic_file_handler("text/tiff",
                                        path,
                                        parameters,
                                        buffer,
                                        buffer_len);
}
