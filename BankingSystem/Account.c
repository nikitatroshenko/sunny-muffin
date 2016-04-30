#include "Account.h"

void *Account_alloc();
void Account_init(account *self, int _number, double _balance, enum AccountType _type);
void Account_release(account *self);
Class_t Account_getClass(account *self);

const Class_t Account = {
	.alloc = Account_alloc,
	.class = &Account,
	.super = &BSObject
};

void *Account_alloc() {
	account *acc = bs_calloc(1, sizeof(account));

	acc->init = Account_init;
	acc->release = Account_release;
	acc->getClass = Account_getClass;
	acc->private = NULL;
	
	return acc;
}

void Account_init(account *self, int number, double balance, enum AccountType type) {
	self->number = number;
	self->balance = balance;
	self->account_type = type;
}

void Account_release(account *self) {
	bs_free(self);
}

Class_t Account_getClass(account *self) {
	return Account;
}
