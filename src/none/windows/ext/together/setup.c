#include <stdint.h>
#include <windows.h>

#include <MinHook.h>
#include <dangling/core/err.h>
#include <dangling/core/macros.h>
#include <dangling/mem/secure.h>
#include <dangling/str/fmt.h>
#include <libresteam/ext/together/setup.h>

#define BETTER_TOGETHER_SETUP_MAGIC UINT64_C(0x4258524F50453031)
#define BETTER_TOGETHER_SETUP_BACK_IDX 0U
#define BETTER_TOGETHER_SETUP_FRONT_IDX 1U
#define BETTER_TOGETHER_SETUP_NATIVE_NONE 0U
#define BETTER_TOGETHER_SETUP_RENAME_CUNT 1U
#define BETTER_TOGETHER_SETUP_QWORD_ALIGN_MASK (sizeof(uint64_t) - 1U)
#define BETTER_TOGETHER_SETUP_PE_TIMESTAMP 0x56952868U
#define BETTER_TOGETHER_SETUP_IMG_SIZE 0x04D13000U
#define BETTER_TOGETHER_SETUP_ENTRY_RVA 0x144ADB0U
#define BETTER_TOGETHER_SETUP_ROPE_RVA 0x14A7820U
#define BETTER_TOGETHER_SETUP_BIND_RVA 0x14AEA90U
#define BETTER_TOGETHER_SETUP_IK_RVA 0x14BE4A0U
#define BETTER_TOGETHER_SETUP_FRAME_RVA 0x144A710U
#define BETTER_TOGETHER_SETUP_WORK_RVA 0x1476180U
#define BETTER_TOGETHER_SETUP_PREPARE_RVA 0x14B25A0U
#define BETTER_TOGETHER_SETUP_FINISH_RVA 0x14B1B70U
#define BETTER_TOGETHER_SETUP_WORK_EN_RVA 0x852B08U
#define BETTER_TOGETHER_SETUP_GAME_OWNER_OFF 0x358U
#define BETTER_TOGETHER_SETUP_ROPE_ACTIVE_OFF 0x4D8U
#define BETTER_TOGETHER_SETUP_HASH_RVA 0x1796D30U
#define BETTER_TOGETHER_SETUP_COMP_RVA 0x17B93E0U
#define BETTER_TOGETHER_SETUP_GLOBAL_RVA 0x8D3AB8U
#define BETTER_TOGETHER_SETUP_COMP_ID_RVA 0x8D0EE8U
#define BETTER_TOGETHER_SETUP_ROPE_COMP_ID 12U
#define BETTER_TOGETHER_SETUP_OWNER_BYTES 0xB0U
#define BETTER_TOGETHER_SETUP_OWNER_DESC_OFF 0x90U
#define BETTER_TOGETHER_SETUP_ROPE_NAME_OFF 0x468U
#define BETTER_TOGETHER_SETUP_ROPE_ENTITY_OFF 0x808U
#define BETTER_TOGETHER_SETUP_ENTITY_HASH_OFF 0x08U
#define BETTER_TOGETHER_SETUP_COMP_OWNER_OFF 0x38U
#define BETTER_TOGETHER_SETUP_ROPE_CUNT 2U
#define BETTER_TOGETHER_SETUP_SMALL_STR_CAP 15U
#define BETTER_TOGETHER_SETUP_CODE_SLOT_BYTES 256U
#define BETTER_TOGETHER_SETUP_THUNK_ALL_CUNT (BETTER_TOGETHER_SETUP_THUNK_CUNT + 4U)
#define BETTER_TOGETHER_SETUP_CODE_BYTES (BETTER_TOGETHER_SETUP_CODE_SLOT_BYTES * BETTER_TOGETHER_SETUP_THUNK_ALL_CUNT)
#define BETTER_TOGETHER_SETUP_ENTRY_SLOT 0U
#define BETTER_TOGETHER_SETUP_ROPE_SLOT 1U
#define BETTER_TOGETHER_SETUP_BIND_SLOT 2U
#define BETTER_TOGETHER_SETUP_IK_SLOT 3U
#define BETTER_TOGETHER_SETUP_FRAME_SLOT 4U
#define BETTER_TOGETHER_SETUP_WORK_SLOT 5U
#define BETTER_TOGETHER_SETUP_STATE_READY 1U
#define BETTER_TOGETHER_SETUP_STATE_COMPLETE 2U
#define BETTER_TOGETHER_SETUP_STATE_FAILED 3U
#define BETTER_TOGETHER_SETUP_STATE_RUNNING 4U
#define BETTER_TOGETHER_SETUP_MUT_CAP 96U

typedef struct better_together_setup_name
{
    uint8_t text[BETTER_TOGETHER_SETUP_SMALL_STR_CAP + LDG_STR_TERM_SIZE];
    uint64_t len;
    uint64_t cap;
} better_together_setup_name_t;

typedef struct better_together_setup_hash_name
{
    uint64_t data;
    uint64_t len;
} better_together_setup_hash_name_t;

typedef uint64_t (*better_together_setup_entry_func_t)(uint64_t owner, uint64_t world, uint64_t arg);
typedef uint64_t (*better_together_setup_rope_func_t)(uint64_t self, uint64_t world, uint64_t params, const better_together_setup_name_t *name);
typedef uint32_t (*better_together_setup_hash_func_t)(const better_together_setup_hash_name_t *name);
typedef uint64_t (*better_together_setup_comp_func_t)(uint64_t entity, uint32_t kind);
typedef void (*better_together_setup_frame_func_t)(uint64_t owner, uint64_t frame);
typedef void (*better_together_setup_tick_func_t)(uint64_t object);

typedef struct better_together_setup_snapshot
{
    uint64_t owner;
    uint64_t world;
    uint64_t rope[BETTER_TOGETHER_SETUP_ROPE_CUNT];
    uint64_t attachment;
    uint64_t entity;
    uint64_t rope_entity[BETTER_TOGETHER_SETUP_ROPE_CUNT];
    uint64_t comp[BETTER_TOGETHER_SETUP_ROPE_CUNT];
    uint64_t back[BETTER_TOGETHER_SETUP_ROPE_CUNT];
} better_together_setup_snapshot_t;

typedef struct better_together_setup_scope
{
    uint64_t world;
    uint32_t thread;
    uint8_t active;
    uint8_t pudding[3];
    uint32_t renamed[BETTER_TOGETHER_SETUP_ROPE_CUNT];
    uint32_t res;
} better_together_setup_scope_t;

