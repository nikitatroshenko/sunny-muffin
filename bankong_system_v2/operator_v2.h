#ifndef _OPERATOR_H_
#define _OPERATOR_H_

#include "stddefs.h"
#include "Account.h"
#include "User_v2.h"

struct operator_v2;

struct operator_v2_vars {
	struct operator_v2_private *private;
};

struct operator_v2_methods {
	ctor(_with_system_and_username, banking_system bs, bs_const_string z_username);
	dtor();
	method(int, debit, bs_const_string client_id, int account_id, double sum);
	method(int, credit, bs_const_string client_id, int account_id, double sum);
	method(int, transfer, bs_const_string client_id, int account_id, double sum);
};

struct operator_v2 {
	struct user_v2 super;
	struct operator_v2_vars vars;
	struct operator_v2_methods methods;
};

extern struct bs_class_struct_v2 Operator_v2;

#endif // !_USER_H_
