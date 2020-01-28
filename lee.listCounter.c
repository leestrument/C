#include "ext.h"
#include "ext_obex.h"

// 구조체
typedef struct _listCounter {
    
    t_object leestrument;
    t_atom list[10];
    long length;
    long mode;
    long counter;
    void* outlet0;
    
} listCounter;

// 클래스 미리 선언
t_class* listCounterClass;

// 함수 미리 선언
void* new(long n);
void bang(listCounter* instance);
void list(listCounter* instance, t_symbol* s, long ac, t_atom* av);


// 클래스 생성자
void ext_main(void *r) {
    
    // 클래스 생성
    t_class* c = class_new("lee.listCounter", (method) new, (method) NULL, sizeof(listCounter), 0L, A_DEFLONG, 0);
    
    // 메소드 추가
    class_addmethod(c, (method) bang, "bang", 0);
    class_addmethod(c, (method) list, "list", A_GIMME, 0);
    
    // 등록
    listCounterClass = c; // C 안에 등록
    class_register(CLASS_BOX, c); // Max 등록

}

// 인스턴스 생성자
void* new(long n) {
    
    // 인스턴스 메모리에 등록
    listCounter* instance = (listCounter *)object_alloc(listCounterClass);
    
    // inlet 추가
    inlet_new(instance, NULL);
    
    // outlet 추가
    instance->outlet0 = intout(instance);
    
    // 초기화
    instance->length = 0;
    instance->mode = 0;
    instance->counter = 0;
    
    // 반환
    return instance;
    
}

// method
void bang(listCounter* instance) {
    
    if (instance->length > 0) {
        
        // outlet
        long n = atom_getlong(&(instance->list[instance->counter]));

        outlet_int(instance->outlet0, n);
        
        // increase counter.
        instance->counter+= 1;
        
        if (instance->counter > instance->length - 1) {
            
            instance->counter = 0;
            
        }
        
    }
    
}

void list(listCounter* instance, t_symbol* s, long ac, t_atom* av) {
    
    // save ac.
    instance->length = ac;
    
    // save av.
    long i;
    
    for (i = 0; i < ac; i++) {
        
        instance->list[i] = av[i];
        
    }
    
}

