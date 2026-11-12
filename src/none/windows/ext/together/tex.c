#include <stdint.h>
#include <windows.h>
#include <d3d11.h>

#include <dangling/core/bits.h>
#include <dangling/core/err.h>
#include <dangling/core/macros.h>
#include <dangling/mem/secure.h>
#include <libresteam/ext/together/model.h>
#include <libresteam/ext/together/tex.h>

#define BETTER_TOGETHER_TEX_CREATE_RVA UINT64_C(0x15A1360)
#define BETTER_TOGETHER_TEX_BIND_RVA UINT64_C(0x1805C10)
#define BETTER_TOGETHER_TEX_PARAM_RVA UINT64_C(0x18066B0)
#define BETTER_TOGETHER_TEX_HASH_RVA UINT64_C(0x1796D30)
#define BETTER_TOGETHER_TEX_FMT_RVA UINT64_C(0x903D50)
#define BETTER_TOGETHER_TEX_SYS_VTABLE_RVA UINT64_C(0x6C3018)
#define BETTER_TOGETHER_TEX_LUA_GLOB_OFF 0x20U
#define BETTER_TOGETHER_TEX_LUA_USER_OFF 0x18U
#define BETTER_TOGETHER_TEX_USER_BEHAVIOR_OFF 0x18U
#define BETTER_TOGETHER_TEX_MNGR_OFF 0x10U
#define BETTER_TOGETHER_TEX_SYS_LIST_OFF 0x98U
#define BETTER_TOGETHER_TEX_SYS_CAP 64U
#define BETTER_TOGETHER_TEX_OWNER_ENTITY_OFF 0x20U
#define BETTER_TOGETHER_TEX_ENTITY_HASH_OFF 0x08U
#define BETTER_TOGETHER_TEX_ENTITY_MASK_OFF 0x20U
#define BETTER_TOGETHER_TEX_ENTITY_LIST_OFF 0x68U
#define BETTER_TOGETHER_TEX_RENDER_KIND 7U
#define BETTER_TOGETHER_TEX_RENDER_INST_OFF 0x08U
#define BETTER_TOGETHER_TEX_RENDER_OWNER_OFF 0x28U
#define BETTER_TOGETHER_TEX_RENDER_PRIVATE_OFF 0x21U
#define BETTER_TOGETHER_TEX_SYS_RANGE_OFF 0x308U
#define BETTER_TOGETHER_TEX_SYS_INST_OFF 0x338U
#define BETTER_TOGETHER_TEX_RANGE_FIRST_MASK 0xFFFFFFU
#define BETTER_TOGETHER_TEX_RANGE_CUNT_SHIFT 24U
#define BETTER_TOGETHER_TEX_INST_MTLS_OFF 0x20U
#define BETTER_TOGETHER_TEX_MODEL_MTLS_OFF 0x60U
#define BETTER_TOGETHER_TEX_MTL_PARAM_OFF 0x08U
#define BETTER_TOGETHER_TEX_PARAM_LIST_OFF 0x10U
#define BETTER_TOGETHER_TEX_LIST_CUNT_MASK 0x7FFFFFFFU
#define BETTER_TOGETHER_TEX_PARAM_SIZE_MASK 0x1FFFU
#define BETTER_TOGETHER_TEX_PARAM_CAP 128U
#define BETTER_TOGETHER_TEX_PARAM_BYTES_MAX 0x10000U
#define BETTER_TOGETHER_TEX_MTL_CUNT 4U
#define BETTER_TOGETHER_TEX_YARN_CUNT 2U
#define BETTER_TOGETHER_TEX_BIND_TYPE UINT64_C(0x42)
#define BETTER_TOGETHER_TEX_DIFFUSE_HASH UINT64_C(0x5683545E)
#define BETTER_TOGETHER_TEX_NAME "DiffuseTexture"
#define BETTER_TOGETHER_TEX_TINT_NAME "DiffuseTint"
#define BETTER_TOGETHER_TEX_HUE_NAME "DiffuseHueShift"
#define BETTER_TOGETHER_TEX_VEC_AXES 3U
#define BETTER_TOGETHER_TEX_UNIT_BITS 0x3F800000U
#define BETTER_TOGETHER_TEX_WIDTH 128U
#define BETTER_TOGETHER_TEX_HEIGHT 512U
#define BETTER_TOGETHER_TEX_BLK_EDGE 4U
#define BETTER_TOGETHER_TEX_BLK_BYTES 16U
#define BETTER_TOGETHER_TEX_ENDPOINT_OFF 8U
#define BETTER_TOGETHER_TEX_ENDPOINT_CUNT 2U
#define BETTER_TOGETHER_TEX_RED_SHIFT 11U
#define BETTER_TOGETHER_TEX_GREEN_SHIFT 5U
#define BETTER_TOGETHER_TEX_RB_MAX 31U
#define BETTER_TOGETHER_TEX_G_MAX 63U
#define BETTER_TOGETHER_TEX_GRASS_RED 82U
#define BETTER_TOGETHER_TEX_GRASS_GREEN 131U
#define BETTER_TOGETHER_TEX_GRASS_BLUE 51U
#define BETTER_TOGETHER_TEX_SKY_RED 57U
#define BETTER_TOGETHER_TEX_SKY_GREEN 128U
#define BETTER_TOGETHER_TEX_SKY_BLUE 198U
#define BETTER_TOGETHER_TEX_QUANTIZE_ROUND 0.5
#define BETTER_TOGETHER_TEX_ROW_BYTES (BETTER_TOGETHER_TEX_WIDTH / BETTER_TOGETHER_TEX_BLK_EDGE * BETTER_TOGETHER_TEX_BLK_BYTES)
#define BETTER_TOGETHER_TEX_ROWS (BETTER_TOGETHER_TEX_HEIGHT / BETTER_TOGETHER_TEX_BLK_EDGE)
#define BETTER_TOGETHER_TEX_PIXEL_BYTES (BETTER_TOGETHER_TEX_ROW_BYTES * BETTER_TOGETHER_TEX_ROWS)
#define BETTER_TOGETHER_TEX_GPU_REC_BYTES 0x40U
#define BETTER_TOGETHER_TEX_NAME_TAG_BIT UINT64_C(1)
#define BETTER_TOGETHER_TEX_RENDER_KIND_BIT (UINT64_C(1) << BETTER_TOGETHER_TEX_RENDER_KIND)
#define BETTER_TOGETHER_TEX_ENTRY_STEP 1U
#define BETTER_TOGETHER_TEX_RANGE_EXPECTED_CUNT 1U
#define BETTER_TOGETHER_TEX_BLUE_IDX (BETTER_TOGETHER_TEX_VEC_AXES - 1U)
#define BETTER_TOGETHER_TEX_ARR_CUNT 1U
#define BETTER_TOGETHER_TEX_SAMPLE_CUNT 1U
#define BETTER_TOGETHER_TEX_SUBRESOURCE 0U
#define BETTER_TOGETHER_TEX_MAP_FLAGS 0U
#define BETTER_TOGETHER_TEX_QWORD_ALIGN_MASK (sizeof(uint64_t) - 1U)
#define BETTER_TOGETHER_TEX_MTL_LAST_IDX (BETTER_TOGETHER_TEX_MTL_CUNT - 1U)
#define BETTER_TOGETHER_TEX_RECORD_BYTES 0xB0U
#define BETTER_TOGETHER_TEX_HRESULT_FAILED 0x80000000U
#define BETTER_TOGETHER_TEX_RETRY_MS LDG_MS_PER_SEC
#define BETTER_TOGETHER_TEX_PROFILE_BYTES 12U
#define BETTER_TOGETHER_TEX_RETRY_CAP 30U

