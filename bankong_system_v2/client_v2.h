#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "stddefs.h"

struct client_v2 {
	char id[20];
	char z_name[50];
	boolean is_individual;
	char address[40];
};

#endif // !_CLIENT_H_
