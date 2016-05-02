#ifndef _OPERATOR_H_
#define _OPERATOR_H_

#include "stddefs.h"
#include "Account.h"

typedef struct operator {
	void *private;
	void(*release)(void *self);
	void(*init)(void *self, ...);
	Class_t (*getClass)(void *self);

	// User methods.
	boolean(*authorize)(void *self, bs_c_string z_password);
	bs_c_string(*getUsername)(void *self);
	boolean(*isAuthorized)(void *self);

	// Operator methods.
	int(*debit)(struct operator *self, struct account *p_account, int sum);
	int(*credit)(struct operator *self, struct account *p_account, int sum);
	int(*transfer)(struct operator *self, struct account *p_account, int sum);
} operator;

extern const Class_t Operator;

#endif // !_USER_H_
