#ifndef HashMap_h
#define HashMap_h

#include "../../../Suny/Suny/Suny.h"

#include <stdio.h>

struct HashMap {
    struct Slist* key;
    struct Slist* value;

    int size;
};

struct HashMap* HashMapNew(void);

int HashMapFree(struct HashMap *map);

struct HashMap* HashMapPush(struct HashMap* map, struct Sobj* key, struct Sobj* value);

struct Sobj* Sobj_make_hashmap(struct Slist* list);

struct Sobj* SHashMap_print(struct Sobj* obj);

struct Sobj* SHashMap_get_value(struct Sobj* hashmap, struct Sobj* key);

int SHashMap_free(struct Sobj* obj);

int SHashMap_store_item(struct Sobj* obj, struct Sobj* key, struct Sobj* value);

#endif // HashMap_h
