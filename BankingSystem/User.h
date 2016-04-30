#ifndef _USER_H_
#define _USER_H_

#include "stddefs.h"

typedef struct {
	void *private;
	void(*release)(void *self);
	void(*init)(void *self, ...);
	Class_t *(*getClass)(void *self);

	// User methods.
	boolean(*authorize)(void *self, const char *z_password);
	bs_c_string(*getUsername)(void *self);
	boolean(*isAuthorized)(void *self);
} user;

extern const Class_t User;

#endif // !_USER_H_
