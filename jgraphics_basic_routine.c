/***** include *****/

#include "ext.h"
#include "jgraphics.h"

/**** class & struct *****/

t_class* leeClass = NULL;

typedef struct _lee {

    t_jbox jbox;
    void* outlet_1;

} lee;

/**** prototypes ****/

void*   lee_new     (t_symbol* s, long ac, t_atom* av);
void    lee_free    (lee* instance);
void    lee_paint   (lee* instance, t_object* view);

/**** main ****/

void    ext_main    (void* r) {

    /* create class. */
    leeClass = class_new("lee.empty", (method)lee_new, (method)lee_free, sizeof(lee), NULL, A_GIMME, 0);

    /* init class */
    jbox_initclass(leeClass, 0);
    CLASS_ATTR_DEFAULT(leeClass, "patching_rect", 0, "0 0 169 169");

    /* add method to this class */
    class_addmethod(leeClass, (method)lee_paint, "paint", A_CANT, 0);

    /* register class */
    class_register(CLASS_BOX, leeClass);

}

/**** definition ****/

void*   lee_new     (t_symbol* s, long ac, t_atom* av) {

    /** in this area, you have to finish struct lee's inside.**/
    /* create instance. */
    lee* instance = (lee*)object_alloc(leeClass);

    /* create jbox. */

        // set flags for jbox.
        long flags = 0 | JBOX_DRAWFIRSTIN | JBOX_TRANSPARENT | JBOX_GROWBOTH | JBOX_HILITE;

        // create jbox.
        jbox_new(&instance->jbox, flags, ac, av);
        
        // makes this instance get messages through jbox inlets. I don't understand this part well, but just do this !
        instance->jbox.b_firstin = (void*)instance;

        // ready jbox.
        jbox_ready(&instance->jbox);

    /* create outlet. */
    instance->outlet_1 = intout(instance);
    
    /* return instance */
    return instance;

}

void    lee_free    (lee* instance) {

    jbox_free(&instance->jbox);

}

void    lee_paint   (lee* instance, t_object* view) {

    /* create jgraphics. */
    t_jgraphics* jg = (t_jgraphics*)patcherview_get_jgraphics(view);

    /* draw. */
    jgraphics_set_source_rgb(jg, 0.5, 0.5, 0.5);
	jgraphics_rectangle_fill_fast(jg, 0, 0, 100, 100);

}