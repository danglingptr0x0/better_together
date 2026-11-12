#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include <windows.h>

#include <MinHook.h>
#include <dangling/core/err.h>
#include <dangling/core/macros.h>
#include <dangling/mem/secure.h>
#include <dangling/str/fmt.h>
#include <libresteam/ext/together/cam.h>
#include <libresteam/ext/together/input.h>
#include <libresteam/ext/together/model.h>
#include <libresteam/ext/together/setup.h>
#include <libresteam/ext/together/tex.h>

#define BETTER_TOGETHER_INPUT_MAGIC UINT64_C(0x424F584945494E50)
#define BETTER_TOGETHER_INPUT_FLOAT 0U
#define BETTER_TOGETHER_INPUT_BOOL 1U
#define BETTER_TOGETHER_INPUT_PRESSED 2U
#define BETTER_TOGETHER_INPUT_FUNC_OFF 0x20U
#define BETTER_TOGETHER_INPUT_OWNER_ENTITY_OFF 0x20U
#define BETTER_TOGETHER_INPUT_ENTITY_HASH_OFF 0x08U
#define BETTER_TOGETHER_INPUT_ENTITY_GENERATION_OFF 0x18U
#define BETTER_TOGETHER_INPUT_ENTITY_GENERATION_MASK 0x7FFFFFFFU
#define BETTER_TOGETHER_INPUT_HANDLE_GENERATION_SHIFT 48U
#define BETTER_TOGETHER_INPUT_HANDLE_OFF 0x48U
#define BETTER_TOGETHER_INPUT_HANDLE_MASK UINT64_C(0xFFFFFFFFFFFF)
#define BETTER_TOGETHER_INPUT_LUA_TOP_OFF 0x10U
#define BETTER_TOGETHER_INPUT_LUA_BASE_OFF 0x18U
#define BETTER_TOGETHER_INPUT_LUA_CI_OFF 0x28U
#define BETTER_TOGETHER_INPUT_LUA_BASE_CI_OFF 0x50U
#define BETTER_TOGETHER_INPUT_CI_BYTES 0x28U
#define BETTER_TOGETHER_INPUT_VAL_BYTES 0x10U
#define BETTER_TOGETHER_INPUT_TAG_NUMBER 3U
#define BETTER_TOGETHER_INPUT_TAG_FUNC 6U
#define BETTER_TOGETHER_INPUT_TAG_USERDATA 7U
#define BETTER_TOGETHER_INPUT_TAG_LIGHT_USERDATA 2U
#define BETTER_TOGETHER_INPUT_CLOSURE_UPVALUES 2U
#define BETTER_TOGETHER_INPUT_FLOAT_RVA 0x17AFE50U
#define BETTER_TOGETHER_INPUT_BOOL_RVA 0x17B0050U
#define BETTER_TOGETHER_INPUT_BOOL_TGT_RVA 0x17AF610U
#define BETTER_TOGETHER_INPUT_PRESSED_TGT_RVA 0x17AF7F0U
#define BETTER_TOGETHER_INPUT_FLOAT_VTABLE_RVA 0x6BB898U
#define BETTER_TOGETHER_INPUT_BOOL_VTABLE_RVA 0x6BB8D8U
#define BETTER_TOGETHER_INPUT_PUSH_NUMBER_RVA 0x1936D90U
#define BETTER_TOGETHER_INPUT_PUSH_BOOL_RVA 0x1936B50U
#define BETTER_TOGETHER_INPUT_HASH_RVA 0x1796D30U
#define BETTER_TOGETHER_INPUT_PE_TIMESTAMP 0x56952868U
#define BETTER_TOGETHER_INPUT_IMG_BYTES 0x4D13000U
#define BETTER_TOGETHER_INPUT_CODE_SLOT_BYTES 128U
#define BETTER_TOGETHER_INPUT_SCAN_BYTES (64U * LDG_KIB)
#define BETTER_TOGETHER_INPUT_SCAN_BUDGET (2U * LDG_MIB)
#define BETTER_TOGETHER_INPUT_REGION_BUDGET 128U
#define BETTER_TOGETHER_INPUT_TIMEOUT_MS 250U
#define BETTER_TOGETHER_INPUT_ACTION_NAME_CAP 16U
#define BETTER_TOGETHER_INPUT_FUNC_SLOT_CUNT 1U
#define BETTER_TOGETHER_INPUT_LUA_RES_CUNT 1U
#define BETTER_TOGETHER_INPUT_QWORD_ALIGN_MASK (sizeof(uint64_t) - 1U)
#define BETTER_TOGETHER_INPUT_THRESHOLD 0.5
#define BETTER_TOGETHER_INPUT_ARG_CUNT 2U
#define BETTER_TOGETHER_INPUT_FRAME_RVA 0x1474E80U
#define BETTER_TOGETHER_INPUT_GLOBAL_RVA 0x8D3AB8U
#define BETTER_TOGETHER_INPUT_GAME_OWNER_OFF 0x358U
#define BETTER_TOGETHER_INPUT_HOST 0U
#define BETTER_TOGETHER_INPUT_GUEST 1U
#define BETTER_TOGETHER_INPUT_PCALL_RVA 0x1936AB0U
#define BETTER_TOGETHER_INPUT_PCALL_SLOT (BETTER_TOGETHER_INPUT_QUERY_CUNT + 1U)
#define BETTER_TOGETHER_INPUT_CODE_CUNT (BETTER_TOGETHER_INPUT_PCALL_SLOT + 1U)
#define BETTER_TOGETHER_INPUT_THUNK_CTX_REG 0U
#define BETTER_TOGETHER_INPUT_THUNK_CTX_STACK 1U
#define BETTER_TOGETHER_INPUT_ENTITY_ID_OFF 0x1CU
#define BETTER_TOGETHER_INPUT_CAM_SRC_CAP (32U * LDG_KIB)
#define BETTER_TOGETHER_INPUT_CAM_SIG_BYTES 13U
#define BETTER_TOGETHER_INPUT_CAM_API_CUNT 5U
#define BETTER_TOGETHER_INPUT_CAM_RES_CUNT 4U
#define BETTER_TOGETHER_INPUT_CAM_STATE_IDX 1U
#define BETTER_TOGETHER_INPUT_CAM_DIST_IDX 2U
#define BETTER_TOGETHER_INPUT_CAM_ATTACH_IDX 3U
#define BETTER_TOGETHER_INPUT_CAM_ATTACH_ERR_IDX 4U
#define BETTER_TOGETHER_INPUT_CAM_NO_ARGS 0U
#define BETTER_TOGETHER_INPUT_CAM_NO_RES 0U
#define BETTER_TOGETHER_INPUT_CAM_NO_HANDLER 0U
#define BETTER_TOGETHER_INPUT_CAM_TOP_ERR_IDX UINT32_MAX
#define BETTER_TOGETHER_INPUT_CAM_UNKNOWN_DIST -1.0
#define BETTER_TOGETHER_INPUT_CAM_CHUNK "@together-camera"
#define BETTER_TOGETHER_INPUT_CAM_STOP_CHUNK "@together-camera-stop"
#define BETTER_TOGETHER_INPUT_CAM_ACTIVE 1U
#define BETTER_TOGETHER_INPUT_CAM_FAILED 2U
#define BETTER_TOGETHER_INPUT_CAM_GETTOP_RVA 0x1936650U
#define BETTER_TOGETHER_INPUT_CAM_SETTOP_RVA 0x1937480U
#define BETTER_TOGETHER_INPUT_CAM_LD_RVA 0x1938A40U
#define BETTER_TOGETHER_INPUT_CAM_NUMBER_RVA 0x1937760U
#define BETTER_TOGETHER_INPUT_CAM_STR_RVA 0x19376B0U

typedef uint32_t (*better_together_input_lua_func_t)(uint64_t lua);
typedef void (*better_together_input_number_func_t)(uint64_t lua, double val);
typedef void (*better_together_input_bool_func_t)(uint64_t lua, uint32_t val);
typedef uint32_t (*better_together_input_hash_func_t)(const uint64_t *name);
typedef double better_together_input_native_dt_t __attribute__((mode(SF)));
typedef uint64_t (*better_together_input_frame_func_t)(uint64_t game, better_together_input_native_dt_t dt, uint64_t arg);
typedef uint32_t (*better_together_input_pcall_func_t)(uint64_t lua, uint32_t args, uint32_t res_cunt, uint32_t handler);
typedef uint32_t (*better_together_input_cam_top_func_t)(uint64_t lua);
typedef void (*better_together_input_cam_top_set_func_t)(uint64_t lua, uint32_t top);
typedef uint32_t (*better_together_input_cam_ld_func_t)(uint64_t lua, const char *src, uint64_t len, const char *name);
typedef double (*better_together_input_cam_number_func_t)(uint64_t lua, uint32_t idx);
typedef const char *(*better_together_input_cam_str_func_t)(uint64_t lua, uint32_t idx, uint64_t *len);

typedef struct better_together_input_val
{
    uint64_t data;
    uint32_t tag;
    uint32_t pudding;
} better_together_input_val_t;

typedef struct better_together_input_closure
{
    uint64_t next;
    uint8_t tag;
    uint8_t marked;
    uint8_t is_c;
    uint8_t upvalues;
    uint32_t pudding;
    uint64_t gc_list;
    uint64_t env;
    uint64_t func;
    better_together_input_val_t userdata;
    better_together_input_val_t other;
} better_together_input_closure_t;

typedef struct better_together_input_userdata
{
    uint64_t next;
    uint8_t tag;
    uint8_t marked;
    uint8_t pudding[6];
    uint64_t metatable;
    uint64_t env;
    uint64_t len;
    uint64_t binding;
} better_together_input_userdata_t;

typedef struct better_together_input_binding
{
    uint64_t vtable;
    uint64_t entry;
    char name[16];
    uint64_t len;
    uint64_t cap;
    uint64_t pudding[3];
    uint64_t tgt;
} better_together_input_binding_t;

