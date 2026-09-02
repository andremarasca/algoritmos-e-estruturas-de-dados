#ifndef OBJECT_POOL_H
#define OBJECT_POOL_H

#include <stddef.h>

/*
 * Um item do pool representa uma requisição reutilizável do firmware simulado.
 * next_free só tem significado enquanto o item pertence à lista de livres.
 */
typedef struct pool_item {
    int id;
    int sensor_id;
    float value;
    struct pool_item *next_free;
} PoolItem;

/*
 * storage possui uma única região contígua com todos os itens.
 * free_start é a única fonte de verdade sobre a disponibilidade dos itens.
 */
typedef struct object_pool {
    PoolItem *storage;
    PoolItem *free_start;
    size_t capacity;
    size_t available;
} ObjectPool;

/* Aloca uma região contígua e encadeia todos os itens como livres. */
int pool_init(ObjectPool *pool, size_t capacity);

/* Remove e retorna o primeiro item livre em tempo constante. */
PoolItem *pool_acquire(ObjectPool *pool);

/*
 * Devolve um item adquirido ao início da lista de livres.
 * O chamador não pode devolver o mesmo item duas vezes.
 */
int pool_release(ObjectPool *pool, PoolItem *item);

size_t pool_available(const ObjectPool *pool);
size_t pool_capacity(const ObjectPool *pool);

/* Imprime os identificadores ao seguir as ligações da lista de livres. */
void pool_print_free(const ObjectPool *pool);

/* Libera a única região criada por pool_init. */
void pool_destroy(ObjectPool *pool);

#endif
