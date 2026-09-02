#include <stdio.h>
#include <stdlib.h>

#include "ObjectPool.h"

int pool_init(ObjectPool *pool, size_t capacity)
{
    if (pool == NULL || capacity == 0U) {
        return 0;
    }

    /* Reinicia o descritor antes de tentar realizar a única alocação. */
    pool->storage = NULL;
    pool->free_start = NULL;
    pool->capacity = 0U;
    pool->available = 0U;

    pool->storage = (PoolItem *)malloc(capacity * sizeof(PoolItem));
    if (pool->storage == NULL) {
        return 0;
    }

    /* Constrói a lista inicial de livres sobre a região contígua. */
    for (size_t index = 0U; index < capacity; index++) {
        pool->storage[index].id = (int)index;
        pool->storage[index].sensor_id = 0;
        pool->storage[index].value = 0.0f;

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

    /* A ligação deixa de ter significado enquanto o item está adquirido. */
    item->next_free = NULL;
    return item;
}

int pool_release(ObjectPool *pool, PoolItem *item)
{
    if (pool == NULL || pool->storage == NULL || item == NULL) {
        return 0;
    }

    /* Insere o item devolvido no início da lista em tempo constante. */
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

    /* Um único free corresponde ao único malloc realizado por pool_init. */
    free(pool->storage);
    pool->storage = NULL;
    pool->free_start = NULL;
    pool->capacity = 0U;
    pool->available = 0U;
}
