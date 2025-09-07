#include <stdio.h>
#include "html_formatting.h"

int html_format(char *temp_ptr, char *file_contents, int file_length)
{
    int tot_char = 0;
    int num_chars = sprintf(temp_ptr, "HTTP/1.0 200 OK\r\n");
    temp_ptr += num_chars;
    tot_char += num_chars;

    num_chars = sprintf(temp_ptr, "Content-Type: text/html; charset=UTF-8\r\n");
    temp_ptr += num_chars;
    tot_char += num_chars;

    num_chars = sprintf(temp_ptr, "Content-Length: %d\r\n", file_length);
    temp_ptr += num_chars;
    tot_char += num_chars;

    num_chars = sprintf(temp_ptr, "Accept-Ranges: bytes\r\n");
    temp_ptr += num_chars;
    tot_char += num_chars;

    num_chars = sprintf(temp_ptr, "Connection: close\r\n\r\n");
    temp_ptr += num_chars;
    tot_char += num_chars;

    num_chars = sprintf(temp_ptr, "%s\r\n", file_contents);
    temp_ptr += num_chars;
    tot_char += num_chars;

    temp_ptr -= tot_char;

    return tot_char;
}