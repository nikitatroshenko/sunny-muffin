#ifndef _OBJECT_H
#define _OBJECT_H

#include "stddefs.h"
#if defined(NULL)
#define Nil NULL
#else
#define Nil ((void *)0)
#endif

typedef struct _banking_system *banking_system;
typedef struct bs_class_struct_v2 *class_t_v2;

struct bs_object_v2;

struct bs_class_struct_v2 {
	class_t_v2 class;
	class_t_v2 super;
	void *(*alloc)();
	void *(*static_alloc)(void *self);
	const char *string_val;
};

struct bs_object_v2_private;

struct bs_object_v2_vars {
	struct bs_object_v2_private *private;
	banking_system system;
};

#define ctor(prefix, ...) void *(*init ## prefix)(void *self, __VA_ARGS__)
#define dtor() void (*release)(void *self)
#define method(ret_t, name, ... ) ret_t(*name)(void *self, __VA_ARGS__ )

#define typeof(object) ((struct bs_object_v2 *)object)->class
#define property_get(object, identificator) (object)->vars.identificator
#define property_set(object, identificator, value) (object)->vars.identificator = value
#define method_invoke(reciever, selector, ... ) (reciever)->methods.selector(reciever, __VA_ARGS__)
#define virtual_method_invoke(reciever, selector, ... ) reciever->virtual_methods.selector(reciever, __VA_ARGS__)
#define delete(object) virtual_method_invoke(((struct bs_object_v2 *)object), release)

struct bs_object_v2_methods {
	ctor(_with_system, banking_system bs);
	dtor();
	method(boolean, is_a, struct bs_class_struct_v2 class_);
};

struct bs_object_v2_virtual_methods {
	dtor();	
};

struct bs_object_v2 {
	class_t_v2 class;
	struct bs_object_v2_virtual_methods virtual_methods;
	struct bs_object_v2_vars vars;
	struct bs_object_v2_methods methods;
};

extern struct bs_class_struct_v2 BSObject_v2;

#endif // !_OBJECT_H