typedef struct better_together_tex_binding
{
    uint64_t type;
    uint64_t sampler;
    uint64_t resource;
} better_together_tex_binding_t;

typedef struct better_together_tex_list
{
    uint32_t cunt;
    uint32_t pudding;
    uint64_t entries;
} better_together_tex_list_t;

typedef struct better_together_tex_param
{
    uint64_t flags;
    uint64_t name;
    uint16_t off;
    uint16_t size;
    uint32_t shader_off;
} better_together_tex_param_t;

typedef struct better_together_tex_gpu_rec
{
    uint64_t unknown;
    ID3D11Texture2D *tex;
    ID3D11Texture2D *secondary;
    ID3D11ShaderResourceView *srv;
    uint8_t pudding[BETTER_TOGETHER_TEX_GPU_REC_BYTES - sizeof(uint64_t) * 4];
} better_together_tex_gpu_rec_t;

typedef struct better_together_tex_record
{
    uint64_t fmt;
    uint64_t unknown;
    uint32_t mip_idx;
    uint32_t mip_other;
    uint64_t flags;
    uint64_t samples;
    uint32_t width;
    uint32_t height;
    uint64_t unknown_tail;
    uint32_t gpu_cunt;
    uint32_t pudding;
    better_together_tex_gpu_rec_t gpu;
    D3D11_TEXTURE2D_DESC desc;
    uint32_t pudding_tail;
} better_together_tex_record_t;

typedef struct better_together_tex_view
{
    uint64_t renderable;
    uint64_t inst;
    uint64_t model;
    uint64_t mtls;
    better_together_tex_binding_t current[BETTER_TOGETHER_TEX_MTL_CUNT];
    better_together_tex_binding_t orig[BETTER_TOGETHER_TEX_MTL_CUNT];
} better_together_tex_view_t;

typedef uint32_t (*better_together_tex_create_func_t)(better_together_tex_record_t *, const uint8_t *, uint32_t, uint32_t, uint32_t, void *);
typedef void (*better_together_tex_bind_func_t)(uint64_t, uint64_t, uint64_t, const better_together_tex_binding_t *, uint32_t);
typedef void (*better_together_tex_param_func_t)(uint64_t, uint64_t, uint64_t, const uint32_t *, uint32_t);
typedef uint64_t (*better_together_tex_hash_func_t)(const uint64_t *);

typedef struct better_together_tex_ctx
{
    uint64_t base;
    uint64_t next;
    uint32_t epoch;
    uint32_t create_res;
    uint32_t retries;
    uint8_t busy;
    uint8_t created;
    uint8_t attempted;
    uint8_t pudding[5];
    better_together_tex_create_func_t create;
    better_together_tex_bind_func_t bind;
    better_together_tex_param_func_t param;
    uint64_t tint_hash;
    uint64_t hue_hash;
    ID3D11Device *dev;
    better_together_tex_record_t green;
    better_together_tex_record_t sky;
    better_together_tex_report_t report;
    uint8_t pixels[BETTER_TOGETHER_TEX_PIXEL_BYTES];
    uint8_t recolored[BETTER_TOGETHER_TEX_PIXEL_BYTES];
} better_together_tex_ctx_t;

static uint32_t better_together_tex_mem_rd(uint64_t addr, void *out, uint64_t len)
{
    SIZE_T copied = 0;

    if (LDG_UNLIKELY(!out)) { return LDG_ERR_FUNC_ARG_NULL; }

    if (LDG_UNLIKELY(!addr || addr > UINTPTR_MAX - len)) { return LDG_ERR_FUNC_ARG_INVALID; }

    if (LDG_UNLIKELY(!ReadProcessMemory(GetCurrentProcess(), (const void *)(uintptr_t)addr, out, len, &copied) || copied != len)) { return LDG_ERR_MEM_BAD; }

    return LDG_ERR_AOK;
}