typedef struct better_together_input_ctx
{
    uint64_t magic;
    uint64_t base;
    uint64_t owner;
    uint64_t min;
    uint64_t max;
    uint64_t cursor;
    uint64_t scan_owner;
    uint64_t received;
    uint64_t pending_received;
    uint64_t orig_owner;
    uint64_t pad_received;
    uint64_t roots[BETTER_TOGETHER_PAD_PLAYER_CUNT];
    void *code;
    void *pad;
    void *tex;
    HMODULE module;
    SRWLOCK lock;
    better_together_input_spawn_cb_t spawn_cb;
    void *spawn_user;
    RUNTIME_FUNCTION unwind[BETTER_TOGETHER_INPUT_CODE_CUNT];
    better_together_input_lua_func_t orig[BETTER_TOGETHER_INPUT_QUERY_CUNT];
    better_together_input_number_func_t number;
    better_together_input_bool_func_t boolean;
    better_together_input_frame_func_t frame;
    better_together_input_pcall_func_t pcall;
    better_together_pad_sample_t pad_samples[BETTER_TOGETHER_PAD_PLAYER_CUNT];
    uint32_t actions[BETTER_TOGETHER_PAD_ACTION_CUNT];
    better_together_input_packet_t packet;
    better_together_input_packet_t pending;
    better_together_input_report_t report;
    uint32_t thread;
    uint8_t active;
    uint8_t unwind_up;
    uint8_t external;
    uint8_t cam_busy;
    uint64_t cam_next;
    uint64_t cam_owner;
    uint64_t cam_lua;
    uint32_t cam_epoch;
    uint32_t cam_seen_epoch;
    uint32_t cam_thread;
    uint32_t scan_epoch;
    better_together_input_cam_top_func_t cam_top;
    better_together_input_cam_top_set_func_t cam_top_set;
    better_together_input_cam_ld_func_t cam_ld;
    better_together_input_cam_number_func_t cam_number;
    better_together_input_cam_str_func_t cam_str;
    char cam_src[BETTER_TOGETHER_INPUT_CAM_SRC_CAP];
    uint64_t scan[BETTER_TOGETHER_INPUT_SCAN_BYTES / sizeof(uint64_t)];
} better_together_input_ctx_t;

static uint32_t better_together_input_mem_rd(uint64_t addr, void *out, uint64_t len)
{
    SIZE_T copied = 0;

    if (LDG_UNLIKELY(!addr || !out || !len || addr > UINTPTR_MAX - len)) { return LDG_ERR_FUNC_ARG_INVALID; }

    if (LDG_UNLIKELY(!ReadProcessMemory(GetCurrentProcess(), (const void *)(uintptr_t)addr, out, (SIZE_T)len, &copied) || copied != len)) { return LDG_ERR_MEM_BAD; }

    return LDG_ERR_AOK;
}

static uint32_t better_together_input_pcall_owner_get(uint64_t lua, uint32_t args, const uint64_t *roots, uint32_t *player)
{
    better_together_input_val_t lua_val = LDG_STRUCT_ZERO_INIT;
    better_together_input_userdata_t userdata = LDG_STRUCT_ZERO_INIT;
    uint64_t top = 0;
    uint64_t base = 0;
    uint64_t bytes = ((uint64_t)args + BETTER_TOGETHER_INPUT_FUNC_SLOT_CUNT) * BETTER_TOGETHER_INPUT_VAL_BYTES;
    uint64_t handle = 0;
    uint64_t entity = 0;
    uint64_t expected = 0;
    uint32_t hash = 0;
    uint32_t generation = 0;
    uint32_t idx = 0;
    uint32_t res = LDG_ERR_AOK;

    *player = UINT32_MAX;
    if (LDG_UNLIKELY(!lua || !args || args > INT32_MAX || lua > UINTPTR_MAX - BETTER_TOGETHER_INPUT_LUA_BASE_OFF - sizeof(base))) { return LDG_ERR_NOT_FOUND; }

    res = better_together_input_mem_rd(lua + BETTER_TOGETHER_INPUT_LUA_TOP_OFF, &top, sizeof(top));
    if (res == LDG_ERR_AOK) { res = better_together_input_mem_rd(lua + BETTER_TOGETHER_INPUT_LUA_BASE_OFF, &base, sizeof(base)); }

    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(!base || top < base || top - base < bytes || ((top - base) % BETTER_TOGETHER_INPUT_VAL_BYTES))) { return LDG_ERR_NOT_FOUND; }

    res = better_together_input_mem_rd(top - bytes, &lua_val, sizeof(lua_val));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(lua_val.tag != BETTER_TOGETHER_INPUT_TAG_FUNC)) { return LDG_ERR_NOT_FOUND; }

    res = better_together_input_mem_rd(top - bytes + BETTER_TOGETHER_INPUT_VAL_BYTES, &lua_val, sizeof(lua_val));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(lua_val.tag != BETTER_TOGETHER_INPUT_TAG_USERDATA || !lua_val.data || lua_val.data > UINTPTR_MAX - BETTER_TOGETHER_INPUT_HANDLE_OFF - sizeof(handle))) { return LDG_ERR_NOT_FOUND; }

    res = better_together_input_mem_rd(lua_val.data, &userdata, sizeof(userdata));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(userdata.tag != BETTER_TOGETHER_INPUT_TAG_USERDATA || userdata.len < BETTER_TOGETHER_INPUT_HANDLE_OFF + sizeof(handle) - offsetof(better_together_input_userdata_t, binding))) { return LDG_ERR_NOT_FOUND; }

    res = better_together_input_mem_rd(lua_val.data + BETTER_TOGETHER_INPUT_HANDLE_OFF, &handle, sizeof(handle));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    entity = handle & BETTER_TOGETHER_INPUT_HANDLE_MASK;
    if (LDG_UNLIKELY(!entity || entity > UINTPTR_MAX - BETTER_TOGETHER_INPUT_ENTITY_GENERATION_OFF - sizeof(generation))) { return LDG_ERR_NOT_FOUND; }

    for (idx = 0; idx < BETTER_TOGETHER_PAD_PLAYER_CUNT; idx++)
    {
        if (!roots[idx] || roots[idx] > UINTPTR_MAX - BETTER_TOGETHER_INPUT_OWNER_ENTITY_OFF - sizeof(expected)) { continue; }

        res = better_together_input_mem_rd(roots[idx] + BETTER_TOGETHER_INPUT_OWNER_ENTITY_OFF, &expected, sizeof(expected));
        if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

        if (entity != expected) { continue; }

        res = better_together_input_mem_rd(entity + BETTER_TOGETHER_INPUT_ENTITY_HASH_OFF, &hash, sizeof(hash));
        if (res == LDG_ERR_AOK) { res = better_together_input_mem_rd(entity + BETTER_TOGETHER_INPUT_ENTITY_GENERATION_OFF, &generation, sizeof(generation)); }

        if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

        if (LDG_UNLIKELY(hash != (idx == BETTER_TOGETHER_INPUT_HOST ? BETTER_TOGETHER_YARNY_HASH : BETTER_TOGETHER_BOXIE_HASH) || (handle >> BETTER_TOGETHER_INPUT_HANDLE_GENERATION_SHIFT) != (generation & BETTER_TOGETHER_INPUT_ENTITY_GENERATION_MASK))) { return LDG_ERR_NOT_FOUND; }

        *player = idx;
        return LDG_ERR_AOK;
    }
    return LDG_ERR_NOT_FOUND;
}

static uint32_t better_together_input_cam_init(better_together_input_ctx_t *ctx)
{
    const uint32_t rvas[BETTER_TOGETHER_INPUT_CAM_API_CUNT] = { BETTER_TOGETHER_INPUT_CAM_GETTOP_RVA, BETTER_TOGETHER_INPUT_CAM_SETTOP_RVA, BETTER_TOGETHER_INPUT_CAM_LD_RVA, BETTER_TOGETHER_INPUT_CAM_NUMBER_RVA, BETTER_TOGETHER_INPUT_CAM_STR_RVA };
    const uint8_t sig[BETTER_TOGETHER_INPUT_CAM_API_CUNT][BETTER_TOGETHER_INPUT_CAM_SIG_BYTES] = { { 0x48, 0x8B, 0x41, 0x10, 0x48, 0x2B, 0x41, 0x18, 0x48, 0xC1, 0xF8, 0x04, 0xC3 }, { 0x85, 0xD2, 0x78, 0x3E, 0x4C, 0x63, 0xC2, 0x48, 0x8B, 0x51, 0x18, 0x49, 0xC1 }, { 0x48, 0x83, 0xEC, 0x38, 0x48, 0x89, 0x54, 0x24, 0x20, 0x4C, 0x89, 0x44, 0x24 }, { 0x48, 0x83, 0xEC, 0x38, 0xE8, 0xC7, 0xE6, 0xFF, 0xFF, 0x83, 0x78, 0x08, 0x03 }, { 0x48, 0x89, 0x5C, 0x24, 0x08, 0x48, 0x89, 0x74, 0x24, 0x10, 0x57, 0x48, 0x83 } };
    uint8_t actual[BETTER_TOGETHER_INPUT_CAM_SIG_BYTES] = LDG_ARR_ZERO_INIT;
    uint32_t idx = 0;
    uint32_t res = LDG_ERR_AOK;

    for (idx = 0; idx < BETTER_TOGETHER_INPUT_CAM_API_CUNT; idx++)
    {
        res = better_together_input_mem_rd(ctx->base + rvas[idx], actual, sizeof(actual));
        if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

        if (LDG_UNLIKELY(ldg_mem_secure_eq_is(actual, sig[idx], sizeof(actual)) != LDG_TRUTH_TRUE)) { return LDG_ERR_UNSUPPORTED; }
    }
    ctx->cam_top = (better_together_input_cam_top_func_t)(uintptr_t)(ctx->base + BETTER_TOGETHER_INPUT_CAM_GETTOP_RVA);
    ctx->cam_top_set = (better_together_input_cam_top_set_func_t)(uintptr_t)(ctx->base + BETTER_TOGETHER_INPUT_CAM_SETTOP_RVA);
    ctx->cam_ld = (better_together_input_cam_ld_func_t)(uintptr_t)(ctx->base + BETTER_TOGETHER_INPUT_CAM_LD_RVA);
    ctx->cam_number = (better_together_input_cam_number_func_t)(uintptr_t)(ctx->base + BETTER_TOGETHER_INPUT_CAM_NUMBER_RVA);
    ctx->cam_str = (better_together_input_cam_str_func_t)(uintptr_t)(ctx->base + BETTER_TOGETHER_INPUT_CAM_STR_RVA);
    return LDG_ERR_AOK;
}

