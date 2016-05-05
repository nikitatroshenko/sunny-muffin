#ifndef _STD_DEFS_H_
#define _STD_DEFS_H_

#include <stdlib.h>
#include <assert.h>

typedef int boolean;
#define YES 1
#define NO 0

#if !NDEBUG
#include <stdio.h>
#define LOG(...) fprintf(stderr, __VA_ARGS__)
#define ASSERT(expression) assert(expression)
#endif // NDEBUG


#ifndef _countof
#define _countof(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif


boolean bs_memory_equals(const void *buf1, const void *buf2, size_t size);

void * bs_memory_copy(void *dest_buf, const void *src_buf, size_t size);

void * bs_malloc(size_t size);

void * bs_calloc(size_t count, size_t size);

void * bs_realloc(void *memory, size_t new_size);

void bs_free(void *memory);

int bs_strlen(const char *string);

char * bs_strcpy(char *dst, const char *src);

char * bs_to_lower_case(char *str);

#endif // !_STD_DEFS_H_

