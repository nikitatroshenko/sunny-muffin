#include "admin_v2.h"
#include "client_v2.h"
#include <sqlite3.h>
#include "System.h"
#include <string.h>
#include "Account.h"

void * Admin_alloc();
void * Admin_static_alloc(struct admin_v2 *self);
void Admin_release(struct admin_v2 *self);

int Admin_add_client(struct admin_v2 *self, struct client_v2 *p_client);
int Admin_remove_client(struct admin_v2 *self, const char * client_id);
int Admin_update_client_info(struct admin_v2 *self, const char * client_id, struct client_v2 *p_new_client);
int Admin_create_account(struct admin_v2 *self, struct Account *p_account);
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

///////////////////////// METHODS IMPLEMENTATION ////////////////////////////

#define ERROR_CHECK(code)	if ((code) != BS_OK) {\
								system_set_errcode(code, property_get((struct bs_object_v2 *) self, system));\
								return code;\
							}

int Admin_add_client(struct admin_v2 *self, struct client_v2 *p_client) {
	ASSERT(self != Nil);
	LOG("%s adds client\n", method_invoke(super, get_username));
	sqlite3 *db = system_get_database_handler(property_get((struct bs_object_v2 *)self, system));
	char z_sql[150];
	int rc;

	sprintf(z_sql, "INSERT INTO BANK_CLIENTS "\
		"(id, is_individual, name, address) VALUES "\
		"('%s', %d, '%s', '%s');", p_client->id, p_client->is_individual, p_client->z_name, p_client->address);
	rc = sqlite3_exec(db, z_sql, Nil, Nil, Nil);

	rc = (rc == SQLITE_OK) ? BS_OK : BS_ERR_DATABASE;
	LOG("%s: 0x%x", (rc == BS_OK) ? "Succeed" : "Failed", rc);
	system_set_errcode(rc, property_get((struct bs_object_v2 *) self, system));

	return rc;
}

struct validate_client_id_params {
	const char * client_id;
	int type;
	boolean validated;
};

int validate_client_id_callback(
	struct validate_client_id_params *params,
	int n_cols,
	const char *field_vals[],
	const char *columns_names[]) {
	params->validated = !strcmp(params->client_id, field_vals[0]);

	params->type = (field_vals[2] == Nil) ? DEFAULT : parse_account_type(field_vals[2]);

	return !params->validated || (params->type == DEFAULT);
}

int Admin_change_account_type_private(struct admin_v2 *self, int account_id, const char *client_id, enum AccountType type) {
	sqlite3 *db = system_get_database_handler(property_get((struct bs_object_v2 *)self, system));
	struct validate_client_id_params params = {
		.client_id = client_id,
		.type = DEFAULT,
		.validated = NO
	};
	char z_sql[150];
	int rc;

	sprintf(z_sql, "SELECT BANK_CLIENTS_id FROM BANK_ACCOUNTS WHERE account_number = %d", account_id);
	rc = sqlite3_exec(db, z_sql, validate_client_id_callback, &params, Nil);

	rc = (params.validated) ? BS_OK :
		(rc == SQLITE_OK) ? BS_ERR_ACCOUNT_ID :
		(params.type == DEFAULT) ? BS_ERR_INVALID_ARG :
		BS_ERR_CLIENT_ID;

	if (rc != BS_OK) {
		system_set_errcode(rc, property_get((struct bs_object_v2 *) self, system));
		return rc;
	}

	if (!is_valid_type_change(params.type, type)) {
		system_set_errcode(BS_ERR_ACCOUNT_TYPE, property_get((struct bs_object_v2 *) self, system));
		return BS_ERR_ACCOUNT_TYPE;
	}

	sprintf(z_sql, "UPDATE type WHERE account_number = %d", account_id);
	rc = sqlite3_exec(db, z_sql, Nil, Nil, Nil);
	rc = (rc == SQLITE_OK) ? BS_OK : BS_ERR_DATABASE;

	system_set_errcode(rc, property_get((struct bs_object_v2 *) self, system));

	return 0;
}

int Admin_close_account(struct admin_v2 *self, const char * client_id, int account_id) {
	int rc;

	LOG("%s closes account %d..\n", method_invoke(super, get_username), account_id);

	rc = Admin_change_account_type_private(self, account_id, client_id, CLOSED);

	LOG("%s: 0x%x", (rc == BS_OK) ? "Succeed" : "Failed", rc);
	system_set_errcode(rc, property_get((struct bs_object_v2 *) self, system));
	return rc;
}

void callback(){
	return 1;
}

