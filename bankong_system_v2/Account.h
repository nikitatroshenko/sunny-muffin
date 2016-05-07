#ifndef _ACCOUNT_H_
#define _ACCOUNT_H_

#include "stddefs.h"

enum AccountType {
	CHEQUE		= 0x01,
	OVERDRAFT	= 0x02,
	SAVING		= 0x04,
	CLOSED		= 0x08,
	DEFAULT		= 0x10
};

boolean is_valid_type_change(enum AccountType type_from, enum AccountType type_to);

const char * account_type_to_string(enum AccountType type);

enum AccountType parse_account_type(const char *z_type);

struct Account{
	int number;
	double balance;
	const char *client_id;
	enum AccountType account_type;
};

#endif // !_ACCOUNT_H_
