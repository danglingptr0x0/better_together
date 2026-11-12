#define COBJMACROS
#include <stdint.h>
#include <windows.h>
#include <commctrl.h>
#include <d3d11.h>
#include <dxgi.h>

#include <MinHook.h>
#include <dangling/core/err.h>
#include <dangling/core/macros.h>
#include <dangling/mem/alloc.h>
#include <dangling/mem/secure.h>
#include <libresteam/ext/together/window.h>

#define BETTER_TOGETHER_WINDOW_PRESENT_SLOT 8U
#define BETTER_TOGETHER_WINDOW_FULLSCREEN_SLOT 10U
#define BETTER_TOGETHER_WINDOW_HOOK_CUNT 2U
#define BETTER_TOGETHER_WINDOW_CODE_SLOT_BYTES 32U
#define BETTER_TOGETHER_WINDOW_PROBE_SIDE 16U
#define BETTER_TOGETHER_WINDOW_STYLE_FRAME (WS_CAPTION | WS_THICKFRAME | WS_BORDER | WS_DLGFRAME)
#define BETTER_TOGETHER_WINDOW_EX_FRAME (WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE | WS_EX_TOPMOST)
#define BETTER_TOGETHER_WINDOW_SYSCMD_MASK 0xFFF0U
#define BETTER_TOGETHER_WINDOW_RETRY_MS 1000U
#define BETTER_TOGETHER_WINDOW_ATOMIC_RD_VAL 0
#define BETTER_TOGETHER_WINDOW_RETRY_NOW 0
#define BETTER_TOGETHER_WINDOW_MSG_HANDLED 0
#define BETTER_TOGETHER_WINDOW_MSG_NO_PARAM 0
#define BETTER_TOGETHER_WINDOW_PROBE_ORIG 0
#define BETTER_TOGETHER_WINDOW_PROBE_SAMPLE_CUNT 1U
#define BETTER_TOGETHER_WINDOW_PROBE_BUFF_CUNT 1U
#define BETTER_TOGETHER_WINDOW_DEV_FLAGS 0U
#define BETTER_TOGETHER_WINDOW_FEATURE_LEVEL_CUNT 0U
#define BETTER_TOGETHER_WINDOW_POS_KEEP 0
#define BETTER_TOGETHER_WINDOW_MATCH_CUNT 1U

typedef HRESULT (STDMETHODCALLTYPE *better_together_window_present_cb_t)(IDXGISwapChain *, UINT, UINT);
typedef HRESULT (STDMETHODCALLTYPE *better_together_window_fullscreen_cb_t)(IDXGISwapChain *, BOOL, IDXGIOutput *);

typedef struct better_together_window_ctx
{
    HWND window;
    HMODULE module;
    uint32_t thread;
    uint32_t matches;
    UINT msg;
    LONG active;
    LONG borderless;
    LONG pending;
    LONG applying;
    LONG res;
    LONG native_res;
    LONG hook_res;
    LONG64 presents;
    LONG64 rejected;
    LONG64 next_retry;
    LONG style;
    LONG ex_style;
    WINDOWPLACEMENT placement;
    void *code;
    void *tgt[BETTER_TOGETHER_WINDOW_HOOK_CUNT];
    better_together_window_present_cb_t present;
    better_together_window_fullscreen_cb_t fullscreen;
    uint8_t subclass;
} better_together_window_ctx_t;

static BOOL CALLBACK better_together_window_find_cb(HWND window, LPARAM param)
{
    better_together_window_ctx_t *ctx = (better_together_window_ctx_t *)(uintptr_t)param;
    DWORD pid = 0;
    DWORD thread = GetWindowThreadProcessId(window, &pid);

    if (pid != GetCurrentProcessId() || thread != ctx->thread || !IsWindowVisible(window) || GetWindow(window, GW_OWNER)) { return TRUE; }

    ctx->window = window;
    ctx->matches++;
    return TRUE;
}

