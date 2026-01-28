#include "HashMap.h"

struct Sobj* ShashMap(struct Sframe* frame) {
    struct Sobj* mapl = Sframe_pop(frame);

    if (mapl->type != LIST_OBJ) {
        MOVETOGC(mapl, frame->gc_pool);
        return null_obj;
    }

    _SUNYINCREF(mapl);

    struct Sobj* map = Sobj_make_hashmap(mapl->f_type->f_list);

    return map;
}

SUNY_API struct Sframe* Smain(struct Sframe* frame, struct Stable *table) {
    Sinitialize_c_api_func(frame, table, 20, 1, "map", ShashMap);
    return frame;
}
