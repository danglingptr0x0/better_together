#ifndef BETTER_TOGETHER_SETUP_H
#define BETTER_TOGETHER_SETUP_H

#include <stdint.h>

#define BETTER_TOGETHER_SETUP_THUNK_CUNT 2
#define BETTER_TOGETHER_SETUP_REPORT_WORDS 16

typedef struct better_together_setup_layout
{
    uint64_t src;
    uint64_t len;
    uint64_t code_end;
    uint64_t ctx_off;
    uint64_t cb_off;
    uint64_t unwind_off;
} better_together_setup_layout_t;

uint32_t better_together_setup_layout_get(better_together_setup_layout_t *out);
uint32_t better_together_setup_bind_layout_get(better_together_setup_layout_t *out);
uint32_t better_together_setup_ik_layout_get(better_together_setup_layout_t *out);
uint64_t better_together_setup_size_get(void);
uint32_t better_together_setup_init(void *storage, uint64_t cap, uint64_t owner);
uint32_t better_together_setup_owner_bind(void *storage, uint64_t owner);
uint32_t better_together_setup_enter(void *storage, uint64_t world, uint64_t arg);
uint32_t better_together_setup_report(void *storage, uint64_t *out, uint64_t cap);
uint32_t better_together_setup_validate(void *storage, uint64_t world);

#endif
