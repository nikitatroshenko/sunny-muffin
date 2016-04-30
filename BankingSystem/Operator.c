#include "Operator.h"
#include "User.h"

typedef struct {
	user *super;
} operator_private;

void *Operator_alloc();
void Operator_init(operator *self, bs_c_string z_username);
void Operator_release(operator *self);
Class_t Operator_get_class(operator *self);

boolean Operator_authorize(operator *self, bs_c_string z_password);
bs_c_string Operator_get_username(operator *self);
boolean Operator_is_authorized(operator *self);

int Operator_debit(operator *self, const account *p_account, int sum);
int Operator_credit(operator *self, const account *p_account, int sum);
int Operator_transfer(operator *self, const account *p_account, int sum);

const Class_t Operator = {
	.alloc = Operator_alloc,
	.class = &Operator,
	.super = &User
};

void * Operator_alloc() {
	operator *self = bs_calloc(1, sizeof(operator));

	self->private = bs_calloc(1, sizeof(operator_private));
	self->init = Operator_init;
	self->release = Operator_release;
	self->getClass = Operator_get_class;

	self->authorize = Operator_authorize;
	self->getUsername = Operator_get_username;
	self->isAuthorized = Operator_is_authorized;

	self->debit = Operator_debit;
	self->credit = Operator_credit;
	self->transfer = Operator_transfer;
}

void Operator_init(operator *self, const char *z_username) {
	operator_private *self_private = self->private;

	self_private->super = User.alloc();
	self_private->super->init(self_private->super, z_username);
}

void Operator_release(operator *self) {
	user *super = ((operator_private *)self->private)->super;

	super->release(super);
	bs_free(self);
}

Class_t Operator_get_class(operator *self) {
	return Operator;
}

int Operator_debit(operator *self, const account *p_account, int sum) {
	// TODO: Implement me :)
	// (for Tsapliuk)

	return 0;
}

int Operator_credit(operator *self, const account *p_account, int sum) {
	// TODO: Implement me :)
	// (for Tsapliuk)

	return 0;
}

int Operator_transfer(operator *self, const account *p_account, int sum) {
	// TODO: Implement me :)
	// (for Tsapliuk)

	return 0;
}
