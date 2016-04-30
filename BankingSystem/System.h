#ifndef _BS_SYSTEM_H
#define _BS_SYSTEM_H
#include "stddefs.h"
#include <sqlite3.h>
#include "User.h"

typedef struct _banking_system *banking_system;

banking_system system_init(bs_c_string z_database_filename);

sqlite3 *system_get_database_handler(banking_system system);

user *system_authentificate(banking_system system, bs_c_string z_username);

void system_finalize(banking_system system);

#endif // !_BS_SYSTEM_H
