#ifndef OBJECT_POOL_H
#define OBJECT_POOL_H

#include <stddef.h>

/*
 * A pool item represents a reusable request in the simulated firmware.
 * next links acquired items in the application list.
 * next_free links items while they belong to the free list.
 */
typedef struct pool_item {
    int id;
    int sensor_id;
    float value;
    struct pool_item *next;
    struct pool_item *next_free;
} PoolItem;

/*
 * storage owns one contiguous array with every item.
 * free_start reaches only the items currently available.
 */
typedef struct object_pool {
    PoolItem *storage;
    PoolItem *free_start;
    size_t capacity;
    size_t available;
} ObjectPool;

/* Allocates one contiguous array and links every item as available. */
int pool_init(ObjectPool *pool, size_t capacity);

/* Removes and returns the first available item in constant time. */
PoolItem *pool_acquire(ObjectPool *pool);

/*
 * Returns an acquired item to the beginning of the free list.
 * The caller must not return the same item twice.
 */
int pool_release(ObjectPool *pool, PoolItem *item);

size_t pool_available(const ObjectPool *pool);
size_t pool_capacity(const ObjectPool *pool);

/* Prints identifiers by following the free-list links. */
void pool_print_free(const ObjectPool *pool);

/* Releases the only array allocated by pool_init. */
void pool_destroy(ObjectPool *pool);

#endif
