#include "Operator.h"
#include "User.h"

typedef struct {
	user *super;
} operator_private;

void *Operator_alloc();
void Operator_init(struct operator *self, bs_c_string z_username);
void Operator_release(struct operator *self);
Class_t Operator_get_class(struct operator *self);

boolean Operator_authorize(struct operator *self, bs_c_string z_password);
bs_c_string Operator_get_username(struct operator *self);
boolean Operator_is_authorized(struct operator *self);

int Operator_debit(struct operator *self, struct account *p_account, int sum);
int Operator_credit(struct operator *self, struct account *p_account, int sum);
int Operator_transfer(struct operator *self, struct account *p_account, int sum);

const Class_t Operator = {
	.alloc = Operator_alloc,
	.class = &Operator,
	.super = &User
};

void * Operator_alloc() {
	struct operator *self = bs_calloc(1, sizeof(operator));

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

	return self;
}

#define self_private ((operator_private *)self->private)

void Operator_init(struct operator *self, const char *z_username) {
	self_private->super = User.alloc();
	self_private->super->init(self_private->super, z_username);
}

void Operator_release(struct operator *self) {
	user *super = self_private->super;

	super->release(super);
	bs_free(self->private);
	bs_free(self);
}

Class_t Operator_get_class(struct operator *self) {
	return Operator;
}

boolean Operator_authorize(struct operator *self, bs_c_string z_password) {
	return NO;
}

bs_c_string Operator_get_username(struct operator *self) {
	return self_private->super->getUsername(self_private->super);
}

boolean Operator_is_authorized(struct operator *self) {
	return self_private->super->isAuthorized(self_private->super);
}

int Operator_debit(struct operator *self, struct account *p_account, int sum) {
	// TODO: Implement me :)
	// (for Tsapliuk)
#if DEBUG
	LOG("Debit\n");
#endif // DEBUG

	return 0;
}

int Operator_credit(struct operator *self, struct account *p_account, int sum) {
	// TODO: Implement me :)
	// (for Tsapliuk)
#if DEBUG
	LOG("Credit\n");
#endif // DEBUG

	return 0;
}

int Operator_transfer(struct operator *self, struct account *p_account, int sum) {
	// TODO: Implement me :)
	// (for Tsapliuk)
#if DEBUG
	LOG("Transfer\n");
#endif // DEBUG

	return 0;
}
