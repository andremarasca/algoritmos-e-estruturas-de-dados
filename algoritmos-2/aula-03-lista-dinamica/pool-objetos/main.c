#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "ObjectPool.h"

static void list_print(const PoolItem *list_start)
{
    printf("Lista da aplicacao: ");

    if (list_start == NULL) {
        printf("vazia\n");
        return;
    }

    const PoolItem *item = list_start;
    while (item != NULL) {
        printf("%d", item->id);
        item = item->next;
        if (item != NULL) {
            printf(" -> ");
        }
    }
    printf("\n");
}

int main(void)
{
    ObjectPool pool;

    /* The pool reserves all memory before normal processing starts. */
    if (pool_init(&pool, 3U) == 0) {
        fprintf(stderr, "Nao foi possivel inicializar o pool de objetos.\n");
        return EXIT_FAILURE;
    }

    assert(pool_capacity(&pool) == 3U);
    assert(pool_available(&pool) == 3U);
    pool_print_free(&pool);

    /* Acquires every item without allocating or traversing the array. */
    PoolItem *request_a = pool_acquire(&pool);
    PoolItem *request_b = pool_acquire(&pool);
    PoolItem *request_c = pool_acquire(&pool);

    assert(request_a != NULL && request_a->id == 0);
    assert(request_b != NULL && request_b->id == 1);
    assert(request_c != NULL && request_c->id == 2);
    assert(pool_available(&pool) == 0U);
    assert(pool_acquire(&pool) == NULL);

    request_a->sensor_id = 101;
    request_a->value = 24.5f;
    request_b->sensor_id = 102;
    request_b->value = 61.0f;
    request_c->sensor_id = 103;
    request_c->value = 18.8f;

    /* The application links acquired items in its own list. */
    PoolItem *list_start = request_a;
    request_a->next = request_b;
    request_b->next = request_c;
    request_c->next = NULL;

    printf(
        "Requisicoes adquiridas: %d, %d, %d\n",
        request_a->id,
        request_b->id,
        request_c->id
    );
    list_print(list_start);
    pool_print_free(&pool);

    /* Removes A and B from the application list before returning them. */
    list_start = request_a->next;
    request_a->next = NULL;
    assert(pool_release(&pool, request_a) == 1);

    list_start = request_b->next;
    request_b->next = NULL;
    assert(pool_release(&pool, request_b) == 1);
    assert(pool_available(&pool) == 2U);
    list_print(list_start);
    pool_print_free(&pool);

    /* The next acquisitions verify reuse in LIFO order. */
    PoolItem *reused_b = pool_acquire(&pool);
    PoolItem *reused_a = pool_acquire(&pool);
    assert(reused_b == request_b);
    assert(reused_a == request_a);
    assert(pool_available(&pool) == 0U);

    reused_b->next = list_start;
    list_start = reused_b;
    reused_a->next = list_start;
    list_start = reused_a;

    printf("Requisicoes reutilizadas: %d, %d\n", reused_b->id, reused_a->id);
    list_print(list_start);

    /* Removes and returns every item before destroying the pool. */
    while (list_start != NULL) {
        PoolItem *removed = list_start;
        list_start = removed->next;
        removed->next = NULL;
        assert(pool_release(&pool, removed) == 1);
    }

    assert(pool_available(&pool) == 3U);
    list_print(list_start);
    pool_print_free(&pool);

    pool_destroy(&pool);
    assert(pool_capacity(&pool) == 0U);
    assert(pool_available(&pool) == 0U);

    return EXIT_SUCCESS;
}
