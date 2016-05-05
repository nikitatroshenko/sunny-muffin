#include <time.h>
#include "stddefs.h"
#include "object_v2.h"
#include "operator_v2.h"
#include "admin_v2.h"
#include "System.h"

extern int get_allocated_memory_size();
char * read_line(char * buf, size_t max_count, FILE *in);

void login(banking_system bs, struct user_v2 **p_usr);
void authentificate(banking_system bs, struct user_v2 **p_usr);
void authorize(banking_system bs, struct user_v2 *usr);

void main_cycle(struct user_v2 *usr, banking_system bs);

#if !NDEBUG
#define log_memory_usage() LOG("%d bytes allocated\n", get_allocated_memory_size())
#else
#define log_memory_usage()
#endif

#define clear_screen() system("cls")

char _get_c(){
	fflush(stdin);
	printf("$ ");
	return getchar();
}

#define DATABASE_PATH "./Bank_System_Database"
#define MAX_LOGIN_LEN 20

void sleep(int duration);

int main() {
	banking_system bs;
	struct user_v2 *usr;
	int rc = system_init(DATABASE_PATH, &bs);

	if (rc != BS_OK) {
		printf("System initialization failed: %s.\n", system_get_errmsg(bs));
		system_finalize(bs);
		log_memory_usage();

		return EXIT_FAILURE;
	}
	printf("System initialized successfully.\n");
	sleep(1000);

	login(bs, &usr);
	if (Nil == usr || method_invoke(usr, is_authorized)) {
		if (Nil != usr) delete(usr);
		system_finalize(bs);
		printf("Goodbye...\n");
		sleep(1000);
		log_memory_usage();

		return EXIT_SUCCESS;
	}

	printf("%s successfully logged in as %s.\n", method_invoke(usr, get_username),
		typeof(usr)->string_val);
	sleep(1000);

	main_cycle(usr, bs);

	delete(usr);
	system_finalize(bs);
	log_memory_usage();

	return EXIT_SUCCESS;
}

char * read_line(char * buf, size_t max_cnt, FILE *in) {
	ASSERT(buf);
	char *c;

	do {
		fflush(stdin);
		printf("$ ");
		c = fgets(buf, max_cnt, in);

		while ('\0' != *c) {
			if (*c == '\n') {
				*c = '\0';
				break;
			}
			
			c++;
		}

		if ('\0' != buf[0]) break;
	} while (YES);

	return buf;
}

void login(banking_system bs, struct user_v2 **p_usr) {
	authentificate(bs, p_usr);

	if (*p_usr != Nil)
		authorize(bs, *p_usr);
	clear_screen();
}

void authentificate(banking_system bs, struct user_v2 **p_usr) {
	char login[MAX_LOGIN_LEN];
	char c_retry;

	clear_screen();
	do {
		printf("Enter login:\n");
		read_line(login, MAX_LOGIN_LEN, stdin);

		*p_usr = system_authentificate(bs, login);

		if (*p_usr != Nil) return;

		printf("Authentification failed: %s.\nWanna retry? [y/n] ", system_get_errmsg(bs));
		do {
			c_retry = _get_c();
		} while (c_retry != 'y' && c_retry != 'n');
	} while (c_retry == 'y');
}

void authorize(banking_system bs, struct user_v2 *usr) {
	char z_password[MAX_LOGIN_LEN];
	char c_retry;

	clear_screen();
	do {
		printf("Enter password:\n");
		read_line(z_password, MAX_LOGIN_LEN, stdin);

		if (method_invoke(usr, authorize, z_password)) {
			return;
		};

		printf("Authorization is failed: %s.\nWanna retry? [y/n] ", system_get_errmsg(bs));
		do {
			c_retry = _get_c();
		} while (c_retry != 'y' && c_retry != 'n');
	} while (c_retry == 'y');
}

typedef void(*operation)(struct user_v2 * usr, banking_system bs);

void debit(struct user_v2 *usr, banking_system bs);
void credit(struct user_v2 *usr, banking_system bs);
void add_client(struct user_v2 *usr, banking_system bs);
void remove_client(struct user_v2 *usr, banking_system bs);
void update_client_info(struct user_v2 *usr, banking_system bs);
void create_account(struct user_v2 *usr, banking_system bs);
void change_account_type(struct user_v2 *usr, banking_system bs);
void close_account(struct user_v2 *usr, banking_system bs);

