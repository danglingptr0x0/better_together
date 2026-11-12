#ifndef BETTER_TOGETHER_MODEL_H
#define BETTER_TOGETHER_MODEL_H

#include <stdint.h>

#include <libresteam/fp32/fp32.h>

#define BETTER_TOGETHER_YARNY_HASH 0xE6D9923AU
#define BETTER_TOGETHER_BOXIE_HASH 0xB3E3425DU
#define BETTER_TOGETHER_MODEL_KIND 7U
#define BETTER_TOGETHER_PATH_CAP 32U
#define BETTER_TOGETHER_AXIS_CUNT 3U
#define BETTER_TOGETHER_BASIS_CUNT 9U

typedef struct better_together_vec
{
    uint64_t begin;
    uint64_t end;
    uint64_t cap;
} better_together_vec_t;

typedef struct better_together_desc
{
    better_together_vec_t comps;
    libresteam_fp32_t basis[BETTER_TOGETHER_BASIS_CUNT];
    libresteam_fp32_t scale[BETTER_TOGETHER_AXIS_CUNT];
    libresteam_fp32_t pos[BETTER_TOGETHER_AXIS_CUNT];
    uint32_t group;
} better_together_desc_t;

typedef struct better_together_comp
{
    uint32_t kind;
    uint8_t pudding[4];
    uint64_t payload;
    uint64_t cleanup;
} better_together_comp_t;

typedef struct better_together_model
{
    uint64_t data;
    uint8_t pudding[8];
    uint64_t len;
    uint64_t cap;
    uint16_t flags;
    uint8_t pudding_tail[6];
} better_together_model_t;

typedef struct better_together_packet
{
    better_together_desc_t desc;
    better_together_comp_t comp;
    better_together_model_t model;
    char path[BETTER_TOGETHER_PATH_CAP];
} better_together_packet_t;

uint32_t better_together_model_prepare(better_together_packet_t *out, uint64_t cleanup, const libresteam_fp32_t *pos);

#endif