static uint32_t better_together_tex_binding_cmp(const better_together_tex_binding_t *lhs, const better_together_tex_binding_t *rhs)
{
    return lhs->type == rhs->type && lhs->sampler == rhs->sampler && lhs->resource == rhs->resource ? LDG_ERR_AOK : LDG_ERR_INVALID;
}

static uint32_t better_together_tex_mtls_rd(uint64_t addr, better_together_tex_binding_t *out)
{
    better_together_tex_list_t list = LDG_STRUCT_ZERO_INIT;
    better_together_tex_list_t params = LDG_STRUCT_ZERO_INIT;
    better_together_tex_param_t param = LDG_STRUCT_ZERO_INIT;
    uint64_t entry = 0;
    uint64_t blk = 0;
    uint32_t bytes = 0;
    uint32_t idx = 0;
    uint32_t param_idx = 0;
    uint32_t res = better_together_tex_mem_rd(addr, &list, sizeof(list));
    char name[sizeof(BETTER_TOGETHER_TEX_NAME)] = LDG_ARR_ZERO_INIT;
    uint8_t matched = 0;
    uint32_t ch = 0;

    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY((list.cunt & BETTER_TOGETHER_TEX_LIST_CUNT_MASK) != BETTER_TOGETHER_TEX_MTL_CUNT || !list.entries)) { return LDG_ERR_UNSUPPORTED; }

    for (idx = 0; idx < BETTER_TOGETHER_TEX_MTL_CUNT; idx++)
    {
        res = better_together_tex_mem_rd(list.entries + idx * sizeof(entry), &entry, sizeof(entry));
        if (res == LDG_ERR_AOK) { res = better_together_tex_mem_rd(entry + BETTER_TOGETHER_TEX_MTL_PARAM_OFF, &blk, sizeof(blk)); }

        if (res == LDG_ERR_AOK) { res = better_together_tex_mem_rd(blk, &bytes, sizeof(bytes)); }

        if (res == LDG_ERR_AOK) { res = better_together_tex_mem_rd(blk + BETTER_TOGETHER_TEX_PARAM_LIST_OFF, &params, sizeof(params)); }

        if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

        params.cunt &= BETTER_TOGETHER_TEX_LIST_CUNT_MASK;
        if (LDG_UNLIKELY(!params.cunt || params.cunt > BETTER_TOGETHER_TEX_PARAM_CAP || !params.entries || bytes > BETTER_TOGETHER_TEX_PARAM_BYTES_MAX)) { return LDG_ERR_INVALID; }

        matched = LDG_TRUTH_FALSE;
        for (param_idx = 0; param_idx < params.cunt; param_idx++)
        {
            res = better_together_tex_mem_rd(params.entries + param_idx * sizeof(param), &param, sizeof(param));
            if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

            if ((param.size & BETTER_TOGETHER_TEX_PARAM_SIZE_MASK) != sizeof(better_together_tex_binding_t)) { continue; }

            res = better_together_tex_mem_rd(param.name & ~BETTER_TOGETHER_TEX_NAME_TAG_BIT, name, sizeof(name));
            if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

            for (ch = 0; ch < sizeof(name) && name[ch] == BETTER_TOGETHER_TEX_NAME[ch]; ch++) {}
            if (ch != sizeof(name)) { continue; }

            if (LDG_UNLIKELY(param.off < sizeof(bytes) || (uint32_t)param.off + sizeof(better_together_tex_binding_t) > bytes)) { return LDG_ERR_BOUNDS; }

            res = better_together_tex_mem_rd(blk + param.off, &out[idx], sizeof(out[idx]));
            if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

            if (LDG_UNLIKELY(out[idx].type != BETTER_TOGETHER_TEX_BIND_TYPE || !out[idx].resource)) { return LDG_ERR_UNSUPPORTED; }

            matched = LDG_TRUTH_TRUE;
            break;
        }
        if (LDG_UNLIKELY(!matched)) { return LDG_ERR_NOT_FOUND; }
    }
    return LDG_ERR_AOK;
}

static uint32_t better_together_tex_sys_get(uint64_t lua, uint64_t base, uint64_t *out)
{
    uint64_t ptr = 0;
    uint64_t range[2] = LDG_ARR_ZERO_INIT;
    uint64_t sys = 0;
    uint64_t vtable = 0;
    uint64_t idx = 0;
    uint32_t res = better_together_tex_mem_rd(lua + BETTER_TOGETHER_TEX_LUA_GLOB_OFF, &ptr, sizeof(ptr));

    *out = 0;
    if (res == LDG_ERR_AOK) { res = better_together_tex_mem_rd(ptr + BETTER_TOGETHER_TEX_LUA_USER_OFF, &ptr, sizeof(ptr)); }

    if (res == LDG_ERR_AOK) { res = better_together_tex_mem_rd(ptr + BETTER_TOGETHER_TEX_USER_BEHAVIOR_OFF, &ptr, sizeof(ptr)); }

    if (res == LDG_ERR_AOK) { res = better_together_tex_mem_rd(ptr + BETTER_TOGETHER_TEX_MNGR_OFF, &ptr, sizeof(ptr)); }

    if (res == LDG_ERR_AOK) { res = better_together_tex_mem_rd(ptr + BETTER_TOGETHER_TEX_SYS_LIST_OFF, range, sizeof(range)); }

    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(!range[0] || range[1] < range[0] || (range[1] - range[0]) % sizeof(ptr) || (range[1] - range[0]) / sizeof(ptr) > BETTER_TOGETHER_TEX_SYS_CAP)) { return LDG_ERR_INVALID; }

    for (idx = range[0]; idx < range[1]; idx += sizeof(ptr))
    {
        res = better_together_tex_mem_rd(idx, &sys, sizeof(sys));
        if (res == LDG_ERR_AOK) { res = better_together_tex_mem_rd(sys, &vtable, sizeof(vtable)); }

        if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

        if (vtable == base + BETTER_TOGETHER_TEX_SYS_VTABLE_RVA) { *out = sys; return LDG_ERR_AOK; }
    }
    return LDG_ERR_NOT_FOUND;
}