static uint32_t better_together_input_cam_ids_get(uint64_t host, uint64_t guest, uint32_t *host_id, uint32_t *guest_id)
{
    uint64_t entity = 0;
    uint32_t hash = 0;
    uint32_t res = LDG_ERR_AOK;

    if (LDG_UNLIKELY(!host_id || !guest_id)) { return LDG_ERR_FUNC_ARG_NULL; }

    *host_id = 0;
    *guest_id = 0;
    if (LDG_UNLIKELY(!host || !guest || host > UINTPTR_MAX - BETTER_TOGETHER_INPUT_OWNER_ENTITY_OFF - sizeof(entity) || guest > UINTPTR_MAX - BETTER_TOGETHER_INPUT_OWNER_ENTITY_OFF - sizeof(entity))) { return LDG_ERR_FUNC_ARG_INVALID; }

    res = better_together_input_mem_rd(host + BETTER_TOGETHER_INPUT_OWNER_ENTITY_OFF, &entity, sizeof(entity));
    if (res == LDG_ERR_AOK && (!entity || entity > UINTPTR_MAX - BETTER_TOGETHER_INPUT_ENTITY_ID_OFF - sizeof(*host_id))) { res = LDG_ERR_INVALID; }

    if (res == LDG_ERR_AOK) { res = better_together_input_mem_rd(entity + BETTER_TOGETHER_INPUT_ENTITY_HASH_OFF, &hash, sizeof(hash)); }

    if (res == LDG_ERR_AOK && hash != BETTER_TOGETHER_YARNY_HASH) { res = LDG_ERR_INVALID; }

    if (res == LDG_ERR_AOK) { res = better_together_input_mem_rd(entity + BETTER_TOGETHER_INPUT_ENTITY_ID_OFF, host_id, sizeof(*host_id)); }

    if (res == LDG_ERR_AOK) { res = better_together_input_mem_rd(guest + BETTER_TOGETHER_INPUT_OWNER_ENTITY_OFF, &entity, sizeof(entity)); }

    if (res == LDG_ERR_AOK && (!entity || entity > UINTPTR_MAX - BETTER_TOGETHER_INPUT_ENTITY_ID_OFF - sizeof(*guest_id))) { res = LDG_ERR_INVALID; }

    if (res == LDG_ERR_AOK) { res = better_together_input_mem_rd(entity + BETTER_TOGETHER_INPUT_ENTITY_HASH_OFF, &hash, sizeof(hash)); }

    if (res == LDG_ERR_AOK && hash != BETTER_TOGETHER_BOXIE_HASH) { res = LDG_ERR_INVALID; }

    if (res == LDG_ERR_AOK) { res = better_together_input_mem_rd(entity + BETTER_TOGETHER_INPUT_ENTITY_ID_OFF, guest_id, sizeof(*guest_id)); }

    if (res == LDG_ERR_AOK && *host_id == *guest_id) { res = LDG_ERR_INVALID; }

    return res;
}

static void better_together_input_cam_disable(better_together_input_ctx_t *ctx, uint64_t lua)
{
    uint32_t top = 0;
    uint32_t status = 0;

    AcquireSRWLockExclusive(&ctx->lock);
    if (!ctx->cam_ld || ctx->cam_busy || ctx->cam_lua != lua || ctx->cam_thread != GetCurrentThreadId()) { ReleaseSRWLockExclusive(&ctx->lock); return; }

    ctx->cam_busy = LDG_TRUTH_TRUE;
    ReleaseSRWLockExclusive(&ctx->lock);
    top = ctx->cam_top(lua);
    status = ctx->cam_ld(lua, BETTER_TOGETHER_CAM_STOP_SCRIPT, sizeof(BETTER_TOGETHER_CAM_STOP_SCRIPT) - LDG_STR_TERM_SIZE, BETTER_TOGETHER_INPUT_CAM_STOP_CHUNK);
    if (!status) { status = ctx->pcall(lua, BETTER_TOGETHER_INPUT_CAM_NO_ARGS, BETTER_TOGETHER_INPUT_CAM_NO_RES, BETTER_TOGETHER_INPUT_CAM_NO_HANDLER); }

    ctx->cam_top_set(lua, top);
    AcquireSRWLockExclusive(&ctx->lock);
    ctx->cam_lua = 0;
    ctx->cam_owner = 0;
    ctx->cam_next = 0;
    ctx->cam_busy = LDG_TRUTH_FALSE;
    if (status) { ctx->report.cam_res = LDG_ERR_INVALID; }

    ReleaseSRWLockExclusive(&ctx->lock);
}

static void better_together_input_cam_tick(better_together_input_ctx_t *ctx, uint64_t lua, uint64_t host, uint64_t guest)
{
    uint64_t now = GetTickCount64();
    uint64_t len = 0;
    uint32_t host_id = 0;
    uint32_t guest_id = 0;
    uint32_t top = 0;
    uint32_t status = 0;
    uint32_t state = 0;
    uint32_t attach_state = 0;
    uint32_t epoch = 0;
    uint32_t res = LDG_ERR_AOK;
    double val = 0;
    double dist = BETTER_TOGETHER_INPUT_CAM_UNKNOWN_DIST;
    const char *err = 0x0;
    const char *attach_err = 0x0;
    char text[sizeof(ctx->report.cam_err)] = LDG_ARR_ZERO_INIT;
    char attach_text[sizeof(ctx->report.attach_err)] = LDG_ARR_ZERO_INIT;

    AcquireSRWLockExclusive(&ctx->lock);
    if (!ctx->cam_ld || ctx->cam_busy || ctx->owner != guest || !ctx->active || ctx->report.root_thread != GetCurrentThreadId() || (ctx->cam_owner == guest && ctx->cam_lua == lua && ctx->cam_seen_epoch == ctx->cam_epoch && now < ctx->cam_next)) { ReleaseSRWLockExclusive(&ctx->lock); return; }

    ctx->cam_busy = LDG_TRUTH_TRUE;
    ctx->cam_next = now + LDG_MS_PER_SEC;
    ctx->cam_owner = guest;
    ctx->cam_lua = lua;
    ctx->cam_thread = GetCurrentThreadId();
    epoch = ctx->cam_epoch;
    ctx->cam_seen_epoch = epoch;
    ReleaseSRWLockExclusive(&ctx->lock);
    res = better_together_input_cam_ids_get(host, guest, &host_id, &guest_id);
    if (res == LDG_ERR_AOK) { res = ldg_str_fmt(ctx->cam_src, sizeof(ctx->cam_src), &len, BETTER_TOGETHER_CAM_SCRIPT, host_id, guest_id, epoch, now); }

    if (res == LDG_ERR_AOK)
    {
        top = ctx->cam_top(lua);
        status = ctx->cam_ld(lua, ctx->cam_src, len, BETTER_TOGETHER_INPUT_CAM_CHUNK);
        if (!status) { status = ctx->pcall(lua, BETTER_TOGETHER_INPUT_CAM_NO_ARGS, BETTER_TOGETHER_INPUT_CAM_RES_CUNT, BETTER_TOGETHER_INPUT_CAM_NO_HANDLER); }

        if (status) { res = LDG_ERR_INVALID; err = ctx->cam_str(lua, BETTER_TOGETHER_INPUT_CAM_TOP_ERR_IDX, &len); }
        else
        {
            val = ctx->cam_number(lua, top + BETTER_TOGETHER_INPUT_CAM_STATE_IDX);
            if (val == 0.0 || val == (double)BETTER_TOGETHER_INPUT_CAM_ACTIVE || val == (double)BETTER_TOGETHER_INPUT_CAM_FAILED) { state = (uint32_t)val; }
            else { res = LDG_ERR_INVALID; }

            if (state == BETTER_TOGETHER_INPUT_CAM_FAILED) { res = LDG_ERR_INVALID; err = ctx->cam_str(lua, top + BETTER_TOGETHER_INPUT_CAM_DIST_IDX, &len); }

            if (res == LDG_ERR_AOK) { dist = ctx->cam_number(lua, top + BETTER_TOGETHER_INPUT_CAM_DIST_IDX); }

            if (!isfinite(dist)) { res = LDG_ERR_INVALID; }
        }

        if (err)
        {
            if (len >= sizeof(text)) { len = sizeof(text) - LDG_STR_TERM_SIZE; }

            if (better_together_input_mem_rd((uint64_t)(uintptr_t)err, text, len) != LDG_ERR_AOK) { text[0] = LDG_STR_TERM; }
        }

        if (!status)
        {
            val = ctx->cam_number(lua, top + BETTER_TOGETHER_INPUT_CAM_ATTACH_IDX);
            if (val == (double)BETTER_TOGETHER_ATTACH_WAITING || val == (double)BETTER_TOGETHER_ATTACH_STARTED || val == (double)BETTER_TOGETHER_ATTACH_COMPLETE || val == (double)BETTER_TOGETHER_ATTACH_FAILED) { attach_state = (uint32_t)val; }
            else { attach_state = BETTER_TOGETHER_ATTACH_FAILED; res = LDG_ERR_INVALID; }

            attach_err = ctx->cam_str(lua, top + BETTER_TOGETHER_INPUT_CAM_ATTACH_ERR_IDX, &len);
            if (attach_err && len)
            {
                if (len >= sizeof(attach_text)) { len = sizeof(attach_text) - LDG_STR_TERM_SIZE; }

                if (better_together_input_mem_rd((uint64_t)(uintptr_t)attach_err, attach_text, len) != LDG_ERR_AOK) { attach_text[0] = LDG_STR_TERM; res = LDG_ERR_INVALID; }
            }
        }

        ctx->cam_top_set(lua, top);
    }

    AcquireSRWLockExclusive(&ctx->lock);
    ctx->report.cam_samples++;
    ctx->report.cam_state = state;
    ctx->report.cam_res = res;
    ctx->report.cam_dist = dist;
    ctx->report.cam_host = host_id;
    ctx->report.cam_guest = guest_id;
    ctx->report.attach_state = attach_state;
    ctx->report.attach_epoch = epoch;
    res = ldg_mem_secure_cpy(ctx->report.cam_err, text, sizeof(text));
    if (res == LDG_ERR_AOK) { res = ldg_mem_secure_cpy(ctx->report.attach_err, attach_text, sizeof(attach_text)); }

    if (res != LDG_ERR_AOK) { ctx->report.cam_res = res; }

    ctx->cam_busy = LDG_TRUTH_FALSE;
    ReleaseSRWLockExclusive(&ctx->lock);
}

