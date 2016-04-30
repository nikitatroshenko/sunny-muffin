#include "stddefs.h"
#include "BSObject.h"
#include "User.h"
#include <string.h>

void *User_alloc();
void User_init(user *self, bs_c_string z_username);
void User_release(user *self);
boolean User_authorize(user *self, bs_c_string z_password);
boolean User_is_authorized(user *self);
const char *User_getUsername(user *self);

typedef struct {
	char *z_username;
	boolean isAuthorized;
} user_private;

const Class_t User = {
	.alloc = User_alloc,
	.class = &User,
	.super = &BSObject
};

Class_t User_getClass(void *self);

void *User_alloc() {
	user *o = bs_calloc(1, sizeof(user));
	
	o->init = User_init;
	o->release = User_release;
	o->getClass = User_getClass;
	o->authorize = User_authorize;
	o->getUsername = User_getUsername;
	o->private = bs_calloc(1, sizeof(user_private));

	return o;
}

void User_init(user *self, bs_c_string z_password) {
	user_private *self_private = self->private;

	self_private->z_username = bs_calloc(strlen(z_password), bs_strlen(z_password));
	bs_strcpy(self_private->z_username, z_password);
}

void User_release(user *self) {
	user_private *self_private = self->private;

	bs_free(self_private->z_username);
	bs_free(self_private);
	bs_free(self);
}

boolean User_authorize(user *self, bs_c_string z_password) {
	// TODO: Implement me :)
	// (for Tsapliuk)

	return NO;
}

bs_c_string User_getUsername(user *self) {
	return ((user_private *)self->private)->z_username;
}

Class_t User_getClass(void *self) {
	return User;
}