static uint32_t better_together_window_style_set(HWND window, uint32_t idx, LONG style)
{
    LONG_PTR previous = 0;

    SetLastError(ERROR_SUCCESS);
    previous = SetWindowLongPtrW(window, (int32_t)idx, (LONG_PTR)style);
    return previous || GetLastError() == ERROR_SUCCESS ? LDG_ERR_AOK : LDG_ERR_INVALID;
}

static uint32_t better_together_window_apply(better_together_window_ctx_t *ctx)
{
    MONITORINFO monitor = LDG_STRUCT_ZERO_INIT;
    HMONITOR tgt = MonitorFromWindow(ctx->window, MONITOR_DEFAULTTONEAREST);
    uint32_t res = LDG_ERR_AOK;
    LONG previous = 0;
    LONG style = GetWindowLongW(ctx->window, GWL_STYLE);
    LONG ex_style = GetWindowLongW(ctx->window, GWL_EXSTYLE);

    monitor.cbSize = sizeof(monitor);
    if (LDG_UNLIKELY(!tgt || !GetMonitorInfoW(tgt, &monitor))) { return LDG_ERR_NOT_FOUND; }

    res = better_together_window_style_set(ctx->window, (uint32_t)GWL_STYLE, (style & ~BETTER_TOGETHER_WINDOW_STYLE_FRAME) | WS_POPUP);
    if (res == LDG_ERR_AOK) { res = better_together_window_style_set(ctx->window, (uint32_t)GWL_EXSTYLE, ex_style & ~BETTER_TOGETHER_WINDOW_EX_FRAME); }

    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(!SetWindowPos(ctx->window, HWND_NOTOPMOST, monitor.rcMonitor.left, monitor.rcMonitor.top, monitor.rcMonitor.right - monitor.rcMonitor.left, monitor.rcMonitor.bottom - monitor.rcMonitor.top, SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_NOOWNERZORDER))) { return LDG_ERR_INVALID; }

    previous = InterlockedExchange(&ctx->borderless, LDG_TRUTH_TRUE);
    return previous == 0 || previous == LDG_TRUTH_TRUE ? LDG_ERR_AOK : LDG_ERR_INVALID;
}

