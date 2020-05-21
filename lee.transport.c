#include "ext.h"
#include "ext_obex.h"
#include "jpatcher_api.h"
#include "jgraphics.h"

/* struct */
typedef struct _lee {

	t_jbox jBox;
	long bars;
	long bar;	
	long beat;  
	double sync;  
	void* outlet_1;

} lee;

/* Max prototype */
t_class* leeClass = NULL;
lee* create(t_symbol* s, short ac, t_atom* av);
void delete(lee* instance);
void paint(lee* instance, t_object* view);

/* own prototype */
void bars(lee* instance, long n);
void bar(lee* instance, long n);
void beat(lee* instance, long n);
void sync(lee* instacne, double f);
void stop(lee* instance);

/* main */
void ext_main(void* r) {

	// create class.
	leeClass = class_new("lee.transport", (method)create, (method)delete, sizeof(lee), (method)NULL, A_GIMME, 0L);

	// init class.
	jbox_initclass(leeClass, 0);
	CLASS_ATTR_DEFAULT(leeClass, "patching_rect", 0, "0 0 169 169");

	// add method
	class_addmethod(leeClass, (method)paint, "paint", A_CANT, 0);
	class_addmethod(leeClass, (method)bars, "bars", A_LONG, 0);
	class_addmethod(leeClass, (method)bar, "bar", A_LONG, 0);
	class_addmethod(leeClass, (method)beat, "beat", A_LONG, 0);
	class_addmethod(leeClass, (method)sync, "sync", A_FLOAT, 0);
	class_addmethod(leeClass, (method)stop, "stop", 0);

	// register
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
	instance->bars = 4;
	instance->bar = 1;
	instance->beat = 1;
	instance->sync = 0;
	instance->outlet_1 = intout(instance);

	// return instance
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

	// bar
	double barGap = 4.;
	double barW = boxSize.width / instance->bar;
	double barDrawW = barW - (barGap * 2.);
	double barH = boxSize.height - (barGap * 2.);
	double barY = barGap;
	long i;

	for (i = 0; i < instance->bar; i++) {

		double barX = (i * barW) + barGap;

		jgraphics_set_source_rgb(jg, 0.68, 0.31, 0.45);
		jgraphics_rectangle_fill_fast(jg, barX, barY, barDrawW, barH);

	}

	// beat
	double beatGap = 8.;
	double beatW = barW / 4.;
	double beatX = ((instance->beat - 1.) * beatW) + ((instance->bar - 1.) * barW);
	double beatDrawX = beatX + beatGap;
	double beatDrawY = beatGap;
	double beatDrawW = beatW - (beatGap * 2.);
	double beatDrawH = boxSize.height - (beatGap * 2.);

	jgraphics_set_source_rgb(jg, 0.16, 0.16, 0.16);
	jgraphics_rectangle_fill_fast(jg, beatDrawX, beatDrawY, beatDrawW, beatDrawH);

	// sync
	double syncGap = 4.;
	double syncX = (instance->sync * barW) + ((instance->bar - 1.) * barW);

	jgraphics_set_source_rgb(jg, 0.8, 0.8, 0.8);
	jgraphics_set_line_width(jg, 2.);
	jgraphics_move_to(jg, syncX, barGap);
	jgraphics_line_to(jg, syncX, boxSize.height - barGap);
	jgraphics_stroke(jg);


}



/* method */
void bars(lee* instance, long n) {

	// store
	instance->bars = n;

}

void bar(lee* instance, long n) {

	// outlet loop start & end.



	// loop back.
	if (n > instance->bars) {

		// init struct.
		instance->bar = 1;
		instance->beat = 1;

		// outlet 0 when loop is done to init [transport]
		outlet_int(instance->outlet_1, 0);

	}

	else {

		instance->bar = n;

	}

	// redraw
	jbox_redraw((t_jbox*)instance);


}

void beat(lee* instance, long n) {

	// store
	instance->beat = n;

	// redraw
	jbox_redraw((t_jbox*)instance);

}

void sync(lee* instance, double f) {

	// store
	instance->sync = f;

	// redraw
	jbox_redraw((t_jbox*)instance);

}

void stop(lee* instance) {

	// init
	instance->bar = 1;
	instance->beat = 1;
	instance->sync = 0;

	// redraw
	jbox_redraw((t_jbox*)instance);

}
