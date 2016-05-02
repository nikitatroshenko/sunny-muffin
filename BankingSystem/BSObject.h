#ifndef _BS_OBJECT_H_
#define _BS_OBJECT_H_

#include "stddefs.h"

typedef struct _Class_t {
	void*(*alloc)();
	const struct _Class_t * const class;
	const struct _Class_t * const super;
} Class_t;

extern const Class_t Class;

// Instance methods.
typedef struct _bsObject {
	void *private;
	void (*release)(void *self);
	void (*init)(void *self, ...);
	Class_t (*getClass)(void *self);
} bsObject;

boolean instanceOf(bsObject *object, Class_t aClass);

extern const Class_t BSObject;

#endif // !_BS_OBJECT_H_