static uint32_t better_together_input_pcall_cb(uint64_t lua, uint32_t args, uint32_t res_cunt, uint32_t handler, better_together_input_ctx_t *ctx)
{
    better_together_tex_report_t tex = LDG_STRUCT_ZERO_INIT;
    uint64_t roots[BETTER_TOGETHER_PAD_PLAYER_CUNT] = LDG_ARR_ZERO_INIT;
    uint64_t previous = 0;
    uint64_t sel = 0;
    uint64_t observed = 0;
    uint32_t player = UINT32_MAX;
    uint32_t ret = 0;
    uint32_t res = LDG_ERR_AOK;
    uint32_t tex_res = LDG_ERR_AOK;
    uint32_t epoch = 0;
    uint8_t active = LDG_TRUTH_FALSE;
    uint8_t changed = LDG_TRUTH_FALSE;

    AcquireSRWLockShared(&ctx->lock);
    active = ctx->active && ctx->report.root_thread == GetCurrentThreadId();
    roots[BETTER_TOGETHER_INPUT_HOST] = ctx->roots[BETTER_TOGETHER_INPUT_HOST];
    roots[BETTER_TOGETHER_INPUT_GUEST] = ctx->roots[BETTER_TOGETHER_INPUT_GUEST];
    epoch = ctx->cam_epoch;
    ReleaseSRWLockShared(&ctx->lock);
    if (!active || !roots[BETTER_TOGETHER_INPUT_HOST] || !roots[BETTER_TOGETHER_INPUT_GUEST])
    {
        better_together_input_cam_disable(ctx, lua);
        return ctx->pcall(lua, args, res_cunt, handler);
    }

    res = better_together_input_mem_rd(ctx->base + BETTER_TOGETHER_INPUT_GLOBAL_RVA, &previous, sizeof(previous));
    if (res == LDG_ERR_AOK && previous != roots[BETTER_TOGETHER_INPUT_HOST] && previous != roots[BETTER_TOGETHER_INPUT_GUEST]) { return ctx->pcall(lua, args, res_cunt, handler); }

    if (res == LDG_ERR_AOK && previous == roots[BETTER_TOGETHER_INPUT_HOST])
    {
        if (ctx->tex)
        {
            tex_res = better_together_tex_tick(ctx->tex, lua, roots[BETTER_TOGETHER_INPUT_HOST], roots[BETTER_TOGETHER_INPUT_GUEST], epoch, &tex);
            if (tex_res == LDG_ERR_AOK)
            {
                AcquireSRWLockExclusive(&ctx->lock);
                ctx->report.tex = tex;
                ReleaseSRWLockExclusive(&ctx->lock);
            }
        }

        better_together_input_cam_tick(ctx, lua, roots[BETTER_TOGETHER_INPUT_HOST], roots[BETTER_TOGETHER_INPUT_GUEST]);
    }

    if (res == LDG_ERR_AOK) { res = better_together_input_pcall_owner_get(lua, args, roots, &player); }

    if (res == LDG_ERR_NOT_FOUND) { return ctx->pcall(lua, args, res_cunt, handler); }

    if (res == LDG_ERR_AOK)
    {
        sel = roots[player];
        if (sel != previous)
        {
            observed = (uint64_t)InterlockedCompareExchange64((volatile LONG64 *)(uintptr_t)(ctx->base + BETTER_TOGETHER_INPUT_GLOBAL_RVA), (LONG64)sel, (LONG64)previous);
            if (observed == previous) { changed = LDG_TRUTH_TRUE; }
            else { res = LDG_ERR_BUSY; }
        }
    }

    ret = ctx->pcall(lua, args, res_cunt, handler);
    if (changed)
    {
        observed = (uint64_t)InterlockedCompareExchange64((volatile LONG64 *)(uintptr_t)(ctx->base + BETTER_TOGETHER_INPUT_GLOBAL_RVA), (LONG64)previous, (LONG64)sel);
        if (observed != sel) { res = LDG_ERR_MEM_CORRUPTION; }
    }

    AcquireSRWLockExclusive(&ctx->lock);
    if (res == LDG_ERR_AOK) { ctx->report.root_calls[player]++; }
    else { ctx->report.root_errors++; ctx->report.root_res = res; }

    ReleaseSRWLockExclusive(&ctx->lock);
    return ret;
}

static uint32_t better_together_input_handle_get(uint64_t lua, uint64_t *out)
{
    uint64_t ci = 0;
    uint64_t first = 0;
    uint64_t base = 0;
    better_together_input_val_t self = LDG_STRUCT_ZERO_INIT;
    uint32_t res = LDG_ERR_AOK;

    *out = 0;
    if (LDG_UNLIKELY(!lua || lua > UINTPTR_MAX - BETTER_TOGETHER_INPUT_LUA_BASE_CI_OFF - sizeof(uint64_t))) { return LDG_ERR_INVALID; }

    res = better_together_input_mem_rd(lua + BETTER_TOGETHER_INPUT_LUA_CI_OFF, &ci, sizeof(ci));
    if (res == LDG_ERR_AOK) { res = better_together_input_mem_rd(lua + BETTER_TOGETHER_INPUT_LUA_BASE_CI_OFF, &first, sizeof(first)); }

    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(!first || ci < first || ci - first < BETTER_TOGETHER_INPUT_CI_BYTES || (ci - first) % BETTER_TOGETHER_INPUT_CI_BYTES)) { return LDG_ERR_BOUNDS; }

    res = better_together_input_mem_rd(ci - BETTER_TOGETHER_INPUT_CI_BYTES, &base, sizeof(base));
    if (res == LDG_ERR_AOK) { res = better_together_input_mem_rd(base, &self, sizeof(self)); }

    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(self.tag != BETTER_TOGETHER_INPUT_TAG_USERDATA || !self.data || self.data > UINTPTR_MAX - BETTER_TOGETHER_INPUT_HANDLE_OFF - sizeof(uint64_t))) { return LDG_ERR_INVALID; }

    res = better_together_input_mem_rd(self.data + BETTER_TOGETHER_INPUT_HANDLE_OFF, out, sizeof(*out));
    *out &= BETTER_TOGETHER_INPUT_HANDLE_MASK;
    return res;
}

static uint32_t better_together_input_args_get(uint64_t lua, uint32_t *group, uint32_t *action)
{
    better_together_input_val_t args[BETTER_TOGETHER_INPUT_ARG_CUNT] = LDG_ARR_ZERO_INIT;
    double arg_val[BETTER_TOGETHER_INPUT_ARG_CUNT] = LDG_ARR_ZERO_INIT;
    uint64_t base = 0;
    uint64_t top = 0;
    uint32_t idx = 0;
    uint32_t res = LDG_ERR_AOK;

    res = better_together_input_mem_rd(lua + BETTER_TOGETHER_INPUT_LUA_BASE_OFF, &base, sizeof(base));
    if (res == LDG_ERR_AOK) { res = better_together_input_mem_rd(lua + BETTER_TOGETHER_INPUT_LUA_TOP_OFF, &top, sizeof(top)); }

    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(!base || top < base || top - base < sizeof(args))) { return LDG_ERR_BOUNDS; }

    res = better_together_input_mem_rd(base, args, sizeof(args));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    for (idx = 0; idx < BETTER_TOGETHER_INPUT_ARG_CUNT; idx++)
    {
        if (LDG_UNLIKELY(args[idx].tag != BETTER_TOGETHER_INPUT_TAG_NUMBER)) { return LDG_ERR_INVALID; }

        res = ldg_mem_secure_cpy(&arg_val[idx], &args[idx].data, sizeof(arg_val[idx]));
        if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

        if (LDG_UNLIKELY(!isfinite(arg_val[idx]) || arg_val[idx] < 0.0 || arg_val[idx] > (double)UINT32_MAX || (double)(uint32_t)arg_val[idx] != arg_val[idx])) { return LDG_ERR_INVALID; }
    }
    *group = (uint32_t)arg_val[0];
    *action = (uint32_t)arg_val[1];
    return LDG_ERR_AOK;
}