static LRESULT CALLBACK better_together_window_msg_cb(HWND window, UINT msg, WPARAM word, LPARAM param, UINT_PTR id, DWORD_PTR user)
{
    better_together_window_ctx_t *ctx = (better_together_window_ctx_t *)(uintptr_t)user;
    LONG previous __attribute__((unused)) = 0;
    uint32_t res = LDG_ERR_AOK;

    if (msg == WM_NCDESTROY)
    {
        previous = InterlockedExchange(&ctx->active, LDG_TRUTH_FALSE);
        ctx->subclass = RemoveWindowSubclass(window, better_together_window_msg_cb, id) ? LDG_TRUTH_FALSE : LDG_TRUTH_TRUE;
        return DefSubclassProc(window, msg, word, param);
    }

    if (!InterlockedCompareExchange(&ctx->active, LDG_TRUTH_FALSE, LDG_TRUTH_FALSE)) { return DefSubclassProc(window, msg, word, param); }

    if (msg == ctx->msg)
    {
        res = better_together_window_apply(ctx);
        previous = InterlockedExchange(&ctx->res, (LONG)res);
        previous = InterlockedExchange(&ctx->pending, LDG_TRUTH_FALSE);
        return BETTER_TOGETHER_WINDOW_MSG_HANDLED;
    }

    if ((msg == WM_ACTIVATEAPP && !word) || (msg == WM_ACTIVATE && LOWORD(word) == WA_INACTIVE) || msg == WM_KILLFOCUS)
    {
        if (!ClipCursor(0x0)) { previous = InterlockedExchange(&ctx->res, LDG_ERR_INVALID); }

        if (GetCapture() == window && !ReleaseCapture()) { previous = InterlockedExchange(&ctx->res, LDG_ERR_INVALID); }

        return BETTER_TOGETHER_WINDOW_MSG_HANDLED;
    }

    if (msg == WM_SYSCOMMAND && (word & BETTER_TOGETHER_WINDOW_SYSCMD_MASK) == SC_KEYMENU && param == VK_RETURN) { return BETTER_TOGETHER_WINDOW_MSG_HANDLED; }

    if (msg == WM_STYLECHANGING && InterlockedCompareExchange(&ctx->borderless, LDG_TRUTH_FALSE, LDG_TRUTH_FALSE))
    {
        STYLESTRUCT *style = (STYLESTRUCT *)(uintptr_t)param;

        if ((int32_t)word == GWL_STYLE) { style->styleNew = (style->styleNew & ~BETTER_TOGETHER_WINDOW_STYLE_FRAME) | WS_POPUP; }

        if ((int32_t)word == GWL_EXSTYLE) { style->styleNew &= ~BETTER_TOGETHER_WINDOW_EX_FRAME; }
    }

    if (msg == WM_WINDOWPOSCHANGING && InterlockedCompareExchange(&ctx->borderless, LDG_TRUTH_FALSE, LDG_TRUTH_FALSE) && !IsIconic(window))
    {
        WINDOWPOS *pos = (WINDOWPOS *)(uintptr_t)param;
        MONITORINFO monitor = LDG_STRUCT_ZERO_INIT;
        HMONITOR tgt = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);

        monitor.cbSize = sizeof(monitor);
        if (tgt && GetMonitorInfoW(tgt, &monitor))
        {
            pos->x = monitor.rcMonitor.left;
            pos->y = monitor.rcMonitor.top;
            pos->cx = monitor.rcMonitor.right - monitor.rcMonitor.left;
            pos->cy = monitor.rcMonitor.bottom - monitor.rcMonitor.top;
            pos->flags &= ~(SWP_NOMOVE | SWP_NOSIZE);
        }
    }

    return DefSubclassProc(window, msg, word, param);
}

static uint8_t better_together_window_chain_is(better_together_window_ctx_t *ctx, IDXGISwapChain *chain)
{
    DXGI_SWAP_CHAIN_DESC desc = LDG_STRUCT_ZERO_INIT;
    HRESULT res = IDXGISwapChain_GetDesc(chain, &desc);

    return SUCCEEDED(res) && desc.OutputWindow == ctx->window ? LDG_TRUTH_TRUE : LDG_TRUTH_FALSE;
}

static HRESULT STDMETHODCALLTYPE better_together_window_fullscreen_cb(IDXGISwapChain *chain, BOOL fullscreen, IDXGIOutput *output, better_together_window_ctx_t *ctx)
{
    LONG64 previous __attribute__((unused)) = 0;
    LONG previous32 __attribute__((unused)) = 0;

    if (InterlockedCompareExchange(&ctx->active, LDG_TRUTH_FALSE, LDG_TRUTH_FALSE) && better_together_window_chain_is(ctx, chain) == LDG_TRUTH_TRUE)
    {
        if (fullscreen) { previous = InterlockedIncrement64(&ctx->rejected); }

        previous32 = InterlockedExchange(&ctx->borderless, LDG_TRUTH_FALSE);
        previous = InterlockedExchange64(&ctx->next_retry, BETTER_TOGETHER_WINDOW_RETRY_NOW);
        return ctx->fullscreen(chain, FALSE, 0x0);
    }

    return ctx->fullscreen(chain, fullscreen, output);
}

