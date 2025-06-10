#ifndef _ERROR_H_
#define _ERROR_H_

typedef enum {
    DL_OK = 0,

    // File errors
    DL_FILE_OPEN_ERR = 20,
    DL_FILE_READ_ERR = 21,
    DL_FILE_OPER_ERR = 22,

    // Alloc errors
    DL_MEMORY_ALLOC_ERR = 50,

    // Parser errors
    DL_TOO_MANY_CONSTS = 60,

    DL_COMPILER_ERR = 100, DL_RUNTIME_ERR = 101
} dl_result_t;

#endif // _ERROR_H_