typedef struct better_together_setup_ctx
{
    uint64_t magic;
    uint64_t base;
    uint64_t owner;
    uint64_t attempts;
    uint64_t successes;
    better_together_setup_entry_func_t entry;
    better_together_setup_rope_func_t rope;
    void *bind;
    void *ik;
    better_together_setup_frame_func_t frame;
    better_together_setup_tick_func_t work;
    better_together_setup_tick_func_t prepare;
    better_together_setup_tick_func_t finish;
    better_together_setup_comp_func_t comp;
    void *code;
    HANDLE singleton;
    HMODULE module;
    SRWLOCK lock;
    RUNTIME_FUNCTION unwind[BETTER_TOGETHER_SETUP_THUNK_ALL_CUNT];
    better_together_setup_name_t names[BETTER_TOGETHER_SETUP_ROPE_CUNT];
    better_together_setup_scope_t scope;
    better_together_setup_snapshot_t orig;
    better_together_setup_snapshot_t boxie;
    uint32_t hashes[BETTER_TOGETHER_SETUP_ROPE_CUNT];
    uint32_t tls;
    uint32_t state;
    uint32_t res;
    uint32_t hook_res;
    uint8_t en;
    uint8_t unwind_up;
    uint8_t pudding[6];
} better_together_setup_ctx_t;

static uint32_t better_together_setup_mem_rd(uint64_t addr, void *out, uint64_t len)
{
    SIZE_T copied = 0;

    if (LDG_UNLIKELY(!addr || !out)) { return LDG_ERR_FUNC_ARG_NULL; }

    if (LDG_UNLIKELY(!len || addr > UINT64_MAX - len)) { return LDG_ERR_BOUNDS; }

    if (LDG_UNLIKELY(!ReadProcessMemory(GetCurrentProcess(), (const void *)(uintptr_t)addr, out, (SIZE_T)len, &copied) || copied != len)) { return LDG_ERR_MEM_BAD; }

    return LDG_ERR_AOK;
}

static uint32_t better_together_setup_mem_wr_validate(uint64_t addr, uint64_t len)
{
    MEMORY_BASIC_INFORMATION info = LDG_STRUCT_ZERO_INIT;
    uint64_t end = 0;
    uint64_t next = 0;
    uint32_t prot = 0;

    if (LDG_UNLIKELY(!addr || !len || addr > UINT64_MAX - len)) { return LDG_ERR_BOUNDS; }

    end = addr + len;
    while (addr < end)
    {
        if (LDG_UNLIKELY(VirtualQuery((const void *)(uintptr_t)addr, &info, sizeof(info)) != sizeof(info))) { return LDG_ERR_MEM_BAD; }

        prot = info.Protect & 0xFF;
        if (LDG_UNLIKELY(info.State != MEM_COMMIT || (info.Protect & PAGE_GUARD) || (prot != PAGE_READWRITE && prot != PAGE_WRITECOPY && prot != PAGE_EXECUTE_READWRITE && prot != PAGE_EXECUTE_WRITECOPY))) { return LDG_ERR_DENIED; }

        next = (uint64_t)(uintptr_t)info.BaseAddress + info.RegionSize;
        if (LDG_UNLIKELY(next <= addr)) { return LDG_ERR_OVERFLOW; }

        addr = next;
    }
    return LDG_ERR_AOK;
}

static uint32_t better_together_setup_code_validate(uint64_t addr, const uint8_t *expected, uint64_t len)
{
    uint8_t bytes[16] = LDG_ARR_ZERO_INIT;
    MEMORY_BASIC_INFORMATION info = LDG_STRUCT_ZERO_INIT;
    uint32_t res = LDG_ERR_AOK;
    uint32_t prot = 0;

    if (LDG_UNLIKELY(!expected || !len || len > sizeof(bytes))) { return LDG_ERR_FUNC_ARG_INVALID; }

    if (LDG_UNLIKELY(VirtualQuery((const void *)(uintptr_t)addr, &info, sizeof(info)) != sizeof(info))) { return LDG_ERR_MEM_BAD; }

    prot = info.Protect & 0xFF;
    if (LDG_UNLIKELY(info.State != MEM_COMMIT || (info.Protect & PAGE_GUARD) || (prot != PAGE_EXECUTE_READ && prot != PAGE_EXECUTE_READWRITE && prot != PAGE_EXECUTE_WRITECOPY && prot != PAGE_EXECUTE))) { return LDG_ERR_DENIED; }

    res = better_together_setup_mem_rd(addr, bytes, len);
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    return ldg_mem_secure_eq_is(bytes, expected, len) == LDG_TRUTH_TRUE ? LDG_ERR_AOK : LDG_ERR_UNSUPPORTED;
}

static uint32_t better_together_setup_profile_validate(uint64_t base)
{
    IMAGE_DOS_HEADER dos = LDG_STRUCT_ZERO_INIT;
    IMAGE_NT_HEADERS64 pe = LDG_STRUCT_ZERO_INIT;
    const uint8_t entry[] = { 0x48, 0x89, 0x11, 0xE9, 0xD8, 0xF3, 0xFF, 0xFF };
    const uint8_t rope[] = { 0x48, 0x89, 0xE0, 0x55, 0x56, 0x57, 0x41, 0x54, 0x41, 0x55, 0x41, 0x56, 0x41, 0x57, 0x48, 0x8D };
    const uint8_t hash[] = { 0x48, 0x89, 0x5C, 0x24, 0x08, 0x48, 0x89, 0x74, 0x24, 0x10, 0x48, 0x89, 0x7C, 0x24, 0x18, 0x4C };
    const uint8_t comp[] = { 0x4C, 0x8B, 0x49, 0x20, 0x89, 0xD0, 0x49, 0x89, 0xC8, 0x49, 0x0F, 0xA3, 0xC1 };
    const uint8_t bind[] = { 0x48, 0x89, 0xE0, 0x55, 0x56, 0x57, 0x41, 0x54, 0x41, 0x55, 0x41, 0x56, 0x41, 0x57, 0x48, 0x8D };
    const uint8_t ik[] = { 0x49, 0x89, 0xE3, 0x55, 0x41, 0x57, 0x49, 0x8D, 0xAB, 0xF8, 0xFE, 0xFF, 0xFF, 0x48, 0x81, 0xEC };
    const uint8_t frame[] = { 0x53, 0x48, 0x83, 0xEC, 0x60, 0x48, 0x89, 0xCB };
    const uint8_t work[] = { 0x57, 0x48, 0x83, 0xEC, 0x20, 0x80, 0x3D, 0x7C, 0xC9, 0x3D, 0xFF, 0x00, 0x48, 0x89, 0xCF };
    const uint8_t prepare[] = { 0x48, 0x89, 0xE0, 0x48, 0x89, 0x58, 0x10, 0x48, 0x89, 0x70, 0x18, 0x48, 0x89, 0x78, 0x20, 0x55 };
    const uint8_t finish[] = { 0x48, 0x89, 0xE0, 0x55, 0x53, 0x56, 0x57, 0x41, 0x54, 0x41, 0x56, 0x41, 0x57, 0x48, 0x8D, 0x68 };
    uint32_t kind = 0;
    uint32_t res = LDG_ERR_AOK;

    if (LDG_UNLIKELY(!base)) { return LDG_ERR_FUNC_ARG_NULL; }

    res = better_together_setup_mem_rd(base, &dos, sizeof(dos));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(dos.e_magic != IMAGE_DOS_SIGNATURE || (uint32_t)dos.e_lfanew > BETTER_TOGETHER_SETUP_IMG_SIZE - sizeof(pe))) { return LDG_ERR_UNSUPPORTED; }

    res = better_together_setup_mem_rd(base + (uint32_t)dos.e_lfanew, &pe, sizeof(pe));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(pe.Signature != IMAGE_NT_SIGNATURE || pe.FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64 || pe.FileHeader.TimeDateStamp != BETTER_TOGETHER_SETUP_PE_TIMESTAMP || pe.OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC || pe.OptionalHeader.SizeOfImage != BETTER_TOGETHER_SETUP_IMG_SIZE)) { return LDG_ERR_UNSUPPORTED; }

    res = better_together_setup_code_validate(base + BETTER_TOGETHER_SETUP_ENTRY_RVA, entry, sizeof(entry));
    if (res == LDG_ERR_AOK) { res = better_together_setup_code_validate(base + BETTER_TOGETHER_SETUP_ROPE_RVA, rope, sizeof(rope)); }

    if (res == LDG_ERR_AOK) { res = better_together_setup_code_validate(base + BETTER_TOGETHER_SETUP_HASH_RVA, hash, sizeof(hash)); }

    if (res == LDG_ERR_AOK) { res = better_together_setup_code_validate(base + BETTER_TOGETHER_SETUP_COMP_RVA, comp, sizeof(comp)); }

    if (res == LDG_ERR_AOK) { res = better_together_setup_code_validate(base + BETTER_TOGETHER_SETUP_BIND_RVA, bind, sizeof(bind)); }

    if (res == LDG_ERR_AOK) { res = better_together_setup_code_validate(base + BETTER_TOGETHER_SETUP_IK_RVA, ik, sizeof(ik)); }

    if (res == LDG_ERR_AOK) { res = better_together_setup_code_validate(base + BETTER_TOGETHER_SETUP_FRAME_RVA, frame, sizeof(frame)); }

    if (res == LDG_ERR_AOK) { res = better_together_setup_code_validate(base + BETTER_TOGETHER_SETUP_WORK_RVA, work, sizeof(work)); }

    if (res == LDG_ERR_AOK) { res = better_together_setup_code_validate(base + BETTER_TOGETHER_SETUP_PREPARE_RVA, prepare, sizeof(prepare)); }

    if (res == LDG_ERR_AOK) { res = better_together_setup_code_validate(base + BETTER_TOGETHER_SETUP_FINISH_RVA, finish, sizeof(finish)); }

    if (res == LDG_ERR_AOK) { res = better_together_setup_mem_rd(base + BETTER_TOGETHER_SETUP_COMP_ID_RVA, &kind, sizeof(kind)); }

    if (res == LDG_ERR_AOK && kind != BETTER_TOGETHER_SETUP_ROPE_COMP_ID) { res = LDG_ERR_UNSUPPORTED; }

    return res;
}

