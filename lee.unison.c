#include "ext.h"
#include "ext_obex.h"

// 구조체
typedef struct _unison {
    
    t_object leestrument;
    t_atom list[256];
    long length;
    long unison;
    void* outlet_0;
    
} unison;

// 클래스 미리선언
t_class* unison_class;

// 함수 미리선언
void* unison_new (long n);
void unison_bang(unison* instance); //1st inlet
void unison_int(unison* instance, long n); //2nd inlst - set unison value
void unison_list(unison* instance, t_symbol* s, long length, t_atom* list); // 3rd inlet


// 클래스 생성자
void ext_main(void* r) {
    
    // 클래스 생성
    t_class* c = class_new("lee.unison", (method)unison_new, (method)NULL, sizeof(unison), 0L, A_DEFLONG, 0);
 
    // 메소드 추가
    // 메소드 추가시, inlet 이 어떤 type 의 메세지를 받을지 결정해야함. A_LONG 은 그냥 정수, 리스트 쓸거면 A_GIMME
    class_addmethod(c, (method)unison_bang, "bang", 0); // 1번째 inlet
    class_addmethod(c, (method)unison_int, "int", A_LONG, 0); // 2번쨰 inlet
    class_addmethod(c, (method)unison_list, "list", A_GIMME, 0); // 3번째 inlet

    // 등록
    unison_class = c;
    class_register(CLASS_BOX, c);
    
}

// 인스턴스 생성자
void* unison_new(long n) {
    
    // 인스턴스 메모리에 등록
    unison* instance = (unison* )object_alloc(unison_class);
    
    // 초기화
    instance->length = 0;
    instance->unison = 0;
    instance->outlet_0 = intout(instance); // outlet 추가
    
    return instance;
}

// 메소드
void unison_bang(unison* instance) {
    
    long length = instance->length;
    
    if (length > 0) {
        
        long unison = instance->unison + 1;
        long total = length * unison;
        t_atom outList[total];
        
        long i, j;
        long index = 0;
        
        for (i = 0; i < unison; i++) {
            
            for (j = 0; j < length; j++) {
                
                long n = atom_getlong(&(instance->list[j])) + (i * 12);
                
                atom_setlong(&(outList[index]), n);
                
                index++;
                
            }
        
        }
        
        outlet_list(instance->outlet_0, NULL, total, outList);
        
    }
    
}



void unison_int(unison* instance, long n) {
    
    // store n
    instance->unison = n;
    
}

void unison_list(unison* instance, t_symbol* s, long length, t_atom* list) {
        
    // store list
    long i;
    
    for (i = 0; i < length; i++) {
        
        instance->list[i] = list[i];
        
    }
    
    // store length
    instance->length = length;
    
}
