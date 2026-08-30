#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "ObjectPool.h"

int main(void)
{
    ObjectPool pool;

    /* O pool reserva toda a memória antes do processamento normal. */
    if (pool_init(&pool, 3U) == 0) {
        fprintf(stderr, "Nao foi possivel inicializar o pool de objetos.\n");
        return EXIT_FAILURE;
    }

    assert(pool_capacity(&pool) == 3U);
    assert(pool_available(&pool) == 3U);
    pool_print_free(&pool);

    /* Adquire todos os itens sem realizar malloc nem percorrer a lista. */
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

    printf(
        "Requisicoes adquiridas: %d, %d, %d\n",
        request_a->id,
        request_b->id,
        request_c->id
    );
    pool_print_free(&pool);

    /* Devolver A e depois B produz a ordem livre B -> A. */
    assert(pool_release(&pool, request_a) == 1);
    assert(pool_release(&pool, request_b) == 1);
    assert(pool_available(&pool) == 2U);
    pool_print_free(&pool);

    /* As próximas aquisições comprovam a reutilização em ordem LIFO. */
    PoolItem *reused_b = pool_acquire(&pool);
    PoolItem *reused_a = pool_acquire(&pool);
    assert(reused_b == request_b);
    assert(reused_a == request_a);
    assert(pool_available(&pool) == 0U);

    printf("Requisicoes reutilizadas: %d, %d\n", reused_b->id, reused_a->id);

    /* Devolve cada item adquirido exatamente uma vez antes da destruição. */
    assert(pool_release(&pool, request_c) == 1);
    assert(pool_release(&pool, reused_a) == 1);
    assert(pool_release(&pool, reused_b) == 1);
    assert(pool_available(&pool) == 3U);
    pool_print_free(&pool);

    pool_destroy(&pool);
    assert(pool_capacity(&pool) == 0U);
    assert(pool_available(&pool) == 0U);

    return EXIT_SUCCESS;
}
