#ifndef _ADMIN_H_
#define _ADMIN_H_

#include "User.h"

typedef struct admin{
	void *private;
	void(*release)(void *self);
	void(*init)(void *self, ...);
	Class_t (*getClass)(void *self);

	// User methods.
	boolean(*authorize)(void *self, const char *z_password);
	const char *(*getUsername)(void *self);
	boolean(*isAuthorized)(void *self);

	// Admin methods.
	int(*add_client)(void *self, struct client *p_client);
	int(*remove_client)(void *self, struct client *p_client);
	int(*update_client_info)(void *self, struct client *p_old_client, struct client *p_new_client);
	int(*create_account)(void *self, struct client *p_client, enum AccountType account_type);
	int(*change_account_type)(void *self, struct client *p_client, enum AccountType account_type);
} admin;

extern const Class_t Admin;

#endif // !_ADMIN_H_