static uint32_t better_together_input_query_cb(uint64_t lua, better_together_input_ctx_t *ctx, uint32_t kind)
{
    uint64_t owner = 0;
    uint64_t host_owner = 0;
    uint64_t entity = 0;
    uint64_t handle = 0;
    uint64_t now = GetTickCount64();
    uint32_t hash = 0;
    uint32_t group = 0;
    uint32_t action = 0;
    uint32_t idx = 0;
    uint32_t player = UINT32_MAX;
    uint32_t truth = 0;
    uint32_t res = LDG_ERR_AOK;
    uint8_t route = LDG_TRUTH_FALSE;
    uint8_t found = LDG_TRUTH_FALSE;
    double current = 0x0;
    double prior = 0x0;

    AcquireSRWLockShared(&ctx->lock);
    if (ctx->active && ctx->report.ready && ctx->thread == GetCurrentThreadId())
    {
        owner = ctx->owner;
        if (ctx->report.pads.host < BETTER_TOGETHER_PAD_SLOT_CUNT) { host_owner = ctx->orig_owner; }
    }

    ReleaseSRWLockShared(&ctx->lock);
    if (owner)
    {
        res = better_together_input_handle_get(lua, &handle);
        if (res == LDG_ERR_AOK && handle && handle <= UINTPTR_MAX - BETTER_TOGETHER_INPUT_ENTITY_HASH_OFF - sizeof(hash))
        {
            res = better_together_input_mem_rd(handle + BETTER_TOGETHER_INPUT_ENTITY_HASH_OFF, &hash, sizeof(hash));
            if (res == LDG_ERR_AOK && hash == BETTER_TOGETHER_BOXIE_HASH && owner <= UINTPTR_MAX - BETTER_TOGETHER_INPUT_OWNER_ENTITY_OFF - sizeof(entity))
            {
                res = better_together_input_mem_rd(owner + BETTER_TOGETHER_INPUT_OWNER_ENTITY_OFF, &entity, sizeof(entity));
                if (res == LDG_ERR_AOK && handle == entity) { player = BETTER_TOGETHER_INPUT_GUEST; }
            }

            if (res == LDG_ERR_AOK && hash == BETTER_TOGETHER_YARNY_HASH && host_owner && host_owner <= UINTPTR_MAX - BETTER_TOGETHER_INPUT_OWNER_ENTITY_OFF - sizeof(entity))
            {
                res = better_together_input_mem_rd(host_owner + BETTER_TOGETHER_INPUT_OWNER_ENTITY_OFF, &entity, sizeof(entity));
                if (res == LDG_ERR_AOK && handle == entity) { player = BETTER_TOGETHER_INPUT_HOST; }
            }

            if (player != UINT32_MAX) { res = better_together_input_args_get(lua, &group, &action); }

            if (player != UINT32_MAX && res == LDG_ERR_AOK && group == ctx->report.group) { route = LDG_TRUTH_TRUE; }
        }
    }

    AcquireSRWLockExclusive(&ctx->lock);
    if (!ctx->active || ctx->owner != owner || (player == BETTER_TOGETHER_INPUT_HOST && ctx->orig_owner != host_owner)) { route = LDG_TRUTH_FALSE; }

    if (route)
    {
        if (player == BETTER_TOGETHER_INPUT_HOST) { ctx->report.host_routed++; }
        else { ctx->report.routed[kind]++; }

        if (player == BETTER_TOGETHER_INPUT_GUEST && ctx->external)
        {
            if (ctx->packet.seq && now >= ctx->received && now - ctx->received < BETTER_TOGETHER_INPUT_TIMEOUT_MS) { for (idx = 0; idx < ctx->packet.cunt; idx++)
                {
                    if (ctx->packet.samples[idx].action != action) { continue; }

                    current = ctx->packet.samples[idx].current;
                    prior = ctx->packet.samples[idx].prior;
                    found = LDG_TRUTH_TRUE;
                    break;
                }
            }
        }
        else
        {
            for (idx = 0; idx < BETTER_TOGETHER_PAD_ACTION_CUNT; idx++)
            {
                if (ctx->actions[idx] != action) { continue; }

                found = LDG_TRUTH_TRUE;
                if (ctx->report.pads.frames && now >= ctx->pad_received && now - ctx->pad_received < BETTER_TOGETHER_INPUT_TIMEOUT_MS)
                {
                    current = ctx->pad_samples[player].current[idx];
                    prior = ctx->pad_samples[player].prior[idx];
                }

                break;
            }
        }

        if (!found)
        {
            ctx->report.unknown++;
            ctx->report.unknown_action = action;
        }
    }
    else { ctx->report.passed++; }

    ReleaseSRWLockExclusive(&ctx->lock);
    if (!route) { return ctx->orig[kind](lua); }

    if (kind == BETTER_TOGETHER_INPUT_FLOAT) { ctx->number(lua, current); }
    else
    {
        truth = kind == BETTER_TOGETHER_INPUT_BOOL ? current > BETTER_TOGETHER_INPUT_THRESHOLD : fabs(current) > BETTER_TOGETHER_INPUT_THRESHOLD && fabs(prior) < BETTER_TOGETHER_INPUT_THRESHOLD;
        ctx->boolean(lua, truth);
    }

    return BETTER_TOGETHER_INPUT_LUA_RES_CUNT;
}

static uint32_t better_together_input_float_cb(uint64_t lua, uint64_t unused_a __attribute__((unused)), uint64_t unused_b __attribute__((unused)), better_together_input_ctx_t *ctx)
{
    return better_together_input_query_cb(lua, ctx, BETTER_TOGETHER_INPUT_FLOAT);
}

static uint32_t better_together_input_bool_cb(uint64_t lua, uint64_t unused_a __attribute__((unused)), uint64_t unused_b __attribute__((unused)), better_together_input_ctx_t *ctx)
{
    return better_together_input_query_cb(lua, ctx, BETTER_TOGETHER_INPUT_BOOL);
}

static uint32_t better_together_input_pressed_cb(uint64_t lua, uint64_t unused_a __attribute__((unused)), uint64_t unused_b __attribute__((unused)), better_together_input_ctx_t *ctx)
{
    return better_together_input_query_cb(lua, ctx, BETTER_TOGETHER_INPUT_PRESSED);
}

static uint32_t better_together_input_closure_validate(better_together_input_ctx_t *ctx, uint64_t addr, uint32_t *kind)
{
    better_together_input_closure_t closure = LDG_STRUCT_ZERO_INIT;
    better_together_input_userdata_t userdata = LDG_STRUCT_ZERO_INIT;
    better_together_input_binding_t binding = LDG_STRUCT_ZERO_INIT;
    uint32_t res = LDG_ERR_AOK;

    *kind = UINT32_MAX;
    res = better_together_input_mem_rd(addr, &closure, sizeof(closure));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(closure.tag != BETTER_TOGETHER_INPUT_TAG_FUNC || closure.is_c != LDG_TRUTH_TRUE || closure.upvalues != BETTER_TOGETHER_INPUT_CLOSURE_UPVALUES || closure.userdata.tag != BETTER_TOGETHER_INPUT_TAG_USERDATA || closure.other.tag != BETTER_TOGETHER_INPUT_TAG_LIGHT_USERDATA)) { return LDG_ERR_INVALID; }

    if (LDG_UNLIKELY(closure.func != (uint64_t)(uintptr_t)ctx->orig[BETTER_TOGETHER_INPUT_FLOAT] && closure.func != (uint64_t)(uintptr_t)ctx->orig[BETTER_TOGETHER_INPUT_BOOL])) { return LDG_ERR_INVALID; }

    res = better_together_input_mem_rd(closure.userdata.data, &userdata, sizeof(userdata));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(userdata.tag != BETTER_TOGETHER_INPUT_TAG_USERDATA || userdata.len != sizeof(uint64_t))) { return LDG_ERR_INVALID; }

    res = better_together_input_mem_rd(userdata.binding, &binding, sizeof(binding));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(binding.entry != closure.func || binding.cap != sizeof(binding.name) - LDG_STR_TERM_SIZE || binding.tgt < ctx->base || binding.tgt - ctx->base >= BETTER_TOGETHER_INPUT_IMG_BYTES)) { return LDG_ERR_INVALID; }

    if (binding.vtable == ctx->base + BETTER_TOGETHER_INPUT_FLOAT_VTABLE_RVA && binding.len == sizeof("GetFloat") - LDG_STR_TERM_SIZE && closure.func == (uint64_t)(uintptr_t)ctx->orig[BETTER_TOGETHER_INPUT_FLOAT] && ldg_mem_secure_eq_is(binding.name, "GetFloat", sizeof("GetFloat")) == LDG_TRUTH_TRUE) { *kind = BETTER_TOGETHER_INPUT_FLOAT; }

    if (binding.vtable == ctx->base + BETTER_TOGETHER_INPUT_BOOL_VTABLE_RVA && binding.len == sizeof("GetBool") - LDG_STR_TERM_SIZE && binding.tgt == ctx->base + BETTER_TOGETHER_INPUT_BOOL_TGT_RVA && closure.func == (uint64_t)(uintptr_t)ctx->orig[BETTER_TOGETHER_INPUT_BOOL] && ldg_mem_secure_eq_is(binding.name, "GetBool", sizeof("GetBool")) == LDG_TRUTH_TRUE) { *kind = BETTER_TOGETHER_INPUT_BOOL; }

    if (binding.vtable == ctx->base + BETTER_TOGETHER_INPUT_BOOL_VTABLE_RVA && binding.len == sizeof("IsPressed") - LDG_STR_TERM_SIZE && binding.tgt == ctx->base + BETTER_TOGETHER_INPUT_PRESSED_TGT_RVA && closure.func == (uint64_t)(uintptr_t)ctx->orig[BETTER_TOGETHER_INPUT_PRESSED] && ldg_mem_secure_eq_is(binding.name, "IsPressed", sizeof("IsPressed")) == LDG_TRUTH_TRUE) { *kind = BETTER_TOGETHER_INPUT_PRESSED; }

    return *kind == UINT32_MAX ? LDG_ERR_NOT_FOUND : LDG_ERR_AOK;
}

static uint32_t better_together_input_closure_install(better_together_input_ctx_t *ctx, uint64_t addr)
{
    MEMORY_BASIC_INFORMATION info = LDG_STRUCT_ZERO_INIT;
    uint64_t end = 0;
    uint32_t kind = UINT32_MAX;
    uint32_t res = LDG_ERR_AOK;
    void *previous = 0x0;
    void *hook = 0x0;

    if (LDG_UNLIKELY((addr & (BETTER_TOGETHER_INPUT_QWORD_ALIGN_MASK)) || addr > UINTPTR_MAX - sizeof(better_together_input_closure_t))) { return LDG_ERR_INVALID; }

    if (LDG_UNLIKELY(VirtualQuery((const void *)(uintptr_t)addr, &info, sizeof(info)) != sizeof(info))) { return LDG_ERR_MEM_BAD; }

    end = (uint64_t)(uintptr_t)info.BaseAddress + info.RegionSize;
    if (LDG_UNLIKELY(end <= addr || end - addr < sizeof(better_together_input_closure_t) || info.State != MEM_COMMIT || info.Protect != PAGE_READWRITE || info.Type != MEM_PRIVATE)) { return LDG_ERR_DENIED; }

    res = better_together_input_closure_validate(ctx, addr, &kind);
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    hook = (uint8_t *)ctx->code + kind * BETTER_TOGETHER_INPUT_CODE_SLOT_BYTES;
    previous = InterlockedCompareExchangePointer((PVOID volatile *)(uintptr_t)(addr + BETTER_TOGETHER_INPUT_FUNC_OFF), hook, (void *)(uintptr_t)ctx->orig[kind]);
    if (LDG_UNLIKELY(previous != (void *)(uintptr_t)ctx->orig[kind])) { return LDG_ERR_BUSY; }

    AcquireSRWLockExclusive(&ctx->lock);
    ctx->report.installed[kind]++;
    ctx->report.ready = ctx->report.installed[BETTER_TOGETHER_INPUT_FLOAT] && ctx->report.installed[BETTER_TOGETHER_INPUT_BOOL] && ctx->report.installed[BETTER_TOGETHER_INPUT_PRESSED];
    ReleaseSRWLockExclusive(&ctx->lock);
    return LDG_ERR_AOK;
}