static uint32_t better_together_setup_name_get(uint64_t addr, better_together_setup_name_t *out)
{
    uint64_t ptr = 0;
    uint32_t res = LDG_ERR_AOK;

    if (LDG_UNLIKELY(!out)) { return LDG_ERR_FUNC_ARG_NULL; }

    res = better_together_setup_mem_rd(addr, out, sizeof(*out));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(out->len > BETTER_TOGETHER_SETUP_SMALL_STR_CAP || out->cap < out->len)) { return LDG_ERR_RANGE; }

    if (out->cap > BETTER_TOGETHER_SETUP_SMALL_STR_CAP)
    {
        res = ldg_mem_secure_cpy(&ptr, out->text, sizeof(ptr));
        if (res == LDG_ERR_AOK) { res = better_together_setup_mem_rd(ptr, out->text, out->len + LDG_STR_TERM_SIZE); }

        if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }
    }

    return out->text[out->len] == 0 ? LDG_ERR_AOK : LDG_ERR_INVALID;
}

static uint32_t better_together_setup_names_init(better_together_setup_ctx_t *ctx)
{
    uint32_t res = LDG_ERR_AOK;

    res = ldg_mem_secure_cpy(ctx->names[0].text, "BoxieBackYarn", sizeof("BoxieBackYarn"));
    if (res == LDG_ERR_AOK) { res = ldg_mem_secure_cpy(ctx->names[1].text, "BoxieFrontYarn", sizeof("BoxieFrontYarn")); }

    ctx->names[0].len = sizeof("BoxieBackYarn") - LDG_STR_TERM_SIZE;
    ctx->names[1].len = sizeof("BoxieFrontYarn") - LDG_STR_TERM_SIZE;
    ctx->names[0].cap = BETTER_TOGETHER_SETUP_SMALL_STR_CAP;
    ctx->names[1].cap = BETTER_TOGETHER_SETUP_SMALL_STR_CAP;
    return res;
}

static uint32_t better_together_setup_snapshot_get(better_together_setup_ctx_t *ctx, uint64_t owner, better_together_setup_snapshot_t *out)
{
    uint32_t res = LDG_ERR_AOK;
    uint32_t idx = 0;

    res = ldg_mem_secure_zero(out, sizeof(*out));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    out->owner = owner;
    res = better_together_setup_mem_rd(owner, &out->world, sizeof(uint64_t) * 5);
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    for (idx = 0; idx < BETTER_TOGETHER_SETUP_ROPE_CUNT; idx++)
    {
        if (!out->rope[idx]) { continue; }

        res = better_together_setup_mem_rd(out->rope[idx] + BETTER_TOGETHER_SETUP_ROPE_ENTITY_OFF, &out->rope_entity[idx], sizeof(uint64_t));
        if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

        if (LDG_UNLIKELY(!out->rope_entity[idx])) { return LDG_ERR_NOT_INIT; }

        out->comp[idx] = ctx->comp(out->rope_entity[idx], BETTER_TOGETHER_SETUP_ROPE_COMP_ID);
        if (LDG_UNLIKELY(!out->comp[idx])) { return LDG_ERR_NOT_FOUND; }

        res = better_together_setup_mem_rd(out->comp[idx] + BETTER_TOGETHER_SETUP_COMP_OWNER_OFF, &out->back[idx], sizeof(uint64_t));
        if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

        if (LDG_UNLIKELY(out->back[idx] != out->rope[idx])) { return LDG_ERR_MEM_CORRUPTION; }
    }
    return LDG_ERR_AOK;
}

