//#include "User.h"
#include "Admin.h"

typedef struct {
	user *super;
} admin_private;

void *Admin_alloc();
void Admin_init(struct admin *self, const char *z_username);
void Admin_release(struct admin *self);
Class_t Admin_get_class(struct admin *self);

bs_c_string Admin_get_username(admin *self);
boolean Admin_authorize(admin *self, bs_c_string z_password);
boolean Admin_is_authorized(admin *self);

int Admin_add_client(admin *self, struct client *p_client);
int Admin_remove_client(admin *self, struct client *p_client);
int Admin_update_client_info(admin *self, struct client *p_old_client, struct client *p_new_client);
int Admin_create_account(admin *self, struct client *p_client, enum AccountType account_type);
int Admin_change_account_type(admin *self, struct client *p_client, enum AccountType account_type);

const Class_t Admin = {
	.alloc = Admin_alloc,
	.class = &Admin,
	.super = &User
};

void *Admin_alloc() {
	struct admin *self = bs_calloc(1, sizeof(struct admin));

	self->private = bs_calloc(1, sizeof(admin_private));
	self->init = Admin_init;
	self->release = Admin_release;
	self->getClass = Admin_get_class;

	self->authorize = Admin_authorize;
	self->getUsername = Admin_get_username;
	self->isAuthorized = Admin_is_authorized;

	self->add_client = Admin_add_client;
	self->update_client_info = Admin_update_client_info;
	self->remove_client = Admin_remove_client;
	self->create_account = Admin_create_account;
	self->change_account_type = Admin_change_account_type;

	return self;
}

void Admin_init(struct admin *self, const char *z_username) {
	admin_private *self_private = self->private;
	
	self_private->super = User.alloc();
	self_private->super->init(self_private->super, z_username);
}

void Admin_release(struct admin *self) {
	user *super = ((admin_private *)self->private)->super;
	
	super->release(super);
	bs_free(self->private);
	bs_free(self);
}

Class_t Admin_get_class(struct admin *self) {
	return Admin;
}

bs_c_string Admin_get_username(struct admin *self) {
	user *super = ((admin_private *)self->private)->super;
	return super->getUsername(super);
}

boolean Admin_authorize(struct admin *self, bs_c_string z_password) {
	user *super = ((admin_private *)self->private)->super;
	return super->authorize(super, z_password);
}

boolean Admin_is_authorized(struct admin *self) {
	user *super = ((admin_private *)self->private)->super;
	return super->isAuthorized(super);
}

int Admin_add_client(struct admin *self, struct client *p_client) {
	// TODO: Implement me :)
	// (for Venskiy)
#if DEBUG
	LOG("Add client\n");
#endif // DEBUG

	return 0;
}

int Admin_remove_client(struct admin *self, struct client *p_client) {
	// TODO: Implement me :)
	// (for Venskiy)
#if DEBUG
	LOG("Remove client\n");
#endif // DEBUG

	return 0;
}

int Admin_update_client_info(struct admin *self, struct client *p_old_client, struct client *p_new_client) {
	// TODO: Implement me :)
	// (for Venskiy)
#if DEBUG
	LOG("Update client info\n");
#endif // DEBUG

	return 0;
}

int Admin_create_account(struct admin *self, struct client *p_client, enum AccountType account_type) {
	// TODO: Implement me :)
	// (for Venskiy)
#if DEBUG
	LOG("Create account\n");
#endif // DEBUG

	return 0;
}

int Admin_change_account_type(struct admin *self, struct client *p_client, enum AccountType account_type) {
	// TODO: Implement me :)
	// (for Venskiy)
#if DEBUG
	LOG("Change account type\n");
#endif // DEBUG

	return 0;
}
