#include <stdint.h>
#include <windows.h>

#include <dangling/core/err.h>
#include <dangling/core/macros.h>
#include <dangling/mem/alloc.h>
#include <dangling/mem/secure.h>
#include <libresteam/ext/together/input.h>
#include <libresteam/ext/together/spawn.h>
#include <libresteam/ext/together/together.h>
#include <libresteam/ext/together/window.h>

#define BETTER_TOGETHER_OWNER_NONE 0U
#define BETTER_TOGETHER_READY_LOG "together ready; stage: native Boxie spawn; spawn ctx: 0x%x64; host X offset: -0.197623; host Y/Z preserved; camera: automatic X/Z midpoint; input: XInput; binding: sole connected controller first for host then sole additional controller for guest; unassigned slot: 4294967295; automatic respawn: 1"

typedef struct better_together_ctx
{
    const libresteam_shim_mod_api_t *api;
    void *mod;
    void *spawn;
    void *input;
    void *window;
    uint64_t next_window_log;
    uint32_t window_ready;
    uint32_t window_res;
    uint64_t next_input_log;
    uint64_t input_routed;
    uint64_t input_scan_passes;
    uint64_t root_calls;
    uint64_t root_errors;
    uint64_t cam_samples;
    uint32_t attach_state;
    uint32_t attach_epoch;
    uint64_t tex_attempts;
    uint64_t reported_owner;
    uint64_t reported_req;
    uint32_t input_ready;
    uint32_t input_res;
    uint32_t input_host;
    uint32_t input_guest;
    uint32_t input_mask;
    uint32_t input_rebind;
    uint32_t reported_state;
    uint32_t last_res;
    LONG active;
    LONG inflight;
} better_together_ctx_t;