static uint32_t better_together_window_chain_apply(better_together_window_ctx_t *ctx, IDXGISwapChain *chain)
{
    IDXGIFactory *factory = 0x0;
    BOOL fullscreen = FALSE;
    HRESULT native_res = IDXGISwapChain_GetFullscreenState(chain, &fullscreen, 0x0);
    ULONG refs __attribute__((unused)) = 0;
    LONG previous __attribute__((unused)) = 0;

    if (SUCCEEDED(native_res) && fullscreen) { native_res = ctx->fullscreen(chain, FALSE, 0x0); }

    if (SUCCEEDED(native_res) && (!InterlockedCompareExchange(&ctx->borderless, LDG_TRUTH_FALSE, LDG_TRUTH_FALSE) || fullscreen))
    {
        native_res = IDXGISwapChain_GetParent(chain, &IID_IDXGIFactory, (void **)&factory);
        if (SUCCEEDED(native_res))
        {
            native_res = IDXGIFactory_MakeWindowAssociation(factory, ctx->window, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);
            refs = IDXGIFactory_Release(factory);
        }

        if (SUCCEEDED(native_res) && !InterlockedCompareExchange(&ctx->pending, LDG_TRUTH_TRUE, LDG_TRUTH_FALSE)) { if (!PostMessageW(ctx->window, ctx->msg, BETTER_TOGETHER_WINDOW_MSG_NO_PARAM, BETTER_TOGETHER_WINDOW_MSG_NO_PARAM))
            {
                previous = InterlockedExchange(&ctx->pending, LDG_TRUTH_FALSE);
                native_res = E_FAIL;
            }
        }
    }

    previous = InterlockedExchange(&ctx->native_res, (LONG)native_res);
    return SUCCEEDED(native_res) ? LDG_ERR_AOK : LDG_ERR_INVALID;
}

static HRESULT STDMETHODCALLTYPE better_together_window_present_cb(IDXGISwapChain *chain, UINT sync, UINT flags, better_together_window_ctx_t *ctx)
{
    DWORD saved = GetLastError();
    uint64_t now = GetTickCount64();
    uint32_t res = LDG_ERR_AOK;
    LONG previous __attribute__((unused)) = 0;
    LONG64 previous64 __attribute__((unused)) = 0;

    if (InterlockedCompareExchange(&ctx->active, LDG_TRUTH_FALSE, LDG_TRUTH_FALSE) && !(flags & DXGI_PRESENT_TEST) && better_together_window_chain_is(ctx, chain) == LDG_TRUTH_TRUE)
    {
        previous64 = InterlockedIncrement64(&ctx->presents);
        if (now >= (uint64_t)InterlockedCompareExchange64(&ctx->next_retry, BETTER_TOGETHER_WINDOW_ATOMIC_RD_VAL, BETTER_TOGETHER_WINDOW_ATOMIC_RD_VAL) && !InterlockedCompareExchange(&ctx->applying, LDG_TRUTH_TRUE, LDG_TRUTH_FALSE))
        {
            if (InterlockedCompareExchange(&ctx->active, LDG_TRUTH_FALSE, LDG_TRUTH_FALSE)) { res = better_together_window_chain_apply(ctx, chain); }

            if (res != LDG_ERR_AOK) { previous = InterlockedExchange(&ctx->res, (LONG)res); }

            previous64 = InterlockedExchange64(&ctx->next_retry, (LONG64)(now + BETTER_TOGETHER_WINDOW_RETRY_MS));
            previous = InterlockedExchange(&ctx->applying, LDG_TRUTH_FALSE);
        }
    }

    SetLastError(saved);
    return ctx->present(chain, sync, flags);
}

