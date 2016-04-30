#ifndef _ACCOUNT_H_
#define _ACCOUNT_H_

#include "BSObject.h"

enum AccountType {
	CHEQUE = 2,
	OVERDRAFT = 3,
	SAVING = 4,
	CLOSED = 0
};

typedef struct {
	void *private;
	void(*release)(void *self);
	void(*init)(void *self, ...);
	Class_t (*getClass)(void *self);

	// Account properties
	int number;
	double balance;
	enum AccountType account_type;
} account;

extern const Class_t Account;

#endif // !_ACCOUNT_H_
