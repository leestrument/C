
// 라이브러리
#include "ext.h"
#include "ext_obex.h"
#include "stdlib.h"

// 구조체
typedef struct leestrument {
    
    t_object loptimist;
    long inlet0; // 1st inlet will get "bang" to generate random
    long inlet1; // 2nd inlet will get "range" to set random range.
    void* outlet0;
    
} randomOther;

// 함수 미리 선언.
void bang(randomOther* object);
void range(randomOther* object, long n);
void* new(long n);

// 클래스 미리 선언
t_class* randomOtherClass;

// 클래스 생성자
void ext_main(void *r) {
    
    // 클래스 생성
    t_class* myClass;
    myClass = class_new("lee.randomOther", (method)new, (method)NULL, sizeof(randomOther), 0L, A_DEFLONG, 0);
    
    // 메쏘드 추가
    class_addmethod(myClass, (method)bang, "bang", 0);
    class_addmethod(myClass, (method)range, "in1", A_LONG, 0);
    
    // 생성된 클래스 Max 에 등록
    class_register(CLASS_BOX, myClass);
    
    // 생성된 클래스 c 코드안에 등록.
    randomOtherClass = myClass;
    
}

// 인스턴스 생성자
void* new(long n) {
    
    // 인스턴스 메모리에 등록
    randomOther* instance;
    instance = (randomOther *)object_alloc(randomOtherClass);
    
    // instance 에 2번째 inlet 추가. (최대 9까지 가능)
    // inlet 1개는 자동으로 생성되므로 따로 등록할 필요없음.
    intin(instance, 1);
    
    // instance 에 outlet 추가.
    instance->outlet0 = intout(instance);
    
    // instance 값 초기화
    instance->inlet0 = 0;
    instance->inlet1 = 100;
    
    // instance 반환.
    return (instance);
    
}

// 메쏘드 (위의 기본 설정까지가 개같음. 여기서부터 행복)
void bang(randomOther* instance) {
    
    int n = rand() % instance->inlet1;
    
    while (instance->inlet0 == n) {
        
        n = random() % instance->inlet1;
        
    }
    
    instance->inlet0 = n;
    
    // outlet_int(outlet 선택, inlet값 선택)
    outlet_int(instance->outlet0, instance->inlet0);
    
}


void range(randomOther* instance, long n) {
    
    // 사용자는 1을 입력할수없음.
    // 1 들어오면 무한 while 됨..
    if (n > 1) {
        
        instance->inlet1 = n;
        
    }
    
}