static uint32_t better_together_setup_boxie_validate(better_together_setup_ctx_t *ctx, const better_together_setup_snapshot_t *orig, const better_together_setup_snapshot_t *boxie, uint8_t fresh)
{
    better_together_setup_name_t name = LDG_STRUCT_ZERO_INIT;
    uint32_t hash = 0;
    uint32_t idx = 0;
    uint32_t other = 0;
    uint32_t res = LDG_ERR_AOK;

    if (fresh && !boxie->world && !boxie->rope[0] && !boxie->rope[1] && !boxie->attachment && !boxie->entity) { return LDG_ERR_AOK; }

    if (LDG_UNLIKELY(!boxie->world || !boxie->rope[0] || !boxie->rope[1] || !boxie->attachment || boxie->rope[0] == boxie->rope[1] || boxie->rope_entity[0] == boxie->rope_entity[1] || boxie->attachment == orig->attachment)) { return LDG_ERR_INVALID; }

    for (idx = 0; idx < BETTER_TOGETHER_SETUP_ROPE_CUNT; idx++)
    {
        for (other = 0; other < BETTER_TOGETHER_SETUP_ROPE_CUNT; other++) { if (LDG_UNLIKELY(boxie->rope[idx] == orig->rope[other] || boxie->rope_entity[idx] == orig->rope_entity[other] || boxie->comp[idx] == orig->comp[other])) { return LDG_ERR_EXISTS; } }
        res = better_together_setup_name_get(boxie->rope[idx] + BETTER_TOGETHER_SETUP_ROPE_NAME_OFF, &name);
        if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

        if (LDG_UNLIKELY(name.len != ctx->names[idx].len || ldg_mem_secure_eq_is(name.text, ctx->names[idx].text, name.len) != LDG_TRUTH_TRUE)) { return LDG_ERR_INVALID; }

        res = better_together_setup_mem_rd(boxie->rope_entity[idx] + BETTER_TOGETHER_SETUP_ENTITY_HASH_OFF, &hash, sizeof(hash));
        if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

        if (LDG_UNLIKELY(hash != ctx->hashes[idx] || boxie->back[idx] != boxie->rope[idx])) { return LDG_ERR_INVALID; }
    }
    return LDG_ERR_AOK;
}

static uint64_t better_together_setup_rope_cb(uint64_t self, uint64_t world, uint64_t params, const better_together_setup_name_t *name, better_together_setup_ctx_t *ctx)
{
    better_together_setup_scope_t *scope = (better_together_setup_scope_t *)TlsGetValue(ctx->tls);
    better_together_setup_name_t val = LDG_STRUCT_ZERO_INIT;
    uint32_t idx = UINT32_MAX;
    uint32_t res = LDG_ERR_AOK;

    if (scope != &ctx->scope || !scope->active || scope->thread != GetCurrentThreadId() || scope->world != world) { return ctx->rope(self, world, params, name); }

    res = better_together_setup_name_get((uint64_t)(uintptr_t)name, &val);
    if (res == LDG_ERR_RANGE) { return ctx->rope(self, world, params, name); }

    if (res != LDG_ERR_AOK) { scope->res = res; return BETTER_TOGETHER_SETUP_NATIVE_NONE; }

    if (val.len == sizeof("BackYarn") - LDG_STR_TERM_SIZE && ldg_mem_secure_eq_is(val.text, "BackYarn", val.len) == LDG_TRUTH_TRUE) { idx = BETTER_TOGETHER_SETUP_BACK_IDX; }

    if (val.len == sizeof("FrontYarn") - LDG_STR_TERM_SIZE && ldg_mem_secure_eq_is(val.text, "FrontYarn", val.len) == LDG_TRUTH_TRUE) { idx = BETTER_TOGETHER_SETUP_FRONT_IDX; }

    if (idx == UINT32_MAX) { return ctx->rope(self, world, params, name); }

    scope->renamed[idx]++;
    return ctx->rope(self, world, params, &ctx->names[idx]);
}

static uint32_t better_together_setup_before_validate(better_together_setup_ctx_t *ctx)
{
    uint64_t orig = 0;
    uint32_t hash = 0;
    uint32_t idx = 0;
    uint32_t res = LDG_ERR_AOK;

    res = better_together_setup_mem_rd(ctx->base + BETTER_TOGETHER_SETUP_GLOBAL_RVA, &orig, sizeof(orig));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(!orig || orig == ctx->owner)) { return LDG_ERR_DENIED; }

    res = better_together_setup_snapshot_get(ctx, orig, &ctx->orig);
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(!ctx->orig.world || !ctx->orig.entity || !ctx->orig.attachment || !ctx->orig.rope[0] || !ctx->orig.rope[1])) { return LDG_ERR_NOT_INIT; }

    for (idx = 0; idx < BETTER_TOGETHER_SETUP_ROPE_CUNT; idx++)
    {
        res = better_together_setup_mem_rd(ctx->orig.rope_entity[idx] + BETTER_TOGETHER_SETUP_ENTITY_HASH_OFF, &hash, sizeof(hash));
        if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

        if (LDG_UNLIKELY(hash == ctx->hashes[0] || hash == ctx->hashes[1])) { return LDG_ERR_EXISTS; }
    }
    if (res == LDG_ERR_AOK) { res = better_together_setup_snapshot_get(ctx, ctx->owner, &ctx->boxie); }

    if (res == LDG_ERR_AOK) { res = better_together_setup_boxie_validate(ctx, &ctx->orig, &ctx->boxie, LDG_TRUTH_TRUE); }

    return res;
}

static uint32_t better_together_setup_after_validate(better_together_setup_ctx_t *ctx, uint64_t world)
{
    better_together_setup_snapshot_t orig = LDG_STRUCT_ZERO_INIT;
    uint64_t owner = 0;
    uint32_t res = LDG_ERR_AOK;

    res = better_together_setup_mem_rd(ctx->base + BETTER_TOGETHER_SETUP_GLOBAL_RVA, &owner, sizeof(owner));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(owner != ctx->orig.owner)) { return LDG_ERR_MEM_CORRUPTION; }

    res = better_together_setup_snapshot_get(ctx, owner, &orig);
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(ldg_mem_secure_eq_is(&orig, &ctx->orig, sizeof(orig)) != LDG_TRUTH_TRUE)) { return LDG_ERR_MEM_CORRUPTION; }

    res = better_together_setup_snapshot_get(ctx, ctx->owner, &ctx->boxie);
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(ctx->boxie.world != world)) { return LDG_ERR_INVALID; }

    return better_together_setup_boxie_validate(ctx, &ctx->orig, &ctx->boxie, LDG_TRUTH_FALSE);
}

