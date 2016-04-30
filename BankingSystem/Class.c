#include "BSObject.h"

Class_t Class = {
	.class = &Class,
	.super = &BSObject,
	.alloc = NULL
};

boolean instanceOf(bsObject *object, Class_t aClass) {
	Class_t o_class = object->getClass(object);

	while (o_class.super != NULL) {
		if (bs_memory_equals(&o_class, &aClass, sizeof(Class_t)))
			return YES;

		o_class = *o_class.super;
	}

	return bs_memory_equals(&o_class, &BSObject, sizeof(Class_t));
}
