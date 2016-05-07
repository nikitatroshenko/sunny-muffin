#include <string.h>
#include "stddefs.h"

boolean bs_strcmp(const char *str1, const char *str2, boolean ignore_case) {
	if (ignore_case) return !_strcmpi(str1, str2);
	return !strcmp(str1, str2);
}

boolean bs_memory_equals(const void *buf1, const void *buf2, size_t size) {
	return !memcmp(buf1, buf2, size);
}

void *bs_memory_copy(void *dest_buf, const void *src_buf, size_t size) {
	return memcpy(dest_buf, src_buf, size);
}

int bs_strlen(const char * string) {
	return strlen(string);
}

char *bs_strcpy(char * dst, const char * src) {
	return strcpy(dst, src);
}

char * bs_to_lower_case(char * str) {
	char *c = str;

	while ('\0' != *c) {
		*c |= 32;
		c++;
	}

	return str;
}