static uint64_t better_together_setup_entry_cb(uint64_t owner, uint64_t world, uint64_t arg, better_together_setup_ctx_t *ctx)
{
    better_together_setup_scope_t *previous = (better_together_setup_scope_t *)TlsGetValue(ctx->tls);
    uint64_t ret = 0;
    uint32_t res = LDG_ERR_AOK;

    if (!ctx->en || owner != ctx->owner)
    {
        if (previous != &ctx->scope || !previous->active) { return ctx->entry(owner, world, arg); }

        if (!TlsSetValue(ctx->tls, 0x0)) { previous->res = LDG_ERR_DENIED; return BETTER_TOGETHER_SETUP_NATIVE_NONE; }

        ret = ctx->entry(owner, world, arg);
        if (!TlsSetValue(ctx->tls, previous)) { previous->res = LDG_ERR_DENIED; }

        return ret;
    }

    if (previous == &ctx->scope && previous->active) { previous->res = LDG_ERR_BUSY; return BETTER_TOGETHER_SETUP_NATIVE_NONE; }

    AcquireSRWLockExclusive(&ctx->lock);
    if (ctx->state == BETTER_TOGETHER_SETUP_STATE_FAILED) { ReleaseSRWLockExclusive(&ctx->lock); return BETTER_TOGETHER_SETUP_NATIVE_NONE; }

    res = better_together_setup_before_validate(ctx);
    if (res == LDG_ERR_AOK && !world) { res = LDG_ERR_FUNC_ARG_NULL; }

    if (res != LDG_ERR_AOK)
    {
        ctx->res = res;
        ReleaseSRWLockExclusive(&ctx->lock);
        return BETTER_TOGETHER_SETUP_NATIVE_NONE;
    }

    ctx->scope.world = world;
    ctx->scope.thread = GetCurrentThreadId();
    ctx->scope.renamed[0] = 0;
    ctx->scope.renamed[1] = 0;
    ctx->scope.res = LDG_ERR_AOK;
    ctx->scope.active = LDG_TRUTH_TRUE;
    if (!TlsSetValue(ctx->tls, &ctx->scope))
    {
        ctx->scope.active = LDG_TRUTH_FALSE;
        ctx->res = LDG_ERR_DENIED;
        ReleaseSRWLockExclusive(&ctx->lock);
        return BETTER_TOGETHER_SETUP_NATIVE_NONE;
    }

    ctx->state = BETTER_TOGETHER_SETUP_STATE_RUNNING;
    ctx->attempts++;
    ret = ctx->entry(owner, world, arg);
    ctx->scope.active = LDG_TRUTH_FALSE;
    res = ctx->scope.res;
    if (!TlsSetValue(ctx->tls, previous)) { res = LDG_ERR_DENIED; }

    if (res == LDG_ERR_AOK && (ctx->scope.renamed[0] != BETTER_TOGETHER_SETUP_RENAME_CUNT || ctx->scope.renamed[1] != BETTER_TOGETHER_SETUP_RENAME_CUNT)) { res = LDG_ERR_INVALID; }

    if (res == LDG_ERR_AOK) { res = better_together_setup_after_validate(ctx, world); }

    ctx->res = res;
    ctx->state = res == LDG_ERR_AOK ? BETTER_TOGETHER_SETUP_STATE_COMPLETE : BETTER_TOGETHER_SETUP_STATE_FAILED;
    if (res == LDG_ERR_AOK) { ctx->successes++; }

    ReleaseSRWLockExclusive(&ctx->lock);
    return ret;
}

static uint32_t better_together_setup_guest_get(better_together_setup_ctx_t *ctx, uint64_t primary, uint64_t *fields)
{
    uint64_t host[5] = LDG_ARR_ZERO_INIT;
    uint32_t res = LDG_ERR_AOK;

    if (LDG_UNLIKELY(!ctx->en || ctx->state != BETTER_TOGETHER_SETUP_STATE_COMPLETE || !primary || !ctx->owner || primary == ctx->owner || primary != ctx->orig.owner)) { return LDG_ERR_NOT_INIT; }

    res = better_together_setup_mem_rd(primary, host, sizeof(host));
    if (res == LDG_ERR_AOK) { res = better_together_setup_mem_rd(ctx->owner, fields, sizeof(host)); }

    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(!host[0] || !host[4] || fields[0] != host[0] || fields[0] != ctx->boxie.world || !fields[4] || fields[4] == host[4])) { return LDG_ERR_NOT_INIT; }

    if (LDG_UNLIKELY(!fields[1] || !fields[2] || fields[1] == fields[2] || fields[1] != ctx->boxie.rope[0] || fields[2] != ctx->boxie.rope[1] || fields[1] == host[1] || fields[1] == host[2] || fields[2] == host[1] || fields[2] == host[2])) { return LDG_ERR_INVALID; }

    return LDG_ERR_AOK;
}

static void better_together_setup_frame_cb(uint64_t owner, uint64_t frame, uint64_t unused __attribute__((unused)), better_together_setup_ctx_t *ctx)
{
    uint64_t fields[5] = LDG_ARR_ZERO_INIT;
    uint32_t res = LDG_ERR_AOK;

    ctx->frame(owner, frame);
    AcquireSRWLockExclusive(&ctx->lock);
    res = better_together_setup_guest_get(ctx, owner, fields);
    if (res == LDG_ERR_AOK) { ctx->frame(ctx->owner, frame); }

    if (res != LDG_ERR_AOK && res != LDG_ERR_NOT_INIT) { ctx->res = res; }

    ReleaseSRWLockExclusive(&ctx->lock);
}

static void better_together_setup_work_cb(uint64_t game, uint64_t unused_a __attribute__((unused)), uint64_t unused_b __attribute__((unused)), better_together_setup_ctx_t *ctx)
{
    uint64_t fields[5] = LDG_ARR_ZERO_INIT;
    uint64_t primary = 0;
    uint32_t idx = BETTER_TOGETHER_SETUP_ROPE_CUNT;
    uint32_t res = LDG_ERR_AOK;
    uint8_t active = 0;

    ctx->work(game);
    AcquireSRWLockExclusive(&ctx->lock);
    res = better_together_setup_mem_rd(ctx->base + BETTER_TOGETHER_SETUP_WORK_EN_RVA, &active, sizeof(active));
    if (res == LDG_ERR_AOK && !active) { ReleaseSRWLockExclusive(&ctx->lock); return; }

    if (res == LDG_ERR_AOK) { res = better_together_setup_mem_rd(game + BETTER_TOGETHER_SETUP_GAME_OWNER_OFF, &primary, sizeof(primary)); }

    if (res == LDG_ERR_AOK) { res = better_together_setup_guest_get(ctx, primary, fields); }

    while (res == LDG_ERR_AOK && idx)
    {
        if (fields[idx])
        {
            res = better_together_setup_mem_rd(fields[idx] + BETTER_TOGETHER_SETUP_ROPE_ACTIVE_OFF, &active, sizeof(active));
            if (res == LDG_ERR_AOK && active)
            {
                ctx->prepare(fields[idx]);
                ctx->finish(fields[idx]);
            }
        }

        idx--;
    }
    if (res != LDG_ERR_AOK && res != LDG_ERR_NOT_INIT) { ctx->res = res; }

    ReleaseSRWLockExclusive(&ctx->lock);
}

