/* include */
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


typedef struct {

    t_object* time;
    long pitch;
    long velocity;

} note;


/* new & free prototypes */
void*   lee_new     (t_symbol* s, long ac, t_atom* av);
void    lee_free    (lee* instance);
void    lee_clear   (lee* instance);
void    lee_tick    (lee* instance);

/* own prototypes */

void    lee_record  (lee* instance, t_symbol* s, long ac, t_atom* av);

/* ext_main */
void    ext_main    (void *r) {

    // create class.
    leeClass = class_new("lee.empty", (method)lee_new, (method)lee_free, sizeof(lee), NULL, A_GIMME, 0);

    // add methods to this class.
    class_addmethod(leeClass, (method)lee_clear,    "clear", 0);

    class_addmethod(leeClass, (method)lee_record,   "record", A_GIMME, 0);
    

    // register class.
    class_register(CLASS_BOX, leeClass);

}

/* new & free definition */
void*   lee_new     (t_symbol* s, long ac, t_atom* av) {

    // create instance.
    lee* instance = (lee*)object_alloc(leeClass);

    // get itm (a.k.a global transport of Max)
    instance->itm = itm_getglobal();

    // create linklist.
    instance->linklist = linklist_new();

    // set flags of linklist to free memoery by myself.
    linklist_flags(instance->linklist, OBJ_FLAG_MEMORY);

    // create outlets.
    instance->outlet_1 = bangout(instance);

    // return instance
    return instance;
    
}

void    lee_free    (lee* instance) {

    lee_clear(instance);

}

void    lee_clear   (lee* instance) {

    /* 
    
        Jeremy Bernstein 

        Note that you need to manually iterate the linklist and free the t_time* objects in clear(). 
        The linklist should have OBJ_FLAG_MEM set so that it frees the allocated structs (after you call object_free() on the t_time*s). 
    
    */

    // step 1 : free t_time* by object_free() manully before free linklist.
    long size = linklist_getsize(instance->linklist);
    long i;

    for (i = 0; i < size; i++) {

        note* noteInstance = (note*)linklist_getindex(instance->linklist, i);

        object_free(noteInstance->time);

    }
    
    // step 2 : free note strut
    linklist_clear(instance->linklist);

}

void    lee_tick    (lee* instance) {

    outlet_bang(instance->outlet_1);

}

/* own definition */
void    lee_record  (lee* instance, t_symbol* s, long ac, t_atom* av) {

    // create note instance.
    note* noteInstance = (note*)sysmem_newptr(sizeof(note));

    // create & set time.
    noteInstance->time = (t_object*)time_new((t_object*)instance, symbol_unique(), (method)lee_tick, TIME_FLAGS_TICKSONLY | TIME_FLAGS_USECLOCK | TIME_FLAGS_PERMANENT | TIME_FLAGS_LOCATION | TIME_FLAGS_POSITIVE);
    double currentTick = itm_getticks(instance->itm);
    t_atom timepoint;
    atom_setfloat(&timepoint, currentTick);
    time_setvalue(noteInstance->time, NULL, 1, &timepoint);

    // store pitch, velocity
    noteInstance->pitch     = atom_getlong(&av[0]);
    noteInstance->velocity  = atom_getlong(&av[1]);
    
    // append noteInstance to linklist
    linklist_append(instance->linklist, noteInstance);

    // debug
    long lastIndex = linklist_getsize(instance->linklist);

    object_post((t_object*)instance, "current tick : %f", currentTick);
    object_post((t_object*)instance, "linklist size : %ld", lastIndex);

}