static void better_together_tick_cb(void *user)
{
    better_together_ctx_t *ctx = (better_together_ctx_t *)user;
    better_together_spawn_report_t report = LDG_STRUCT_ZERO_INIT;
    better_together_input_report_t input = LDG_STRUCT_ZERO_INIT;
    better_together_window_report_t window = LDG_STRUCT_ZERO_INIT;
    uint64_t now = GetTickCount64();
    uint64_t routed = 0;
    LONG previous = 0;
    uint32_t res = LDG_ERR_AOK;

    if (!ctx || !InterlockedCompareExchange(&ctx->active, LDG_TRUTH_FALSE, LDG_TRUTH_FALSE)) { return; }

    previous = InterlockedCompareExchange(&ctx->inflight, LDG_TRUTH_TRUE, LDG_TRUTH_FALSE);
    if (previous) { return; }

    res = better_together_spawn_report(ctx->spawn, &report);
    if (res == LDG_ERR_AOK && (report.state != ctx->reported_state || report.owner != ctx->reported_owner))
    {
        ctx->last_res = ctx->api->log(ctx->mod, "together spawn; state: %u32; res: %u32; thread: %u32; owner: 0x%x64; entity: 0x%x64; original owner: 0x%x64; world: 0x%x64; main redirects: %u32; top redirects: %u32; bottom redirects: %u32; hook res: %u32", report.state, report.res, report.thread, report.owner, report.entity, report.orig, report.world, report.renamed[0], report.renamed[1], report.renamed[2], report.hook_res);
        if (ctx->last_res == LDG_ERR_AOK) { ctx->reported_state = report.state; ctx->reported_owner = report.owner; }
    }

    if (res == LDG_ERR_AOK)
    {
        if (report.req_cunt != ctx->reported_req)
        {
            ctx->last_res = ctx->api->log(ctx->mod, "together manual spawn; request: %u64; res: %u32; already present: %u32", report.req_cunt, report.req_res, report.req_existing);
            if (ctx->last_res == LDG_ERR_AOK) { ctx->reported_req = report.req_cunt; }
        }

        ctx->input_res = better_together_input_tick(ctx->input, report.state == BETTER_TOGETHER_SPAWN_COMPLETE ? report.owner : BETTER_TOGETHER_OWNER_NONE, report.thread);
        res = better_together_input_report(ctx->input, &input);
        if (res == LDG_ERR_AOK && input.tex.attempts != ctx->tex_attempts)
        {
            ctx->last_res = ctx->api->log(ctx->mod, "together texture; sky and grass applied: %u32; res: %u32; attempts: %u64", input.tex.applied, input.tex.res, input.tex.attempts);
            if (ctx->last_res == LDG_ERR_AOK) { ctx->tex_attempts = input.tex.attempts; }
        }

        if (res == LDG_ERR_AOK && input.cam_samples != ctx->cam_samples)
        {
            ctx->last_res = ctx->api->log(ctx->mod, "together camera; state: %u32; res: %u32; Yarny ID: %u32; Boxie ID: %u32; distance: %f; error: %s", input.cam_state, input.cam_res, input.cam_host, input.cam_guest, input.cam_dist, input.cam_err);
            if (ctx->last_res == LDG_ERR_AOK) { ctx->cam_samples = input.cam_samples; }
        }

        if (res == LDG_ERR_AOK && (input.attach_state != ctx->attach_state || input.attach_epoch != ctx->attach_epoch))
        {
            ctx->last_res = ctx->api->log(ctx->mod, "together attachment; epoch: %u32; state: %u32; error: %s", input.attach_epoch, input.attach_state, input.attach_err);
            if (ctx->last_res == LDG_ERR_AOK) { ctx->attach_state = input.attach_state; ctx->attach_epoch = input.attach_epoch; }
        }

        if (res == LDG_ERR_AOK && now >= ctx->next_input_log)
        {
            routed = input.routed[0] + input.routed[1] + input.routed[2];
            if (input.root_calls[0] + input.root_calls[1] != ctx->root_calls || input.root_errors != ctx->root_errors || input.ready != ctx->input_ready || routed != ctx->input_routed || input.scan_passes != ctx->input_scan_passes || ctx->input_res != LDG_ERR_AOK || input.pads.host != ctx->input_host || input.pads.guest != ctx->input_guest || input.pads.connected != ctx->input_mask || input.pads.rebind != ctx->input_rebind)
            {
                ctx->last_res = ctx->api->log(ctx->mod, "together input; ready: %u32; res: %u32; ctx: 0x%x64; owner: 0x%x64; closures: %u32/%u32/%u32; Boxie: %u64/%u64/%u64; Yarny: %u64; unknown: %u64; action: 0x%x32; scan passes: %u64; host slot: %u32; guest slot: %u32; connected mask: 0x%x32; rebind: %u32; provider: %u32; frames: %u64; hook res: %u32; Lua roots Yarny/Boxie: %u64/%u64; root errors: %u64; root res: %u32", input.ready, ctx->input_res, (uint64_t)(uintptr_t)ctx->input, input.owner, input.installed[0], input.installed[1], input.installed[2], input.routed[0], input.routed[1], input.routed[2], input.host_routed, input.unknown, input.unknown_action, input.scan_passes, input.pads.host, input.pads.guest, input.pads.connected, input.pads.rebind, input.pads.provider, input.pads.frames, input.hook_res, input.root_calls[0], input.root_calls[1], input.root_errors, input.root_res);
                if (ctx->last_res == LDG_ERR_AOK)
                {
                    ctx->input_ready = input.ready;
                    ctx->input_routed = routed;
                    ctx->input_scan_passes = input.scan_passes;
                    ctx->root_calls = input.root_calls[0] + input.root_calls[1];
                    ctx->root_errors = input.root_errors;
                    ctx->input_host = input.pads.host;
                    ctx->input_guest = input.pads.guest;
                    ctx->input_mask = input.pads.connected;
                    ctx->input_rebind = input.pads.rebind;
                }
            }

            ctx->next_input_log = now + LDG_MS_PER_SEC;
        }
    }

    if (now >= ctx->next_window_log)
    {
        res = better_together_window_report(ctx->window, &window);
        if (res == LDG_ERR_AOK && (window.borderless != ctx->window_ready || window.res != ctx->window_res))
        {
            ctx->last_res = ctx->api->log(ctx->mod, "together window; active: %u32; borderless: %u32; window: 0x%x64; thread: %u32; presents: %u64; rejected fullscreen: %u64; res: %u32; native res: 0x%x32; hook res: %u32", window.active, window.borderless, window.window, window.thread, window.presents, window.rejected, window.res, window.native_res, window.hook_res);
            if (ctx->last_res == LDG_ERR_AOK)
            {
                ctx->window_ready = window.borderless;
                ctx->window_res = window.res;
            }
        }

        ctx->next_window_log = now + LDG_MS_PER_SEC;
    }

    previous = InterlockedExchange(&ctx->inflight, LDG_TRUTH_FALSE);
    if (previous != LDG_TRUTH_TRUE) { ctx->last_res = LDG_ERR_INVALID; }
}

static void better_together_cleanup_cb(void *user)
{
    better_together_ctx_t *ctx = (better_together_ctx_t *)user;
    LONG previous = 0;
    uint32_t res = LDG_ERR_AOK;
    uint32_t window_res = LDG_ERR_AOK;

    if (!ctx) { return; }

    previous = InterlockedExchange(&ctx->active, LDG_TRUTH_FALSE);
    res = better_together_spawn_stop(ctx->spawn);
    if (res == LDG_ERR_AOK || res == LDG_ERR_NOT_INIT) { res = better_together_input_stop(ctx->input); }

    if (ctx->window) { window_res = better_together_window_stop(ctx->window); }

    ctx->last_res = ctx->api->log(ctx->mod, "together stopped; was active: %u32; res: %u32; window res: %u32; native objects and hook storage retained until process exit", (uint32_t)previous, res, window_res);
}

