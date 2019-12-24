#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


#ifndef UTILS_C
#define UTILS_C


bool read_fp_into_fp(FILE *src, FILE *dst, ssize_t max_bytes) {

    // Maybe im using too many variables here, but ut's 5:59 AM so whatever.
    size_t read_bytes, written_bytes, to_be_read, total_read;
    size_t buffer_size = 1024 * 1024;
    unsigned char *buffer = malloc(sizeof(char) * buffer_size);
    
    do {
        
        if (max_bytes < 0)
            to_be_read = buffer_size;
        else if (max_bytes < buffer_size)
            to_be_read = max_bytes;
        else if (total_read + buffer_size > max_bytes)
            to_be_read = max_bytes - total_read;
        else
            to_be_read = buffer_size;
        
        read_bytes = fread(buffer, sizeof(char), to_be_read, src);
        if (ferror(src)) {
            return false;
        }

        written_bytes = fwrite(buffer, sizeof(char), read_bytes, dst);
        if (ferror(dst)) {
            return false;
        }
        
        total_read += to_be_read;

    } while (!feof(src) && (total_read < max_bytes || max_bytes < 0));
    
    return true;
}


bool read_path_into_fp(const char *src, FILE *dst, ssize_t max_bytes) {

    FILE *src_file = fopen(src, "rb");
    if (src_file == NULL)
        return false;

    if (!read_fp_into_fp(src_file, dst, max_bytes))
        return false;
    
    if (fclose(src_file) == EOF)
        return false;

    return true;
}


bool read_fp_into_path(FILE *src, const char *dst, ssize_t max_bytes) {

    FILE *dst_file = fopen(dst, "wb");
    if (dst_file == NULL)
        return false;

    if (!read_fp_into_fp(src, dst_file, max_bytes))
        return false;
    
    if (fclose(dst_file) == EOF)
        return false;

    return true;
}


#endif

