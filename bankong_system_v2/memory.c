#include "stddefs.h"
#include <string.h>
#include <assert.h>

static size_t gs_allocated_memory_size = 0;

void *bs_malloc(size_t size) {
	size_t *memory_block = malloc(size + sizeof(size_t));

	assert(memory_block != NULL);
	gs_allocated_memory_size += size;
	memory_block[0] = size;

	LOG("Allocating block of %d bytes.\n", size);

	return &memory_block[1];
}

void * bs_calloc(size_t count, size_t size) {
	size_t *memory_block = calloc(1, size * count + sizeof(size_t));

	assert(memory_block != NULL);
	gs_allocated_memory_size += size * count;
	memory_block[0] = size * count;

	LOG("Allocating block of %d bytes.\n", size * count);

	return &memory_block[1];
}

void *bs_realloc(void *memory, size_t new_size) {
	size_t *memory_block = (size_t *)memory - 1;
	memory_block = realloc(memory_block, new_size + sizeof(size_t));

	ASSERT(memory_block != NULL);

	LOG("Reallocating block of %d bytes with %d bytes.\n", memory_block[0], new_size);

	gs_allocated_memory_size += new_size - memory_block[0];
	memory_block[0] = new_size;

	return &memory_block[1];
}

void bs_free(void *memory) {
	size_t *memory_block = (size_t *)memory - 1;

	gs_allocated_memory_size -= memory_block[0];

	LOG("Deallocating block of %d bytes.\n", memory_block[0]);
	free(memory_block);
}

int get_allocated_memory_size() {
	return gs_allocated_memory_size;
}
