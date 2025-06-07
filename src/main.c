#include "compiler.h"
#include "debug.h"
#include "err.h"

#include <stdio.h>
#include <stdlib.h>

#define ASSERT(_e, ...) if (!(_e)) { fprintf(stderr, __VA_ARGS__); exit(1); }

dl_result_t get_file_size(
    FILE *file,
    size_t *out_len
) {
    if (fseek(file, 0, SEEK_END) != 0) return DL_FILE_OPER_ERR;

    long file_len = ftell(file);

    if (file_len == -1L) return DL_FILE_OPER_ERR;
    if (fseek(file, 0, SEEK_SET) != 0) return DL_FILE_OPER_ERR;

    *out_len = (size_t)file_len;

    return DL_OK;
}

dl_result_t read_file(
    const char *filepath,
    char **buffer
) {
    FILE *file = fopen(filepath, "rb");
    if (file == NULL) {
        return DL_FILE_OPEN_ERR;
    }

    size_t file_len;
    if (get_file_size(file, &file_len) != DL_OK) {
        fclose(file);
        return DL_FILE_OPER_ERR;
    }

    char *file_buffer = (char *)malloc(sizeof(char) * (file_len + 1));
    if (file_buffer == NULL) {
        fclose(file);
        free(file_buffer);
        return DL_MEMORY_ALLOC_ERR;
    }

    size_t read_size = fread(file_buffer, 1, file_len, file);
    if (read_size != file_len) {
        fclose(file);
        free(file_buffer);
        return DL_FILE_READ_ERR;
    }

    file_buffer[file_len] = '\0';
    fclose(file);

    *buffer = file_buffer;

    return DL_OK;
}

dl_result_t process_file(
    const char *filepath
) {
    char *src_buffer = NULL;
    dl_result_t file_result = read_file(filepath, &src_buffer);
    if (file_result != DL_OK) return file_result;

    compile(src_buffer);

    free(src_buffer);
    return DL_OK;
}

int32_t main(
    int32_t argc,
    char *argv[]
) {
    ASSERT(argc >= 2, "Error: Specify input files\n");

    for (int32_t i = 1; i < argc; i++) {
        dl_result_t result = process_file(argv[i]);
        if (result != DL_OK) {
            fprintf(stderr, "Error [Code: %d]: Cannot process file [%s]\n", result, argv[i]);
        }
    }

    return DL_OK;
}