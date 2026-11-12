#include <stdint.h>
#include <math.h>
#include <windows.h>
#include <xinput.h>

#include <dangling/core/bits.h>
#include <dangling/core/err.h>
#include <dangling/core/macros.h>
#include <dangling/mem/secure.h>
#include <libresteam/ext/together/pad.h>

#define BETTER_TOGETHER_PAD_MAGIC UINT64_C(0x424F584945504144)
#define BETTER_TOGETHER_PAD_HOST 0U
#define BETTER_TOGETHER_PAD_GUEST 1U
#define BETTER_TOGETHER_PAD_WALK 0U
#define BETTER_TOGETHER_PAD_JMP 1U
#define BETTER_TOGETHER_PAD_GRAB 2U
#define BETTER_TOGETHER_PAD_CLIMB 3U
#define BETTER_TOGETHER_PAD_WALK_MOD 4U
#define BETTER_TOGETHER_PAD_SHOOT 5U
#define BETTER_TOGETHER_PAD_ATTACH 6U
#define BETTER_TOGETHER_PAD_AIM_X 7U
#define BETTER_TOGETHER_PAD_AIM_Y 8U
#define BETTER_TOGETHER_PAD_GRAB_OBJECT 9U
#define BETTER_TOGETHER_PAD_AXIS_POS_MAX 32767.0
#define BETTER_TOGETHER_PAD_AXIS_NEG_MAX 32768.0
#define BETTER_TOGETHER_PAD_TRIGGER_MAX 255.0
#define BETTER_TOGETHER_PAD_DLL_CUNT 3U
#define BETTER_TOGETHER_PAD_DLL_NAME_CAP 16U
#define BETTER_TOGETHER_PAD_DLL_XINPUT_1_3 "xinput1_3.dll"
#define BETTER_TOGETHER_PAD_DLL_XINPUT_1_4 "xinput1_4.dll"
#define BETTER_TOGETHER_PAD_DLL_XINPUT_9_1_0 "xinput9_1_0.dll"
#define BETTER_TOGETHER_PAD_SOLE_CUNT 1U
#define BETTER_TOGETHER_PAD_QWORD_ALIGN_MASK (sizeof(uint64_t) - 1U)

typedef uint32_t (*better_together_pad_get_func_t)(uint32_t idx, XINPUT_STATE *state);

typedef struct better_together_pad_ctx
{
    uint64_t magic;
    HMODULE module;
    better_together_pad_get_func_t get;
    better_together_pad_report_t report;
    better_together_pad_sample_t samples[BETTER_TOGETHER_PAD_PLAYER_CUNT];
    uint8_t auto_host;
    uint8_t online[BETTER_TOGETHER_PAD_PLAYER_CUNT];
    uint8_t pudding[5];
} better_together_pad_ctx_t;

static double better_together_pad_axis_get(double raw)
{
    double deadzone = (double)XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
    double magnitude = fabs(raw);

    if (magnitude <= deadzone) { return 0.0; }

    return raw < 0.0 ? -(magnitude - deadzone) / (BETTER_TOGETHER_PAD_AXIS_NEG_MAX - deadzone) : (magnitude - deadzone) / (BETTER_TOGETHER_PAD_AXIS_POS_MAX - deadzone);
}

