#include <stdio.h>
#include <stdlib.h>

#include "ObjectPool.h"

int pool_init(ObjectPool *pool, size_t capacity)
{
    if (pool == NULL || capacity == 0U) {
        return 0;
    }

    /* Resets the descriptor before attempting the only allocation. */
    pool->storage = NULL;
    pool->free_start = NULL;
    pool->capacity = 0U;
    pool->available = 0U;

    pool->storage = (PoolItem *)malloc(capacity * sizeof(PoolItem));
    if (pool->storage == NULL) {
        return 0;
    }

    /* Builds the initial free list over the contiguous array. */
    for (size_t index = 0U; index < capacity; index++) {
        pool->storage[index].id = (int)index;
        pool->storage[index].sensor_id = 0;
        pool->storage[index].value = 0.0f;
        pool->storage[index].next = NULL;

        if (index + 1U < capacity) {
            pool->storage[index].next_free = &pool->storage[index + 1U];
        } else {
            pool->storage[index].next_free = NULL;
        }
    }

    pool->free_start = &pool->storage[0];
    pool->capacity = capacity;
    pool->available = capacity;
    return 1;
}

PoolItem *pool_acquire(ObjectPool *pool)
{
    if (pool == NULL || pool->free_start == NULL) {
        return NULL;
    }

    /* Removes the first free item without traversing the storage or list. */
    PoolItem *item = pool->free_start;
    pool->free_start = item->next_free;
    pool->available--;

    /* Clears both links before the application uses the item. */
    item->next = NULL;
    item->next_free = NULL;
    return item;
}

int pool_release(ObjectPool *pool, PoolItem *item)
{
    if (pool == NULL || pool->storage == NULL || item == NULL) {
        return 0;
    }

    /* Inserts the returned item at the beginning in constant time. */
    item->next = NULL;
    item->next_free = pool->free_start;
    pool->free_start = item;
    pool->available++;
    return 1;
}

size_t pool_available(const ObjectPool *pool)
{
    if (pool == NULL) {
        return 0U;
    }
    return pool->available;
}

size_t pool_capacity(const ObjectPool *pool)
{
    if (pool == NULL) {
        return 0U;
    }
    return pool->capacity;
}

void pool_print_free(const ObjectPool *pool)
{
    if (pool == NULL) {
        return;
    }

    printf("Itens livres: ");
    const PoolItem *item = pool->free_start;

    if (item == NULL) {
        printf("nenhum\n");
        return;
    }

    while (item != NULL) {
        printf("%d", item->id);
        item = item->next_free;
        if (item != NULL) {
            printf(" -> ");
        }
    }
    printf("\n");
}

void pool_destroy(ObjectPool *pool)
{
    if (pool == NULL) {
        return;
    }

    /* One free matches the only malloc performed by pool_init. */
    free(pool->storage);
    pool->storage = NULL;
    pool->free_start = NULL;
    pool->capacity = 0U;
    pool->available = 0U;
}
