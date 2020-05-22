/* include */
#include "ext.h"

/* class & struct */
t_class* leeClass = NULL;

typedef struct {

    t_object header;

    void* outlet_1;

} lee;

/* new & free prototypes */
void*   lee_new     (t_symbol* s, long ac, t_atom* av);
void    lee_free    (lee* instance);

/* own protoypes */
void    lee_list    (lee* instance, t_symbol* s, long ac, t_atom* av);

/* ext_main */
void    ext_main    (void* r){

    // create class.
    leeClass = class_new("lee.empty", (method)lee_new, (method)lee_free, sizeof(lee), NULL, A_GIMME, 0);

    // add methods to class.
    class_addmethod(leeClass, (method)lee_list, "list", A_GIMME, 0);

    // register class.
    class_register(CLASS_BOX, leeClass);
    
}


/* definition */
void*   lee_new     (t_symbol* s, long ac, t_atom* av) {

    // create instance.
    lee* instance = (lee*)object_alloc(leeClass);

    // create outlets.
    instance->outlet_1 = intout(instance);

    // return instance
    return = instance;

}

void    lee_free    (lee* instance) {

}

void    lee_list    (lee* instance, t_symbol* s, long ac, t_atom* av) {

    int i;

    for (i = 0; i < ac; i++) {

        outlet_int(instance->outlet_1, atom_getlong(av));

    }

}