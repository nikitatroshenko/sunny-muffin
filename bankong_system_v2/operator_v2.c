#include <string.h>
#include <sqlite3.h>
#include "stddefs.h"
#include "operator_v2.h"
#include "System.h"

void * Operator_alloc();
void * Operator_static_alloc(struct operator_v2 *self);
void * Operator_init_with_system_and_username(struct operator_v2 *self, banking_system bs, const char *z_username);
void Operator_release(struct operator_v2 *self);

int Operator_debit(struct operator_v2 *self, const char *client_id, int account_id, double *sum);
int Operator_credit(struct operator_v2 *self, const char *client_id, int account_id, double *sum);
int Operator_transfer(struct operator_v2 *self, const char *client_id, int account_from_id, int account_to_id, double *sum);

struct bs_class_struct_v2 Operator_v2 = {
	.alloc = Operator_alloc,
	.class = &Operator_v2,
	.super = &User_v2,
	.static_alloc = Operator_static_alloc,
	.string_val = "Operator"
};

struct operator_v2_private {
	boolean is_dynamic;
	double overdraft_limit;
	double per_transaction_fee;
	int monthly_quota;
};

// These macros are used for quick access to superclass and private fields corresponsively.
#define super ((struct user_v2 *)self)
#define __ (self->vars.private)->

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
	super->virtual_methods.init_with_system_and_username = Operator_init_with_system_and_username;

	self->methods.init_with_system_and_username = Operator_init_with_system_and_username;
	self->methods.release = Operator_release;
	self->methods.debit = Operator_debit;
	self->methods.credit = Operator_credit;
	self->methods.transfer = Operator_transfer;
	self->vars.private = bs_calloc(1, sizeof(struct operator_v2_private));
	__ is_dynamic = NO;

	return self;
}

void * Operator_init_with_system_and_username(struct operator_v2 *self, banking_system bs, const char *z_username) {
	method_invoke(super, init_with_system_and_username, bs, z_username);

	__ overdraft_limit = strtod(system_get_property(bs, BS_PROP_OVERDRAFT_LIMIT), Nil);
	__ per_transaction_fee = strtod(system_get_property(bs, BS_PROP_PER_TRANSACTION_FEE), Nil);
	__ monthly_quota = atoi(system_get_property(bs, BS_PROP_CHEQUE_MONTHLY_QUOTA));

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
	const char *client_id;
	double balance;
	int transactions_cnt;
	boolean result;
};

// If this callback is invoked, then it checks client id to be equal to account's owner. If params->validated if false,
// then function returns nonzero, which will occur into SQLITE_ABORT error.
int fetch_account_details_callback(
	struct fetch_account_details_params *params,
	int n_cols,
	char *field_vals[],
	char *field_names[]) {
	params->result = !strcmp(params->client_id, field_vals[0]);
	params->balance = strtod(field_vals[1], Nil);
	params->transactions_cnt = atoi(field_vals[2]);

	return !params->result; // That means we've found the account but client is not its owner, so return value is nonzero.
}

int get_min_and_sum(double *sum, int n_cols, char **fields, char **cols) {
	*sum = (fields[0] == Nil) ? 0 : strtod(fields[0], Nil);

	return 0;
}

// Decreases specified account balance with specified sum. Client id is used to make sure operation is legal.
int Operator_debit(struct operator_v2 *self, const char *client_id, int account_id, double *sum) {
	ASSERT(self != Nil);
	sqlite3 *db = system_get_database_handler(property_get((struct bs_object_v2 *) self, system));
	char z_sql[150];
	int rc;
	struct fetch_account_details_params params = {
		.client_id = client_id,
		.result = NO,
		.transactions_cnt = 0
	};
	double clients_debts;

	LOG("%s invokes debit %lf to account %d of %s$.\n", method_invoke(super, get_username), sum, account_id, client_id);

	sprintf(z_sql, "SELECT BANK_CLIENTS_id, balance, transactions_cnt"
		" FROM BANK_ACCOUNTS WHERE account_number = %d;", account_id);
	rc = sqlite3_exec(db, z_sql, fetch_account_details_callback, &params, Nil);
	
	if (SQLITE_OK == rc) {
		if (params.result) {
			rc = BS_OK;
		} else {
			rc = BS_ERR_ACCOUNT_ID;
		}
	} else {
		if (SQLITE_ABORT == rc) {
			rc = BS_ERR_CLIENT_ID;
		} else {
			rc = BS_ERR_DATABASE;
		}
	}

	system_set_errcode(rc, property_get((struct bs_object_v2 *) self, system));

	if (rc == BS_OK) {		
		// Gather information about client's debts.
		sprintf(z_sql, "SELECT Sum(balance)"
			" FROM BANK_ACCOUNTS"			
			" WHERE BANK_CLIENTS_id = '%s'"	
			" AND type like 'OVERDRAFT'"		
			" AND balance < 0;",
			client_id);
		rc = sqlite3_exec(db, z_sql, get_min_and_sum, &clients_debts, Nil);
		if (rc != SQLITE_OK) {
			system_set_errcode(BS_ERR_DATABASE, property_get((struct bs_object_v2 *) self, system));
			return BS_ERR_DATABASE;
		}

		*sum += (params.transactions_cnt > __ monthly_quota) ? (params.transactions_cnt - __ monthly_quota) * __ per_transaction_fee : 0;
		if ((-__ overdraft_limit > clients_debts - *sum) || (-__ overdraft_limit > params.balance - *sum)) {
			rc = BS_ERR_OVERDRAFT_LIMIT_EXCEEDED;
		} else {
			sprintf(z_sql, "UPDATE BANK_ACCOUNTS SET balance = %lf WHERE account_number = %d;", params.balance - *sum, account_id);
			rc = sqlite3_exec(db, z_sql, Nil, Nil, Nil);
			rc = (rc == SQLITE_OK) ? BS_OK : BS_ERR_DATABASE;
			if (rc == BS_OK) {
				sprintf(z_sql, "UPDATE BANK_ACCOUNTS SET transactions_cnt = %d WHERE account_number = %d;", params.transactions_cnt + 1, account_id);
				sqlite3_exec(db, z_sql, Nil, Nil, Nil);
			}
		}
	}
	system_set_errcode(rc, property_get((struct bs_object_v2 *) self, system));

	return rc;
}

