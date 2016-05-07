#include "object_v2.h"
#include "stddefs.h"

void * BSObject_v2_alloc();
void * BSObject_v2_static_alloc(struct bs_object_v2 *self);
void * bs_object_v2_init(struct bs_object_v2 *self, banking_system bs);
void bs_object_v2_release(struct bs_object_v2 *self);
boolean bs_object_v2_instance_of(struct bs_object_v2 * self, struct bs_class_struct_v2 class);

struct bs_class_struct_v2 BSObject_v2 = {
	.class = &BSObject_v2,
	.super = NULL,
	.alloc = BSObject_v2_alloc,
	.static_alloc = BSObject_v2_static_alloc,
	.string_val = "Object"
};

struct bs_object_v2_private {
	boolean is_dynamic;
};

#define __ ((struct bs_object_v2_private *)self->vars.private)->

////////////////// Implementation of object methods /////////////////////////

void *BSObject_v2_alloc() {
	struct bs_object_v2 *self = bs_calloc(1, sizeof(struct bs_object_v2));
	
	BSObject_v2_static_alloc(self);
	__ is_dynamic = YES;

	return self;
}

void * BSObject_v2_static_alloc(struct bs_object_v2 *self) {
	ASSERT(self != Nil);
	
	self->class = BSObject_v2.class;
	self->virtual_methods.release = bs_object_v2_release;
	self->virtual_methods.init_with_system = bs_object_v2_init;
	self->methods.init_with_system = bs_object_v2_init;
	self->methods.release = bs_object_v2_release;
	self->methods.is_a = bs_object_v2_instance_of;
	self->vars.private = bs_calloc(1, sizeof(struct bs_object_v2_private));
	__ is_dynamic = NO;

	return self;
}

void * bs_object_v2_init(struct bs_object_v2 *self, banking_system bs) {
	ASSERT(self != Nil);

	self->vars.system = bs;

	return self;
}

void bs_object_v2_release(struct bs_object_v2 *self) {
	ASSERT(self != Nil);
	boolean is_dynamic = __ is_dynamic;

	bs_free(self->vars.private);
	if (is_dynamic) bs_free(self);
}

boolean bs_object_v2_instance_of(struct bs_object_v2 *object, struct bs_class_struct_v2 a_class) {
	ASSERT(object != Nil);
	class_t_v2 o_class = object->class;

	while (o_class->super != NULL) {
		if (bs_memory_equals(o_class, &a_class, sizeof(struct bs_class_struct_v2)))
			return YES;

		o_class = o_class->super;
	}

	return bs_memory_equals(&o_class, &BSObject_v2, sizeof(struct bs_class_struct_v2));
}