static uint32_t better_together_input_scan(better_together_input_ctx_t *ctx)
{
    MEMORY_BASIC_INFORMATION info = LDG_STRUCT_ZERO_INIT;
    uint64_t end = 0;
    uint64_t len = 0;
    uint64_t idx = 0;
    uint64_t spent = 0;
    uint64_t field = 0;
    uint64_t own = (uint64_t)(uintptr_t)ctx;
    uint32_t regions = 0;
    uint32_t res = LDG_ERR_AOK;

    if (ctx->cursor >= ctx->max) { return LDG_ERR_AOK; }

    while (spent < BETTER_TOGETHER_INPUT_SCAN_BUDGET && regions < BETTER_TOGETHER_INPUT_REGION_BUDGET && ctx->cursor < ctx->max)
    {
        if (LDG_UNLIKELY(VirtualQuery((const void *)(uintptr_t)ctx->cursor, &info, sizeof(info)) != sizeof(info))) { return LDG_ERR_MEM_BAD; }

        end = (uint64_t)(uintptr_t)info.BaseAddress + info.RegionSize;
        if (LDG_UNLIKELY(end <= ctx->cursor)) { return LDG_ERR_OVERFLOW; }

        if (end > ctx->max) { end = ctx->max; }

        regions++;
        if (info.State != MEM_COMMIT || info.Protect != PAGE_READWRITE || info.Type != MEM_PRIVATE) { ctx->cursor = end; continue; }

        len = end - ctx->cursor;
        if (len > sizeof(ctx->scan)) { len = sizeof(ctx->scan); }

        res = better_together_input_mem_rd(ctx->cursor, ctx->scan, len);
        if (res == LDG_ERR_AOK) { for (idx = 0; idx + sizeof(uint64_t) <= len; idx += sizeof(uint64_t))
            {
                field = ctx->cursor + idx;
                if (field >= own && field < own + sizeof(*ctx)) { continue; }

                if (field < BETTER_TOGETHER_INPUT_FUNC_OFF) { continue; }

                if (ctx->scan[idx / sizeof(uint64_t)] != (uint64_t)(uintptr_t)ctx->orig[BETTER_TOGETHER_INPUT_FLOAT] && ctx->scan[idx / sizeof(uint64_t)] != (uint64_t)(uintptr_t)ctx->orig[BETTER_TOGETHER_INPUT_BOOL]) { continue; }

                res = better_together_input_closure_install(ctx, field - BETTER_TOGETHER_INPUT_FUNC_OFF);
                if (LDG_UNLIKELY(res == LDG_ERR_BUSY)) { return res; }
            }
        }

        ctx->cursor += len;
        spent += len;
    }
    if (ctx->cursor >= ctx->max)
    {
        AcquireSRWLockExclusive(&ctx->lock);
        ctx->report.scan_passes++;
        ReleaseSRWLockExclusive(&ctx->lock);
    }

    return LDG_ERR_AOK;
}

static uint32_t better_together_input_samples_commit(better_together_input_ctx_t *ctx)
{
    uint32_t idx = 0;
    uint32_t res = LDG_ERR_AOK;

    if (ctx->pad)
    {
        res = better_together_pad_poll(ctx->pad, ctx->pad_samples, &ctx->report.pads);
        if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

        ctx->pad_received = GetTickCount64();
    }

    if (ctx->pending.seq > ctx->packet.seq)
    {
        res = ldg_mem_secure_cpy(&ctx->packet, &ctx->pending, sizeof(ctx->packet));
        if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

        ctx->received = ctx->pending_received;
        ctx->report.seq = ctx->packet.seq;
    }
    else { for (idx = 0; idx < ctx->packet.cunt; idx++) { ctx->packet.samples[idx].prior = ctx->packet.samples[idx].current; } }

    ctx->report.frame_time = GetTickCount64();
    return LDG_ERR_AOK;
}

static uint64_t better_together_input_frame_cb(uint64_t game, better_together_input_native_dt_t dt, uint64_t arg, better_together_input_ctx_t *ctx)
{
    uint64_t orig = 0;
    uint64_t owner = 0;
    uint32_t res = LDG_ERR_AOK;
    uint8_t scan = LDG_TRUTH_FALSE;
    better_together_input_spawn_cb_t spawn_cb = 0x0;
    void *spawn_user = 0x0;

    res = better_together_input_mem_rd(ctx->base + BETTER_TOGETHER_INPUT_GLOBAL_RVA, &orig, sizeof(orig));
    if (res == LDG_ERR_AOK && game && game <= UINTPTR_MAX - BETTER_TOGETHER_INPUT_GAME_OWNER_OFF - sizeof(owner)) { res = better_together_input_mem_rd(game + BETTER_TOGETHER_INPUT_GAME_OWNER_OFF, &owner, sizeof(owner)); }

    if (res == LDG_ERR_AOK && orig && orig == owner)
    {
        AcquireSRWLockExclusive(&ctx->lock);
        if (ctx->active)
        {
            if (ctx->owner && (ctx->scan_owner != ctx->owner || ctx->orig_owner != orig || ctx->scan_epoch != ctx->cam_epoch))
            {
                ctx->cursor = ctx->min;
                ctx->scan_owner = ctx->owner;
                ctx->scan_epoch = ctx->cam_epoch;
            }

            spawn_cb = ctx->spawn_cb;
            spawn_user = ctx->spawn_user;
            ctx->orig_owner = orig;
            ctx->thread = GetCurrentThreadId();
            res = better_together_input_samples_commit(ctx);
            ctx->report.res = res;
            scan = ctx->owner && res == LDG_ERR_AOK && ctx->cursor < ctx->max;
        }

        ReleaseSRWLockExclusive(&ctx->lock);
        if (spawn_cb)
        {
            res = spawn_cb(spawn_user, game);
            if (res != LDG_ERR_AOK && res != LDG_ERR_BUSY)
            {
                AcquireSRWLockExclusive(&ctx->lock);
                ctx->report.res = res;
                ReleaseSRWLockExclusive(&ctx->lock);
            }
        }

        if (scan)
        {
            res = better_together_input_scan(ctx);
            AcquireSRWLockExclusive(&ctx->lock);
            ctx->report.res = res;
            ReleaseSRWLockExclusive(&ctx->lock);
        }
    }

    return ctx->frame(game, dt, arg);
}

static uint32_t better_together_input_code_init(better_together_input_ctx_t *ctx)
{
    better_together_setup_layout_t layout[BETTER_TOGETHER_SETUP_THUNK_CUNT] = LDG_ARR_ZERO_INIT;
    uint64_t hook_cb[BETTER_TOGETHER_INPUT_CODE_CUNT] = { (uint64_t)(uintptr_t)better_together_input_float_cb, (uint64_t)(uintptr_t)better_together_input_bool_cb, (uint64_t)(uintptr_t)better_together_input_pressed_cb, (uint64_t)(uintptr_t)better_together_input_frame_cb, (uint64_t)(uintptr_t)better_together_input_pcall_cb };
    uint64_t hook_ctx = (uint64_t)(uintptr_t)ctx;
    uint8_t *dst = 0x0;
    uint32_t idx = 0;
    uint32_t off = 0;
    uint32_t kind = 0;
    uint32_t res = LDG_ERR_AOK;
    DWORD old = 0;

    res = better_together_setup_layout_get(layout);
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    for (idx = 0; idx < BETTER_TOGETHER_SETUP_THUNK_CUNT; idx++) { if (LDG_UNLIKELY(!layout[idx].src || layout[idx].len < sizeof(uint64_t) || layout[idx].len > BETTER_TOGETHER_INPUT_CODE_SLOT_BYTES || layout[idx].code_end > layout[idx].len || layout[idx].ctx_off > layout[idx].len - sizeof(uint64_t) || layout[idx].cb_off > layout[idx].len - sizeof(uint64_t) || layout[idx].unwind_off > layout[idx].len - sizeof(uint32_t) || (layout[idx].unwind_off & 3))) { return LDG_ERR_INVALID; } }
    ctx->code = VirtualAlloc(0x0, BETTER_TOGETHER_INPUT_CODE_SLOT_BYTES * BETTER_TOGETHER_INPUT_CODE_CUNT, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (LDG_UNLIKELY(!ctx->code)) { return LDG_ERR_ALLOC_NULL; }

    for (idx = 0; idx < BETTER_TOGETHER_INPUT_CODE_CUNT; idx++)
    {
        kind = idx == BETTER_TOGETHER_INPUT_PCALL_SLOT ? BETTER_TOGETHER_INPUT_THUNK_CTX_STACK : BETTER_TOGETHER_INPUT_THUNK_CTX_REG;
        off = idx * BETTER_TOGETHER_INPUT_CODE_SLOT_BYTES;
        dst = (uint8_t *)ctx->code + off;
        res = ldg_mem_secure_cpy(dst, (const void *)(uintptr_t)layout[kind].src, layout[kind].len);
        if (res == LDG_ERR_AOK) { res = ldg_mem_secure_cpy(dst + layout[kind].ctx_off, &hook_ctx, sizeof(hook_ctx)); }

        if (res == LDG_ERR_AOK) { res = ldg_mem_secure_cpy(dst + layout[kind].cb_off, &hook_cb[idx], sizeof(hook_cb[idx])); }

        if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

        ctx->unwind[idx].BeginAddress = off;
        ctx->unwind[idx].EndAddress = off + (uint32_t)layout[kind].code_end;
        ctx->unwind[idx].UnwindData = off + (uint32_t)layout[kind].unwind_off;
    }
    if (LDG_UNLIKELY(!VirtualProtect(ctx->code, BETTER_TOGETHER_INPUT_CODE_SLOT_BYTES * BETTER_TOGETHER_INPUT_CODE_CUNT, PAGE_EXECUTE_READ, &old) || !FlushInstructionCache(GetCurrentProcess(), ctx->code, BETTER_TOGETHER_INPUT_CODE_SLOT_BYTES * BETTER_TOGETHER_INPUT_CODE_CUNT))) { return LDG_ERR_DENIED; }

    if (LDG_UNLIKELY(!RtlAddFunctionTable(ctx->unwind, BETTER_TOGETHER_INPUT_CODE_CUNT, (DWORD64)(uintptr_t)ctx->code))) { return LDG_ERR_INVALID; }

    ctx->unwind_up = LDG_TRUTH_TRUE;
    return LDG_ERR_AOK;
}

static uint32_t better_together_input_root_slot_validate(uint64_t addr)
{
    MEMORY_BASIC_INFORMATION region = LDG_STRUCT_ZERO_INIT;
    uint64_t base = 0;
    uint64_t offset = 0;

    if (LDG_UNLIKELY(!addr || (addr & (BETTER_TOGETHER_INPUT_QWORD_ALIGN_MASK)))) { return LDG_ERR_DENIED; }

    if (LDG_UNLIKELY(!VirtualQuery((const void *)(uintptr_t)addr, &region, sizeof(region)))) { return LDG_ERR_MEM_BAD; }

    if (LDG_UNLIKELY(region.State != MEM_COMMIT || (region.Protect & PAGE_GUARD) || !(region.Protect & (PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)))) { return LDG_ERR_DENIED; }

    base = (uint64_t)(uintptr_t)region.BaseAddress;
    if (LDG_UNLIKELY(addr < base)) { return LDG_ERR_DENIED; }

    offset = addr - base;
    if (LDG_UNLIKELY(offset > region.RegionSize || sizeof(uint64_t) > region.RegionSize - offset)) { return LDG_ERR_DENIED; }

    return LDG_ERR_AOK;
}

static uint32_t better_together_input_profile_validate(better_together_input_ctx_t *ctx)
{
    IMAGE_DOS_HEADER dos = LDG_STRUCT_ZERO_INIT;
    IMAGE_NT_HEADERS64 pe = LDG_STRUCT_ZERO_INIT;
    const uint8_t pcall[] = { 0x48, 0x89, 0x5C, 0x24, 0x08, 0x57, 0x48, 0x83, 0xEC, 0x40, 0x44, 0x89, 0xC7 };
    const uint8_t number[] = { 0x48, 0x8B, 0x41, 0x10, 0xF2, 0x0F, 0x11, 0x08, 0xC7, 0x40, 0x08, 0x03, 0x00, 0x00, 0x00, 0x48, 0x83, 0x41, 0x10, 0x10, 0xC3 };
    const uint8_t boolean[] = { 0x4C, 0x8B, 0x41, 0x10, 0x31, 0xC0, 0x85, 0xD2, 0x0F, 0x95, 0xD0, 0x41, 0xC7, 0x40, 0x08, 0x01, 0x00, 0x00, 0x00, 0x41, 0x89, 0x00, 0x48, 0x83, 0x41, 0x10, 0x10, 0xC3 };
    const uint8_t frame[] = { 0x57, 0x48, 0x83, 0xEC, 0x50, 0x48, 0xC7, 0x44, 0x24, 0x20, 0xFE, 0xFF, 0xFF, 0xFF, 0x48, 0x89, 0x5C, 0x24, 0x60 };
    uint8_t actual[sizeof(boolean)] = LDG_ARR_ZERO_INIT;
    uint32_t res = LDG_ERR_AOK;

    res = better_together_input_mem_rd(ctx->base, &dos, sizeof(dos));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(dos.e_magic != IMAGE_DOS_SIGNATURE || (uint32_t)dos.e_lfanew > BETTER_TOGETHER_INPUT_IMG_BYTES - sizeof(pe))) { return LDG_ERR_UNSUPPORTED; }

    res = better_together_input_mem_rd(ctx->base + (uint32_t)dos.e_lfanew, &pe, sizeof(pe));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(pe.Signature != IMAGE_NT_SIGNATURE || pe.FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64 || pe.FileHeader.TimeDateStamp != BETTER_TOGETHER_INPUT_PE_TIMESTAMP || pe.OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC || pe.OptionalHeader.SizeOfImage != BETTER_TOGETHER_INPUT_IMG_BYTES)) { return LDG_ERR_UNSUPPORTED; }

    res = better_together_input_mem_rd(ctx->base + BETTER_TOGETHER_INPUT_PUSH_NUMBER_RVA, actual, sizeof(number));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(ldg_mem_secure_eq_is(actual, number, sizeof(number)) != LDG_TRUTH_TRUE)) { return LDG_ERR_UNSUPPORTED; }

    res = better_together_input_mem_rd(ctx->base + BETTER_TOGETHER_INPUT_PUSH_BOOL_RVA, actual, sizeof(boolean));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(ldg_mem_secure_eq_is(actual, boolean, sizeof(boolean)) != LDG_TRUTH_TRUE)) { return LDG_ERR_UNSUPPORTED; }

    res = better_together_input_mem_rd(ctx->base + BETTER_TOGETHER_INPUT_FRAME_RVA, actual, sizeof(frame));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(ldg_mem_secure_eq_is(actual, frame, sizeof(frame)) != LDG_TRUTH_TRUE)) { return LDG_ERR_UNSUPPORTED; }

    res = better_together_input_mem_rd(ctx->base + BETTER_TOGETHER_INPUT_PCALL_RVA, actual, sizeof(pcall));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(ldg_mem_secure_eq_is(actual, pcall, sizeof(pcall)) != LDG_TRUTH_TRUE)) { return LDG_ERR_UNSUPPORTED; }

    return better_together_input_root_slot_validate(ctx->base + BETTER_TOGETHER_INPUT_GLOBAL_RVA);
}