static uint32_t better_together_tex_view_get(uint64_t owner, uint32_t hash, uint64_t sys, better_together_tex_view_t *out)
{
    uint64_t entity = 0;
    uint64_t mask = 0;
    uint64_t list[2] = LDG_ARR_ZERO_INIT;
    uint64_t ptr = 0;
    uint64_t back = 0;
    uint32_t name = 0;
    uint32_t slot = 0;
    uint32_t idx = 0;
    uint32_t component[2] = LDG_ARR_ZERO_INIT;
    uint32_t range = 0;
    uint32_t res = better_together_tex_mem_rd(owner + BETTER_TOGETHER_TEX_OWNER_ENTITY_OFF, &entity, sizeof(entity));

    if (res == LDG_ERR_AOK) { res = better_together_tex_mem_rd(entity + BETTER_TOGETHER_TEX_ENTITY_HASH_OFF, &name, sizeof(name)); }

    if (res == LDG_ERR_AOK) { res = better_together_tex_mem_rd(entity + BETTER_TOGETHER_TEX_ENTITY_MASK_OFF, &mask, sizeof(mask)); }

    if (res == LDG_ERR_AOK) { res = better_together_tex_mem_rd(entity + BETTER_TOGETHER_TEX_ENTITY_LIST_OFF, list, sizeof(list)); }

    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(name != hash || !(mask & (BETTER_TOGETHER_TEX_RENDER_KIND_BIT)))) { return LDG_ERR_NOT_FOUND; }

    for (idx = 0; idx < BETTER_TOGETHER_TEX_RENDER_KIND; idx++) { slot += (uint32_t)((mask >> idx) & 1); }
    if (LDG_UNLIKELY(!list[0] || list[1] < list[0] || list[1] - list[0] < (slot + BETTER_TOGETHER_TEX_ENTRY_STEP) * sizeof(ptr))) { return LDG_ERR_BOUNDS; }

    res = better_together_tex_mem_rd(list[0] + slot * sizeof(ptr), &out->renderable, sizeof(ptr));
    if (res == LDG_ERR_AOK) { res = better_together_tex_mem_rd(out->renderable, component, sizeof(component)); }

    if (res == LDG_ERR_AOK) { res = better_together_tex_mem_rd(out->renderable + BETTER_TOGETHER_TEX_RENDER_OWNER_OFF, &back, sizeof(back)); }

    if (res == LDG_ERR_AOK) { res = better_together_tex_mem_rd(out->renderable + BETTER_TOGETHER_TEX_RENDER_INST_OFF, &out->inst, sizeof(ptr)); }

    if (res == LDG_ERR_AOK) { res = better_together_tex_mem_rd(sys + BETTER_TOGETHER_TEX_SYS_RANGE_OFF, list, sizeof(list)); }

    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(component[0] != BETTER_TOGETHER_TEX_RENDER_KIND || back != entity || !out->inst || !list[0] || list[1] < list[0] || component[1] >= (list[1] - list[0]) / sizeof(range))) { return LDG_ERR_INVALID; }

    res = better_together_tex_mem_rd(list[0] + component[1] * sizeof(range), &range, sizeof(range));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY((range >> BETTER_TOGETHER_TEX_RANGE_CUNT_SHIFT) != BETTER_TOGETHER_TEX_RANGE_EXPECTED_CUNT)) { return LDG_ERR_UNSUPPORTED; }

    res = better_together_tex_mem_rd(sys + BETTER_TOGETHER_TEX_SYS_INST_OFF, &ptr, sizeof(ptr));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    res = better_together_tex_mem_rd(ptr + (range & BETTER_TOGETHER_TEX_RANGE_FIRST_MASK) * sizeof(ptr), &ptr, sizeof(ptr));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(ptr != out->inst)) { return LDG_ERR_INVALID; }

    res = better_together_tex_mem_rd(out->inst, &out->model, sizeof(ptr));
    if (res == LDG_ERR_AOK) { res = better_together_tex_mem_rd(out->inst + BETTER_TOGETHER_TEX_INST_MTLS_OFF, &out->mtls, sizeof(ptr)); }

    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    res = better_together_tex_mtls_rd(out->model + BETTER_TOGETHER_TEX_MODEL_MTLS_OFF, out->orig);
    if (res == LDG_ERR_AOK) { res = better_together_tex_mtls_rd(out->mtls, out->current); }

    return res;
}

static uint32_t better_together_tex_endpoint_val_get(const uint8_t *src, double *out)
{
    uint32_t packed = 0;
    double red = 0x0;
    double green = 0x0;
    double blue = 0x0;

    if (LDG_UNLIKELY(!src || !out)) { return LDG_ERR_FUNC_ARG_NULL; }

    *out = 0;
    packed = (uint32_t)src[0] | ((uint32_t)src[1] << LDG_BYTE_BITS);
    red = (double)(packed >> BETTER_TOGETHER_TEX_RED_SHIFT) / BETTER_TOGETHER_TEX_RB_MAX;
    green = (double)((packed >> BETTER_TOGETHER_TEX_GREEN_SHIFT) & BETTER_TOGETHER_TEX_G_MAX) / BETTER_TOGETHER_TEX_G_MAX;
    blue = (double)(packed & BETTER_TOGETHER_TEX_RB_MAX) / BETTER_TOGETHER_TEX_RB_MAX;
    *out = red > green ? (red > blue ? red : blue) : (green > blue ? green : blue);
    return LDG_ERR_AOK;
}

