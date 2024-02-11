#include "utils.h"

int read_file(FILE *fp, char **data_ptr, size_t *size_ptr) {
    if (fp == NULL || data_ptr == NULL || size_ptr == NULL) {
        return FILE_READ_INVALID;
    }

    char *file_data = NULL;

    size_t total_read = 0;
    size_t buffer_size = 0;
    size_t temp = 0;

    while (true) {
        if (READ_CHUNK_SIZE + total_read + 1 > buffer_size) {
            // Buffer too small
            // Reallocate additional READ_CHUNK_SIZE + 1 bytes
            buffer_size += READ_CHUNK_SIZE + 1;
            file_data = (char *) realloc(file_data, buffer_size);
        }

        temp = fread(file_data + total_read, 1, READ_CHUNK_SIZE, fp);
        if (temp == 0) {
            // Nothing was read
            // We have reached EOF or something went wrong
            break;
        }

        total_read += temp;
    }

    // Reallocate, null-terminate, assign
    file_data = (char *) realloc(file_data, total_read + 1);
    file_data[total_read] = '\0';
    *data_ptr = file_data;

    *size_ptr = total_read;

    return FILE_READ_OK;
}

// Add 200 header (with newlines) to content
char *add_200_header(char *body) {
    char *header = (char *) malloc(HTTP_200_HEADER_LENGTH + 1);
    strcpy(header, HTTP_200_HEADER);

    // Append body to header
    size_t body_len = strlen(body);
    char *output = (char *) malloc(HTTP_200_HEADER_LENGTH + body_len + 1);
    strcpy(output, header);
    strcat(output, body);
    
    // Free allocated memory
    free(header);
    
    return output;
}