static uint32_t better_together_input_hooks_init(better_together_input_ctx_t *ctx)
{
    void *orig = 0x0;
    uint32_t res = LDG_ERR_AOK;
    MH_STATUS status = MH_Initialize();

    if (status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED) { ctx->report.hook_res = (uint32_t)status; return LDG_ERR_INVALID; }

    status = MH_CreateHook((void *)(uintptr_t)(ctx->base + BETTER_TOGETHER_INPUT_FRAME_RVA), (uint8_t *)ctx->code + BETTER_TOGETHER_INPUT_QUERY_CUNT * BETTER_TOGETHER_INPUT_CODE_SLOT_BYTES, &orig);
    ctx->report.hook_res = (uint32_t)status;
    if (LDG_UNLIKELY(status != MH_OK)) { return LDG_ERR_INVALID; }

    res = ldg_mem_secure_cpy(&ctx->frame, &orig, sizeof(orig));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    status = MH_CreateHook((void *)(uintptr_t)(ctx->base + BETTER_TOGETHER_INPUT_PCALL_RVA), (uint8_t *)ctx->code + BETTER_TOGETHER_INPUT_PCALL_SLOT * BETTER_TOGETHER_INPUT_CODE_SLOT_BYTES, &orig);
    ctx->report.hook_res = (uint32_t)status;
    if (LDG_UNLIKELY(status != MH_OK)) { return LDG_ERR_INVALID; }

    res = ldg_mem_secure_cpy(&ctx->pcall, &orig, sizeof(orig));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    status = MH_QueueEnableHook((void *)(uintptr_t)(ctx->base + BETTER_TOGETHER_INPUT_PCALL_RVA));
    if (status == MH_OK) { status = MH_QueueEnableHook((void *)(uintptr_t)(ctx->base + BETTER_TOGETHER_INPUT_FRAME_RVA)); }

    if (status == MH_OK) { status = MH_ApplyQueued(); }

    ctx->report.hook_res = (uint32_t)status;
    return status == MH_OK ? LDG_ERR_AOK : LDG_ERR_INVALID;
}

uint64_t better_together_input_size_get(void)
{
    return sizeof(better_together_input_ctx_t) + better_together_tex_size_get() + better_together_pad_size_get();
}

uint32_t better_together_input_init(void *storage, uint64_t cap)
{
    better_together_input_ctx_t *ctx = (better_together_input_ctx_t *)storage;
    HMODULE game = GetModuleHandleA("Unravel.exe");
    SYSTEM_INFO info = LDG_STRUCT_ZERO_INIT;
    const uint64_t name[BETTER_TOGETHER_INPUT_ARG_CUNT] = { (uint64_t)(uintptr_t)"InGamePlayerInput", sizeof("InGamePlayerInput") - LDG_STR_TERM_SIZE };
    const char action_name[BETTER_TOGETHER_PAD_ACTION_CUNT][BETTER_TOGETHER_INPUT_ACTION_NAME_CAP] = { "Walk", "Jump", "Grab", "Climb", "WalkMod", "Shoot", "Attach", "AimX", "AimY", "GrabObject" };
    const uint64_t action_len[BETTER_TOGETHER_PAD_ACTION_CUNT] = { sizeof("Walk") - LDG_STR_TERM_SIZE, sizeof("Jump") - LDG_STR_TERM_SIZE, sizeof("Grab") - LDG_STR_TERM_SIZE, sizeof("Climb") - LDG_STR_TERM_SIZE, sizeof("WalkMod") - LDG_STR_TERM_SIZE, sizeof("Shoot") - LDG_STR_TERM_SIZE, sizeof("Attach") - LDG_STR_TERM_SIZE, sizeof("AimX") - LDG_STR_TERM_SIZE, sizeof("AimY") - LDG_STR_TERM_SIZE, sizeof("GrabObject") - LDG_STR_TERM_SIZE };
    uint64_t action[BETTER_TOGETHER_INPUT_ARG_CUNT] = LDG_ARR_ZERO_INIT;
    better_together_input_hash_func_t hash = 0x0;
    uint64_t magic = 0;
    uint32_t idx = 0;
    uint32_t res = LDG_ERR_AOK;

    if (LDG_UNLIKELY(!storage)) { return LDG_ERR_FUNC_ARG_NULL; }

    if (LDG_UNLIKELY(cap < better_together_input_size_get() || ((uintptr_t)storage & (BETTER_TOGETHER_INPUT_QWORD_ALIGN_MASK)))) { return LDG_ERR_FUNC_ARG_INVALID; }

    res = better_together_input_mem_rd((uint64_t)(uintptr_t)storage, &magic, sizeof(magic));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(magic == BETTER_TOGETHER_INPUT_MAGIC)) { return LDG_ERR_EXISTS; }

    if (LDG_UNLIKELY(!game || game != GetModuleHandleA(0x0))) { return LDG_ERR_NOT_FOUND; }

    res = ldg_mem_secure_zero(ctx, better_together_input_size_get());
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    ctx->magic = BETTER_TOGETHER_INPUT_MAGIC;
    ctx->base = (uint64_t)(uintptr_t)game;
    InitializeSRWLock(&ctx->lock);
    res = better_together_input_profile_validate(ctx);
    if (res == LDG_ERR_AOK) { res = better_together_input_cam_init(ctx); }

    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    ctx->orig[BETTER_TOGETHER_INPUT_FLOAT] = (better_together_input_lua_func_t)(uintptr_t)(ctx->base + BETTER_TOGETHER_INPUT_FLOAT_RVA);
    ctx->orig[BETTER_TOGETHER_INPUT_BOOL] = (better_together_input_lua_func_t)(uintptr_t)(ctx->base + BETTER_TOGETHER_INPUT_BOOL_RVA);
    ctx->orig[BETTER_TOGETHER_INPUT_PRESSED] = ctx->orig[BETTER_TOGETHER_INPUT_BOOL];
    ctx->number = (better_together_input_number_func_t)(uintptr_t)(ctx->base + BETTER_TOGETHER_INPUT_PUSH_NUMBER_RVA);
    ctx->boolean = (better_together_input_bool_func_t)(uintptr_t)(ctx->base + BETTER_TOGETHER_INPUT_PUSH_BOOL_RVA);
    hash = (better_together_input_hash_func_t)(uintptr_t)(ctx->base + BETTER_TOGETHER_INPUT_HASH_RVA);
    ctx->report.group = hash(name);
    for (idx = 0; idx < BETTER_TOGETHER_PAD_ACTION_CUNT; idx++)
    {
        action[0] = (uint64_t)(uintptr_t)action_name[idx];
        action[1] = action_len[idx];
        ctx->actions[idx] = hash(action);
    }
    ctx->tex = (uint8_t *)storage + sizeof(*ctx);
    res = better_together_tex_init(ctx->tex, better_together_tex_size_get(), ctx->base);
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    ctx->pad = (uint8_t *)ctx->tex + better_together_tex_size_get();
    res = better_together_pad_init(ctx->pad, better_together_pad_size_get());
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    ctx->report.pads.host = UINT32_MAX;
    ctx->report.pads.guest = UINT32_MAX;
    ctx->report.pads.provider = UINT32_MAX;
    GetSystemInfo(&info);
    ctx->min = (uint64_t)(uintptr_t)info.lpMinimumApplicationAddress;
    ctx->max = (uint64_t)(uintptr_t)info.lpMaximumApplicationAddress;
    ctx->cursor = ctx->min;
    if (LDG_UNLIKELY(!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN, (const char *)(uintptr_t)better_together_input_float_cb, &ctx->module))) { return LDG_ERR_DENIED; }

    res = better_together_input_code_init(ctx);
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    ctx->active = LDG_TRUTH_TRUE;
    res = better_together_input_hooks_init(ctx);
    if (res != LDG_ERR_AOK) { ctx->active = LDG_TRUTH_FALSE; }

    return res;
}

