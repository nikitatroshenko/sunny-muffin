#include <string.h>
#include <sqlite3.h>
#include "stddefs.h"
#include "operator_v2.h"
#include "System.h"

void * Operator_alloc();
void * Operator_static_alloc(struct operator_v2 *self);
void Operator_release(struct operator_v2 *self);

int Operator_debit(struct operator_v2 *self, bs_const_string client_id, int account_id, double sum);
int Operator_credit(struct operator_v2 *self, bs_const_string client_id, int account_id, double sum);
int Operator_transfer(struct operator_v2 *self, bs_const_string client_id, int account_from_id, int account_to_id, double sum);

struct bs_class_struct_v2 Operator_v2 = {
	.alloc = Operator_alloc,
	.class = &Operator_v2,
	.super = &User_v2,
	.static_alloc = Operator_static_alloc,
	.string_val = "Operator"
};

struct operator_v2_private {
	boolean is_dynamic;
};

// These macros are used for quick access to superclass and private fields corresponsively.
#define super ((struct user_v2 *)self)
#define __ ((struct operator_v2_private *)self->vars.private)->

void * Operator_alloc() {
	struct operator_v2 *self = bs_calloc(1, sizeof(struct operator_v2));

	Operator_static_alloc(self);
	__ is_dynamic = YES;

	return self;
}

void * Operator_static_alloc(struct operator_v2 *self) {
	ASSERT(self != Nil);
	User_v2.static_alloc(super);

	typeof(self) = Operator_v2.class;
	((struct bs_object_v2 *)self)->virtual_methods.release = Operator_release;

	self->methods.init_with_system_and_username = super->methods.init_with_system_and_username;
	self->methods.release = Operator_release;
	self->methods.debit = Operator_debit;
	self->methods.credit = Operator_credit;
	self->methods.transfer = Operator_transfer;
	self->vars.private = bs_calloc(1, sizeof(struct operator_v2_private));
	__ is_dynamic = NO;

	return self;
}

void Operator_release(struct operator_v2 *self) {
	boolean is_dynamic = __ is_dynamic;

	bs_free(self->vars.private);
	method_invoke(super, release);

	if (is_dynamic) bs_free(self);
}

// _In_out_ params for callback function.
struct fetch_account_details_params {
	bs_const_string client_id;
	double balance;
	boolean result;
};

// If this callback is invoked, then it checks client id to be equal to account's owner. If params->result if false,
// then function returns nonzero, which will occur into SQLITE_ABORT error.
int fetch_account_details(
	struct fetch_account_details_params *params,
	int n_cols,
	const char *field_vals[],
	const char *field_names[]) {
	params->result = !strcmp(params->client_id, field_vals[0]);
	params->balance = strtod(field_vals[1], Nil);

	return !params->result; // That means we've found the account but client is not its owner, so return value is nonzero.
}

// Decreases specified account balance with specified sum. Client id is used to make sure operation is legal.
int Operator_debit(struct operator_v2 *self, bs_const_string client_id, int account_id, double sum) {
	ASSERT(self != Nil);
	sqlite3 *db = system_get_database_handler(property_get((struct bs_object_v2 *) self, system));
	char z_sql[150];
	int rc;
	struct fetch_account_details_params params = {
		.client_id = client_id,
		.balance = -1,
		.result = NO
	};

	LOG("%s invokes debit %lf to account %d of %s.\n", method_invoke(super, get_username), sum, account_id, client_id);

	sprintf(z_sql, "SELECT BANK_CLIENTS_id, balance FROM BANK_ACCOUNTS WHERE account_number = %d;", account_id);
	rc = sqlite3_exec(db, z_sql, fetch_account_details, &params, Nil);

	if (rc == SQLITE_ABORT) {
		system_set_errcode(BS_ERR_CLIENT_ID, property_get((struct bs_object_v2 *) self, system));

		return BS_ERR_CLIENT_ID;
	} else if (rc == SQLITE_OK) {
		if (!params.result) { // That means that callback was not invoked, hence there is no such account.
			system_set_errcode(BS_ERR_ACCOUNT_ID, property_get((struct bs_object_v2 *) self, system));

			return BS_ERR_ACCOUNT_ID;
		}

		system_set_errcode(BS_OK, property_get((struct bs_object_v2 *) self, system));

		sprintf(z_sql, "UPDATE BANK_ACCOUNTS SET balance = %lf WHERE account_number = %d;", params.balance - sum, account_id);
		rc = sqlite3_exec(db, z_sql, Nil, Nil, Nil);

		if (rc == SQLITE_OK) {
			system_set_errcode(BS_OK, property_get((struct bs_object_v2 *) self, system));
			return BS_OK;
		}

		system_set_errcode(BS_ERR_DATABASE, property_get((struct bs_object_v2 *) self, system));
		return BS_ERR_DATABASE;
	}

	return 0;
}

// Increases specified account balance with specified sum. Client id is used to make sure operation is legal.
int Operator_credit(struct operator_v2 *self, bs_const_string client_id, int account_id, double sum) {
	// TODO: Implement me :)
	// (for Tsapliuk)

	LOG("%s invokes credit.\n", method_invoke(super, get_username));

	return 0;
}

// Transfers sum from one specified account to another. Client id is used to make sure operation is legal.
// Destination account can belong not only to client specified by client_id.
int Operator_transfer(struct operator_v2 *self, bs_const_string client_id, int account_from_id, int account_to_id, double sum) {
	// TODO: Implement me :)
	// (for Tsapliuk)

	LOG("%s invokes transfer.\n", method_invoke(super, get_username));

	return 0;
}