static uint32_t better_together_setup_code_init(better_together_setup_ctx_t *ctx)
{
    better_together_setup_layout_t layout[BETTER_TOGETHER_SETUP_THUNK_ALL_CUNT] = LDG_ARR_ZERO_INIT;
    uint64_t hook_cb[BETTER_TOGETHER_SETUP_THUNK_ALL_CUNT] = { (uint64_t)(uintptr_t)better_together_setup_entry_cb, (uint64_t)(uintptr_t)better_together_setup_rope_cb, (uint64_t)(uintptr_t)&ctx->bind, (uint64_t)(uintptr_t)&ctx->ik, (uint64_t)(uintptr_t)better_together_setup_frame_cb, (uint64_t)(uintptr_t)better_together_setup_work_cb };
    uint64_t hook_ctx = (uint64_t)(uintptr_t)ctx;
    uint64_t owner = (uint64_t)(uintptr_t)&ctx->owner;
    uint8_t *dst = 0x0;
    uint32_t idx = 0;
    uint32_t off = 0;
    uint32_t res = LDG_ERR_AOK;
    DWORD old = 0;

    res = better_together_setup_layout_get(layout);
    if (res == LDG_ERR_AOK) { res = better_together_setup_bind_layout_get(&layout[BETTER_TOGETHER_SETUP_BIND_SLOT]); }

    if (res == LDG_ERR_AOK) { res = better_together_setup_ik_layout_get(&layout[BETTER_TOGETHER_SETUP_IK_SLOT]); }

    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    layout[BETTER_TOGETHER_SETUP_FRAME_SLOT] = layout[BETTER_TOGETHER_SETUP_ENTRY_SLOT];
    layout[BETTER_TOGETHER_SETUP_WORK_SLOT] = layout[BETTER_TOGETHER_SETUP_ENTRY_SLOT];
    ctx->code = VirtualAlloc(0x0, BETTER_TOGETHER_SETUP_CODE_BYTES, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (LDG_UNLIKELY(!ctx->code)) { return LDG_ERR_ALLOC_NULL; }

    for (idx = 0; idx < BETTER_TOGETHER_SETUP_THUNK_ALL_CUNT; idx++)
    {
        if (LDG_UNLIKELY(!layout[idx].src || layout[idx].len > BETTER_TOGETHER_SETUP_CODE_SLOT_BYTES || layout[idx].len < sizeof(uint64_t) || layout[idx].code_end > layout[idx].len || layout[idx].ctx_off > layout[idx].len - sizeof(uint64_t) || layout[idx].cb_off > layout[idx].len - sizeof(uint64_t) || layout[idx].unwind_off > layout[idx].len - sizeof(uint32_t) || (layout[idx].unwind_off & 3))) { return LDG_ERR_INVALID; }

        off = idx * BETTER_TOGETHER_SETUP_CODE_SLOT_BYTES;
        dst = (uint8_t *)ctx->code + off;
        res = ldg_mem_secure_cpy(dst, (const void *)(uintptr_t)layout[idx].src, layout[idx].len);
        if (res == LDG_ERR_AOK) { res = ldg_mem_secure_cpy(dst + layout[idx].ctx_off, idx == BETTER_TOGETHER_SETUP_BIND_SLOT || idx == BETTER_TOGETHER_SETUP_IK_SLOT ? &owner : &hook_ctx, sizeof(hook_ctx)); }

        if (res == LDG_ERR_AOK) { res = ldg_mem_secure_cpy(dst + layout[idx].cb_off, &hook_cb[idx], sizeof(hook_cb[idx])); }

        if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

        ctx->unwind[idx].BeginAddress = off;
        ctx->unwind[idx].EndAddress = off + (uint32_t)layout[idx].code_end;
        ctx->unwind[idx].UnwindData = off + (uint32_t)layout[idx].unwind_off;
    }
    if (LDG_UNLIKELY(!VirtualProtect(ctx->code, BETTER_TOGETHER_SETUP_CODE_BYTES, PAGE_EXECUTE_READ, &old))) { return LDG_ERR_DENIED; }

    if (LDG_UNLIKELY(!FlushInstructionCache(GetCurrentProcess(), ctx->code, BETTER_TOGETHER_SETUP_CODE_BYTES))) { return LDG_ERR_INVALID; }

    if (LDG_UNLIKELY(!RtlAddFunctionTable(ctx->unwind, BETTER_TOGETHER_SETUP_THUNK_ALL_CUNT, (DWORD64)(uintptr_t)ctx->code))) { return LDG_ERR_INVALID; }

    ctx->unwind_up = LDG_TRUTH_TRUE;
    return LDG_ERR_AOK;
}

static uint32_t better_together_setup_hooks_init(better_together_setup_ctx_t *ctx)
{
    void *entry = 0x0;
    void *rope = 0x0;
    void *frame = 0x0;
    void *work = 0x0;
    MH_STATUS status = MH_OK;
    uint32_t res = LDG_ERR_AOK;

    status = MH_Initialize();
    if (status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED) { ctx->hook_res = (uint32_t)status; return LDG_ERR_NOT_INIT; }

    status = MH_CreateHook((void *)(uintptr_t)(ctx->base + BETTER_TOGETHER_SETUP_ENTRY_RVA), ctx->code, &entry);
    if (status != MH_OK) { ctx->hook_res = (uint32_t)status; return LDG_ERR_INVALID; }

    res = ldg_mem_secure_cpy(&ctx->entry, &entry, sizeof(entry));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    status = MH_CreateHook((void *)(uintptr_t)(ctx->base + BETTER_TOGETHER_SETUP_ROPE_RVA), (uint8_t *)ctx->code + BETTER_TOGETHER_SETUP_CODE_SLOT_BYTES, &rope);
    if (status != MH_OK) { ctx->hook_res = (uint32_t)status; return LDG_ERR_INVALID; }

    res = ldg_mem_secure_cpy(&ctx->rope, &rope, sizeof(rope));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    status = MH_CreateHook((void *)(uintptr_t)(ctx->base + BETTER_TOGETHER_SETUP_BIND_RVA), (uint8_t *)ctx->code + BETTER_TOGETHER_SETUP_BIND_SLOT * BETTER_TOGETHER_SETUP_CODE_SLOT_BYTES, &ctx->bind);
    if (status != MH_OK) { ctx->hook_res = (uint32_t)status; return LDG_ERR_INVALID; }

    status = MH_CreateHook((void *)(uintptr_t)(ctx->base + BETTER_TOGETHER_SETUP_IK_RVA), (uint8_t *)ctx->code + BETTER_TOGETHER_SETUP_IK_SLOT * BETTER_TOGETHER_SETUP_CODE_SLOT_BYTES, &ctx->ik);
    if (status != MH_OK) { ctx->hook_res = (uint32_t)status; return LDG_ERR_INVALID; }

    status = MH_CreateHook((void *)(uintptr_t)(ctx->base + BETTER_TOGETHER_SETUP_FRAME_RVA), (uint8_t *)ctx->code + BETTER_TOGETHER_SETUP_FRAME_SLOT * BETTER_TOGETHER_SETUP_CODE_SLOT_BYTES, &frame);
    if (status != MH_OK) { ctx->hook_res = (uint32_t)status; return LDG_ERR_INVALID; }

    res = ldg_mem_secure_cpy(&ctx->frame, &frame, sizeof(frame));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    status = MH_CreateHook((void *)(uintptr_t)(ctx->base + BETTER_TOGETHER_SETUP_WORK_RVA), (uint8_t *)ctx->code + BETTER_TOGETHER_SETUP_WORK_SLOT * BETTER_TOGETHER_SETUP_CODE_SLOT_BYTES, &work);
    if (status != MH_OK) { ctx->hook_res = (uint32_t)status; return LDG_ERR_INVALID; }

    res = ldg_mem_secure_cpy(&ctx->work, &work, sizeof(work));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    ctx->en = LDG_TRUTH_TRUE;
    status = MH_QueueEnableHook((void *)(uintptr_t)(ctx->base + BETTER_TOGETHER_SETUP_ROPE_RVA));
    if (status == MH_OK) { status = MH_QueueEnableHook((void *)(uintptr_t)(ctx->base + BETTER_TOGETHER_SETUP_BIND_RVA)); }

    if (status == MH_OK) { status = MH_QueueEnableHook((void *)(uintptr_t)(ctx->base + BETTER_TOGETHER_SETUP_IK_RVA)); }

    if (status == MH_OK) { status = MH_QueueEnableHook((void *)(uintptr_t)(ctx->base + BETTER_TOGETHER_SETUP_FRAME_RVA)); }

    if (status == MH_OK) { status = MH_QueueEnableHook((void *)(uintptr_t)(ctx->base + BETTER_TOGETHER_SETUP_WORK_RVA)); }

    if (status == MH_OK) { status = MH_QueueEnableHook((void *)(uintptr_t)(ctx->base + BETTER_TOGETHER_SETUP_ENTRY_RVA)); }

    if (status == MH_OK) { status = MH_ApplyQueued(); }

    ctx->hook_res = (uint32_t)status;
    return status == MH_OK ? LDG_ERR_AOK : LDG_ERR_INVALID;
}

uint64_t better_together_setup_size_get(void)
{
    return sizeof(better_together_setup_ctx_t);
}

uint32_t better_together_setup_validate(void *storage, uint64_t world)
{
    better_together_setup_ctx_t *ctx = (better_together_setup_ctx_t *)storage;
    uint32_t res = LDG_ERR_AOK;

    if (LDG_UNLIKELY(!ctx || !world)) { return LDG_ERR_FUNC_ARG_NULL; }

    if (LDG_UNLIKELY(ctx->magic != BETTER_TOGETHER_SETUP_MAGIC)) { return LDG_ERR_NOT_INIT; }

    AcquireSRWLockExclusive(&ctx->lock);
    res = ctx->state == BETTER_TOGETHER_SETUP_STATE_COMPLETE ? better_together_setup_after_validate(ctx, world) : LDG_ERR_NOT_INIT;
    ReleaseSRWLockExclusive(&ctx->lock);
    return res;
}

uint32_t better_together_setup_init(void *storage, uint64_t cap, uint64_t owner)
{
    better_together_setup_ctx_t *ctx = (better_together_setup_ctx_t *)storage;
    better_together_setup_hash_name_t name = LDG_STRUCT_ZERO_INIT;
    better_together_setup_hash_func_t hash = 0x0;
    HMODULE game = 0x0;
    HANDLE singleton = 0x0;
    uint64_t fields[5] = LDG_ARR_ZERO_INIT;
    uint64_t desc[3] = LDG_ARR_ZERO_INIT;
    uint64_t global = 0;
    uint64_t len = 0;
    char mut_name[BETTER_TOGETHER_SETUP_MUT_CAP] = LDG_ARR_ZERO_INIT;
    uint32_t res = LDG_ERR_AOK;
    uint32_t idx = 0;
    DWORD error = 0;

    if (LDG_UNLIKELY(!storage || !owner)) { return LDG_ERR_FUNC_ARG_NULL; }

    if (LDG_UNLIKELY(cap < sizeof(*ctx) || ((uintptr_t)storage & (BETTER_TOGETHER_SETUP_QWORD_ALIGN_MASK)))) { return LDG_ERR_BOUNDS; }

    if (LDG_UNLIKELY((uint64_t)(uintptr_t)storage > UINT64_MAX - sizeof(*ctx) || owner > UINT64_MAX - BETTER_TOGETHER_SETUP_OWNER_BYTES)) { return LDG_ERR_BOUNDS; }

    if (LDG_UNLIKELY((uint64_t)(uintptr_t)storage < owner + BETTER_TOGETHER_SETUP_OWNER_BYTES && owner < (uint64_t)(uintptr_t)storage + sizeof(*ctx))) { return LDG_ERR_FUNC_ARG_INVALID; }

    game = GetModuleHandleA("Unravel.exe");
    if (LDG_UNLIKELY(!game || game != GetModuleHandleA(0x0))) { return LDG_ERR_NOT_FOUND; }

    res = better_together_setup_profile_validate((uint64_t)(uintptr_t)game);
    if (res == LDG_ERR_AOK) { res = better_together_setup_mem_wr_validate((uint64_t)(uintptr_t)storage, sizeof(*ctx)); }

    if (res == LDG_ERR_AOK) { res = better_together_setup_mem_wr_validate(owner, BETTER_TOGETHER_SETUP_OWNER_BYTES); }

    if (res == LDG_ERR_AOK) { res = better_together_setup_mem_rd((uint64_t)(uintptr_t)game + BETTER_TOGETHER_SETUP_GLOBAL_RVA, &global, sizeof(global)); }

    if (res == LDG_ERR_AOK) { res = better_together_setup_mem_rd(owner, fields, sizeof(fields)); }

    if (res == LDG_ERR_AOK) { res = better_together_setup_mem_rd(owner + BETTER_TOGETHER_SETUP_OWNER_DESC_OFF, desc, sizeof(desc)); }

    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(!global || owner == global || fields[0] || fields[1] || fields[2] || fields[3] || fields[4] || !desc[0] || !desc[1] || !desc[2])) { return LDG_ERR_DENIED; }

    res = ldg_str_fmt(mut_name, sizeof(mut_name), &len, "Local\\LibreSteam.Boxie.Ropes.%u32", (uint32_t)GetCurrentProcessId());
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    singleton = CreateMutexA(0x0, FALSE, mut_name);
    error = GetLastError();
    if (LDG_UNLIKELY(!singleton)) { return LDG_ERR_ALLOC_NULL; }

    if (error == ERROR_ALREADY_EXISTS) { return CloseHandle(singleton) ? LDG_ERR_EXISTS : LDG_ERR_IO_CLOSE; }

    res = ldg_mem_secure_zero(ctx, sizeof(*ctx));
    if (res != LDG_ERR_AOK) { return CloseHandle(singleton) ? res : LDG_ERR_IO_CLOSE; }

    ctx->magic = BETTER_TOGETHER_SETUP_MAGIC;
    ctx->base = (uint64_t)(uintptr_t)game;
    ctx->owner = owner;
    ctx->singleton = singleton;
    ctx->comp = (better_together_setup_comp_func_t)(uintptr_t)(ctx->base + BETTER_TOGETHER_SETUP_COMP_RVA);
    ctx->prepare = (better_together_setup_tick_func_t)(uintptr_t)(ctx->base + BETTER_TOGETHER_SETUP_PREPARE_RVA);
    ctx->finish = (better_together_setup_tick_func_t)(uintptr_t)(ctx->base + BETTER_TOGETHER_SETUP_FINISH_RVA);
    ctx->tls = TLS_OUT_OF_INDEXES;
    ctx->state = BETTER_TOGETHER_SETUP_STATE_FAILED;
    InitializeSRWLock(&ctx->lock);
    res = better_together_setup_names_init(ctx);
    hash = (better_together_setup_hash_func_t)(uintptr_t)(ctx->base + BETTER_TOGETHER_SETUP_HASH_RVA);
    for (idx = 0; res == LDG_ERR_AOK && idx < BETTER_TOGETHER_SETUP_ROPE_CUNT; idx++)
    {
        name.data = (uint64_t)(uintptr_t)ctx->names[idx].text;
        name.len = ctx->names[idx].len;
        ctx->hashes[idx] = hash(&name);
    }
    if (res == LDG_ERR_AOK && ctx->hashes[0] == ctx->hashes[1]) { res = LDG_ERR_EXISTS; }

    if (res == LDG_ERR_AOK)
    {
        ctx->tls = TlsAlloc();
        if (ctx->tls == TLS_OUT_OF_INDEXES) { res = LDG_ERR_FULL; }
    }

    if (res == LDG_ERR_AOK && !GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN, (LPCSTR)(uintptr_t)better_together_setup_entry_cb, &ctx->module)) { res = LDG_ERR_NOT_FOUND; }

    if (res == LDG_ERR_AOK) { res = better_together_setup_code_init(ctx); }

    if (res == LDG_ERR_AOK) { res = better_together_setup_hooks_init(ctx); }

    ctx->res = res;
    ctx->state = res == LDG_ERR_AOK ? BETTER_TOGETHER_SETUP_STATE_READY : BETTER_TOGETHER_SETUP_STATE_FAILED;
    return res;
}

