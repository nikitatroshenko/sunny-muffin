#include "stddefs.h"
#include "BSObject.h"
#include "User.h"
#include "Admin.h"
#include "Operator.h"
#include "System.h"
#include <time.h>

extern int get_allocated_memory_size();

void print_memory_usage() {
	fprintf(stderr, "%d bytes allocated\n", get_allocated_memory_size());
}

int main() {
	banking_system bs;
	user *usr;
	
	srand(time(NULL));
	bs = system_init("database.sqlite");
	usr = system_authentificate(bs, "Nikitos");
	print_memory_usage();

	if (usr == NULL) {
		LOG(Authentification failed);
		system_finalize(bs);
		print_memory_usage();
		return EXIT_FAILURE;
	}
	else if (instanceOf(usr, Admin)) {
		LOG(User is admin);
	}
	else if (instanceOf(usr, Operator)) {
		LOG(User is operator);
	}
	else {
		LOG(smth is wrong);
	}

	fprintf(stdout, "%s is authorized? %s\n", usr->getUsername(usr), usr->isAuthorized(usr) ? "YES" : "NO");
	((struct admin *)usr)->add_client(usr, NULL);

	((struct admin *)usr)->release(usr);
	system_finalize(bs);
	print_memory_usage();
	
	return 0;
}