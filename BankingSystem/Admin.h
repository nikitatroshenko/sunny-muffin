#ifndef _ADMIN_H_
#define _ADMIN_H_

#include "BSObject.h"
#include "Client.h"

typedef struct admin{
	void *private;
	void(*release)(void *self);
	void(*init)(void *self, ...);
	Class_t (*getClass)(void *self);

	// User methods.
	boolean(*authorize)(const char *z_password);
	const char *(*getUsername)(void *self);
	boolean(*isAuthorized)(void *self);

	// Admin methods.
	int(*add_client)(admin *p_admin, const client *p_client);
	int(*remove_client)(admin *p_admin, const client *p_client);
	int(*update_client_info)(admin *p_admin, client *p_old_client, client *p_new_client);
	int(*create_account)(admin *p_admin, client *p_client, enum AccountType account_type);
	int(*change_account_type)(admin *p_admin, client *p_client, enum AccountType account_type);
} admin;

extern const Class_t Admin;

#endif // !_ADMIN_H_