void main_cycle(struct user_v2 *usr, banking_system bs) {
	char c_choice;
	int operation_idx;
	operation call_operation[] = {
		debit,
		credit,
		add_client,
		remove_client,
		update_client_info,
		create_account,
		change_account_type,
		close_account
	};

	do {
		clear_screen();
		printf("%s : %s\n"\
			"======== MAIN MENU ========\n"\
			"1.\tCredit;\n"\
			"2.\tDebit;\n"\
			"3.\tAdd client;\n"\
			"4.\tRemove client;\n"\
			"5.\tUpdate client info;\n"\
			"6.\tCreate new account;\n"\
			"7.\tChange account type;\n"\
			"8.\tClose account;\n"\
			"0.\tExit.\n",
			method_invoke(usr, get_username), typeof(usr)->string_val);

		c_choice = _get_c();
		operation_idx = c_choice - '1';

		if (-1 == operation_idx) break;

		if (operation_idx < _countof(call_operation) && operation_idx >= 0) {
			call_operation[operation_idx](usr, bs);
			printf("Press enter . . . ");
			_get_c();
		}
	} while (YES);
}

void debit(struct user_v2 *usr, banking_system bs) {
	char client_id[MAX_LOGIN_LEN];
	int account_id = 0;
	double sum = -1;
	int rc;

	clear_screen();
	if (!method_invoke((struct bs_object_v2 *)usr, is_a, Operator_v2)) {
		printf("You have no permission to do that.\n");
		return;
	}

	printf("Enter client's id:\n");
	read_line(client_id, MAX_LOGIN_LEN, stdin);
	do{
		printf("Enter sum of transaction:\n$ ");
	} while (1 != scanf("%lf", &sum));

	rc = method_invoke((struct operator_v2 *)usr, debit, client_id, account_id, sum);

	if (rc == BS_OK) {
		printf("Successfully added %.2lf$ to the account %d\n", sum, account_id);
		return;
	}

	printf("Debit is failed: %s.\n", system_get_errmsg(bs));
}

void credit(struct user_v2 *usr, banking_system bs) {
	char client_id[MAX_LOGIN_LEN];
	int account_id = 0;
	double sum = -1;
	int rc;

	clear_screen();
	if (!method_invoke((struct bs_object_v2 *)usr, is_a, Operator_v2)) {
		printf("You have no permission to do that.\n");
		return;
	}
	fflush(stdin);

	printf("Enter client's id:\n");
	read_line(client_id, MAX_LOGIN_LEN, stdin);

	do{
		printf("Enter number of account:\n");
	} while (1 != scanf("%d", &account_id));

	do{
		printf("Enter sum of transaction:\n");
	} while (1 != scanf("%lf", &sum));

	rc = method_invoke((struct operator_v2 *)usr, debit, client_id, account_id, sum);

	if (rc == BS_OK) {
		printf("Successfully added %.2lf$ to the account %d", sum, account_id);
		return;
	}

	printf("Credit is failed: %s.\n", system_get_errmsg(bs));
}

void sleep(int duration) {
	clock_t start = clock();

	duration *= CLOCKS_PER_SEC;
	duration /= 1000;
	while (duration > clock() - start) { ; }
}

void add_client(struct user_v2 *usr, banking_system bs) {
	struct client_v2 new_client;
	int rc;

	clear_screen();
	if (!method_invoke((struct bs_object_v2 *)usr, is_a, Admin_v2)) {
		printf("You have no permission to do that.\n");
		return;
	}
	fflush(stdin);

	printf("Enter new client's id:\n");
	read_line(new_client.id, MAX_LOGIN_LEN, stdin);

	printf("Enter name:\n");
	read_line(new_client.z_name, 50, stdin);

	printf("Is %s an individual? [y/n] ", new_client.z_name);

	do {
		new_client.is_individual = _get_c();
	} while (new_client.is_individual != 'y' && new_client.is_individual != 'n');

	printf("Enter %s's address:\n", new_client.z_name);
	read_line(new_client.address, 40, stdin);

	rc = method_invoke((struct admin_v2 *)usr, add_client, &new_client);

	if (rc == BS_OK) return;

	printf("Adding client is failed: %s.\n", system_get_errmsg(bs));
}

void remove_client(struct user_v2 *usr, banking_system bs) {
	method_invoke((struct admin_v2 *)usr, remove_client, Nil);
}

void update_client_info(struct user_v2 *usr, banking_system bs) {
	method_invoke((struct admin_v2 *)usr, update_client_info, Nil, Nil);
}

void create_account(struct user_v2 *usr, banking_system bs) {
	method_invoke((struct admin_v2 *)usr, create_account, Nil, 0);
}

void change_account_type(struct user_v2 *usr, banking_system bs) {
	method_invoke((struct admin_v2 *)usr, change_account_type, Nil, 0, 0);
}

void close_account(struct user_v2 *usr, banking_system bs) {
	method_invoke((struct admin_v2 *)usr, close_account, Nil, 0);
}
