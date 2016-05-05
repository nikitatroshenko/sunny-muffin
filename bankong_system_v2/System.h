#ifndef _BS_SYSTEM_H
#define _BS_SYSTEM_H
#include "stddefs.h"

typedef struct _banking_system *banking_system;
typedef struct sqlite3 sqlite3;

#define BS_OK						0
#define BS_ERR_USER_LOGIN			1
#define BS_ERR_USER_PASSWORD		2
#define BS_ERR_DATABASE				3
#define BS_ERR_CLIENT_ID			4
#define BS_ERR_ACCOUNT_ID			5
#define BS_ERR_USER_UNAUTHORIZED	6
#define BS_ERR_UNSUFFICIENT_FUNDS	7

int system_init(bs_const_string z_database_filename, banking_system *p_system);

sqlite3 *system_get_database_handler(banking_system system);

void system_set_errcode(int code, banking_system system);

int system_get_errcode(banking_system system);

bs_const_string system_get_errmsg(banking_system system);

struct user_v2 *system_authentificate(banking_system system, bs_const_string z_username);

void system_finalize(banking_system system);

#endif // !_BS_SYSTEM_H