static uint32_t better_together_tex_pixels_recolor(const uint8_t *pixels, uint8_t *out, uint64_t len, const uint8_t *rgb)
{
    uint64_t off = 0;
    uint32_t endpoint = 0;
    uint32_t packed = 0;
    uint8_t *dst = 0x0;
    double peak = 0x0;
    double val = 0x0;
    uint32_t res = LDG_ERR_AOK;

    if (LDG_UNLIKELY(!pixels || !out || !rgb)) { return LDG_ERR_FUNC_ARG_NULL; }

    if (LDG_UNLIKELY(!len || len % BETTER_TOGETHER_TEX_BLK_BYTES)) { return LDG_ERR_FUNC_ARG_INVALID; }

    for (off = 0; off < len; off += BETTER_TOGETHER_TEX_BLK_BYTES) { for (endpoint = 0; endpoint < BETTER_TOGETHER_TEX_ENDPOINT_CUNT; endpoint++)
        {
            res = better_together_tex_endpoint_val_get(pixels + off + BETTER_TOGETHER_TEX_ENDPOINT_OFF + endpoint * sizeof(uint16_t), &val);
            if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

            if (val > peak) { peak = val; }
        }
    }
    if (LDG_UNLIKELY(peak <= 0.0)) { return LDG_ERR_INVALID; }

    res = ldg_mem_secure_cpy(out, pixels, len);
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    for (off = 0; off < len; off += BETTER_TOGETHER_TEX_BLK_BYTES)
    {
        for (endpoint = 0; endpoint < BETTER_TOGETHER_TEX_ENDPOINT_CUNT; endpoint++)
        {
            dst = out + off + BETTER_TOGETHER_TEX_ENDPOINT_OFF + endpoint * sizeof(uint16_t);
            res = better_together_tex_endpoint_val_get(dst, &val);
            if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

            val /= peak;
            packed = (uint32_t)((double)rgb[0] / LDG_BYTE_MASK * val * BETTER_TOGETHER_TEX_RB_MAX + BETTER_TOGETHER_TEX_QUANTIZE_ROUND) << BETTER_TOGETHER_TEX_RED_SHIFT;
            packed |= (uint32_t)((double)rgb[1] / LDG_BYTE_MASK * val * BETTER_TOGETHER_TEX_G_MAX + BETTER_TOGETHER_TEX_QUANTIZE_ROUND) << BETTER_TOGETHER_TEX_GREEN_SHIFT;
            packed |= (uint32_t)((double)rgb[BETTER_TOGETHER_TEX_BLUE_IDX] / LDG_BYTE_MASK * val * BETTER_TOGETHER_TEX_RB_MAX + BETTER_TOGETHER_TEX_QUANTIZE_ROUND);
            dst[0] = (uint8_t)(packed & LDG_BYTE_MASK);
            dst[1] = (uint8_t)(packed >> LDG_BYTE_BITS);
        }
    }
    return LDG_ERR_AOK;
}

static uint32_t better_together_tex_rows_copy(better_together_tex_ctx_t *ctx, const D3D11_MAPPED_SUBRESOURCE *mapped)
{
    uint32_t row = 0;
    uint32_t res = LDG_ERR_AOK;

    if (LDG_UNLIKELY(!mapped->pData || mapped->RowPitch < BETTER_TOGETHER_TEX_ROW_BYTES)) { return LDG_ERR_INVALID; }

    for (row = 0; row < BETTER_TOGETHER_TEX_ROWS; row++)
    {
        res = better_together_tex_mem_rd((uint64_t)(uintptr_t)mapped->pData + (uint64_t)row * mapped->RowPitch, ctx->pixels + row * BETTER_TOGETHER_TEX_ROW_BYTES, BETTER_TOGETHER_TEX_ROW_BYTES);
        if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }
    }
    return LDG_ERR_AOK;
}

