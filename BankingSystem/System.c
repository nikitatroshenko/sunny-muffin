#include "System.h"
#include "Operator.h"
#include "Admin.h"

struct _banking_system {
	sqlite3 *db;
};

banking_system system_init(bs_c_string z_database_filename) {
	banking_system bs;
	int rc;

	LOG(Initializing system);
	bs = bs_calloc(1, sizeof(struct _banking_system));
	rc = sqlite3_open(z_database_filename, &bs->db);

	if (rc) {
		return NULL;
	}
	
	return bs;
}

sqlite3 * system_get_database_handler(banking_system system) {
	return system->db;
}

struct user * system_authentificate(banking_system system, bs_c_string z_username) {
	user *usr;
	int rnd = rand();
	LOG(Authentificating user);

	if (rnd < RAND_MAX / 3) {
		usr = Operator.alloc();
	}
	else if (rnd < (2 * RAND_MAX / 3)){
		usr = Admin.alloc();
	}
	else {
		return NULL;
	}
	usr->init(usr, z_username);

	return usr;
}

void system_finalize(banking_system system) {
	LOG(Finalizing system);

	sqlite3_close(system->db);
	bs_free(system);
}
