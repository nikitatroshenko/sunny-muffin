#include "BSObject.h"
#include "stddefs.h"

void *BSObject_alloc();

const Class_t BSObject = {
	.alloc = BSObject_alloc,
	.class = &BSObject,
	.super = NULL
};

Class_t BSObject_getClass(bsObject *self);
void BSObject_init(bsObject *self);
void BSObject_release(bsObject *self);

void * BSObject_alloc() {
	bsObject *object = bs_calloc(1, sizeof(bsObject));
	
	object->init = BSObject_init;
	object->release = BSObject_release;
	object->private = NULL;
	object->getClass = BSObject_getClass;

	return object;
}

Class_t BSObject_getClass(bsObject *self) {
	return BSObject;
}

void BSObject_init(bsObject *self) { }

void BSObject_release(bsObject *self) {
	free(self);
}
