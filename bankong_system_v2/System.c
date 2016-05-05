#include <time.h>
#include <string.h>
#include <sqlite3.h>
#include "System.h"
#include "operator_v2.h"
#include "admin_v2.h"

struct _banking_system {
	sqlite3 *db_sqlite3;
	int err_code;
};

int system_init(bs_const_string z_database_filename, banking_system *p_bs) {
	int rc;

	LOG("Initializing the system...\n");
	*p_bs = bs_calloc(1, sizeof(struct _banking_system));
	ASSERT(*p_bs != Nil);
	rc = sqlite3_open_v2(z_database_filename, &(*p_bs)->db_sqlite3, SQLITE_OPEN_READWRITE, NULL);
	system_set_errcode(BS_OK, *p_bs);

	if (rc) {
		system_set_errcode(BS_ERR_DATABASE, *p_bs);
		return BS_ERR_DATABASE;
	}

	return BS_OK;
}

int system_get_errcode(banking_system bs) {
	ASSERT(bs != Nil);

	return bs->err_code;
}

bs_const_string system_get_errmsg(banking_system system) {
	static bs_const_string error_names[] = {
		/* BS_OK						*/	"success",
		/* BS_ERR_USR_LOGIN				*/	"incorrect user login",
		/* BS_ERR_USR_PASSW				*/	"incorrect user password",
		/* BS_ERR_DATABASE				*/	"database defined error",
		/* BS_ERR_CLIENT_ID				*/	"incorrect client id",
		/* BS_ERR_ACCOUNT_ID			*/	"incorrect account id",
		/* BS_ERR_USER_UNAUTHORIZED		*/	"user is not authorized",
		/* BS_ERR_UNSUFFICIENT_FUNDS	*/	"not enough money"
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
	if (0 != n_cols)
		strcpy(data->username, field_names[0]);
	else {
		system_set_errcode(BS_ERR_USER_LOGIN, data->bs);
	}

	return 0;
}

struct user_v2 * system_authentificate(banking_system system, bs_const_string z_username) {
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

		if (!strcmp(params.username, "ADMIN")) {
			usr = Admin_v2.alloc();
		}
		else if (!strcmp(params.username, "OPERATOR")) {
			usr = Operator_v2.alloc();
		}
		else {
			system_set_errcode(BS_ERR_USER_LOGIN, system);

			return Nil;
		}
	}
	else {
		system_set_errcode(BS_ERR_DATABASE, system);

		return Nil;
	}

	method_invoke(usr, init_with_system_and_username, system, z_username);

	return usr;
}

void system_finalize(banking_system system) {
	ASSERT(system != Nil);

	sqlite3_close_v2(system->db_sqlite3);
	bs_free(system);
}