static uint32_t better_together_tex_pixels_rd(better_together_tex_ctx_t *ctx, ID3D11Texture2D *src)
{
    ID3D11Device *dev = 0x0;
    ID3D11DeviceContext *imm = 0x0;
    ID3D11Texture2D *staging = 0x0;
    D3D11_TEXTURE2D_DESC desc = LDG_STRUCT_ZERO_INIT;
    D3D11_MAPPED_SUBRESOURCE mapped = LDG_STRUCT_ZERO_INIT;
    uint32_t native_res = 0;
    uint32_t refs = 0;
    uint32_t res = LDG_ERR_AOK;

    src->lpVtbl->GetDesc(src, &desc);
    if (LDG_UNLIKELY(desc.Width != BETTER_TOGETHER_TEX_WIDTH || desc.Height != BETTER_TOGETHER_TEX_HEIGHT || desc.ArraySize != BETTER_TOGETHER_TEX_ARR_CUNT || desc.Format != DXGI_FORMAT_BC3_UNORM || desc.SampleDesc.Count != BETTER_TOGETHER_TEX_SAMPLE_CUNT || desc.SampleDesc.Quality || !desc.MipLevels)) { return LDG_ERR_UNSUPPORTED; }

    src->lpVtbl->GetDevice(src, &dev);
    if (LDG_UNLIKELY(!dev)) { return LDG_ERR_NOT_INIT; }

    dev->lpVtbl->GetImmediateContext(dev, &imm);
    if (!imm) { refs = dev->lpVtbl->Release(dev); return refs == UINT32_MAX ? LDG_ERR_INVALID : LDG_ERR_NOT_INIT; }

    desc.Usage = D3D11_USAGE_STAGING;
    desc.BindFlags = 0;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    desc.MiscFlags = 0;
    native_res = (uint32_t)dev->lpVtbl->CreateTexture2D(dev, &desc, 0x0, &staging);
    if ((native_res & BETTER_TOGETHER_TEX_HRESULT_FAILED) || !staging) { res = LDG_ERR_ALLOC_NULL; }

    if (res == LDG_ERR_AOK)
    {
        imm->lpVtbl->CopyResource(imm, (ID3D11Resource *)staging, (ID3D11Resource *)src);
        native_res = (uint32_t)imm->lpVtbl->Map(imm, (ID3D11Resource *)staging, BETTER_TOGETHER_TEX_SUBRESOURCE, D3D11_MAP_READ, BETTER_TOGETHER_TEX_MAP_FLAGS, &mapped);
        if (native_res & BETTER_TOGETHER_TEX_HRESULT_FAILED) { res = LDG_ERR_IO_RD; }
        else
        {
            res = better_together_tex_rows_copy(ctx, &mapped);
            imm->lpVtbl->Unmap(imm, (ID3D11Resource *)staging, BETTER_TOGETHER_TEX_SUBRESOURCE);
        }
    }

    if (staging) { refs = staging->lpVtbl->Release(staging); if (refs == UINT32_MAX) { res = LDG_ERR_INVALID; } }

    refs = imm->lpVtbl->Release(imm);
    if (refs == UINT32_MAX) { res = LDG_ERR_INVALID; }

    if (res == LDG_ERR_AOK) { ctx->dev = dev; }
    else { refs = dev->lpVtbl->Release(dev); if (refs == UINT32_MAX) { res = LDG_ERR_INVALID; } }

    return res;
}

static uint32_t better_together_tex_color_create(better_together_tex_ctx_t *ctx, better_together_tex_record_t *out, const uint8_t *rgb)
{
    ID3D11Device *dev = 0x0;
    uint32_t refs = 0;
    uint32_t res = better_together_tex_pixels_recolor(ctx->pixels, ctx->recolored, sizeof(ctx->pixels), rgb);

    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    out->fmt = ctx->base + BETTER_TOGETHER_TEX_FMT_RVA;
    out->width = BETTER_TOGETHER_TEX_WIDTH;
    out->height = BETTER_TOGETHER_TEX_HEIGHT;
    out->gpu_cunt = LDG_TRUTH_TRUE;
    ctx->create_res = ctx->create(out, ctx->recolored, LDG_TRUTH_TRUE, LDG_TRUTH_FALSE, LDG_TRUTH_FALSE, 0x0);
    if (LDG_UNLIKELY(ctx->create_res || !out->gpu.tex || !out->gpu.srv)) { return LDG_ERR_NOT_INIT; }

    out->gpu.tex->lpVtbl->GetDevice(out->gpu.tex, &dev);
    res = dev && dev == ctx->dev ? LDG_ERR_AOK : LDG_ERR_UNSUPPORTED;
    if (dev) { refs = dev->lpVtbl->Release(dev); if (refs == UINT32_MAX) { res = LDG_ERR_INVALID; } }

    return res;
}

static uint32_t better_together_tex_pair_create(better_together_tex_ctx_t *ctx, uint64_t orig)
{
    better_together_tex_record_t record = LDG_STRUCT_ZERO_INIT;
    ID3D11Device *dev = 0x0;
    uint32_t refs = 0;
    uint32_t res = better_together_tex_mem_rd(orig, &record, sizeof(record));
    const uint8_t grass[BETTER_TOGETHER_TEX_VEC_AXES] = { BETTER_TOGETHER_TEX_GRASS_RED, BETTER_TOGETHER_TEX_GRASS_GREEN, BETTER_TOGETHER_TEX_GRASS_BLUE };
    const uint8_t sky[BETTER_TOGETHER_TEX_VEC_AXES] = { BETTER_TOGETHER_TEX_SKY_RED, BETTER_TOGETHER_TEX_SKY_GREEN, BETTER_TOGETHER_TEX_SKY_BLUE };

    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(record.fmt != ctx->base + BETTER_TOGETHER_TEX_FMT_RVA || record.width != BETTER_TOGETHER_TEX_WIDTH || record.height != BETTER_TOGETHER_TEX_HEIGHT || (record.gpu_cunt & BETTER_TOGETHER_TEX_LIST_CUNT_MASK) != LDG_TRUTH_TRUE || !record.gpu.tex)) { return LDG_ERR_UNSUPPORTED; }

    if (ctx->created)
    {
        record.gpu.tex->lpVtbl->GetDevice(record.gpu.tex, &dev);
        res = dev && dev == ctx->dev ? LDG_ERR_AOK : LDG_ERR_UNSUPPORTED;
        if (dev) { refs = dev->lpVtbl->Release(dev); if (refs == UINT32_MAX) { res = LDG_ERR_INVALID; } }

        return res;
    }

    res = better_together_tex_pixels_rd(ctx, record.gpu.tex);
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    ctx->attempted = LDG_TRUTH_TRUE;
    res = better_together_tex_color_create(ctx, &ctx->green, grass);
    if (res == LDG_ERR_AOK) { res = better_together_tex_color_create(ctx, &ctx->sky, sky); }

    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    ctx->created = LDG_TRUTH_TRUE;
    return LDG_ERR_AOK;
}

