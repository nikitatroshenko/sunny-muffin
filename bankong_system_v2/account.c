#include "Account.h"

boolean is_valid_type_change(enum AccountType type_from, enum AccountType type_to) {
	static int state_chng_graph[] = {
		[CHEQUE]	= OVERDRAFT | CLOSED,
		[OVERDRAFT]	= CHEQUE | CLOSED,
		[SAVING]	= CLOSED,
		[CLOSED]	= CLOSED
	};

	return state_chng_graph[type_from] & type_to;
}

const char * account_type_to_string(enum AccountType type) {
	static const char *account_types[] = {
		[CHEQUE]	= "CHEQUE",
		[OVERDRAFT]	= "OVERDRAFT",
		[SAVING]	= "SAVING",
		[CLOSED]	= "CLOSED",
		[DEFAULT]	= "DEFAULT"
	};

	return account_types[type];
}

enum AccountType parse_account_type(const char *z_type) {
	if (bs_strcmp(z_type, account_type_to_string(CHEQUE), YES)) return CHEQUE;
	if (bs_strcmp(z_type, account_type_to_string(OVERDRAFT), YES)) return OVERDRAFT;
	if (bs_strcmp(z_type, account_type_to_string(SAVING), YES)) return SAVING;
	if (bs_strcmp(z_type, account_type_to_string(CLOSED), YES)) return CLOSED;
	return DEFAULT;
}
