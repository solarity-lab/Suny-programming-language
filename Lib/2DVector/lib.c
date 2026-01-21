#include "../../Suny/Suny/Suny.h"
#include "Suny/smem.h"

struct Vector {
    float x;
    float y;
};

struct Sobj* Sobj_add_vector(struct Sobj* a, struct Sobj* b);
struct Sobj* Sobj_sub_vector(struct Sobj* a, struct Sobj* b);
int Vector_free(struct Sobj* obj);

struct Vector* Vector_new(float x, float y) {
    struct Vector* vector = (struct Vector*) malloc(sizeof(struct Vector));
    vector->x = x;
    vector->y = y;
    return vector;
}

struct Vector* Vector_add(struct Vector* a, struct Vector* b) {
    return Vector_new(a->x + b->x, a->y + b->y);
}

struct Vector* Vector_sub(struct Vector* a, struct Vector* b) {
    return Vector_new(a->x - b->x, a->y - b->y);
}

struct Vector* Vector_print(struct Vector* vector) {
    printf("Vector(%g, %g)", vector->x, vector->y);
    return vector;
}

struct Sobj* Sobj_vector_print(struct Sobj* obj) {
    struct Vector* vector = get_userdata(obj);
    Vector_print(vector);
    return obj;
}

SUNY_API struct Sobj* Sobj_make_vector(struct Vector* vector) {
    struct Sobj* obj = Sobj_make_userdata(vector);

    obj->meta = Smeta_new();
    
    Smeta_set(obj, "__add__",       Sobj_add_vector);
    Smeta_set(obj, "__sub__",       Sobj_sub_vector);
    Smeta_set(obj, "__tostring__",  Sobj_vector_print);
    Smeta_set(obj, "__free__",      Vector_free);

    return obj;
}

int Vector_free(struct Sobj* obj) {
    struct Vector* vector = get_userdata(obj);
    free(vector);
    return 0;
}

struct Sobj* Sobj_add_vector(struct Sobj* a, struct Sobj* b) {
    struct Vector* va = get_userdata(a);
    struct Vector* vb = get_userdata(b);
    
    struct Vector* vector = Vector_add(va, vb);
    struct Sobj* obj = Sobj_make_vector(vector);

    return obj;
}

struct Sobj* Sobj_sub_vector(struct Sobj* a, struct Sobj* b) {
    struct Vector* va = get_userdata(a);
    struct Vector* vb = get_userdata(b);
    
    struct Vector* vector = Vector_sub(va, vb);
    struct Sobj* obj = Sobj_make_vector(vector);

    return obj;
}

SUNY_API struct Sobj* Svector(struct Sframe* frame) {
    struct Sobj* obj_y = Sframe_pop(frame);
    struct Sobj* obj_x = Sframe_pop(frame);

    float x = ValueOf(obj_x);
    float y = ValueOf(obj_y);

    struct Vector* vector = Vector_new(x, y);

    MOVETOGC(obj_x, frame->gc_pool);
    MOVETOGC(obj_y, frame->gc_pool);

    return Sobj_make_vector(vector);
}

SUNY_API struct Sframe* Smain(struct Sframe* frame, struct Stable* table) {
    Sinitialize_c_api_func(frame, table, 20, 2, "vector", Svector);
    return frame;
}