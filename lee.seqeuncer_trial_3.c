/* Inlcude */
#include "ext.h"
#include "ext_time.h"

/* Class */

    // lee
    t_class* lee_class = NULL;

    typedef struct {

        t_object header;
        t_itm* itm;
        t_linklist* linklist;

        void* outlet_1;
        void* outlet_2;
        
    } lee;

    lee* lee_instance_address = NULL;

    void*   lee_new         (t_symbol* s, long ac, t_atom* av);
    void    lee_free        (lee* lee_instance);
    void    lee_record      (lee* lee_instance, t_symbol* s, long ac, t_atom* av);

    // note
    t_class* note_class = NULL;

    typedef struct {

        t_object header;
        t_object* time;
        long pitch;
        long velocity;

    } note;

    void*   note_new        (lee* lee_instance, long pitch, long velocity);
    void    note_free       (note* note_instance);
    void    note_tick       (note* note_instance);

/* Definition */

    void*   lee_new         (t_symbol* s, long ac, t_atom* av) {

        // create instance.
        lee* lee_instance = (lee*)object_alloc(lee_class);

        // store lee_instance's address as globally to access later by note_instance.
        lee_instance_address = lee_instance;

        // get itm (a.k.a global transport of Max)
        lee_instance->itm = itm_getglobal();

        // create linklist.
        lee_instance->linklist = linklist_new();

        // create outlets.
        lee_instance->outlet_2 = intout(lee_instance); // velocity
        lee_instance->outlet_1 = intout(lee_instance); // pitch
    
        // return instance
        return lee_instance;

    }

    void    lee_free        (lee* lee_instance) {



    }
    
    void    lee_record      (lee* lee_instance, t_symbol* s, long ac, t_atom* av) {

        long pitch      = atom_getlong(&av[0]);
        long velocity   = atom_getlong(&av[1]);

        // invoke note_new when "record $1 $2" comes through [lee.empty]'s inlet.
        note_new(lee_instance, pitch, velocity);

    }

    void*   note_new        (lee* lee_instance, long pitch, long velocity) {

        // create note_instance.
        note* note_instance = (note*)object_alloc(note_class);

        // store note_instance's address in lee_instance's linklist.
        linklist_append(lee_instance->linklist, note_instance);

        // create time and set timevalue in note_instance.
        note_instance->time = (t_object*)time_new((t_object*)note_instance, symbol_unique(), (method)note_tick, TIME_FLAGS_TICKSONLY | TIME_FLAGS_USECLOCK | TIME_FLAGS_PERMANENT | TIME_FLAGS_LOCATION | TIME_FLAGS_POSITIVE);
        double currentTick  = itm_getticks(lee_instance->itm);
        t_atom timevalue;
        atom_setfloat(&timevalue, currentTick);
        time_setvalue(note_instance->time, NULL, 1, &timevalue);

        // store pitch, velocity in note_instance.
        note_instance->pitch    = pitch;
        note_instance->velocity = velocity;

        // return note_instance
        return note_instance;

    }

    void    note_free       (note* note_instance) {



    }

    void    note_tick       (note* note_instance) {

        long velocity = note_instance->velocity;
        long pitch    = note_instance->pitch;

        // console.
        object_post((t_object*)note_instance, "velocity : %ld", velocity);
        object_post((t_object*)note_instance, "velocity : %ld", pitch);

        // outlet
        outlet_int(lee_instance_address->outlet_2, velocity); // veloicty
        outlet_int(lee_instance_address->outlet_1, pitch);     // pitch
   
    }


/* Ext Main */

    void    ext_main    (void* r) {

        // create classes.
        lee_class   = class_new("lee.empty",  (method)lee_new,  (method)lee_free,  sizeof(lee),  NULL, A_GIMME, 0);
        note_class  = class_new("note_class", (method)note_new, (method)note_free, sizeof(note), NULL, A_GIMME, 0);

        // add methods to class.
        class_addmethod(lee_class, (method)lee_record, "record", A_GIMME, 0);
 
        // register class.
        class_register(CLASS_BOX,   lee_class);
        class_register(CLASS_NOBOX, note_class);

    }

