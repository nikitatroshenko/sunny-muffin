#ifndef _STD_DEFS_H_
#define _STD_DEFS_H_

#include <stdlib.h>

typedef char * bs_string;
typedef const char * bs_c_string;

typedef int boolean;
#define YES 1
#define NO 0

boolean bs_memory_equals(const void *buf1, const void *buf2, size_t size);

void *bs_memory_copy(void *dest_buf, const void *src_buf, size_t count, size_t size);

void *bs_malloc(size_t size);

void *bs_calloc(size_t count, size_t size);

void *bs_realloc(void *memory, size_t new_size);

void bs_free(void *memory);

int bs_strlen(const bs_c_string string);

char *bs_strcpy(bs_string dst, bs_c_string src);

#endif // !_STD_DEFS_H_
