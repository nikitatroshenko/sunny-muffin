#ifndef _ACCOUNT_H_
#define _ACCOUNT_H_

#include "object_v2.h"

enum AccountType {
	CHEQUE = 2,
	OVERDRAFT = 3,
	SAVING = 4,
	CLOSED = 0
};

struct Account{
	int number;
	double balance;
	enum AccountType account_type;
};

#endif // !_ACCOUNT_H_