static uint32_t better_together_window_probe(better_together_window_ctx_t *ctx)
{
    DXGI_SWAP_CHAIN_DESC desc = LDG_STRUCT_ZERO_INIT;
    IDXGISwapChain *chain = 0x0;
    ID3D11Device *dev = 0x0;
    HWND window = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, L"STATIC", L"LibreSteam window probe", WS_POPUP, BETTER_TOGETHER_WINDOW_PROBE_ORIG, BETTER_TOGETHER_WINDOW_PROBE_ORIG, BETTER_TOGETHER_WINDOW_PROBE_SIDE, BETTER_TOGETHER_WINDOW_PROBE_SIDE, 0x0, 0x0, ctx->module, 0x0);
    HRESULT native_res = S_OK;
    ULONG refs __attribute__((unused)) = 0;

    if (LDG_UNLIKELY(!window)) { return LDG_ERR_ALLOC_NULL; }

    desc.BufferDesc.Width = BETTER_TOGETHER_WINDOW_PROBE_SIDE;
    desc.BufferDesc.Height = BETTER_TOGETHER_WINDOW_PROBE_SIDE;
    desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = BETTER_TOGETHER_WINDOW_PROBE_SAMPLE_CUNT;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = BETTER_TOGETHER_WINDOW_PROBE_BUFF_CUNT;
    desc.OutputWindow = window;
    desc.Windowed = TRUE;
    desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    native_res = D3D11CreateDeviceAndSwapChain(0x0, D3D_DRIVER_TYPE_HARDWARE, 0x0, BETTER_TOGETHER_WINDOW_DEV_FLAGS, 0x0, BETTER_TOGETHER_WINDOW_FEATURE_LEVEL_CUNT, D3D11_SDK_VERSION, &desc, &chain, &dev, 0x0, 0x0);
    ctx->native_res = (LONG)native_res;
    if (SUCCEEDED(native_res) && chain)
    {
        ctx->tgt[0] = (*(void ***)chain)[BETTER_TOGETHER_WINDOW_PRESENT_SLOT];
        ctx->tgt[1] = (*(void ***)chain)[BETTER_TOGETHER_WINDOW_FULLSCREEN_SLOT];
    }

    if (chain) { refs = IDXGISwapChain_Release(chain); }

    if (dev) { refs = ID3D11Device_Release(dev); }

    if (LDG_UNLIKELY(!DestroyWindow(window))) { return LDG_ERR_INVALID; }

    return SUCCEEDED(native_res) && ctx->tgt[0] && ctx->tgt[1] ? LDG_ERR_AOK : LDG_ERR_UNSUPPORTED;
}

static uint32_t better_together_window_code_init(better_together_window_ctx_t *ctx)
{
    uint8_t code[BETTER_TOGETHER_WINDOW_CODE_SLOT_BYTES * BETTER_TOGETHER_WINDOW_HOOK_CUNT] = LDG_ARR_ZERO_INIT;
    uintptr_t cb[BETTER_TOGETHER_WINDOW_HOOK_CUNT] = LDG_ARR_ZERO_INIT;
    uintptr_t user = (uintptr_t)ctx;
    HRESULT(STDMETHODCALLTYPE * present)(IDXGISwapChain *, UINT, UINT, better_together_window_ctx_t *) = better_together_window_present_cb;
    HRESULT(STDMETHODCALLTYPE * fullscreen)(IDXGISwapChain *, BOOL, IDXGIOutput *, better_together_window_ctx_t *) = better_together_window_fullscreen_cb;
    uint32_t idx = 0;
    uint32_t off = 0;
    uint32_t res = ldg_mem_secure_cpy(&cb[0], &present, sizeof(present));
    DWORD previous = 0;

    if (res == LDG_ERR_AOK) { res = ldg_mem_secure_cpy(&cb[1], &fullscreen, sizeof(fullscreen)); }

    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    for (idx = 0; idx < BETTER_TOGETHER_WINDOW_HOOK_CUNT; idx++)
    {
        off = idx * BETTER_TOGETHER_WINDOW_CODE_SLOT_BYTES;
        code[off++] = 0x49;
        code[off++] = 0xB9;
        res = ldg_mem_secure_cpy(code + off, &user, sizeof(user));
        off += sizeof(user);
        code[off++] = 0x48;
        code[off++] = 0xB8;
        if (res == LDG_ERR_AOK) { res = ldg_mem_secure_cpy(code + off, &cb[idx], sizeof(cb[idx])); }

        off += sizeof(cb[idx]);
        code[off++] = 0xFF;
        code[off] = 0xE0;
        if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }
    }
    if (LDG_UNLIKELY(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN, (LPCWSTR)cb[0], &ctx->module))) { return LDG_ERR_NOT_FOUND; }

    ctx->code = VirtualAlloc(0x0, sizeof(code), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (LDG_UNLIKELY(!ctx->code)) { return LDG_ERR_ALLOC_NULL; }

    res = ldg_mem_secure_cpy(ctx->code, code, sizeof(code));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(!VirtualProtect(ctx->code, sizeof(code), PAGE_EXECUTE_READ, &previous))) { return LDG_ERR_DENIED; }

    return FlushInstructionCache(GetCurrentProcess(), ctx->code, sizeof(code)) ? LDG_ERR_AOK : LDG_ERR_INVALID;
}