int fetch_account_details_credit_callback(
struct fetch_account_details_params *params,
	int n_cols,
	const char *field_vals[],
	const char *fiels_names[]) {
	params->result = YES;
	params->balance = strtod(field_vals[1], Nil);
	params->transactions_cnt = atoi(field_vals[2]);

	return 0;
}

// Increases specified account balance with specified sum. Client id is used to make sure operation is legal.
int Operator_credit(struct operator_v2 *self, const char *client_id, int account_id, double *sum) {
	LOG("%s invokes credit.\n", method_invoke(super, get_username));
	sqlite3 *db = system_get_database_handler(property_get((struct bs_object_v2 *)self, system));
	char z_sql[150];
	int rc;
	struct fetch_account_details_params params = {
		.client_id = client_id,
		.balance = -1,
		.result = NO
	};

	LOG("%s invokes credit %lf to account %d of %s$.\n", method_invoke(super, get_username), *sum, account_id, client_id);

	sprintf(z_sql, "SELECT BANK_CLIENTS_id, balance, transactions_cnt FROM BANK_ACCOUNTS WHERE account_number = %d;", account_id);
	rc = sqlite3_exec(db, z_sql, fetch_account_details_credit_callback, &params, Nil);

	if (rc == SQLITE_OK) {
		if (params.result) {
			rc = BS_OK;
		} else {
			rc = BS_ERR_ACCOUNT_ID;
		}
	} else if (rc != SQLITE_ABORT) {
		rc = BS_ERR_DATABASE;
	}

	system_set_errcode(rc, property_get((struct bs_object_v2 *) self, system));

	if (rc == BS_OK) {
		*sum -= (params.transactions_cnt > __ monthly_quota) ? (params.transactions_cnt - __ monthly_quota) * __ per_transaction_fee : 0;
		sprintf(z_sql, "UPDATE BANK_ACCOUNTS SET balance = %lf WHERE account_number = %d;", params.balance + *sum, account_id);
		rc = sqlite3_exec(db, z_sql, Nil, Nil, Nil);
		rc = (rc == SQLITE_OK) ? BS_OK : BS_ERR_DATABASE;
		
		if (rc == BS_OK) {
			sprintf(z_sql, "UPDATE BANK_ACCOUNTS SET transactions_cnt = %d WHERE account_number = %d;", params.transactions_cnt + 1, account_id);
			sqlite3_exec(db, z_sql, Nil, Nil, Nil);
		}
	}
	system_set_errcode(rc, property_get((struct bs_object_v2 *) self, system));

	return rc;
}

// Transfers sum from one specified account to another. Client id is used to make sure operation is legal.
// Destination account can belong not only to client specified by client_id.
int Operator_transfer(struct operator_v2 *self, const char *client_id, int account_from_id, int account_to_id, double *sum) {
	LOG("%s invokes transfer %lf from account %d to account %d of %s$.\n", method_invoke(super, get_username), *sum,
		account_from_id, account_to_id, client_id);
	int rc;
	sqlite3 *db = system_get_database_handler(property_get((struct bs_object_v2 *) self, system));

	sqlite3_exec(db, "BEGIN", Nil, Nil, Nil);
	rc = Operator_debit(self, client_id, account_from_id, sum);
	if (rc != BS_OK) {
		sqlite3_exec(db, "END", Nil, Nil, Nil);
		system_set_errcode(rc, property_get((struct bs_object_v2 *) self, system));
		return rc;
	}
	rc = Operator_credit(self, client_id, account_to_id, sum);
	if (rc != BS_OK) {
		sqlite3_exec(db, "END", Nil, Nil, Nil);
		system_set_errcode(rc, property_get((struct bs_object_v2 *) self, system));
		return rc;
	}
	sqlite3_exec(db, "END", Nil, Nil, Nil);
	system_set_errcode(rc, property_get((struct bs_object_v2 *) self, system));
	return rc;
}