uint32_t better_together_input_spawn_bind(void *storage, better_together_input_spawn_cb_t cb, void *user)
{
    better_together_input_ctx_t *ctx = (better_together_input_ctx_t *)storage;

    if (LDG_UNLIKELY(!ctx || !cb || !user)) { return LDG_ERR_FUNC_ARG_NULL; }

    if (LDG_UNLIKELY(ctx->magic != BETTER_TOGETHER_INPUT_MAGIC)) { return LDG_ERR_NOT_INIT; }

    AcquireSRWLockExclusive(&ctx->lock);
    if (!ctx->active) { ReleaseSRWLockExclusive(&ctx->lock); return LDG_ERR_NOT_INIT; }

    ctx->spawn_cb = cb;
    ctx->spawn_user = user;
    ReleaseSRWLockExclusive(&ctx->lock);
    return LDG_ERR_AOK;
}

uint32_t better_together_input_roots_bind(void *storage, uint64_t host, uint64_t guest, uint32_t thread)
{
    better_together_input_ctx_t *ctx = (better_together_input_ctx_t *)storage;
    uint64_t current = 0;
    uint32_t res = LDG_ERR_AOK;

    if (LDG_UNLIKELY(!ctx)) { return LDG_ERR_FUNC_ARG_NULL; }

    if (LDG_UNLIKELY(ctx->magic != BETTER_TOGETHER_INPUT_MAGIC)) { return LDG_ERR_NOT_INIT; }

    if (host || guest || thread)
    {
        if (LDG_UNLIKELY(!host || !guest || host == guest || !thread || thread != GetCurrentThreadId())) { return LDG_ERR_FUNC_ARG_INVALID; }

        res = better_together_input_mem_rd(ctx->base + BETTER_TOGETHER_INPUT_GLOBAL_RVA, &current, sizeof(current));
        if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

        if (LDG_UNLIKELY(current != host)) { return LDG_ERR_DENIED; }
    }

    AcquireSRWLockExclusive(&ctx->lock);
    if (host && !ctx->active) { ReleaseSRWLockExclusive(&ctx->lock); return LDG_ERR_NOT_INIT; }

    ctx->roots[BETTER_TOGETHER_INPUT_HOST] = host;
    ctx->roots[BETTER_TOGETHER_INPUT_GUEST] = guest;
    ctx->report.root_thread = thread;
    if (host) { ctx->cam_epoch++; }

    if (!host)
    {
        ctx->report.tex.applied = 0;
        ctx->owner = 0;
        ctx->report.owner = 0;
        ctx->cam_next = 0;
    }

    ReleaseSRWLockExclusive(&ctx->lock);
    return LDG_ERR_AOK;
}

uint32_t better_together_input_tick(void *storage, uint64_t owner, uint32_t thread)
{
    better_together_input_ctx_t *ctx = (better_together_input_ctx_t *)storage;
    uint32_t res = LDG_ERR_AOK;

    if (LDG_UNLIKELY(!ctx)) { return LDG_ERR_FUNC_ARG_NULL; }

    if (LDG_UNLIKELY(ctx->magic != BETTER_TOGETHER_INPUT_MAGIC)) { return LDG_ERR_NOT_INIT; }

    AcquireSRWLockExclusive(&ctx->lock);
    if (!ctx->active) { ReleaseSRWLockExclusive(&ctx->lock); return LDG_ERR_NOT_INIT; }

    if (owner && (!thread || thread != GetCurrentThreadId())) { ReleaseSRWLockExclusive(&ctx->lock); return LDG_ERR_DENIED; }

    ctx->owner = owner;
    ctx->report.owner = owner;
    if (owner) { ctx->thread = thread; }

    res = ctx->report.res;
    ReleaseSRWLockExclusive(&ctx->lock);
    return res;
}

uint32_t better_together_input_submit(void *storage, const better_together_input_packet_t *packet)
{
    better_together_input_ctx_t *ctx = (better_together_input_ctx_t *)storage;
    better_together_input_packet_t copy = LDG_STRUCT_ZERO_INIT;
    uint32_t idx = 0;
    uint32_t other = 0;
    uint32_t res = LDG_ERR_AOK;

    if (LDG_UNLIKELY(!ctx || !packet)) { return LDG_ERR_FUNC_ARG_NULL; }

    if (LDG_UNLIKELY(ctx->magic != BETTER_TOGETHER_INPUT_MAGIC)) { return LDG_ERR_NOT_INIT; }

    res = better_together_input_mem_rd((uint64_t)(uintptr_t)packet, &copy, sizeof(copy));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(!copy.seq || copy.reserved || copy.cunt > BETTER_TOGETHER_INPUT_ACTION_CAP)) { return LDG_ERR_FUNC_ARG_INVALID; }

    for (idx = 0; idx < copy.cunt; idx++)
    {
        if (LDG_UNLIKELY(copy.samples[idx].reserved || !isfinite(copy.samples[idx].current) || !isfinite(copy.samples[idx].prior))) { return LDG_ERR_FUNC_ARG_INVALID; }

        for (other = 0; other < idx; other++) { if (LDG_UNLIKELY(copy.samples[other].action == copy.samples[idx].action)) { return LDG_ERR_FUNC_ARG_INVALID; } }
    }
    AcquireSRWLockExclusive(&ctx->lock);
    if (!ctx->active) { ReleaseSRWLockExclusive(&ctx->lock); return LDG_ERR_NOT_INIT; }

    if (copy.seq <= ctx->pending.seq) { ReleaseSRWLockExclusive(&ctx->lock); return LDG_ERR_AGAIN; }

    res = ldg_mem_secure_cpy(&ctx->pending, &copy, sizeof(copy));
    if (res == LDG_ERR_AOK)
    {
        ctx->pending_received = GetTickCount64();
        ctx->external = LDG_TRUTH_TRUE;
    }

    ReleaseSRWLockExclusive(&ctx->lock);
    return res;
}

uint32_t better_together_input_controllers_sel(void *storage, uint32_t host, uint32_t guest)
{
    better_together_input_ctx_t *ctx = (better_together_input_ctx_t *)storage;
    uint32_t res = LDG_ERR_AOK;

    if (LDG_UNLIKELY(!ctx)) { return LDG_ERR_FUNC_ARG_NULL; }

    if (LDG_UNLIKELY(ctx->magic != BETTER_TOGETHER_INPUT_MAGIC || !ctx->pad)) { return LDG_ERR_NOT_INIT; }

    AcquireSRWLockExclusive(&ctx->lock);
    if (!ctx->active) { ReleaseSRWLockExclusive(&ctx->lock); return LDG_ERR_NOT_INIT; }

    res = better_together_pad_sel(ctx->pad, host, guest);
    if (res == LDG_ERR_AOK)
    {
        ctx->external = LDG_TRUTH_FALSE;
        ctx->pad_received = 0;
        ctx->report.pads.frames = 0;
    }

    ReleaseSRWLockExclusive(&ctx->lock);
    return res;
}

uint32_t better_together_input_report(void *storage, better_together_input_report_t *out)
{
    better_together_input_ctx_t *ctx = (better_together_input_ctx_t *)storage;
    uint32_t res = LDG_ERR_AOK;

    if (LDG_UNLIKELY(!ctx || !out)) { return LDG_ERR_FUNC_ARG_NULL; }

    if (LDG_UNLIKELY(ctx->magic != BETTER_TOGETHER_INPUT_MAGIC)) { return LDG_ERR_NOT_INIT; }

    AcquireSRWLockShared(&ctx->lock);
    res = ldg_mem_secure_cpy(out, &ctx->report, sizeof(*out));
    ReleaseSRWLockShared(&ctx->lock);
    return res;
}

uint32_t better_together_input_stop(void *storage)
{
    better_together_input_ctx_t *ctx = (better_together_input_ctx_t *)storage;

    if (LDG_UNLIKELY(!ctx)) { return LDG_ERR_FUNC_ARG_NULL; }

    if (LDG_UNLIKELY(ctx->magic != BETTER_TOGETHER_INPUT_MAGIC)) { return LDG_ERR_NOT_INIT; }

    AcquireSRWLockExclusive(&ctx->lock);
    ctx->active = LDG_TRUTH_FALSE;
    ctx->spawn_cb = 0x0;
    ctx->spawn_user = 0x0;
    ctx->roots[BETTER_TOGETHER_INPUT_HOST] = 0;
    ctx->roots[BETTER_TOGETHER_INPUT_GUEST] = 0;
    ctx->report.root_thread = 0;
    ctx->owner = 0;
    ctx->report.owner = 0;
    ReleaseSRWLockExclusive(&ctx->lock);
    return LDG_ERR_AOK;
}