uint32_t better_together_window_stop(void *opaque)
{
    better_together_window_ctx_t *ctx = opaque;
    LONG previous __attribute__((unused)) = 0;
    uint32_t res = LDG_ERR_AOK;

    if (LDG_UNLIKELY(!ctx)) { return LDG_ERR_FUNC_ARG_NULL; }

    if (LDG_UNLIKELY(GetCurrentThreadId() != ctx->thread)) { return LDG_ERR_DENIED; }

    previous = InterlockedExchange(&ctx->active, LDG_TRUTH_FALSE);
    previous = InterlockedExchange(&ctx->borderless, LDG_TRUTH_FALSE);
    if (ctx->subclass && IsWindow(ctx->window))
    {
        if (LDG_UNLIKELY(!RemoveWindowSubclass(ctx->window, better_together_window_msg_cb, (UINT_PTR)ctx))) { return LDG_ERR_INVALID; }

        ctx->subclass = LDG_TRUTH_FALSE;
        res = better_together_window_style_set(ctx->window, (uint32_t)GWL_STYLE, ctx->style);
        if (res == LDG_ERR_AOK) { res = better_together_window_style_set(ctx->window, (uint32_t)GWL_EXSTYLE, ctx->ex_style); }

        if (res == LDG_ERR_AOK && !SetWindowPlacement(ctx->window, &ctx->placement)) { res = LDG_ERR_INVALID; }

        if (res == LDG_ERR_AOK && !SetWindowPos(ctx->window, 0x0, BETTER_TOGETHER_WINDOW_POS_KEEP, BETTER_TOGETHER_WINDOW_POS_KEEP, BETTER_TOGETHER_WINDOW_POS_KEEP, BETTER_TOGETHER_WINDOW_POS_KEEP, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED)) { res = LDG_ERR_INVALID; }
    }

    return res;
}

uint32_t better_together_window_report(void *opaque, better_together_window_report_t *out)
{
    better_together_window_ctx_t *ctx = opaque;

    if (LDG_UNLIKELY(!ctx || !out)) { return LDG_ERR_FUNC_ARG_NULL; }

    out->window = (uint64_t)(uintptr_t)ctx->window;
    out->thread = ctx->thread;
    out->active = (uint32_t)InterlockedCompareExchange(&ctx->active, LDG_TRUTH_FALSE, LDG_TRUTH_FALSE);
    out->borderless = (uint32_t)InterlockedCompareExchange(&ctx->borderless, LDG_TRUTH_FALSE, LDG_TRUTH_FALSE);
    out->presents = (uint64_t)InterlockedCompareExchange64(&ctx->presents, BETTER_TOGETHER_WINDOW_ATOMIC_RD_VAL, BETTER_TOGETHER_WINDOW_ATOMIC_RD_VAL);
    out->rejected = (uint64_t)InterlockedCompareExchange64(&ctx->rejected, BETTER_TOGETHER_WINDOW_ATOMIC_RD_VAL, BETTER_TOGETHER_WINDOW_ATOMIC_RD_VAL);
    out->res = (uint32_t)InterlockedCompareExchange(&ctx->res, BETTER_TOGETHER_WINDOW_ATOMIC_RD_VAL, BETTER_TOGETHER_WINDOW_ATOMIC_RD_VAL);
    out->native_res = (uint32_t)InterlockedCompareExchange(&ctx->native_res, BETTER_TOGETHER_WINDOW_ATOMIC_RD_VAL, BETTER_TOGETHER_WINDOW_ATOMIC_RD_VAL);
    out->hook_res = (uint32_t)InterlockedCompareExchange(&ctx->hook_res, BETTER_TOGETHER_WINDOW_ATOMIC_RD_VAL, BETTER_TOGETHER_WINDOW_ATOMIC_RD_VAL);
    return LDG_ERR_AOK;
}