uint32_t better_together_setup_owner_bind(void *storage, uint64_t owner)
{
    better_together_setup_ctx_t *ctx = (better_together_setup_ctx_t *)storage;
    uint64_t fields[5] = LDG_ARR_ZERO_INIT;
    uint64_t desc[3] = LDG_ARR_ZERO_INIT;
    uint64_t global = 0;
    uint32_t res = LDG_ERR_AOK;

    if (LDG_UNLIKELY(!ctx || !owner)) { return LDG_ERR_FUNC_ARG_NULL; }

    if (LDG_UNLIKELY(ctx->magic != BETTER_TOGETHER_SETUP_MAGIC || !ctx->en)) { return LDG_ERR_NOT_INIT; }

    if (LDG_UNLIKELY(owner > UINT64_MAX - BETTER_TOGETHER_SETUP_OWNER_BYTES)) { return LDG_ERR_BOUNDS; }

    res = better_together_setup_mem_wr_validate(owner, BETTER_TOGETHER_SETUP_OWNER_BYTES);
    if (res == LDG_ERR_AOK) { res = better_together_setup_mem_rd(ctx->base + BETTER_TOGETHER_SETUP_GLOBAL_RVA, &global, sizeof(global)); }

    if (res == LDG_ERR_AOK) { res = better_together_setup_mem_rd(owner, fields, sizeof(fields)); }

    if (res == LDG_ERR_AOK) { res = better_together_setup_mem_rd(owner + BETTER_TOGETHER_SETUP_OWNER_DESC_OFF, desc, sizeof(desc)); }

    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(!global || owner == global || fields[0] || fields[1] || fields[2] || fields[3] || fields[4] || !desc[0] || !desc[1] || !desc[2])) { return LDG_ERR_DENIED; }

    AcquireSRWLockExclusive(&ctx->lock);
    if (ctx->state != BETTER_TOGETHER_SETUP_STATE_COMPLETE || ctx->scope.active) { ReleaseSRWLockExclusive(&ctx->lock); return LDG_ERR_BUSY; }

    ctx->owner = owner;
    ctx->state = BETTER_TOGETHER_SETUP_STATE_READY;
    ctx->res = LDG_ERR_AOK;
    ReleaseSRWLockExclusive(&ctx->lock);
    return LDG_ERR_AOK;
}

