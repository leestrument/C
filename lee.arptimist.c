#include "ext.h"
#include "ext_obex.h"
#include "jpatcher_api.h"
#include "jgraphics.h"

/* struct */
typedef struct _arp {
    
    t_jbox jBox;
    long noteIn[128];
    long noteIsPressed;
    long stack;
    long mode;
    long counter;
    void* out_1;
    void* out_2;
    void* out_3;
    
} arp;

/* class prototype */
t_class* arpClass = NULL;

/* new & free */
arp* New(t_symbol* s, short ac, t_atom* av);
void Free(arp* instance);

/* method prototype */
void stack(arp* instance, long n);
void list(arp* instance, t_symbol* s, long length, t_atom* list);
void bang(arp* instance);
void paint(arp* instance, t_object* view);

/* when Max is loaded */
void ext_main(void *r) {
    
    // create class.
    arpClass = class_new("lee.arptimist", (method)New, (method)Free, sizeof(arp), (method)NULL, A_GIMME, 0L);
    
    // init class
    jbox_initclass(arpClass, 0);
    CLASS_ATTR_DEFAULT(arpClass, "patching_rect", 0, "0 0 169 169");
    
    // add methods.
    class_addmethod(arpClass, (method)stack, "stack", A_LONG, 0);
    class_addmethod(arpClass, (method)list, "list", A_GIMME, 0);
    class_addmethod(arpClass, (method)bang, "bang", 0);
    class_addmethod(arpClass, (method)paint, "paint", A_CANT, 0);
    
    // register class
    class_register(CLASS_BOX, arpClass);
    
}

/* when user create new object in Max */
arp* New(t_symbol* s, short ac, t_atom* av) {
    
    // create instance.
    arp* instance = (arp*)object_alloc(arpClass);
    
    // set flags
    long flags = 0
                 | JBOX_DRAWFIRSTIN
                 //        | JBOX_NODRAWBOX
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
    
    // create jbox
    jbox_new(&instance->jBox, flags, ac, av);
    
    // init jbox
    instance->jBox.b_firstin = (t_object *) instance;
    jbox_ready(&instance->jBox);
    
    // init instance
    instance->noteIsPressed = 0;
    instance->stack = 0;
    instance->mode = 0;
    instance->counter = 0;
    instance->out_3 = intout(instance);
    instance->out_2 = intout(instance);
    instance->out_1 = intout(instance);
    
    // return instance
    return instance;
}

/* when user delete the object in Max */
void Free(arp* instance) {
    
    jbox_free(&instance->jBox);

}

/* paint */
void paint(arp* instance, t_object* view) {
    
    
}

/* methods */
void stack(arp* instance, long n) {
    
    instance->stack = n;
    
}

void list(arp* instance, t_symbol* s, long length, t_atom* list) {
    
    // get pitch, velocity
    long pitch = atom_getlong(&list[0]);
    long velocity = atom_getlong(&list[1]);
    
    // update noteIn
    long i;
    long stack = instance->stack;
    
    for (i = 0; i < stack + 1; i++) {
        
        long noteIndex = pitch + (i * 12);
        
        if (noteIndex < 128) {
            
            instance->noteIn[noteIndex] = velocity;
            
        }
        
    }
    
    // update noteIsPressed.
    long noteIsPressed = 0;
    
    for (i = 0; i < 128; i++) {
        
        if (instance->noteIn[i] > 0) {
            
            noteIsPressed = 1;
            
            break;
            
        }
        
    }
    
    if (instance->noteIsPressed != noteIsPressed) {
        
        instance->noteIsPressed = noteIsPressed;
        
        // init counter.
        if (noteIsPressed) {
            
            instance->counter = 0;
            
        }
        
        // turn on or off metro.
        outlet_int(instance->out_3, noteIsPressed);
        
    }
    
}

void bang(arp* instance) {
    
    if (instance->noteIsPressed) {
        
        // get note length.
        long i;
        long noteLen = 0;
        
        for (i = 0; i < 128; i++) {
            
            if (instance->noteIn[i] > 0) {
                
                noteLen++;
                
            }
            
        }
        
        // create noteout
        long noteOut[noteLen][2];
        long noteIndex = 0;
        
        for (i = 0; i < 128; i++) {
            
            if (instance->noteIn[i] > 0) {
                
                long pitch = i;
                long velocity = instance->noteIn[i];
                
                noteOut[noteIndex][0] = pitch;
                noteOut[noteIndex][1] = velocity;
            
                noteIndex++;
                
            }
            
        }
        
        // send noteout of current counter to outlet
        if (instance->counter > noteLen - 1) {
            
            instance->counter = 0;
            
        }
        
        outlet_int(instance->out_2, noteOut[instance->counter][1]);
        outlet_int(instance->out_1, noteOut[instance->counter][0]);
        
        instance->counter++;
            
    }
    
}
