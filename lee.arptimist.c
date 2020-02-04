// ---------------------------------------------------------------------------------------------//
#include "ext.h"
#include "ext_obex.h"
#include "jpatcher_api.h"
#include "jgraphics.h"

/* struct */
typedef struct _lee
{
    
    t_jbox jBox;
    long noteIn[128];
    long noteIsPressed;
    long stack;
    long mode;
    long counter;
    long timer;
    long direction;
    void* out_4; // noteAmount
    void* out_3; // metro on or off
    void* out_2; // velocity
    void* out_1; // pitch
    
} lee;

/* prototype for Max */
t_class* leeClass = NULL;
lee* create                         (t_symbol* s, short ac, t_atom* av);
void delete                         (lee* instance);
void paint                          (lee* instance, t_object* view);

/* prototype for Max inlet */
void stack                          (lee* instance, long n);
void mode                           (lee* instance, long n);
void list                           (lee* instance, t_symbol* s, long length, t_atom* list);
void bang                           (lee* instance);

/* prototype for getter */
long getNoteAmount                  (lee* instance);

/* prototype for counter */
void count_init                     (lee* instance, long noteAmount);
void count_up                       (lee* instance, long noteAmount);
void count_down                     (lee* instance, long noteAmount);
void count_up_down                  (lee* instance, long noteAmount);
void count_down_up                  (lee* instance, long noteAmount);
void count_low_up_high_down         (lee* instance, long noteAmount);
void count_low_down_high_up         (lee* instance, long noteAmount);
void count_low_down_high_down       (lee* instance, long noteAmount);
void count_high_down_low_up         (lee* instance, long noteAmount);
void count_high_up_low_down         (lee* instance, long noteAmount);
void count_random                   (lee* instance, long noteAmount);
void count_random_other             (lee* instance, long noteAmount);
void count_root_random              (lee* instance, long noteAmount);
void count_low_random_high_random   (lee* instance, long noteAmount);

void (*setCounter[13])              (lee* instance, long noteAmount) = {
    
    count_up,                       // 0
    count_down,                     // 1
    count_up_down,                  // 2
    count_down_up,                  // 3
    count_low_up_high_down,         // 4
    count_low_down_high_up,         // 5
    count_low_down_high_down,       // 6
    count_high_down_low_up,         // 7
    count_high_up_low_down,         // 8
    count_random,                   // 9
    count_random_other,             // 10
    count_root_random,              // 11
    count_low_random_high_random    // 12
    
};

// ---------------------------------------------------------------------------------------------//

/* main */
void ext_main(void* r) {
    
    // create class.
    leeClass = class_new("lee.arptimist", (method)create, (method)delete, sizeof(lee), (method)NULL, A_GIMME, 0L);
    
    // init class.
    jbox_initclass(leeClass, 0);
    CLASS_ATTR_DEFAULT(leeClass, "patching_rect", 0, "0 0 169 169");
    
    // add method.
    class_addmethod(leeClass, (method)paint, "paint", A_CANT, 0);
    class_addmethod(leeClass, (method)stack, "stack", A_LONG, 0);
    class_addmethod(leeClass, (method)mode, "mode", A_LONG, 0);
    class_addmethod(leeClass, (method)list, "list", A_GIMME, 0);
    class_addmethod(leeClass, (method)bang, "bang", 0);
    
    // register
    class_register(CLASS_BOX, leeClass);
    
}

/* create & delete */
lee* create                         (t_symbol* s, short ac, t_atom* av)
{
    
    // create instance
    lee* instance = (lee*)object_alloc(leeClass);
    
    // set flags
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
    instance->timer = 0;
    instance->direction = 1;
    instance->out_4 = intout(instance);
    instance->out_3 = intout(instance);
    instance->out_2 = intout(instance);
    instance->out_1 = intout(instance);
    
    // return instance
    return instance;
    
}

void delete                         (lee* instance)
{
    
    jbox_free(&instance->jBox);
    
}

/* paint */
void paint                          (lee* instance, t_object* view)
{
    
}

/* Max inlet */
void stack                          (lee* instance, long n)
{
 
    // update stack.
    instance->stack = n;
    
    // init counter.
    count_init(instance, getNoteAmount(instance));
}

void mode                           (lee* instance, long n)
{
    // update mode.
    instance->mode = n;
    
    // init counter.
    count_init(instance, getNoteAmount(instance));
    
}

