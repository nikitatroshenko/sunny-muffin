#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "stddefs.h"

struct client_v2 {
	bs_char id[20];
	bs_char z_name[50];
	boolean is_individual;
	bs_char address[40];
};

#endif // !_CLIENT_H_
