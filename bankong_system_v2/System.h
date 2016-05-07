#ifndef _BS_SYSTEM_H
#define _BS_SYSTEM_H
#include "stddefs.h"

typedef struct _banking_system *banking_system;
typedef struct sqlite3 sqlite3;

#define BS_OK							0x00
#define BS_ERR_USER_LOGIN				0x01
#define BS_ERR_USER_PASSWORD			0x02
#define BS_ERR_DATABASE					0x03
#define BS_ERR_CLIENT_ID				0x04
#define BS_ERR_ACCOUNT_ID				0x05
#define BS_ERR_USER_UNAUTHORIZED		0x06
#define BS_ERR_OVERDRAFT_LIMIT_EXCEEDED	0x07
#define BS_ERR_CFG_LOAD_FAILED			0x08
#define BS_ERR_INVALID_ARG				0x09
#define BS_ERR_ACCOUNT_TYPE				0x0A
#define BS_ERR_ACCOUNTS_EXIST			0x0B

#define BS_PROP_SAVING_INTEREST_RATE	0x10
#define BS_PROP_OVERDRAFT_INTEREST_RATE	0x11
#define BS_PROP_COMPUNNDING_FREQUENCY	0x12
#define BS_PROP_CHEQUE_MONTHLY_QUOTA	0x13
#define BS_PROP_OVERDRAFT_TERM			0x14
#define BS_PROP_OVERDRAFT_LIMIT			0x15
#define BS_PROP_OVERDRAFT_FINE			0x16
#define BS_PROP_PER_TRANSACTION_FEE		0x17

int system_init(const char *z_database_filename, banking_system *p_system);

sqlite3 * system_get_database_handler(banking_system system);

void system_set_errcode(int code, banking_system system);

int system_get_errcode(banking_system system);

const char * system_get_errmsg(banking_system system);

struct user_v2 * system_authentificate(banking_system system, const char *z_username);

const char * system_get_property(banking_system system, int prop_id);

void system_finalize(banking_system system);

#endif // !_BS_SYSTEM_H