void list                           (lee* instance, t_symbol* s, long length, t_atom* list)
{
    // update noteIn.
    long pitch = atom_getlong(&list[0]);
    long velocity = atom_getlong(&list[1]);
    
    instance->noteIn[pitch] = velocity;
    
    // get noteAmount.
    long noteAmount = getNoteAmount(instance);
    
    // init counter.
    count_init(instance, noteAmount);
    
    // outlet noteAmount.
    outlet_int(instance->out_4, noteAmount);
    
    // update noteIsPressed, then outlet noteIsPressed.
    long noteIsPressed = noteAmount > 0 ? 1 : 0;
    
    if (instance->noteIsPressed != noteIsPressed) {
        
        instance->noteIsPressed = noteIsPressed;
        
        outlet_int(instance->out_3, noteIsPressed);
        
    }
    
    
}

void bang                           (lee* instance)
{
    if (instance->noteIsPressed) {
        
        // create noteAfterStack.
        long noteAfterStack[128] = {0, };
        long i, j;
        
        for (i = 0; i < instance->stack + 1; i++) {
            
            for (j = 0; j < 128; j++) {
                
                if (instance->noteIn[j] > 0) {
                    
                    long pitch = j + (i * 12);
                    
                    if (pitch < 128) {
                        
                        noteAfterStack[pitch] = instance->noteIn[j];
                        
                    }
                    
                }
                
                
            }
            
            
        }
        
        // create noteOut.
        long noteOut[128][2] = {{0}};
        long noteIndex = 0;
        
        for (i = 0; i < 128; i++) {
            
            if (noteAfterStack[i] > 0) {
                
                noteOut[noteIndex][0] = i;
                noteOut[noteIndex][1] = noteAfterStack[i];
                noteIndex++;
                
            }
            
        }
        
        // outlet current note.
        outlet_int(instance->out_2, noteOut[instance->counter][1]); // velocity
        outlet_int(instance->out_1, noteOut[instance->counter][0]); // pitch
        
        // get noteAmount.
        long noteAmount = getNoteAmount(instance);
        
        // increase timer
        instance->timer++;
        
        if (instance->timer > noteAmount - 1) {
            
            instance->timer = 0;
            
        }
        
        // control counter.
        switch (noteAmount) {
                
            case 1 : instance->counter = 0; break;
            default : setCounter[instance->mode](instance, noteAmount);
                
        }
        
    }
    
}

/* getter */
long getNoteAmount                  (lee* instance)
{

    long noteAmount = 0;
    long i, j;
    
    for (i = 0; i < instance->stack + 1; i++) {
        
        for (j = 0; j < 128; j++) {
            
            if (instance->noteIn[j] > 0) {
                
                long pitch = j + (i * 12);
                
                if (pitch < 128) {
                    
                    noteAmount++;
                    
                }
                            
            }
            
        }
        
    }

    return noteAmount;
    
}

/* counter */
void count_init                     (lee* instance, long noteAmount)
{
    long counter = 0;
    
    if (noteAmount > 1) {
        
        switch (instance->mode) {
                
            case 0  : counter = 0; break;                    // up
            case 1  : counter = noteAmount - 1; break;       // down
            case 2  : counter = 0; break;                    // up and down
            case 3  : counter = noteAmount - 1; break;       // down and up
            case 4  : counter = 0; break;                    // low up high down
            case 5  : counter = (noteAmount / 2) - 1; break; // low down high up
            case 6  : counter = (noteAmount / 2) - 1; break; // low down high down
            case 7  : counter = noteAmount - 1; break;       // high down low up
            case 8  : counter = noteAmount / 2; break;       // high up low down
            case 9  : counter = rand() % noteAmount; break;  // random
            case 10 : counter = rand() % noteAmount; break;  // random other
            case 11 : counter = 0; break;                    // root random
            case 12 : counter = 0; break;                    // low random high random
                
        }
        
    }
    
    instance->counter = counter;
    instance->timer = 0;
    
}

void count_up                       (lee* instance, long noteAmount)
{
    instance->counter++;
    
    if (instance->counter > noteAmount - 1) {
        
        instance->counter = 0;
        
    }

}

void count_down                     (lee* instance, long noteAmount)
{
    instance->counter --;
    
    if (instance->counter < 0) {
        
        instance->counter = noteAmount - 1;
        
    }
    
}

void count_up_down                  (lee* instance, long noteAmount)
{
    // set direction
    if (instance->counter == 0) {
        
        instance->direction = 1;
        
    }
    
    else if (instance->counter == noteAmount - 1) {
        
        instance->direction = -1;
        
    }
    
    instance->counter += instance->direction;
    
}

void count_down_up                  (lee* instance, long noteAmount)
{
    
    // set direction
    if (instance->counter == noteAmount - 1) {
        
        instance->direction = -1;
        
    }
    
    else if (instance->counter == 0) {
        
        instance->direction = 1;
        
    }
    
    instance->counter += instance->direction;
    
}

