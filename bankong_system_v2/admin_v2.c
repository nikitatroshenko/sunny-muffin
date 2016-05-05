#include "admin_v2.h"
#include "client_v2.h"
#include <sqlite3.h>
#include "System.h"
#include <string.h>

void * Admin_alloc();
void * Admin_static_alloc(struct admin_v2 *self);
void Admin_release(struct admin_v2 *self);

int Admin_add_client(struct admin_v2 *self, struct client_v2 *p_client);
int Admin_remove_client(struct admin_v2 *self, const char * client_id);
int Admin_update_client_info(struct admin_v2 *self, const char * client_id, struct client_v2 *p_new_client);
int Admin_create_account(struct admin_v2 *self, const char * client_id, enum AccountType account_type);
int Admin_change_account_type(struct admin_v2 *self, const char * client_id, int account_id, enum AccountType account_type);
int Admin_close_account(struct admin_v2 *self, const char * client_id, int account_id);

struct bs_class_struct_v2 Admin_v2 = {
	.alloc = Admin_alloc,
	.class = &Admin_v2,
	.super = &User_v2,
	.static_alloc = Admin_static_alloc,
	.string_val = "Admin"
};

struct admin_v2_private {
	boolean is_dynamic;
};

#define super ((struct user_v2 *)self)
#define __ ((struct admin_v2_private *)self->vars.private)->

void *Admin_alloc() {
	struct admin_v2 *self = bs_calloc(1, sizeof(struct admin_v2));

	Admin_static_alloc(self);
	__ is_dynamic = YES;

	return self;
}

void * Admin_static_alloc(struct admin_v2 *self) {
	ASSERT(self != Nil);
	User_v2.static_alloc(super);

	typeof(self) = Admin_v2.class;
	((struct bs_object_v2 *)self)->virtual_methods.release = Admin_release;

	self->methods.init_with_system_and_username = super->methods.init_with_system_and_username;
	self->methods.release = super->methods.release;

	self->methods.add_client = Admin_add_client;
	self->methods.update_client_info = Admin_update_client_info;
	self->methods.remove_client = Admin_remove_client;
	self->methods.create_account = Admin_create_account;
	self->methods.change_account_type = Admin_change_account_type;
	self->methods.close_account = Admin_close_account;

	self->vars.private = bs_calloc(1, sizeof(struct admin_v2_private));

	return self;
}

void Admin_release(struct admin_v2 *self) {
	boolean is_dynamic = __ is_dynamic;

	bs_free(self->vars.private);
	method_invoke(super, release);

	if (is_dynamic) bs_free(self);
}

int Admin_add_client(struct admin_v2 *self, struct client_v2 *p_client) {
	ASSERT(self != Nil);
	LOG("%s added client\n", method_invoke(super, get_username));
	sqlite3 *db = system_get_database_handler(property_get((struct bs_object_v2 *)self, system));
	char z_sql[150];
	int rc;

	sprintf(z_sql, "INSERT INTO BANK_CLIENTS "\
		"(id, is_individual, name, address) VALUES "\
		"('%s', %d, '%s', '%s');", p_client->id, p_client->is_individual, p_client->z_name, p_client->address);
	rc = sqlite3_exec(db, z_sql, Nil, Nil, Nil);

	if (rc == SQLITE_OK) return BS_OK;
	system_set_errcode(BS_ERR_DATABASE, property_get((struct bs_object_v2 *) self, system));

	return BS_ERR_DATABASE;
}

struct validate_client_id_callback_params {
	const char * client_id;
	boolean result;
};

int validate_client_id_callback(
	struct validate_client_id_callback_params *params,
	int n_cols,
	const char *field_names[],
	const char *columns_names[]) {
	params->result = !strcmp(params->client_id, field_names[0]);

	return !params->result;
}

int Admin_close_account(struct admin_v2 *self, const char * client_id, int account_id) {
	sqlite3 *db = system_get_database_handler(property_get((struct bs_object_v2 *)self, system));
	struct validate_client_id_callback_params params = {
		.client_id = client_id,
		.result = NO
	};
	char z_sql[50];
	int rc;

	sprintf(z_sql, "SELECT BANK_CLIENTS_id FROM BANK_ACCOUNTS WHERE account_number = %d", account_id);
	rc = sqlite3_exec(db, z_sql, validate_client_id_callback, &params, Nil);

	if (rc == SQLITE_OK) {
		if (params.result) {
			sprintf(z_sql, "UPDATE type WHERE account_number = %d", account_id);
			
			system_set_errcode(BS_OK, property_get((struct bs_object_v2 *)self, system));


		}
	}

	return 0;
}

int Admin_remove_client(struct admin_v2 *self, const char * client_id) {
	// TODO: Implement me :)
	// (for Venskiy)

	LOG("%s removed client\n", method_invoke(super, get_username));
	return 0;
}

int Admin_update_client_info(struct admin_v2 *self, const char * client_id, struct client_v2 *p_new_client) {
	// TODO: Implement me :)
	// (for Venskiy)

	LOG("%s updated client info\n", method_invoke(super, get_username));
	return 0;
}

int Admin_create_account(struct admin_v2 *self, const char * clietn_id, enum AccountType account_type) {
	// TODO: Implement me :)
	// (for Venskiy)

	LOG("%s created an account\n", method_invoke(super, get_username));
	return 0;
}

int Admin_change_account_type(struct admin_v2 *self, const char * client_id, int account_id, enum AccountType account_type) {
	// TODO: Implement me :)
	// (for Venskiy)

	LOG("%s changed account type\n", method_invoke(super, get_username));
	return 0;
}
