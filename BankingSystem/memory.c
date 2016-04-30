#include "stddefs.h"
#include <string.h>

boolean bs_memory_equals(const void *buf1, const void *buf2, size_t size) {
	return !memcmp(buf1, buf2, size);
}

void *bs_memory_copy(void *dest_buf, const void *src_buf, size_t count, size_t size) {
	return memcpy(dest_buf, src_buf, count * size);
}

void *bs_malloc(size_t size) {
	return malloc(size);
}

void * bs_calloc(size_t count, size_t size) {
	return calloc(count, size);
}

void *bs_realloc(void *memory, size_t new_size) {
	return realloc(memory, new_size);
}

void bs_free(void *memory) {
	free(memory);
}

int bs_strlen(bs_c_string string) {
	return strlen(string);
}

char *bs_strcpy(bs_string dst, bs_c_string src) {
	return strcpy(dst, src);
}