void count_low_up_high_down         (lee* instance, long noteAmount)
{

    // 3 notes.
    if (noteAmount == 3) {
        
        switch (instance->timer) {
                
            case 0 : instance->counter = 0; break;
            case 1 : instance->counter = 2; break;
            case 2 : instance->counter = 1; break;
                
        }
        
    }
    
    // more notes.
    else {
        
        if (instance->counter == 0) {
            
            instance->direction = 1;
            
        }
        
        else if (instance->counter == (noteAmount / 2) - 1) {
            
            instance->counter = noteAmount;
            instance->direction = -1;
            
        }
        
        else if (instance->counter == noteAmount / 2) {
            
            instance->counter = -1;
            instance->direction = 1;
            
        }
        
        instance->counter += instance->direction;
        
    }
    
}
 
void count_low_down_high_up         (lee* instance, long noteAmount)
{
 
    // 2, or 3 notes - just up
    if (noteAmount < 4) {
        
        instance->counter++;
        
        if (instance->counter > noteAmount - 1) {
            
            instance->counter = 0;
            
        }
        
    }
    
    // more notes
    else {
        
        
        if (instance->counter == (noteAmount / 2) - 1) {
            
            instance->direction = -1;
            
        }
        
        else if (instance->counter == 0) {
            
            instance->counter = (noteAmount / 2) - 1;
            instance->direction = 1;
            
        }
        
        else if (instance->counter == noteAmount - 1) {
            
            instance->counter = (noteAmount / 2);
            instance->direction = -1;
            
        }
        
        instance->counter += instance->direction;
    
    }
    
}

void count_low_down_high_down       (lee* instance, long noteAmount)
{

    if (instance->counter == 0) {
        
        instance->counter = noteAmount;
        
    }
    
    instance->counter--;
    
    
}

void count_high_down_low_up         (lee* instance, long noteAmount)
{
    
    // 2 or 3 notes. - just down
    if (noteAmount < 4) {
        
        instance->counter--;
        
        if (instance->counter < 0) {
            
            instance->counter = noteAmount - 1;
            
        }
        
    }
    
    // more notes
    else {
        
        if (instance->counter == noteAmount - 1) {
            
            instance->direction = -1;
            
        }
        
        else if (instance->counter == noteAmount / 2) {
            
            instance->counter = -1;
            instance->direction = 1;
        }
        
        else if (instance->counter == (noteAmount / 2) - 1) {
            
            instance->counter = noteAmount;
            instance->direction = -1;
        }
        
        instance->counter += instance->direction;
        
    }

}

void count_high_up_low_down         (lee* instance, long noteAmount)
{
        
    // 2 notes. - just down
    if (noteAmount == 2) {
        
        instance->counter--;
        
        if (instance->counter < 0) {
            
            instance->counter = noteAmount - 1;
            
        }
        
    }
    
    // more notes.
    else {
        
        if (instance->counter == noteAmount / 2) {
            
            instance->direction = 1;
            
        }
        
        else if (instance->counter == noteAmount - 1) {
            
            instance->counter = noteAmount / 2;
            instance->direction = -1;
            
        }
        
        else if (instance->counter == 0) {
            
            instance->counter = (noteAmount / 2) - 1;
            instance->direction = 1;
        }
        
        instance->counter += instance->direction;
        
    }
    
}

void count_random                   (lee* instance, long noteAmount)
{
    
    instance->counter = rand() % noteAmount;
    
}

void count_random_other             (lee* instance, long noteAmount)
{
    
    long counter = rand() % noteAmount;
    
    while (instance->counter == counter) {
        
        counter = rand() % noteAmount;
        
    }
    
    instance->counter = counter;
    
}

void count_root_random              (lee* instance, long noteAmount)
{
    
    if (instance->timer != 0) {
        
        long counter = (rand() % (noteAmount - 1)) + 1;
        
        while (instance->counter == counter) {
            
            counter = (rand() % (noteAmount - 1)) + 1;
            
        }
        
        instance->counter = counter;
        
    }
    
    else {
        
        instance->counter = 0;
        
    }
    

    
    post("%ld", instance->timer);
    
}

void count_low_random_high_random   (lee* instance, long noteAmount)
{
    long counter = 0;
    
    if (instance->timer > 0 && instance->timer < noteAmount / 2) {
        
        counter = rand() % (noteAmount / 2);
        
        while (instance->counter == counter) {
            
            counter = rand() % (noteAmount / 2);
            
        }
        
    }
    
    else if (instance->timer >= noteAmount / 2) {
        
        counter = rand() % (noteAmount - (noteAmount / 2)) + (noteAmount / 2);
        
        while (instance->counter == counter) {
            
            counter = rand() % (noteAmount - (noteAmount / 2)) + (noteAmount / 2);
            
        }
        
    }
    
    instance->counter = counter;
    
}

