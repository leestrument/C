#include "ext.h"
#include "ext_obex.h"
#include "jpatcher_api.h"
#include "jgraphics.h"

/* struct */
typedef struct _lee {

    t_jbox jBox;
    long play;
    long record;
    void* outlet_1;
    void* outlet_2;

} lee;


/* Max prototype */
t_class* leeClass = NULL;
lee* create (t_symbol* s, short ac, t_atom* av);
void delete (lee* instance);
void paint  (lee* instance, t_object* view);

/* Own prototype */
void play   (lee* instance);
void stop   (lee* instance);
void record (lee* instance);

/* main */
void ext_main(void* r) {

    // create class.
    leeClass = class_new("lee.transport_button", (method)create, (method)delete, sizeof(lee), (method)NULL, A_GIMME, 0L);

    // init class
    jbox_initclass(leeClass, 0);
    CLASS_ATTR_DEFAULT(leeClass, "patching_rect", 0, "0 0 300 100");

    // add method
    class_addmethod(leeClass, (method)paint,    "paint", A_CANT, 0);
    class_addmethod(leeClass, (method)play,     "play",   0);
    class_addmethod(leeClass, (method)stop,     "stop",   0);
    class_addmethod(leeClass, (method)record,   "record", 0);

    // register class.
    class_register(CLASS_BOX, leeClass);

}

/* create & delete */
lee* create(t_symbol* s, short ac, t_atom* av) {

    // create instance.
    lee* instance = (lee*)object_alloc(leeClass);

    // set flags.
    long flags = 0
    | JBOX_DRAWFIRSTIN
    // JBOX_NODRAWBOX
    //        | JBOX_DRAWINLAST
    | JBOX_TRANSPARENT
    //        | JBOX_NOGROW
    //        | JBOX_GROWY
    | JBOX_GROWBOTH
    //        | JBOX_IGNORELOCKCLICK
    | JBOX_HILITE
    //        | JBOX_BACKGROUND
    //        | JBOX_NOFLOATINSPECTOR
    //        | JBOX_TEXTFIELD
    ;

	// create jbox.
	jbox_new(&instance->jBox, flags, ac, av);

    // init jbox.
    instance->jBox.b_firstin = (t_object*)instance;
	jbox_ready(&instance->jBox);

    // init instance.
    instance->play   = 0;
    instance->record = 0;

    // create outlet.
    instance->outlet_1 = intout(instance);
	instance->outlet_2 = intout(instance);

    // return instance.
    return instance;

}

void delete(lee* instance) {

    jbox_free(&instance->jBox);

}

/* paint */
void paint(lee* instance, t_object* view) {

    // create JGraphics.
    t_jgraphics* jg = (t_jgraphics*)patcherview_get_jgraphics(view);

    // get box size.
    t_rect boxSize;
    jbox_get_rect_for_view((t_object*)instance, view, &boxSize);

    // globalSize
    double boxW = boxSize.width;
    double boxH = boxSize.height;
    double eachW = boxW / 3.;
    double eachGap = (boxW / boxH) * 5.;

    // set color.
    double playColor[2][4] = {{0.6, 0.6, 0.6}, {0, 0.7, 0}};
    double recColor [2][4] = {{0.6, 0.6, 0.6}, {0.7, 0, 0}};

    // play button (polygon)
    jgraphics_set_source_rgb(jg, playColor[instance->play][0], playColor[instance->play][1], playColor[instance->play][2]);
    jgraphics_move_to(jg, eachGap, eachGap);
    jgraphics_line_to(jg, eachGap, boxH - eachGap);
    jgraphics_line_to(jg, eachW - eachGap, (boxH / 2));
    jgraphics_close_path(jg);
    jgraphics_fill(jg);

    // stop button (rectangle)
    double stopX = eachW + eachGap;
    double stopY = eachGap;
    double stopW = eachW - (eachGap * 2.);
    double stopH = boxH - (eachGap * 2);

    jgraphics_set_source_rgb(jg, 0.6, 0.6, 0.6);
    jgraphics_rectangle_fill_fast(jg, stopX, stopY, stopW, stopH);

    // rec button (ellipse)
    double recX = (eachW * 2) + eachGap;
    double recY = eachGap;
    double recW = eachW - (eachGap * 2);
    double recH = boxH - (eachGap * 2);

    jgraphics_set_source_rgb(jg, recColor[instance->record][0], recColor[instance->record][1], recColor[instance->record][2]);
    jgraphics_ellipse(jg, recX, recY, recW, recH);
    jgraphics_fill(jg);

}

/* method */
void play   (lee* instance) {

    // store
    instance->play = 1;

    // outlet
    outlet_int(instance->outlet_2, 1);

    // redraw
    jbox_redraw((t_jbox*)instance);

}

void stop   (lee* instance) {

    // store
    instance->play = 0;
    instance->record = 0;

    // outlet
    outlet_int(instance->outlet_1, 0);
    outlet_int(instance->outlet_2, 0);

    // redraw
    jbox_redraw((t_jbox*)instance);

}

void record (lee* instance) {

    // store
    long recordState = instance->record;

    switch (recordState) {

        case 0 : instance->record = 1; instance->play = 1; break;
        case 1 : instance->record = 0; break;

    }

    // outlet
    outlet_int(instance->outlet_1, instance->record);
    outlet_int(instance->outlet_2, instance->play);

    // redraw
    jbox_redraw((t_jbox*)instance);

}

