#ifndef BETTER_TOGETHER_WINDOW_H
#define BETTER_TOGETHER_WINDOW_H

#include <stdint.h>

typedef struct better_together_window_report
{
    uint64_t window;
    uint64_t presents;
    uint64_t rejected;
    uint32_t thread;
    uint32_t active;
    uint32_t borderless;
    uint32_t res;
    uint32_t native_res;
    uint32_t hook_res;
} better_together_window_report_t;

uint32_t better_together_window_init(void **out);
uint32_t better_together_window_stop(void *opaque);
uint32_t better_together_window_report(void *opaque, better_together_window_report_t *out);

#endif
