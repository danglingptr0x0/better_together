#ifndef BETTER_TOGETHER_TEX_H
#define BETTER_TOGETHER_TEX_H

#include <stdint.h>

typedef struct better_together_tex_report
{
    uint64_t attempts;
    uint32_t applied;
    uint32_t res;
} better_together_tex_report_t;

uint64_t better_together_tex_size_get(void);
uint32_t better_together_tex_init(void *storage, uint64_t cap, uint64_t base);
uint32_t better_together_tex_tick(void *storage, uint64_t lua, uint64_t host, uint64_t guest, uint32_t epoch, better_together_tex_report_t *out);

#endif
