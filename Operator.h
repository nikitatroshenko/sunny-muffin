#pragma once

#include <sqlite3.h>

void balanceChange(sqlite3 * db, char* sql, char* zErrMsg, int, int);

void debit(sqlite3 * db, char* sql, char* zErrMsg, int, int);

void credit(sqlite3 * db, char* sql, char* zErrMsg, int, int);

void transfer(sqlite3 * db, char* sql, char* zErrMsg, int, int, int);