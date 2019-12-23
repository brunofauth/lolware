#include <stdio.h>
#include <string.h>
#include <stdbool.h>


#include "utils.c"


#define UNPACKER_EXE_SIZE 391346
#define INJECTOR_EXE_SIZE 391346


bool unpack(const char *full_bin_path, char *unpacker, char *injector, char *original) {

    FILE *full_bin = fopen(full_bin_path, "rb");
    if (full_bin == NULL)
        return false;
    
    // fuck race conditions tbh, i need this to work; i need a name
    // and not just a pointer to a handle or whatever.
    unpacker = tmpnam(NULL);
    if (unpacker == NULL)
        return false;

    if (!read_into_file(self, unpacker, UNPACKER_EXE_SIZE))
        return false;
    
    // fuck race conditions tbh, i need this to work; i need a name
    // and not just a pointer to a handle or whatever.
    injector = tmpnam(NULL);
    if (injector == NULL)
        return false;

    if (!read_into_file(self, injector, INJECTOR_EXE_SIZE))
        return false;
    
    // fuck race conditions tbh, i need this to work; i need a name
    // and not just a pointer to a handle or whatever.
    original = tmpnam(NULL);
    if (original == NULL)
        return false;

    if (!read_into_file(self, original, -1))
        return false;

    return true;
}


int main(int argc, char *argv[]) {

    char *unpacker;
    char *injector;
    char *original;

    if (!unpack(argv[0], unpacker, injector, original)) {
        perror(strerror(errno));
        return 1;
    }

    return 0;
}

