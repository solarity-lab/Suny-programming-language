#include "HashMap.h"

struct HashMap* HashMapNew(void) {
    struct HashMap* map = (struct HashMap*) Smem_Malloc(sizeof(struct HashMap));
    map->key = Slist_new();
    map->value = Slist_new();
    map->size = 0;

    return map;
}

struct HashMap* HashMapPush(struct HashMap* map, struct Sobj* key, struct Sobj* value) {
    if (!map || !key || !value) return map;

    Slist_add(map->key, key);
    Slist_add(map->value, value);

    _SUNYINCREF(key);
    _SUNYINCREF(value);

    map->size++;

    return map;
}

int HashMapFree(struct HashMap *map) {
    Smem_Free(map->key);
    Smem_Free(map->value);
    
    return Smem_Free(map);
}

struct Sobj* Sobj_make_hashmap(struct Slist* list) {
    if (!list) {
        __ERROR("Null list");
    }

    int size = list->count;

    if (size % 2 != 0) {
        __ERROR("Missing value for key");
    }

    struct HashMap* map = HashMapNew();

    for (int i = 0; i < size; i += 2) {
        struct Sobj* key   = Slist_get(list, i);
        struct Sobj* value = Slist_get(list, i + 1);

        if (!key) {
            __ERROR("Null key");
        }
        if (!value) {
            __ERROR("Null value");
        }

        HashMapPush(map, key, value);
    }

    struct Sobj* obj = Sobj_make_userdata(map);

    obj->meta = Smeta_new();

    Smeta_set(obj, "__tostring__", SHashMap_print);
    Smeta_set(obj, "__free__", SHashMap_free);
    Smeta_set(obj, "__index__", SHashMap_get_value);
    Smeta_set(obj, "__store__", SHashMap_store_item);

    return obj;
}

struct Sobj* SHashMap_print(struct Sobj* obj) {
    struct HashMap* map = get_userdata(obj);

    printf("{");

    for (int i = 0; i < map->size; i++) {
        struct Sobj* key = Slist_get(map->key, i);
        struct Sobj* value = Slist_get(map->value, i);

        Sio_write(key);

        printf(": ");

        Sio_write(value);

        if (i < map->size - 1) {
            printf(", ");
        }
    }

    printf("}");

    return obj;
}

int SHashMap_free(struct Sobj* obj) {
    struct HashMap* map = get_userdata(obj);

    Slist_free(map->key);
    Slist_free(map->value);

    Smem_Free(map);

    return 0;
}

struct Sobj* SHashMap_get_value(struct Sobj* hashmap, struct Sobj* key) {
    struct HashMap* map = get_userdata(hashmap);

    for (int i = 0; i < map->size; i++) {
        struct Sobj* k = Slist_get(map->key, i);
        struct Sobj* v = Slist_get(map->value, i);

        if (Scompare_equal(k, key)) {
            return v;
        }
    }

    return null_obj;
}

int SHashMap_store_item(struct Sobj* obj, struct Sobj* key, struct Sobj* value) {
    struct HashMap* map = get_userdata(obj);

    for (int i = 0; i < map->size; i++) {
        struct Sobj* k = Slist_get(map->key, i);

        if (Scompare_equal(k, key)) {
            struct Sobj* v = Slist_get(map->value, i);

            _SUNYDECREF(v);

            struct Slist* list = map->value;
            
            Slist_change_item(list, i, value);

            _SUNYINCREF(value);

            return 0;
        }
    }

    return -1;
}