static double better_together_pad_trigger_get(uint8_t raw)
{
    if (raw <= XINPUT_GAMEPAD_TRIGGER_THRESHOLD) { return 0.0; }

    return ((double)raw - (double)XINPUT_GAMEPAD_TRIGGER_THRESHOLD) / (BETTER_TOGETHER_PAD_TRIGGER_MAX - (double)XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
}

static void better_together_pad_sample_get(const XINPUT_GAMEPAD *pad, double *out)
{
    double stick_x = better_together_pad_axis_get((double)pad->sThumbLX);
    double stick_y = better_together_pad_axis_get((double)pad->sThumbLY);

    if (pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT) { stick_x = -1.0; }

    if (pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) { stick_x = 1.0; }

    if (pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN) { stick_y = -1.0; }

    if (pad->wButtons & XINPUT_GAMEPAD_DPAD_UP) { stick_y = 1.0; }

    out[BETTER_TOGETHER_PAD_WALK] = stick_x;
    out[BETTER_TOGETHER_PAD_JMP] = (pad->wButtons & XINPUT_GAMEPAD_A) ? 1.0 : 0.0;
    out[BETTER_TOGETHER_PAD_GRAB] = better_together_pad_trigger_get(pad->bLeftTrigger);
    out[BETTER_TOGETHER_PAD_CLIMB] = stick_y;
    out[BETTER_TOGETHER_PAD_WALK_MOD] = 0.0;
    out[BETTER_TOGETHER_PAD_SHOOT] = better_together_pad_trigger_get(pad->bRightTrigger);
    out[BETTER_TOGETHER_PAD_ATTACH] = (pad->wButtons & XINPUT_GAMEPAD_X) ? 1.0 : 0.0;
    out[BETTER_TOGETHER_PAD_AIM_X] = stick_x;
    out[BETTER_TOGETHER_PAD_AIM_Y] = stick_y;
    out[BETTER_TOGETHER_PAD_GRAB_OBJECT] = (pad->wButtons & XINPUT_GAMEPAD_B) ? 1.0 : 0.0;
}

static void better_together_pad_slots_sel(better_together_pad_ctx_t *ctx, uint32_t mask)
{
    uint32_t idx = 0;
    uint32_t candidate = UINT32_MAX;
    uint32_t cunt = 0;

    for (idx = 0; idx < BETTER_TOGETHER_PAD_SLOT_CUNT; idx++)
    {
        if (!(mask & (LDG_BIT(idx)))) { continue; }

        candidate = idx;
        cunt++;
    }
    if (ctx->auto_host && cunt == BETTER_TOGETHER_PAD_SOLE_CUNT)
    {
        ctx->report.host = candidate;
        ctx->auto_host = LDG_TRUTH_FALSE;
    }

    if (ctx->report.host >= BETTER_TOGETHER_PAD_SLOT_CUNT || !(mask & (LDG_BIT(ctx->report.host)))) { return; }

    if (ctx->report.guest < BETTER_TOGETHER_PAD_SLOT_CUNT) { return; }

    candidate = UINT32_MAX;
    cunt = 0;
    for (idx = 0; idx < BETTER_TOGETHER_PAD_SLOT_CUNT; idx++)
    {
        if (idx == ctx->report.host || !(mask & (LDG_BIT(idx)))) { continue; }

        candidate = idx;
        cunt++;
    }
    if (cunt == BETTER_TOGETHER_PAD_SOLE_CUNT) { ctx->report.guest = candidate; }
}

uint64_t better_together_pad_size_get(void)
{
    return sizeof(better_together_pad_ctx_t);
}

uint32_t better_together_pad_init(void *storage, uint64_t cap)
{
    better_together_pad_ctx_t *ctx = (better_together_pad_ctx_t *)storage;
    const char dll_name[BETTER_TOGETHER_PAD_DLL_CUNT][BETTER_TOGETHER_PAD_DLL_NAME_CAP] = { BETTER_TOGETHER_PAD_DLL_XINPUT_1_3, BETTER_TOGETHER_PAD_DLL_XINPUT_1_4, BETTER_TOGETHER_PAD_DLL_XINPUT_9_1_0 };
    FARPROC sym = 0x0;
    uint32_t idx = 0;
    uint32_t pass = 0;
    uint32_t res = LDG_ERR_AOK;

    if (LDG_UNLIKELY(!ctx)) { return LDG_ERR_FUNC_ARG_NULL; }

    if (LDG_UNLIKELY(cap < sizeof(*ctx) || ((uintptr_t)storage & (BETTER_TOGETHER_PAD_QWORD_ALIGN_MASK)))) { return LDG_ERR_FUNC_ARG_INVALID; }

    if (LDG_UNLIKELY(ctx->magic == BETTER_TOGETHER_PAD_MAGIC)) { return LDG_ERR_EXISTS; }

    res = ldg_mem_secure_zero(ctx, sizeof(*ctx));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    ctx->magic = BETTER_TOGETHER_PAD_MAGIC;
    ctx->auto_host = LDG_TRUTH_TRUE;
    ctx->report.host = UINT32_MAX;
    ctx->report.guest = UINT32_MAX;
    ctx->report.provider = UINT32_MAX;
    for (pass = 0; pass < 2; pass++)
    {
        for (idx = 0; idx < BETTER_TOGETHER_PAD_DLL_CUNT; idx++)
        {
            ctx->module = pass ? LoadLibraryExA(dll_name[idx], 0x0, LOAD_LIBRARY_SEARCH_SYSTEM32) : GetModuleHandleA(dll_name[idx]);
            if (!ctx->module) { continue; }

            sym = GetProcAddress(ctx->module, "XInputGetState");
            if (!sym) { continue; }

            if (LDG_UNLIKELY(!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_PIN, dll_name[idx], &ctx->module))) { return LDG_ERR_DENIED; }

            res = ldg_mem_secure_cpy(&ctx->get, &sym, sizeof(ctx->get));
            if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

            ctx->report.provider = idx;
            return LDG_ERR_AOK;
        }
    }
    return LDG_ERR_NOT_FOUND;
}

