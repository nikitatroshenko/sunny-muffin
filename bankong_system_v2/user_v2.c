#include <string.h>
#include <sqlite3.h>
#include "user_v2.h"
#include "System.h"

void * User_alloc();
void * User_static_alloc(struct user_v2 *self);
void * user_init_with_system_and_username(struct user_v2 *self, banking_system bs, bs_const_string z_username);
void user_release(struct user_v2 *self);

boolean user_is_authorized(struct user_v2 *self);
boolean user_authorize(struct user_v2 *self, bs_const_string z_username);
bs_const_string user_get_username(struct user_v2 *self);

struct user_v2_private {
	boolean is_dynamic;
	boolean b_authorized;
	bs_string z_username;
};

struct bs_class_struct_v2 User_v2 = {
	.alloc = User_alloc,
	.class = &User_v2,
	.super = &BSObject_v2,
	.static_alloc = User_static_alloc,
	.string_val = "User"
};

#define super ((struct bs_object_v2 *)self)
#define __ ((struct user_v2_private *)self->vars.private)->

void * User_alloc() {
	struct user_v2 *self = bs_calloc(1, sizeof(struct user_v2));

	User_static_alloc(self);
	__ is_dynamic = YES;

	return self;
}

void * User_static_alloc(struct user_v2 *self) {
	ASSERT(self != NULL);
	BSObject_v2.static_alloc(super);

	((struct bs_object_v2 *)self)->class = User_v2.class;
	((struct bs_object_v2 *)self)->virtual_methods.release = user_release;

	self->methods.init_with_system_and_username = user_init_with_system_and_username;
	self->methods.release = user_release;

	self->methods.authorize = user_authorize;
	self->methods.get_username = user_get_username;
	self->methods.is_authorized = user_is_authorized;
	self->vars.private = bs_calloc(1, sizeof(struct user_v2_private));
	__ is_dynamic = NO;

	return self;
}

void * user_init_with_system_and_username(struct user_v2 *self, banking_system bs, bs_const_string z_username) {
	method_invoke(super, init_with_system, bs);

	__ b_authorized = NO;
	__ z_username = bs_calloc(bs_strlen(z_username) + 1, sizeof(bs_char));
	bs_strcpy(__ z_username, z_username);

	return self;
}

void user_release(struct user_v2 *self) {
	boolean is_dynamic = __ is_dynamic;

	bs_free(__ z_username);
	bs_free(self->vars.private);
	method_invoke(super, release);

	if (is_dynamic) bs_free(self);
}

boolean user_is_authorized(struct user_v2 *self) {
	return __ b_authorized;
}

struct cmp_password_callback_params {
	const char *z_password;
	boolean result;
};

int cmp_password_callback(struct cmp_password_callback_params *params,
	int n_cols, char *field_names[], char *col_names[]) {
	params->result = YES;

	return 0;
}

boolean user_authorize(struct user_v2 *self, bs_const_string z_password) {
	ASSERT(self != Nil);
	LOG("Trying to authorize %s...\n", __ z_username);

	char z_sql[100];
	int rc;
	struct cmp_password_callback_params params = {
		.z_password = z_password,
		.result = NO
	};
	sqlite3 *db = system_get_database_handler(property_get(super, system));


	sprintf(z_sql, "SELECT * FROM BANK_USERS WHERE username = '%s' AND password = '%s';", __ z_username, z_password);
	rc = sqlite3_exec(db, z_sql, cmp_password_callback, &params, Nil);

	if (rc == SQLITE_OK) {
		system_set_errcode(BS_OK, property_get(super, system));

		if (!params.result) {
			system_set_errcode(BS_ERR_USER_PASSWORD, property_get(super, system));

			return NO;
		}

		system_set_errcode(BS_OK, property_get(super, system));

		return YES;
	}
	system_set_errcode(BS_ERR_DATABASE, property_get(super, system));

	return NO;
}

bs_const_string user_get_username(struct user_v2 *self) {
	return __ z_username;
}
