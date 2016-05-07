#include <time.h>
#include <string.h>
#include <sqlite3.h>
#include "System.h"
#include "operator_v2.h"
#include "admin_v2.h"

#define BS_PROPS_CNT 8
#define BS_PROP_IDX_MASK 0x10

struct _banking_system {
	sqlite3 *db_sqlite3;
	char *cfg_properties[BS_PROPS_CNT];
	int err_code;
};

struct load_config_params {
	banking_system bs;
	int rec_idx;
};

int load_config_callback(struct load_config_params *params, int n_cols, char *field_vals[], char *field_headers[]) {
	if ((BS_PROPS_CNT <= params->rec_idx) || (1 > n_cols)) {
		system_set_errcode(BS_ERR_CFG_LOAD_FAILED, params->bs);

		return BS_ERR_CFG_LOAD_FAILED; // That will cause load breaking with return code SQLITE_ABORT.
	}

	params->bs->cfg_properties[params->rec_idx] = bs_calloc(bs_strlen(field_vals[0]) + 1, sizeof(char));
	bs_strcpy(params->bs->cfg_properties[params->rec_idx], field_vals[0]);
	params->rec_idx++;

	return BS_OK;
}

int system_init(const char *z_database_filename, banking_system *p_bs) {
	LOG("Initializing the system...\n");

	int rc;

	*p_bs = bs_calloc(1, sizeof(struct _banking_system));
	ASSERT(*p_bs != Nil);
	rc = sqlite3_open_v2(z_database_filename, &(*p_bs)->db_sqlite3, SQLITE_OPEN_READWRITE, NULL);
	system_set_errcode(BS_OK, *p_bs);

	if (rc) {
		system_set_errcode(BS_ERR_DATABASE, *p_bs);
		sqlite3_close_v2((*p_bs)->db_sqlite3);
		LOG("Failed.\n");

		return BS_ERR_DATABASE;
	}
	
	struct load_config_params params = {
			.bs = *p_bs,
			.rec_idx = 0
	};

	rc = sqlite3_exec((*p_bs)->db_sqlite3,
		"SELECT value FROM BANK_CONFIG;",
		load_config_callback,
		&params,
		Nil);

	rc = (rc == SQLITE_OK) ? BS_OK :
		     (rc == SQLITE_ABORT) ? BS_ERR_CFG_LOAD_FAILED : BS_ERR_DATABASE;
	
	system_set_errcode(rc, *p_bs);
	LOG((rc == BS_OK) ? "Failed.\n" : "Succeed.\n");

	return rc;
}

int system_get_errcode(banking_system bs) {
	ASSERT(bs != Nil);

	return bs->err_code;
}

const char * system_get_errmsg(banking_system system) {
	static const char *error_names[] = {
		[BS_OK							] = "success",
		[BS_ERR_USER_LOGIN				] = "incorrect user login",
		[BS_ERR_USER_PASSWORD			] = "incorrect user password",
		[BS_ERR_DATABASE				] = "database defined error",
		[BS_ERR_CLIENT_ID				] = "incorrect client id",
		[BS_ERR_ACCOUNT_ID				] = "incorrect account id",
		[BS_ERR_USER_UNAUTHORIZED		] = "user is not authorized",
		[BS_ERR_OVERDRAFT_LIMIT_EXCEEDED] = "overdraft limit cannot be exceeded",
		[BS_ERR_CFG_LOAD_FAILED			] = "could not load configuration properies",
		[BS_ERR_INVALID_ARG				] = "invalid argument passed to system function",
		[BS_ERR_ACCOUNT_TYPE			] = "can not change account to this type",
		[BS_ERR_ACCOUNTS_EXIST			] = "can not delete client as he has unclosed accounts"
	};

	ASSERT(system != Nil);
	if ((system->err_code >= _countof(error_names)) || (system->err_code < 0)) return Nil;
	if (system->err_code == BS_ERR_DATABASE) return sqlite3_errmsg(system->db_sqlite3);

	return error_names[system->err_code];
}

void system_set_errcode(int code, banking_system system) {
	system->err_code = code;
}

sqlite3 * system_get_database_handler(banking_system system) {
	ASSERT(system != Nil);

	return system->db_sqlite3;
}

struct callback_params {
	char username[10];
	banking_system bs;
};

int fetch_username(struct callback_params *data, int n_cols, const char *field_names[], const char *col_names[]) {
	if (0 != n_cols) strcpy(data->username, field_names[0]);
	else {
		system_set_errcode(BS_ERR_USER_LOGIN, data->bs);
	}

	return 0;
}

struct user_v2 * system_authentificate(banking_system system, const char *z_username) {
	struct user_v2 *usr = Nil;
	int rc;
	char z_sql[100];
	struct callback_params params;