static uint32_t better_together_tex_view_validate(const better_together_tex_view_t *view, const better_together_tex_view_t *other)
{
    uint8_t private = 0;
    uint32_t res = LDG_ERR_AOK;

    if (LDG_UNLIKELY(view->inst == other->inst || view->renderable == other->renderable)) { return LDG_ERR_DENIED; }

    res = better_together_tex_mem_rd(view->renderable + BETTER_TOGETHER_TEX_RENDER_PRIVATE_OFF, &private, sizeof(private));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(private && (view->mtls == other->mtls || view->mtls == view->model + BETTER_TOGETHER_TEX_MODEL_MTLS_OFF || view->mtls == other->model + BETTER_TOGETHER_TEX_MODEL_MTLS_OFF))) { return LDG_ERR_DENIED; }

    return LDG_ERR_AOK;
}

static uint32_t better_together_tex_yarn_apply(better_together_tex_ctx_t *ctx, uint64_t sys, const better_together_tex_view_t *view, const better_together_tex_record_t *record)
{
    better_together_tex_binding_t binding = LDG_STRUCT_ZERO_INIT;
    uint32_t idx = 0;
    const uint32_t neutral[BETTER_TOGETHER_TEX_VEC_AXES] = { BETTER_TOGETHER_TEX_UNIT_BITS, BETTER_TOGETHER_TEX_UNIT_BITS, BETTER_TOGETHER_TEX_UNIT_BITS };
    const uint32_t hue[BETTER_TOGETHER_TEX_VEC_AXES] = LDG_ARR_ZERO_INIT;

    for (idx = 0; idx < BETTER_TOGETHER_TEX_YARN_CUNT; idx++)
    {
        ctx->param(sys, view->renderable, ctx->tint_hash, neutral, idx);
        ctx->param(sys, view->renderable, ctx->hue_hash, hue, idx);
        binding = view->orig[idx];
        binding.resource = (uint64_t)(uintptr_t)record;
        ctx->bind(sys, view->renderable, BETTER_TOGETHER_TEX_DIFFUSE_HASH, &binding, idx);
    }
    return LDG_ERR_AOK;
}

static uint32_t better_together_tex_applied_validate(const better_together_tex_view_t *view, const better_together_tex_record_t *record)
{
    better_together_tex_binding_t binding = LDG_STRUCT_ZERO_INIT;
    better_together_tex_binding_t current[BETTER_TOGETHER_TEX_MTL_CUNT] = LDG_ARR_ZERO_INIT;
    uint64_t mtls = 0;
    uint32_t idx = 0;
    uint32_t res = better_together_tex_mem_rd(view->inst + BETTER_TOGETHER_TEX_INST_MTLS_OFF, &mtls, sizeof(mtls));

    if (res == LDG_ERR_AOK) { res = better_together_tex_mtls_rd(mtls, current); }

    for (idx = 0; res == LDG_ERR_AOK && idx < BETTER_TOGETHER_TEX_MTL_CUNT; idx++)
    {
        binding = idx < BETTER_TOGETHER_TEX_YARN_CUNT ? view->orig[idx] : view->current[idx];
        if (idx < BETTER_TOGETHER_TEX_YARN_CUNT) { binding.resource = (uint64_t)(uintptr_t)record; }

        res = better_together_tex_binding_cmp(&current[idx], &binding);
    }
    return res;
}

static uint32_t better_together_tex_apply(better_together_tex_ctx_t *ctx, uint64_t sys, const better_together_tex_view_t *host, const better_together_tex_view_t *guest)
{
    uint32_t res = better_together_tex_view_validate(host, guest);

    if (res == LDG_ERR_AOK) { res = better_together_tex_view_validate(guest, host); }

    if (res == LDG_ERR_AOK) { res = better_together_tex_yarn_apply(ctx, sys, guest, &ctx->green); }

    if (res == LDG_ERR_AOK) { res = better_together_tex_yarn_apply(ctx, sys, host, &ctx->sky); }

    if (res == LDG_ERR_AOK) { res = better_together_tex_applied_validate(guest, &ctx->green); }

    if (res == LDG_ERR_AOK) { res = better_together_tex_applied_validate(host, &ctx->sky); }

    return res;
}

uint64_t better_together_tex_size_get(void)
{
    return sizeof(better_together_tex_ctx_t);
}

