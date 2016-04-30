#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "BSObject.h"
#include "Account.h"

typedef struct {
	void *private;
	void(*init)(void *self);
	void(*release)(void *self);
	Class_t *(*getClass)(void *self);

	// Client methods
	const char *(*getName)(void *self);
	const int(*getId)(void *self);
	account * const(*getAccounts)(void *self);
} client;

typedef struct {
	void *(*alloc)();
	Class_t *class;
	Class_t *super;
} Client_type;

extern const Client_type Client;

#endif // !_CLIENT_H_
