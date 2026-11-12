#ifndef BETTER_TOGETHER_SPAWN_H
#define BETTER_TOGETHER_SPAWN_H

#include <stdint.h>

#define BETTER_TOGETHER_SPAWN_WAITING 1U
#define BETTER_TOGETHER_SPAWN_RUNNING 2U
#define BETTER_TOGETHER_SPAWN_COMPLETE 3U
#define BETTER_TOGETHER_SPAWN_FAILED 4U
#define BETTER_TOGETHER_SPAWN_STALE 5U

typedef struct better_together_spawn_report
{
    uint64_t owner;
    uint64_t entity;
    uint64_t orig;
    uint64_t world;
    uint32_t state;
    uint32_t res;
    uint32_t thread;
    uint32_t hook_res;
    uint32_t renamed[3];
    uint32_t pudding;
    uint64_t req_cunt;
    uint32_t req_res;
    uint32_t req_pending;
    uint32_t req_existing;
    uint32_t req_pudding;
} better_together_spawn_report_t;

typedef uint32_t (*better_together_spawn_roots_cb_t)(void *user, uint64_t host, uint64_t guest, uint32_t thread);

uint64_t better_together_spawn_size_get(void);
uint32_t better_together_spawn_init(void *storage, uint64_t cap, better_together_spawn_roots_cb_t roots, void *user);
uint32_t better_together_spawn_report(void *storage, better_together_spawn_report_t *out);
uint32_t better_together_spawn_req(void *storage);
uint32_t better_together_spawn_tick(void *storage, uint64_t game);
uint32_t better_together_spawn_stop(void *storage);

#endif
