#include "ext.h"
#include "ext_obex.h"
#include "jpatcher_api.h"
#include "jgraphics.h"

// struct
typedef struct _button {
    
    t_jbox jBox;
    
} button;

// prototype
t_class* buttonClass = NULL;
button* buttonNew(t_symbol* s, short argc, t_atom* argv);
void buttonFree(button* instance);
void buttonPaint(button* instance, t_object* view);
void buttonBang(button* instance);

// Max 키면 실행되는 함수
void ext_main(void* r) {
    
    // 클래스 생성
    t_class* c = class_new("lee.button", (method)buttonNew, (method)buttonFree, sizeof(button), (method)NULL, A_GIMME, 0L);
    
    // 클래스 초기화
    jbox_initclass(c, 0);
    
    // 메소드 추가
    class_addmethod(c, (method)buttonPaint, "paint", A_CANT, 0);
    class_addmethod(c, (method)buttonBang, "bang", 0);
    
    // 클래스 속성 초기화
    CLASS_ATTR_DEFAULT(c, "patching_rect", 0, "0 0 1000 1000");
    
    // 클래스 등록
    class_register(CLASS_BOX, c);
    buttonClass = c;
    
}

// Max 에서 이 object 만들었을때 실행되는 함수
button* buttonNew(t_symbol* s, short argc, t_atom* argv) {
    
    // object 생성.
    button* instance = (button *)object_alloc(buttonClass);
    
    // flags 설정.
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
    
    // jbox 생성후 instance 안에 삽입.
    jbox_new(&instance->jBox, flags, argc, argv);
    
    // jbox 초기화
    instance->jBox.b_firstin = (t_object *) instance;
    jbox_ready(&instance->jBox);
    
    // instance 반환
    return instance;
}

// Max 에서 이 object 를 삭제했을때 실행되는 함수
void buttonFree(button* instance) {
    
    jbox_free(&instance->jBox);

}

// Paint
void buttonPaint(button* instance, t_object* view) {
    
    // JGraphics 생성
    t_jgraphics* jg = (t_jgraphics *) patcherview_get_jgraphics(view);
    
    // 그려
    long i;
    
    for (i = 0; i < 100; i++) {
        
        double x = rand() % 1000;
        double y = rand() % 1000;
        double w = rand() % 1000;
        double h = rand() % 1000;
        double r = (double)rand() / (double)RAND_MAX;
        double g = (double)rand() / (double)RAND_MAX;
        double b = (double)rand() / (double)RAND_MAX;
        
        jgraphics_rectangle(jg, x, y, w, h);
        jgraphics_set_source_rgba(jg, r, g, b, 0.5);
        jgraphics_fill(jg);
        
    }
}

// 메소드
void buttonBang(button* instance) {
    
    jbox_redraw((t_jbox *)instance);
    
}