uint32_t better_together_init(const libresteam_shim_mod_api_t *api, void *mod)
{
    better_together_ctx_t *ctx = 0x0;
    void *mem = 0x0;
    uint64_t bytes = 0;
    uint64_t input_bytes = 0;
    uint32_t res = LDG_ERR_AOK;
    uint32_t stop_res = LDG_ERR_AOK;
    LONG previous = 0;

    if (LDG_UNLIKELY(!api || !mod)) { return LDG_ERR_FUNC_ARG_NULL; }

    if (LDG_UNLIKELY(api->version != LIBRESTEAM_SHIM_MOD_API_VERSION || api->size < sizeof(*api) || !api->log || !api->tick_reg || !api->cleanup_reg)) { return LDG_ERR_UNSUPPORTED; }

    bytes = better_together_spawn_size_get();
    if (LDG_UNLIKELY(bytes > UINT64_MAX - sizeof(*ctx))) { return LDG_ERR_OVERFLOW; }

    input_bytes = better_together_input_size_get();
    if (LDG_UNLIKELY(input_bytes > UINT64_MAX - sizeof(*ctx) - bytes)) { return LDG_ERR_OVERFLOW; }

    res = ldg_mem_init();
    if (res == LDG_ERR_AOK) { res = ldg_mem_alloc(sizeof(*ctx) + bytes + input_bytes, &mem); }

    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    res = ldg_mem_secure_zero(mem, sizeof(*ctx) + bytes + input_bytes);
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    ctx = (better_together_ctx_t *)mem;
    ctx->api = api;
    ctx->mod = mod;
    ctx->spawn = (uint8_t *)mem + sizeof(*ctx);
    ctx->input = (uint8_t *)ctx->spawn + bytes;
    res = better_together_input_init(ctx->input, input_bytes);
    if (res != LDG_ERR_AOK)
    {
        ctx->last_res = api->log(mod, "together input init failed; res: %u32; restart required before retry", res);
        return res;
    }

    res = better_together_spawn_init(ctx->spawn, bytes, better_together_input_roots_bind, ctx->input);
    if (res != LDG_ERR_AOK)
    {
        stop_res = better_together_input_stop(ctx->input);
        ctx->last_res = api->log(mod, "together spawn init failed; res: %u32; input stop res: %u32; restart required before retry", res, stop_res);
        return res;
    }

    ctx->window_ready = UINT32_MAX;
    res = better_together_input_spawn_bind(ctx->input, better_together_spawn_tick, ctx->spawn);
    if (res != LDG_ERR_AOK)
    {
        better_together_cleanup_cb(ctx);
        return res;
    }

    res = better_together_window_init(&ctx->window);
    if (res != LDG_ERR_AOK)
    {
        ctx->last_res = api->log(mod, "together window init failed; res: %u32; restart required before retry", res);
        better_together_cleanup_cb(ctx);
        return res;
    }

    res = api->cleanup_reg(mod, better_together_cleanup_cb, ctx);
    if (res == LDG_ERR_AOK)
    {
        previous = InterlockedExchange(&ctx->active, LDG_TRUTH_TRUE);
        res = previous ? LDG_ERR_EXISTS : api->tick_reg(mod, better_together_tick_cb, ctx);
    }

    if (res == LDG_ERR_AOK) { res = api->log(mod, BETTER_TOGETHER_READY_LOG, (uint64_t)(uintptr_t)ctx->spawn); }

    if (res != LDG_ERR_AOK)
    {
        previous = InterlockedExchange(&ctx->active, LDG_TRUTH_FALSE);
        stop_res = better_together_window_stop(ctx->window);
        if (LDG_UNLIKELY(stop_res != LDG_ERR_AOK)) { return stop_res; }

        stop_res = better_together_spawn_stop(ctx->spawn);
        if (LDG_UNLIKELY(stop_res != LDG_ERR_AOK && stop_res != LDG_ERR_NOT_INIT)) { return stop_res; }

        stop_res = better_together_input_stop(ctx->input);
        if (LDG_UNLIKELY(stop_res != LDG_ERR_AOK)) { return stop_res; }

        if (LDG_UNLIKELY(previous != 0 && previous != LDG_TRUTH_TRUE)) { return LDG_ERR_INVALID; }
    }

    return res;
}
