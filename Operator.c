#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sqlite3.h"

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
	int i;
	for (i = 0; i<argc; i++){
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

void balanceChange(sqlite3 * db, char* zErrMsg, int accountNumber, int money)
{
	int balance, newBalance;
	int rc;
	sqlite3_stmt *res;

	char str[300];
	sprintf(str, "SELECT balance FROM BANK_ACCOUNTS WHERE account_number = %d;", accountNumber);

	rc = sqlite3_prepare_v2(db, str, -1, &res, 0);

	if (rc == SQLITE_OK) {
		sqlite3_bind_int(res, 1, 3);
	}
	else {

		fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
	}

	int step = sqlite3_step(res);

	if (step == SQLITE_ROW) {

		balance = sqlite3_column_int(res, 0);
	}

	newBalance = balance + money;
	if (newBalance >= 0)
	{
		char strUpdate[300];
		sprintf(strUpdate, "UPDATE BANK_ACCOUNTS SET balance = %d WHERE account_number = %d;", newBalance, accountNumber);

		rc = sqlite3_exec(db, strUpdate, callback, 0, &zErrMsg);

		if (rc != SQLITE_OK)
		{
			printf("ERROR: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);

		}
		else
		{
			printf("\nSuccess!\n");

		}
	}
	else
		printf("Failed\n");

	sqlite3_finalize(res);
}

void debit(sqlite3 * db, char* zErrMsg, int accountNumber, int moneyDebit)
{
	balanceChange(db, zErrMsg, accountNumber, -moneyDebit);
}

void credit(sqlite3 * db, char* zErrMsg, int accountNumber, int moneyCredit)
{
	balanceChange(db, zErrMsg, accountNumber, moneyCredit);
}

void transfer(sqlite3 * db, char* zErrMsg, int firstUser, int secondUser, int moneyTransfer)
{
	balanceChange(db, zErrMsg, firstUser, -moneyTransfer);

	balanceChange(db, zErrMsg, secondUser, moneyTransfer);
}

int checkPassword(sqlite3 * db, char* zErrMsg, char *username, char *password)
{
	int rc;
	sqlite3_stmt *res;

	char str[300];
	sprintf(str, "SELECT password FROM BANK_USERS WHERE username = \"%s\";", username);

	rc = sqlite3_prepare_v2(db, str, -1, &res, 0);

	if (rc == SQLITE_OK) {
		sqlite3_bind_int(res, 1, 3);
	}
	else 
	{
		fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(res);
		return 0;
	}

	int step = sqlite3_step(res);

	if (step == SQLITE_ROW) {
		if (!strcmp(password, sqlite3_column_text(res, 0)))
		{
			sqlite3_finalize(res);
			return 1;
		}
		else
		{
			sqlite3_finalize(res);
			return 0;
		}
	}
	sqlite3_finalize(res);
	return 0;
}


int authentication(sqlite3 * db, char* zErrMsg, char *username, char *password)
{
	int rc;
	sqlite3_stmt *res;

	char str[300];
	sprintf(str, "SELECT username FROM BANK_USERS WHERE username = \"%s\";", username);

	rc = sqlite3_prepare_v2(db, str, -1, &res, 0);

	if (rc == SQLITE_OK) {
		sqlite3_bind_int(res, 1, 3);
	}
	else {
		fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
	}

	int step = sqlite3_step(res);

	if (step == SQLITE_ROW)
	{
		return checkPassword(db, zErrMsg, username, password);
	}
	else
	{
		return 0;
	}

	sqlite3_finalize(res);
}

int main(int argc, const char *argv[])
{
	sqlite3 *db;
	if (sqlite3_open("Bank_System_Database", &db) == SQLITE_OK)
		printf("Opened db successfully\n");
	else
		printf("Failed to open db\n");

	char *zErrMsg = 0;
	int rc = 0;

	// debit(db, sql, zErrMsg, 0, 50);
	// credit(db, sql, zErrMsg, 0, 50);
	// transfer(db, sql, zErrMsg, 0, 1, 50);
	if (authentication(db, zErrMsg, "Venskiy", "1234"))
		printf("success\n");
	else
		printf("fail\n");

	sqlite3_close(db);
}