uint32_t better_together_pad_sel(void *storage, uint32_t host, uint32_t guest)
{
    better_together_pad_ctx_t *ctx = (better_together_pad_ctx_t *)storage;
    uint32_t res = LDG_ERR_AOK;

    if (LDG_UNLIKELY(!ctx)) { return LDG_ERR_FUNC_ARG_NULL; }

    if (LDG_UNLIKELY(ctx->magic != BETTER_TOGETHER_PAD_MAGIC)) { return LDG_ERR_NOT_INIT; }

    if (LDG_UNLIKELY(host >= BETTER_TOGETHER_PAD_SLOT_CUNT || (guest != UINT32_MAX && (guest >= BETTER_TOGETHER_PAD_SLOT_CUNT || guest == host)))) { return LDG_ERR_FUNC_ARG_INVALID; }

    ctx->auto_host = LDG_TRUTH_FALSE;
    ctx->report.host = host;
    ctx->report.guest = guest;
    ctx->report.rebind = 0;
    res = ldg_mem_secure_zero(ctx->samples, sizeof(ctx->samples));
    if (res == LDG_ERR_AOK) { res = ldg_mem_secure_zero(ctx->online, sizeof(ctx->online)); }

    return res;
}

static uint32_t better_together_pad_state_apply(better_together_pad_ctx_t *ctx, const XINPUT_STATE *states, uint32_t mask)
{
    uint32_t idx = 0;
    uint32_t slot = UINT32_MAX;
    uint32_t res = LDG_ERR_AOK;
    uint8_t online = LDG_TRUTH_FALSE;

    if ((ctx->online[BETTER_TOGETHER_PAD_HOST] && !(mask & (LDG_BIT(ctx->report.host)))) || (ctx->online[BETTER_TOGETHER_PAD_GUEST] && !(mask & (LDG_BIT(ctx->report.guest))))) { ctx->report.rebind = LDG_TRUTH_TRUE; }

    if (!ctx->report.rebind) { better_together_pad_slots_sel(ctx, mask); }

    for (idx = 0; idx < BETTER_TOGETHER_PAD_PLAYER_CUNT; idx++)
    {
        slot = idx == BETTER_TOGETHER_PAD_HOST ? ctx->report.host : ctx->report.guest;
        online = !ctx->report.rebind && slot < BETTER_TOGETHER_PAD_SLOT_CUNT && (mask & (LDG_BIT(slot)));
        res = ldg_mem_secure_cpy(ctx->samples[idx].prior, ctx->samples[idx].current, sizeof(ctx->samples[idx].prior));
        if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

        if (online)
        {
            better_together_pad_sample_get(&states[slot].Gamepad, ctx->samples[idx].current);
            if (!ctx->online[idx])
            {
                res = ldg_mem_secure_cpy(ctx->samples[idx].prior, ctx->samples[idx].current, sizeof(ctx->samples[idx].prior));
                if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }
            }
        }
        else
        {
            res = ldg_mem_secure_zero(&ctx->samples[idx], sizeof(ctx->samples[idx]));
            if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }
        }

        ctx->online[idx] = online;
    }
    ctx->report.connected = mask;
    ctx->report.frames++;
    return LDG_ERR_AOK;
}

uint32_t better_together_pad_poll(void *storage, better_together_pad_sample_t *out, better_together_pad_report_t *report)
{
    better_together_pad_ctx_t *ctx = (better_together_pad_ctx_t *)storage;
    XINPUT_STATE states[BETTER_TOGETHER_PAD_SLOT_CUNT] = LDG_ARR_ZERO_INIT;
    uint32_t mask = 0;
    uint32_t idx = 0;
    uint32_t res = LDG_ERR_AOK;

    if (LDG_UNLIKELY(!ctx || !out || !report)) { return LDG_ERR_FUNC_ARG_NULL; }

    if (LDG_UNLIKELY(ctx->magic != BETTER_TOGETHER_PAD_MAGIC || !ctx->get)) { return LDG_ERR_NOT_INIT; }

    for (idx = 0; idx < BETTER_TOGETHER_PAD_SLOT_CUNT; idx++) { if (ctx->get(idx, &states[idx]) == ERROR_SUCCESS) { mask |= LDG_BIT(idx); } }
    res = better_together_pad_state_apply(ctx, states, mask);
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    res = ldg_mem_secure_cpy(out, ctx->samples, sizeof(ctx->samples));
    if (res == LDG_ERR_AOK) { res = ldg_mem_secure_cpy(report, &ctx->report, sizeof(*report)); }

    return res;
}
