#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "stddefs.h"
#include "BSObject.h"

typedef struct account account;

typedef struct {
	void *private;
	void(*init)(void *self);
	void(*release)(void *self);
	Class_t *(*getClass)(void *self);

	// Client methods
	bs_c_string(*getName)(void *self);
	int(*getId)(void *self);
	account * const(*getAccounts)(void *self);
} client;

extern const Class_t Client;

#endif // !_CLIENT_H_
