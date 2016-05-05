#include "client_v2.h"

struct client_v2_private{
	boolean is_dynamic;
	int id;
	bs_string z_name;
	boolean is_individual;
	struct Account *accounts;
	size_t accounts_cnt;
};

void * Client_alloc();
void * Client_static_alloc(struct client_v2 *self);
void Client_release(struct client_v2 *self);
void * Client_init(struct client_v2 *self, banking_system bs, int id, bs_const_string z_name, boolean is_individual);

int Client_get_id(struct client_v2 *self);
bs_const_string Client_get_name(struct client_v2 *self);
struct Account * Client_get_accounts(struct client_v2 *self);

struct bs_class_struct_v2 Client_v2 = {
	.alloc = Client_alloc,
	.class = &Client_v2,
	.super = &BSObject_v2,
	.static_alloc = Client_static_alloc,
	.string_val = "Client"
};

#define super ((struct bs_object_v2 *)self)
#define __ ((struct client_v2_private *)self->vars.private)->

void * Client_alloc() {
	struct client_v2 *self = bs_calloc(1, sizeof(struct client_v2));

	Client_static_alloc(self);
	__ is_dynamic = YES;

	return self;
}

void *(__cdecl *func)();

void * Client_static_alloc(struct client_v2 *self) {
	ASSERT(self != Nil);
	BSObject_v2.static_alloc(super);

	typeof(self) = Client_v2.class;
	((struct bs_object_v2 *)self)->virtual_methods.release = Client_release;
	
	self->methods.release = Client_release;
	self->methods.init_with_system_and_id_and_name_and_is_individual = Client_init;
	self->methods.get_accounts = Nil;
	self->methods.get_name = Nil;
	self->methods.get_id = Nil;
	self->vars.private = bs_calloc(1, sizeof(struct client_v2_private));
	__ is_dynamic = NO;
	
	return self;
}

void Client_release(struct client_v2 *self) {
	ASSERT(self != Nil);
	
	boolean is_dynamic = __ is_dynamic;

	bs_free(__ z_name);
	bs_free(self->vars.private);
	if (is_dynamic) bs_free(self);
}

void * Client_init(struct client_v2 *self,
                   banking_system bs,
                   int id,
				   bs_const_string z_name,
				   boolean is_individual) {
	ASSERT(self != Nil);
	method_invoke(super, init_with_system, bs);

	__ is_individual = is_individual;
	__ z_name = bs_calloc(bs_strlen(z_name) + 1, sizeof(bs_char));
	bs_strcpy(__ z_name, z_name);

	return self;
}

int Client_get_id(struct client_v2 *self) {
	return __ id;
}

bs_const_string Client_get_name(struct client_v2 *self) {
	return __ z_name;
}

struct Account * Client_get_accounts(struct client_v2 *self) {
	return __ accounts;
}
