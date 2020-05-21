#include "ext.h"
#include "ext_time.h"

/* class & struct */
t_class* leeClass = NULL;

typedef struct {

    t_object header;
    t_itm* itm;
    t_linklist* linklist;
    
    void* outlet_1;

} lee;


/* prototypes*/
void*   lee_new     (t_symbol* s, long ac, t_atom* av);
void    lee_free    (lee* instance);
void    lee_bang    (lee* instance);
void    lee_clear   (lee* instance);
void    lee_tick    (lee* instance);

/* ext_main */
void    ext_main    (void* r){

    // create class.
    leeClass = class_new("lee.empty", (method)lee_new, (method)lee_free, sizeof(lee), NULL, A_GIMME, 0);

    // add methods to class.
    class_addmethod(leeClass, (method)lee_bang, "bang", 0);
    class_addmethod(leeClass, (method)lee_clear, "clear", 0);

    // register class.
    class_register(CLASS_BOX, leeClass);
    
}

/* definition */
void*   lee_new     (t_symbol* s, long ac, t_atom* av) {

    // create instance.
    lee* instance = (lee*)object_alloc(leeClass);

    // get global transport (itm)
    instance->itm = itm_getglobal();

    // create link list.
    instance->linklist = linklist_new();

    // create outlet.
    instance->outlet_1 = bangout(instance);

    // return instance.
    return instance;

}

void    lee_free    (lee* instance) {

    lee_clear(instance);

}

void    lee_bang    (lee* instance) {

    // create time.
    t_object* time = (t_object*)time_new((t_object*)instance, symbol_unique(), (method)lee_tick, TIME_FLAGS_TICKSONLY | TIME_FLAGS_USECLOCK | TIME_FLAGS_PERMANENT | TIME_FLAGS_LOCATION | TIME_FLAGS_POSITIVE);

    // set time.
    double currentTick = itm_getticks(instance->itm);
    t_atom timepoint;
    atom_setfloat(&timepoint, currentTick);
    time_setvalue(time, NULL, 1, &timepoint);

    // append time to linklist.
    linklist_append(instance->linklist, time);

    // debug
    object_post((t_object*)instance, "current tick : %f", currentTick);
    object_post((t_object*)instance, "linklist size : %ld", linklist_getsize(instance->linklist));


}

void    lee_clear   (lee* instance) {

    linklist_clear(instance->linklist);

}

void    lee_tick    (lee* instance) {

    outlet_bang(instance->outlet_1);
    object_post((t_object *)instance, "bang!");

}