uint32_t better_together_tex_init(void *storage, uint64_t cap, uint64_t base)
{
    better_together_tex_ctx_t *ctx = (better_together_tex_ctx_t *)storage;
    const uint8_t create[BETTER_TOGETHER_TEX_PROFILE_BYTES] = { 0x55, 0x53, 0x56, 0x57, 0x41, 0x54, 0x41, 0x55, 0x41, 0x56, 0x41, 0x57 };
    const uint8_t bind[BETTER_TOGETHER_TEX_PROFILE_BYTES] = { 0x49, 0x89, 0xE3, 0x4D, 0x89, 0x4B, 0x20, 0x49, 0x89, 0x53, 0x10, 0x49 };
    uint8_t bytes[BETTER_TOGETHER_TEX_PROFILE_BYTES] = LDG_ARR_ZERO_INIT;
    uint64_t name[2] = LDG_ARR_ZERO_INIT;
    better_together_tex_hash_func_t hash = 0x0;
    uint32_t idx = 0;
    uint32_t res = LDG_ERR_AOK;

    if (LDG_UNLIKELY(!ctx)) { return LDG_ERR_FUNC_ARG_NULL; }

    if (LDG_UNLIKELY(cap < sizeof(*ctx) || ((uintptr_t)storage & (BETTER_TOGETHER_TEX_QWORD_ALIGN_MASK)) || !base || sizeof(better_together_tex_record_t) != BETTER_TOGETHER_TEX_RECORD_BYTES)) { return LDG_ERR_FUNC_ARG_INVALID; }

    res = ldg_mem_secure_zero(ctx, sizeof(*ctx));
    if (res == LDG_ERR_AOK) { res = better_together_tex_mem_rd(base + BETTER_TOGETHER_TEX_CREATE_RVA, bytes, sizeof(bytes)); }

    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    for (idx = 0; idx < sizeof(bytes); idx++) { if (LDG_UNLIKELY(bytes[idx] != create[idx])) { return LDG_ERR_UNSUPPORTED; } }
    res = better_together_tex_mem_rd(base + BETTER_TOGETHER_TEX_BIND_RVA, bytes, sizeof(bytes));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    for (idx = 0; idx < sizeof(bytes); idx++) { if (LDG_UNLIKELY(bytes[idx] != bind[idx])) { return LDG_ERR_UNSUPPORTED; } }
    res = better_together_tex_mem_rd(base + BETTER_TOGETHER_TEX_PARAM_RVA, bytes, sizeof(bytes));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    for (idx = 0; idx < sizeof(bytes); idx++) { if (LDG_UNLIKELY(bytes[idx] != bind[idx])) { return LDG_ERR_UNSUPPORTED; } }
    ctx->base = base;
    ctx->create = (better_together_tex_create_func_t)(uintptr_t)(base + BETTER_TOGETHER_TEX_CREATE_RVA);
    ctx->bind = (better_together_tex_bind_func_t)(uintptr_t)(base + BETTER_TOGETHER_TEX_BIND_RVA);
    ctx->param = (better_together_tex_param_func_t)(uintptr_t)(base + BETTER_TOGETHER_TEX_PARAM_RVA);
    hash = (better_together_tex_hash_func_t)(uintptr_t)(base + BETTER_TOGETHER_TEX_HASH_RVA);
    name[0] = (uint64_t)(uintptr_t)BETTER_TOGETHER_TEX_TINT_NAME;
    name[1] = sizeof(BETTER_TOGETHER_TEX_TINT_NAME) - LDG_STR_TERM_SIZE;
    ctx->tint_hash = hash(name);
    name[0] = (uint64_t)(uintptr_t)BETTER_TOGETHER_TEX_HUE_NAME;
    name[1] = sizeof(BETTER_TOGETHER_TEX_HUE_NAME) - LDG_STR_TERM_SIZE;
    ctx->hue_hash = hash(name);
    return LDG_ERR_AOK;
}

uint32_t better_together_tex_tick(void *storage, uint64_t lua, uint64_t host, uint64_t guest, uint32_t epoch, better_together_tex_report_t *out)
{
    better_together_tex_ctx_t *ctx = (better_together_tex_ctx_t *)storage;
    better_together_tex_view_t host_view = LDG_STRUCT_ZERO_INIT;
    better_together_tex_view_t guest_view = LDG_STRUCT_ZERO_INIT;
    uint64_t sys = 0;
    uint64_t now = GetTickCount64();
    uint32_t res = LDG_ERR_AOK;

    if (LDG_UNLIKELY(!ctx || !out)) { return LDG_ERR_FUNC_ARG_NULL; }

    *out = (better_together_tex_report_t)LDG_STRUCT_ZERO_INIT;
    if (LDG_UNLIKELY(!ctx->base || !ctx->create || !ctx->bind || !ctx->param)) { return LDG_ERR_NOT_INIT; }

    if (ctx->busy) { *out = ctx->report; return LDG_ERR_BUSY; }

    if (ctx->epoch != epoch) { ctx->epoch = epoch; ctx->next = 0; ctx->retries = 0; ctx->report.applied = 0; }

    if (ctx->report.applied || now < ctx->next || ctx->retries >= BETTER_TOGETHER_TEX_RETRY_CAP || (ctx->attempted && !ctx->created)) { *out = ctx->report; return LDG_ERR_AOK; }

    ctx->busy = LDG_TRUTH_TRUE;
    ctx->next = now + BETTER_TOGETHER_TEX_RETRY_MS;
    ctx->report.attempts++;
    ctx->retries++;
    res = better_together_tex_sys_get(lua, ctx->base, &sys);
    if (res == LDG_ERR_AOK) { res = better_together_tex_view_get(host, BETTER_TOGETHER_YARNY_HASH, sys, &host_view); }

    if (res == LDG_ERR_AOK) { res = better_together_tex_view_get(guest, BETTER_TOGETHER_BOXIE_HASH, sys, &guest_view); }

    if (res == LDG_ERR_AOK && (guest_view.orig[0].resource != guest_view.orig[1].resource || guest_view.orig[BETTER_TOGETHER_TEX_YARN_CUNT].resource == guest_view.orig[0].resource || guest_view.orig[BETTER_TOGETHER_TEX_MTL_LAST_IDX].resource != guest_view.orig[BETTER_TOGETHER_TEX_YARN_CUNT].resource)) { res = LDG_ERR_UNSUPPORTED; }

    if (res == LDG_ERR_AOK && (host_view.orig[0].resource != guest_view.orig[0].resource || host_view.orig[1].resource != host_view.orig[0].resource || host_view.orig[BETTER_TOGETHER_TEX_YARN_CUNT].resource == host_view.orig[0].resource || host_view.orig[BETTER_TOGETHER_TEX_MTL_LAST_IDX].resource != host_view.orig[BETTER_TOGETHER_TEX_YARN_CUNT].resource)) { res = LDG_ERR_UNSUPPORTED; }

    if (res == LDG_ERR_AOK) { res = better_together_tex_pair_create(ctx, guest_view.orig[0].resource); }

    if (res == LDG_ERR_AOK) { res = better_together_tex_apply(ctx, sys, &host_view, &guest_view); }

    ctx->report.res = res;
    ctx->report.applied = res == LDG_ERR_AOK;
    ctx->busy = LDG_TRUTH_FALSE;
    *out = ctx->report;
    return LDG_ERR_AOK;
}