	ASSERT(system != Nil);
	params.bs = system;
	memset(params.username, 0, 10);

	LOG("Authentificating %s...\n", z_username);

	sprintf(z_sql, "SELECT user_type FROM BANK_USERS WHERE username = '%s';", z_username);
	rc = sqlite3_exec(system->db_sqlite3, z_sql, fetch_username, &params, Nil);

	if (rc == SQLITE_OK) {
		system_set_errcode(BS_OK, system);

		if (bs_strcmp(params.username, "ADMIN", YES)) {
			usr = Admin_v2.alloc();
		} else if (bs_strcmp(params.username, "OPERATOR", YES)) {
			usr = Operator_v2.alloc();
		} else {
			system_set_errcode(BS_ERR_USER_LOGIN, system);
			LOG("Failed.\n");

			return Nil;
		}
	} else {
		system_set_errcode(BS_ERR_DATABASE, system);
		LOG("Failed.\n");

		return Nil;
	}

	virtual_method_invoke(usr, init_with_system_and_username, system, z_username);

	return usr;
}

const char * system_get_property(banking_system system, int prop_id) {
	prop_id ^= BS_PROP_IDX_MASK;
	if ((0 > prop_id) || (BS_PROPS_CNT < prop_id)) return Nil;

	return system->cfg_properties[prop_id];
}

void system_finalize(banking_system system) {
	ASSERT(system != Nil);
	LOG("Finalizing system.\n");

	for (int i = 0; i < BS_PROPS_CNT; i++) {
		bs_free(system->cfg_properties[i]);
	}
	sqlite3_close_v2(system->db_sqlite3);
	bs_free(system);
}

struct callback_params_account_number_and_balance {
	double balance;
	int account_number;
	banking_system bs;
};

int fetch_account_number_and_balance(void *, int, char **, char **);

void system_month_processing(banking_system system) {
	ASSERT(system != Nil);
	char z_sql[150];
	int rc;
	struct callback_params_account_number_and_balance params;
	time_t current_time_seconds = time(Nil);
	struct tm* time_info = localtime(&current_time_seconds);
	int current_day_in_month = time_info->tm_mday;

	params.bs = system;

	if (current_day_in_month == 1) {
		sprintf(z_sql, "SELECT account_number, balance FROM BANK_ACCOUNTS WHERE balance < 0 AND type like 'Overdraft';");
		rc = sqlite3_exec(system->db_sqlite3, z_sql, fetch_account_number_and_balance, &params, Nil);
		if (rc == SQLITE_OK) {
			system_set_errcode(BS_OK, system);
		}
		else {
			system_set_errcode(BS_ERR_DATABASE, system);
			LOG("Failed.\n");
			return;
		}

		sprintf(z_sql, "UPDATE BANK_ACCOUNTS SET balance = %lf WHERE account_number = '%d';",
			params.balance - atoi(system_get_property(system, BS_PROP_OVERDRAFT_FINE)), params.account_number);
		rc = sqlite3_exec(system->db_sqlite3, z_sql, Nil, Nil, Nil);

		if (rc == SQLITE_OK) {
			system_set_errcode(BS_OK, system);
		}
		else {
			system_set_errcode(BS_ERR_DATABASE, system);
			LOG("Failed.\n");
			return;
		}


		sprintf(z_sql, "SELECT account_number, balance FROM BANK_ACCOUNTS WHERE type like 'SAVING';");
		rc = sqlite3_exec(system->db_sqlite3, z_sql, fetch_account_number_and_balance, &params, Nil);
		if (rc == SQLITE_OK) {
			system_set_errcode(BS_OK, system);
		}
		else {
			system_set_errcode(BS_ERR_DATABASE, system);
			LOG("Failed.\n");
			return;
		}

		sprintf(z_sql, "UPDATE BANK_ACCOUNTS SET balance = %lf WHERE account_number = '%d';",
			params.balance + strtod(system_get_property(system, BS_PROP_SAVING_INTEREST_RATE), Nil) * params.balance
			, params.account_number);
		rc = sqlite3_exec(system->db_sqlite3, z_sql, Nil, Nil, Nil);

		if (rc == SQLITE_OK) {
			system_set_errcode(BS_OK, system);
		}
		else {
			system_set_errcode(BS_ERR_DATABASE, system);
			LOG("Failed.\n");
			return;
		}

	}


	return;
}

int fetch_account_number_and_balance(struct callback_params_account_number_and_balance *data, int n_coals, char*field_names[], char *col_names[]) {
	if (n_coals != 0) {
		data->account_number = atoi(field_names[0]);
		data->balance = strtod(field_names[1], Nil);
	}
	else {
		system_set_errcode(BS_ERR_USER_LOGIN, data->bs);
	}

	return 0;
}
