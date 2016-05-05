#ifndef _ADMIN_H_
#define _ADMIN_H_

#include "client_v2.h"
#include "user_v2.h"

struct admin_v2;

struct admin_v2_private;
struct admin_v2_vars {
	struct admin_v2_private *private;
};

struct admin_v2_methods {
	ctor(_with_system_and_username, banking_system bs, bs_const_string z_username);
	dtor();
	method(int, add_client, struct client_v2 *p_client);
	method(int, remove_client, bs_const_string client_id);
	method(int, update_client_info, bs_const_string client_id, struct client_v2 *p_new_client);
	method(int, create_account, bs_const_string client_id, enum AccountType account_type);
	method(int, change_account_type, bs_const_string client_id, int account_number, enum AccountType account_type);
	method(int, close_account, bs_const_string client_id, int account_number);
};

struct admin_v2 {
	struct user_v2 super;
	struct admin_v2_vars vars;
	struct admin_v2_methods methods;
};

extern struct bs_class_struct_v2 Admin_v2;

#endif // !_ADMIN_H_
