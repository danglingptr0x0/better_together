#include <stddef.h>
#include <stdint.h>
#include <math.h>

#include <dangling/core/err.h>
#include <dangling/core/macros.h>
#include <dangling/mem/secure.h>

#include <libresteam/ext/together/model.h>

#define BETTER_TOGETHER_MODEL_PATH "assets/Characters/yarny"
#define BETTER_TOGETHER_BASIS_DIAGONAL_STEP 4U
#define BETTER_TOGETHER_UNIT_SCALE 1.0
#define BETTER_TOGETHER_ASSERT_LEN(cond) ((cond) ? 1 : -1)

typedef char better_together_desc_size_assert[BETTER_TOGETHER_ASSERT_LEN(sizeof(better_together_desc_t) == 0x58)];
typedef char better_together_desc_basis_assert[BETTER_TOGETHER_ASSERT_LEN(offsetof(better_together_desc_t, basis) == 0x18)];
typedef char better_together_desc_pos_assert[BETTER_TOGETHER_ASSERT_LEN(offsetof(better_together_desc_t, pos) == 0x48)];
typedef char better_together_desc_group_assert[BETTER_TOGETHER_ASSERT_LEN(offsetof(better_together_desc_t, group) == 0x54)];
typedef char better_together_comp_size_assert[BETTER_TOGETHER_ASSERT_LEN(sizeof(better_together_comp_t) == 0x18)];
typedef char better_together_comp_payload_assert[BETTER_TOGETHER_ASSERT_LEN(offsetof(better_together_comp_t, payload) == 0x08)];
typedef char better_together_comp_cleanup_assert[BETTER_TOGETHER_ASSERT_LEN(offsetof(better_together_comp_t, cleanup) == 0x10)];
typedef char better_together_model_size_assert[BETTER_TOGETHER_ASSERT_LEN(sizeof(better_together_model_t) == 0x28)];
typedef char better_together_model_flags_assert[BETTER_TOGETHER_ASSERT_LEN(offsetof(better_together_model_t, flags) == 0x20)];
typedef char better_together_path_cap_assert[BETTER_TOGETHER_ASSERT_LEN(sizeof(BETTER_TOGETHER_MODEL_PATH) <= BETTER_TOGETHER_PATH_CAP)];

uint32_t better_together_model_prepare(better_together_packet_t *out, uint64_t cleanup, const libresteam_fp32_t *pos)
{
    uint32_t idx = 0;
    uint32_t res = LDG_ERR_AOK;

    if (LDG_UNLIKELY(!out || !pos)) { return LDG_ERR_FUNC_ARG_NULL; }

    if (LDG_UNLIKELY(!cleanup)) { return LDG_ERR_FUNC_ARG_INVALID; }

    for (idx = 0; idx < BETTER_TOGETHER_AXIS_CUNT; idx++) { if (LDG_UNLIKELY(!isfinite((double)pos[idx]))) { return LDG_ERR_FUNC_ARG_INVALID; } }
    res = ldg_mem_secure_zero(out, sizeof(*out));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    res = ldg_mem_secure_cpy(out->path, BETTER_TOGETHER_MODEL_PATH, sizeof(BETTER_TOGETHER_MODEL_PATH));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    out->desc.comps.begin = (uint64_t)(uintptr_t)&out->comp;
    out->desc.comps.end = out->desc.comps.begin + sizeof(out->comp);
    out->desc.comps.cap = out->desc.comps.end;
    out->comp.kind = BETTER_TOGETHER_MODEL_KIND;
    out->comp.payload = (uint64_t)(uintptr_t)&out->model;
    out->comp.cleanup = cleanup;
    out->model.data = (uint64_t)(uintptr_t)out->path;
    out->model.len = sizeof(BETTER_TOGETHER_MODEL_PATH) - LDG_STR_TERM_SIZE;
    out->model.cap = sizeof(out->path) - LDG_STR_TERM_SIZE;
    for (idx = 0; idx < BETTER_TOGETHER_AXIS_CUNT; idx++)
    {
        out->desc.basis[idx * BETTER_TOGETHER_BASIS_DIAGONAL_STEP] = (libresteam_fp32_t)BETTER_TOGETHER_UNIT_SCALE;
        out->desc.scale[idx] = (libresteam_fp32_t)BETTER_TOGETHER_UNIT_SCALE;
        out->desc.pos[idx] = pos[idx];
    }
    return LDG_ERR_AOK;
}
