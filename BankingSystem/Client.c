#include "Client.h"

typedef struct {
	int id;
	bs_c_string z_name;
	account *accounts;
} client_private;

void *Client_alloc();
void Client_init(client *self, int id, bs_c_string z_name, account *accounts, size_t accounts_cnt);

const Class_t Client = {
	.alloc = Client_alloc,
	.class = &Client,
	.super = &BSObject
};

void * Client_alloc() {
	return NULL;
}

void Client_init(client *self, int id, bs_c_string z_name, account *accounts, size_t accounts_cnt) {
	
}
