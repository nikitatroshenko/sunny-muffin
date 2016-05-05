#ifndef _USER_H
#define _USER_H
#include "object_v2.h"
#include "stddefs.h"

struct user_v2;

struct user_v2_private;
struct user_v2_vars {
	struct user_v2_private *private;
};

struct user_v2_methods {
	ctor(_with_system_and_username, banking_system bs, bs_const_string z_username);
	dtor();
	method(boolean, authorize, bs_const_string z_password);
	method(boolean, is_authorized);
	method(bs_const_string, get_username);
};

struct user_v2 {
	struct bs_object_v2 super;
	struct user_v2_vars vars;
	struct user_v2_methods methods;
};

extern struct bs_class_struct_v2 User_v2;

#endif // !_USER_H
