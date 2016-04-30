#include "Admin.h"
#include "User.h"

typedef struct {
	user *super;
} admin_private;

void *Admin_alloc();
void Admin_init(admin *self, const char *z_username);
void Admin_release(admin *self);
Class_t Admin_get_class(admin *self);

bs_c_string Admin_get_username(admin *self);
boolean Admin_authorize(admin *self, bs_c_string z_password);
boolean Admin_is_authorized(admin *self);

int Admin_add_client(admin *p_admin, client *p_client);
int Admin_remove_client(admin *p_admin, client *p_client);
int Admin_update_client_info(admin *p_admin, client *p_old_client, client *p_new_client);
int Admin_create_account(admin *p_admin, client *p_client, enum AccountType account_type);
int Admin_change_account_type(admin *p_admin, client *p_client, enum AccountType account_type);

const Class_t Admin = {
	.alloc = Admin_alloc,
	.class = &Admin,
	.super = &User
};

void *Admin_alloc() {
	admin *self = bs_calloc(1, sizeof(admin));

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

void Admin_init(admin *self, const char *z_username) {
	admin_private *self_private = self->private;
	
	self_private->super = User.alloc();
	self_private->super->init(self_private->super, z_username);
}

void Admin_release(admin *self) {
	user *super = ((admin_private *)self->private)->super;
	
	super->release(super);
	bs_free(self);
}

Class_t Admin_get_class(admin *self) {
	return Admin;
}

bs_c_string Admin_get_username(admin *self) {
	user *super = ((admin_private *)self->private)->super;
	return super->getUsername(super);
}

boolean Admin_authorize(admin *self, bs_c_string z_password) {
	user *super = ((admin_private *)self->private)->super;
	return super->authorize(super, z_password);
}

boolean Admin_is_authorized(admin *self) {
	user *super = ((admin_private *)self->private)->super;
	return super->isAuthorized(super);
}

int Admin_add_client(admin *p_admin, client *p_client) {
	// TODO: Implement me :)
	// (for Venskiy)

	return 0;
}

int Admin_remove_client(admin *p_admin, client *p_client) {
	// TODO: Implement me :)
	// (for Venskiy)

	return 0;
}

int Admin_update_client_info(admin *p_admin, client *p_old_client, client *p_new_client) {
	// TODO: Implement me :)
	// (for Venskiy)

	return 0;
}

int Admin_create_account(admin *p_admin, client *p_client, enum AccountType account_type) {
	// TODO: Implement me :)
	// (for Venskiy)

	return 0;
}

int Admin_change_account_type(admin *p_admin, client *p_client, enum AccountType account_type) {
	// TODO: Implement me :)
	// (for Venskiy)

	return 0;
}
