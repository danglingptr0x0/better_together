#ifndef BETTER_TOGETHER_PAD_H
#define BETTER_TOGETHER_PAD_H

#include <stdint.h>

#define BETTER_TOGETHER_PAD_ACTION_CUNT 10U
#define BETTER_TOGETHER_PAD_PLAYER_CUNT 2U
#define BETTER_TOGETHER_PAD_SLOT_CUNT 4U

typedef struct better_together_pad_sample
{
    double current[BETTER_TOGETHER_PAD_ACTION_CUNT];
    double prior[BETTER_TOGETHER_PAD_ACTION_CUNT];
} better_together_pad_sample_t;

typedef struct better_together_pad_report
{
    uint32_t host;
    uint32_t guest;
    uint32_t connected;
    uint32_t provider;
    uint64_t frames;
    uint32_t rebind;
    uint32_t pudding;
} better_together_pad_report_t;

uint64_t better_together_pad_size_get(void);
uint32_t better_together_pad_init(void *storage, uint64_t cap);
uint32_t better_together_pad_sel(void *storage, uint32_t host, uint32_t guest);
uint32_t better_together_pad_poll(void *storage, better_together_pad_sample_t *out, better_together_pad_report_t *report);

#endif