uint32_t better_together_setup_enter(void *storage, uint64_t world, uint64_t arg)
{
    better_together_setup_ctx_t *ctx = (better_together_setup_ctx_t *)storage;
    uint64_t ret = 0;

    if (LDG_UNLIKELY(!ctx || !world)) { return LDG_ERR_FUNC_ARG_NULL; }

    if (LDG_UNLIKELY(ctx->magic != BETTER_TOGETHER_SETUP_MAGIC || !ctx->en || !ctx->entry)) { return LDG_ERR_NOT_INIT; }

    ret = better_together_setup_entry_cb(ctx->owner, world, arg, ctx);
    if (LDG_UNLIKELY(ctx->res == LDG_ERR_AOK && !ret && !ctx->successes)) { return LDG_ERR_INVALID; }

    return ctx->res;
}

uint32_t better_together_setup_report(void *storage, uint64_t *out, uint64_t cap)
{
    better_together_setup_ctx_t *ctx = (better_together_setup_ctx_t *)storage;
    uint64_t addr = (uint64_t)(uintptr_t)out;
    uint64_t hook_ctx = (uint64_t)(uintptr_t)storage;
    uint64_t len = BETTER_TOGETHER_SETUP_REPORT_WORDS * sizeof(*out);
    uint32_t res = LDG_ERR_AOK;

    if (LDG_UNLIKELY(!ctx || !out)) { return LDG_ERR_FUNC_ARG_NULL; }

    if (LDG_UNLIKELY(cap < BETTER_TOGETHER_SETUP_REPORT_WORDS * sizeof(*out))) { return LDG_ERR_BOUNDS; }

    if (LDG_UNLIKELY(ctx->magic != BETTER_TOGETHER_SETUP_MAGIC)) { return LDG_ERR_NOT_INIT; }

    if (LDG_UNLIKELY(addr > UINT64_MAX - len || hook_ctx > UINT64_MAX - sizeof(*ctx))) { return LDG_ERR_BOUNDS; }

    if (LDG_UNLIKELY(addr < hook_ctx + sizeof(*ctx) && hook_ctx < addr + len)) { return LDG_ERR_FUNC_ARG_INVALID; }

    res = better_together_setup_mem_wr_validate(addr, len);
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    AcquireSRWLockShared(&ctx->lock);
    out[0] = ctx->state;
    out[1] = ctx->res;
    out[2] = ctx->owner;
    out[3] = ctx->orig.owner;
    out[4] = ctx->boxie.world;
    out[5] = ctx->boxie.rope[0];
    out[6] = ctx->boxie.rope[1];
    out[7] = ctx->boxie.rope_entity[0];
    out[8] = ctx->boxie.rope_entity[1];
    out[9] = ctx->boxie.back[0];
    out[10] = ctx->boxie.back[1];
    out[11] = ctx->scope.renamed[0];
    out[12] = ctx->scope.renamed[1];
    out[13] = ctx->attempts;
    out[14] = ctx->successes;
    out[15] = ctx->hook_res;
    ReleaseSRWLockShared(&ctx->lock);
    return LDG_ERR_AOK;
}