int Admin_remove_client(struct admin_v2 *self, const char * client_id) {
	// TODO: Implement me :)
	// (for Venskiy)
	ASSERT(self != Nil);
	LOG("%s client removed\n", method_invoke(super, get_username));
	sqlite3 *db = system_get_database_handler(property_get((struct bs_object_v2 *)self, system));
	char z_sql[150];
	int rc;

	sprintf(z_sql, "SELECT * FROM BANK_ACCOUNTS WHERE BANK_CLIENTS_id='%s' AND type!='CLOSED';", client_id);
	rc = sqlite3_exec(db, z_sql, callback, Nil, Nil);

	if (rc == SQLITE_ABORT) {
		system_set_errcode(BS_ERR_ACCOUNTS_EXIST, property_get((struct bs_object_v2 *) self, system));
		return BS_ERR_ACCOUNTS_EXIST;
	}

	sprintf(z_sql, "DELETE FROM BANK_CLIENTS WHERE id='%s'; ", client_id);
	rc = sqlite3_exec(db, z_sql, Nil, Nil, Nil);

	if (rc == SQLITE_OK) return BS_OK;
	system_set_errcode(BS_ERR_DATABASE, property_get((struct bs_object_v2 *) self, system));

	return BS_ERR_DATABASE;
}

int Admin_update_client_info(struct admin_v2 *self, const char * client_id, struct client_v2 *p_new_client) {
	// TODO: Implement me :)
	// (for Venskiy)

	ASSERT(self != Nil);
	LOG("%s updated client info\n", method_invoke(super, get_username));
	sqlite3 *db = system_get_database_handler(property_get((struct bs_object_v2 *)self, system));
	char z_sql[150];
	int rc;

	if (p_new_client->z_name != "") {
		sprintf(z_sql, "UPDATE BANK_CLIENTS "\
			"SET is_individual=%d, name='%s' "\
			"WHERE id=%s; ", p_new_client->is_individual, p_new_client->z_name, p_new_client->id);
	}

	if (p_new_client->address != "") {
		sprintf(z_sql, "UPDATE BANK_CLIENTS "\
			"SET is_individual=%d, address='%s' "\
			"WHERE id=%s; ", p_new_client->is_individual, p_new_client->address, p_new_client->id);
	}

	rc = sqlite3_exec(db, z_sql, Nil, Nil, Nil);

	if (rc == SQLITE_OK) return BS_OK;
	system_set_errcode(BS_ERR_DATABASE, property_get((struct bs_object_v2 *) self, system));

	return BS_ERR_DATABASE;
}

#define POWS_PRECOUNTED 100
int get_next_account_number(const char *client_id, enum AccountType type) {
	int client_hash = 0;
	int result;
	int pow;
	const char *c;
	static int pows31[POWS_PRECOUNTED];
	static char magic;

	if (pows31[0] == 0) {
		pows31[0] = 1;
		for (int i = 1; i < 100; i++) {
			pows31[i] = pows31[i - 1] * 31;
		}
	}

	for (c = client_id, pow = 0; *c != '\0'; c++, pow++) {
		int curr_multiplier = (pow < POWS_PRECOUNTED) ? pows31[pow] : pows31[POWS_PRECOUNTED];
		
		for (int i = POWS_PRECOUNTED; i < pow; i++) {
			curr_multiplier *= 31;
		}
		client_hash += pows31[pow] * *c;
	}

	result = type;
	result &= 0xc;
	result <<= 30;
	result |= client_hash & 0x3fff00;
	result |= magic;
	magic++;

	return result;
}

int account_exists_callback(void *not_used, int n_cols, const char **fields_vals, const char **col_names) {
	return SQLITE_ABORT;
}

int Admin_create_account(struct admin_v2 *self, struct Account *p_account) {
	LOG("%s starts creating an account\n", method_invoke(super, get_username));
	sqlite3 *db = system_get_database_handler(property_get((struct bs_object_v2 *)self, system));
	int rc;
	char z_sql[150];
	char z_remarks[150];

	if ((p_account->account_type == CHEQUE) || (p_account->account_type == OVERDRAFT));
	
	do {
		p_account->number = get_next_account_number(p_account->client_id, p_account->account_type);

		sprintf(z_sql, "SELECT account_number FROM BANK_ACCOUNTS WHERE account_number = %d", p_account->number);
		rc = sqlite3_exec(db, z_sql, account_exists_callback, Nil, Nil);
	} while (rc == SQLITE_ABORT);
	
	sprintf(z_sql, "INSERT INTO BANK_ACCOUNTS (account_number, BANK_CLIENTS_id, type)"\
		" VALUES('%d', '%s', '%s');", p_account->number, p_account->client_id, account_type_to_string(p_account->account_type));
	rc = sqlite3_exec(db, z_sql, Nil, Nil, Nil);

	rc = (rc == SQLITE_OK) ? BS_OK : BS_ERR_DATABASE;
	system_set_errcode(rc, property_get((struct bs_object_v2 *) self, system));

	return rc;
}

int Admin_change_account_type(struct admin_v2 *self, const char * client_id, int account_id, enum AccountType account_type) {
	int rc;

	LOG("%s changes account type..\n", method_invoke(super, get_username));
	if (account_type == CLOSED) {
		rc = BS_ERR_INVALID_ARG;
	}
	else {
		rc = Admin_change_account_type_private(self, account_id, client_id, account_type);
	}

	LOG("%s: 0x%x", (rc == BS_OK) ? "Succeed" : "Failed", rc);
	system_set_errcode(rc, property_get((struct bs_object_v2 *) self, system));
	
	return 0;
}
