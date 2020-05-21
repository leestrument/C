/* include */
#include "ext.h"

/* class & struct */
t_class* leeClass = NULL;

typedef struct _lee {

    // you must put t_object inside struct. It's must. if not, Max crash!
    // Also, don't type like t_object* <-- also Max crash.
    t_object object;

    void* outlet_1;

} lee;


/* prototypes */
void* lee_new(t_symbol* s, long ac, t_atom* av);
void  lee_free(lee* instance);
void  lee_bang(lee* instance);

/* ext_main */
void ext_main(void* r) {

    // create class.
    leeClass = class_new("lee.empty", (method)lee_new, (method)lee_free, sizeof(lee), NULL, A_GIMME, 0);

    // add methods to class.
    class_addmethod(leeClass, (method)lee_bang, "bang", 0);

    // register class.
    class_register(CLASS_BOX, leeClass);

}

/* definition */
void* lee_new(t_symbol* s, long ac, t_atom* av) {

    // create instance.
    lee* instance = (lee*)object_alloc(leeClass);

    // create outlet.
    instance->outlet_1 = bangout(instance);

    // return instance.
    return instance;

}

void  lee_free(lee* instance) {

    

}    

void  lee_bang(lee* instance) {


}