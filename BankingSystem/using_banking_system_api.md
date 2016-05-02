# Using banking system API
Usage of an object variable:
```C
// Declarations ...
bsObject *object = BSObject.alloc();

object->init(object);

// Some of your operations here...

object->release(object);

// ...
```
For each class type there is a `Class_t` variable that contains static methods and fields of this class.

You can initialize variables of some class with instance of derived class:
`user *usr = Admin.alloc();`

You can use `instanceOf(bsObject *, Class_t)` (`#include "object.h"`) function to know whether instance is a kind of a particular class.

