#include "lexer.h"
#include "chunk.h"
#include "debug.h"
#include "err.h"
#include "vm.h"

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

dl_result_t process_file(
    const char *filepath
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

    // Lexer
    lexer_t lexer;
    lexer_init(&lexer, file_buffer, read_size);

    token_t token;
    while (true) {
        token = lexer_scan(&lexer);
        printf("TOK [%d][L:%d][%.*s]\n", token.type, token.line, token.length, token.start);
        if (token.type == TOK_EOF || token.type == TOK_ERROR) break;
    }

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

    // Testing TODO: Remove
    chunk_t chunk;
    chunk_init(&chunk);

    chunk_write_const(&chunk, 1.5, 123);
    chunk_write_const(&chunk, 400, 123);
    chunk_write_instr(&chunk, OP_MULTIPLY, 123);
    chunk_write_instr(&chunk, OP_NEGATE, 123);
    chunk_write_instr(&chunk, OP_NEGATE, 123);

    chunk_write_instr(&chunk, OP_RETURN, 125);

    vm_t vm;
    vm_init(&vm);
    vm_interpret(&vm, &chunk);
    vm_free(&vm);

    chunk_free(&chunk);
    return DL_OK;
}