uint32_t better_together_window_init(void **out)
{
    better_together_window_ctx_t *ctx = 0x0;
    void *orig = 0x0;
    uint32_t idx = 0;
    uint32_t res = LDG_ERR_AOK;
    LONG previous = 0;
    MH_STATUS hook = MH_OK;

    if (LDG_UNLIKELY(!out)) { return LDG_ERR_FUNC_ARG_NULL; }

    *out = 0x0;
    res = ldg_mem_alloc(sizeof(*ctx), out);
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    ctx = *out;
    res = ldg_mem_secure_zero(ctx, sizeof(*ctx));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    ctx->thread = GetCurrentThreadId();
    if (LDG_UNLIKELY(!EnumThreadWindows(ctx->thread, better_together_window_find_cb, (LPARAM)(uintptr_t)ctx) || ctx->matches != BETTER_TOGETHER_WINDOW_MATCH_CUNT)) { return LDG_ERR_NOT_FOUND; }

    ctx->placement.length = sizeof(ctx->placement);
    if (LDG_UNLIKELY(!GetWindowPlacement(ctx->window, &ctx->placement))) { return LDG_ERR_INVALID; }

    ctx->style = GetWindowLongW(ctx->window, GWL_STYLE);
    ctx->ex_style = GetWindowLongW(ctx->window, GWL_EXSTYLE);
    ctx->msg = RegisterWindowMessageW(L"LibreSteam.Together.Borderless.Apply");
    if (LDG_UNLIKELY(!ctx->msg)) { return LDG_ERR_INVALID; }

    res = better_together_window_code_init(ctx);
    if (res == LDG_ERR_AOK) { res = better_together_window_probe(ctx); }

    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    hook = MH_Initialize();
    if (hook != MH_OK && hook != MH_ERROR_ALREADY_INITIALIZED) { ctx->hook_res = (LONG)hook; return LDG_ERR_INVALID; }

    for (idx = 0; idx < BETTER_TOGETHER_WINDOW_HOOK_CUNT; idx++)
    {
        hook = MH_CreateHook(ctx->tgt[idx], (uint8_t *)ctx->code + idx * BETTER_TOGETHER_WINDOW_CODE_SLOT_BYTES, &orig);
        if (hook != MH_OK) { ctx->hook_res = (LONG)hook; return LDG_ERR_UNSUPPORTED; }

        res = idx ? ldg_mem_secure_cpy(&ctx->fullscreen, &orig, sizeof(orig)) : ldg_mem_secure_cpy(&ctx->present, &orig, sizeof(orig));
        if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }
    }
    if (LDG_UNLIKELY(!SetWindowSubclass(ctx->window, better_together_window_msg_cb, (UINT_PTR)ctx, (DWORD_PTR)ctx))) { return LDG_ERR_INVALID; }

    ctx->subclass = LDG_TRUTH_TRUE;
    for (idx = 0; idx < BETTER_TOGETHER_WINDOW_HOOK_CUNT; idx++)
    {
        hook = MH_EnableHook(ctx->tgt[idx]);
        if (hook != MH_OK) { ctx->hook_res = (LONG)hook; return LDG_ERR_INVALID; }
    }
    previous = InterlockedExchange(&ctx->active, LDG_TRUTH_TRUE);
    return previous ? LDG_ERR_EXISTS : LDG_ERR_AOK;
}
