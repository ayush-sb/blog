#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

// File reading constants
#define READ_CHUNK_SIZE 512

#define  FILE_READ_OK          0  /* Success */
#define  FILE_READ_INVALID    -1  /* Invalid parameters */
#define  FILE_READ_ERROR      -2  /* Stream error */
#define  FILE_READ_TOOMUCH    -3  /* Too much input */
#define  FILE_READ_NOMEM      -4  /* Out of memory */

// HTTP header constants
#define HTTP_200_HEADER "HTTP/1.1 200 OK\n\n"
#define HTTP_200_HEADER_LENGTH 17

int read_file(FILE *fp, char **data_ptr, size_t *size_ptr);
char *add_200_header(char *body);

#endif
