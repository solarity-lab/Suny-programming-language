#include "MathLib.h"

SUNY_API struct Sobj* Scomplex(struct Sframe* frame) {
    struct Sobj* real_obj = Sframe_pop(frame);
    struct Sobj* imag_obj = Sframe_pop(frame);

    double real = ValueOf(real_obj);
    double imag = ValueOf(imag_obj);

    struct Sobj* complex_obj = Sobj_make_complex(complex(real, imag));

    MOVETOGC(real_obj, frame->gc_pool);
    MOVETOGC(imag_obj, frame->gc_pool);
    
    return complex_obj;
}

SUNY_API struct Sobj* Ssqrt(struct Sframe* frame) {
    struct Sobj* obj = Sframe_pop(frame);
    MOVETOGC(obj, frame->gc_pool);
    return Sobj_make_number(sqrt(ValueOf(obj)));
}   

SUNY_API struct Sobj* Sabs(struct Sframe* frame) {
    struct Sobj* obj = Sframe_pop(frame);
    MOVETOGC(obj, frame->gc_pool);
    return Sobj_make_number(fabs(ValueOf(obj)));
}

SUNY_API struct Sobj* Slog(struct Sframe* frame) {
    struct Sobj* b = Sframe_pop(frame);
    struct Sobj* a = Sframe_pop(frame);
    
    MOVETOGC(a, frame->gc_pool);
    MOVETOGC(b, frame->gc_pool);

    return Sobj_make_number(log(ValueOf(b)) / log(ValueOf(a)));
}

SUNY_API struct Sobj* Slog10(struct Sframe* frame) {
    struct Sobj* obj = Sframe_pop(frame);
    MOVETOGC(obj, frame->gc_pool);
    return Sobj_make_number(log10(ValueOf(obj)));
}

SUNY_API struct Sobj* Sln(struct Sframe* frame) {
    struct Sobj* obj = Sframe_pop(frame);
    MOVETOGC(obj, frame->gc_pool);
    return Sobj_make_number(log(ValueOf(obj)) / log(E_CONST));
}

SUNY_API struct Sobj* Ssin(struct Sframe* frame) {
    struct Sobj* obj = Sframe_pop(frame);
    MOVETOGC(obj, frame->gc_pool);
    return Sobj_make_number(sin(ValueOf(obj)));
}

SUNY_API struct Sobj* Scos(struct Sframe* frame) {
    struct Sobj* obj = Sframe_pop(frame);
    MOVETOGC(obj, frame->gc_pool);
    return Sobj_make_number(cos(ValueOf(obj)));
}

SUNY_API struct Sobj* Stan(struct Sframe* frame) {
    struct Sobj* obj = Sframe_pop(frame);
    MOVETOGC(obj, frame->gc_pool);
    return Sobj_make_number(tan(ValueOf(obj)));
}

SUNY_API struct Sobj* Scot(struct Sframe* frame) {
    struct Sobj* obj = Sframe_pop(frame);
    MOVETOGC(obj, frame->gc_pool);
    return Sobj_make_number(1 / tan(ValueOf(obj)));
}
