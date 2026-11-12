#include <stdint.h>
#include <math.h>
#include <windows.h>

#include <MinHook.h>
#include <dangling/core/err.h>
#include <dangling/core/macros.h>
#include <dangling/mem/secure.h>
#include <dangling/str/fmt.h>
#include <libresteam/ext/together/model.h>
#include <libresteam/ext/together/owner.h>
#include <libresteam/ext/together/setup.h>
#include <libresteam/ext/together/spawn.h>

#define BETTER_TOGETHER_SPAWN_MAGIC UINT64_C(0x424F584945535057)
#define BETTER_TOGETHER_SPAWN_PE_TIMESTAMP 0x56952868U
#define BETTER_TOGETHER_SPAWN_IMG_SIZE 0x04D13000U
#define BETTER_TOGETHER_SPAWN_ENTRY_RVA 0x1449B30U
#define BETTER_TOGETHER_SPAWN_HASH_RVA 0x1796D30U
#define BETTER_TOGETHER_SPAWN_ALLOC_RVA 0x191507CU
#define BETTER_TOGETHER_SPAWN_OWNER_RVA 0x1447620U
#define BETTER_TOGETHER_SPAWN_OWNER_CONT_RVA 0x1447681U
#define BETTER_TOGETHER_SPAWN_OWNER_PUBLISH_RVA 0x144767AU
#define BETTER_TOGETHER_SPAWN_GLOBAL_RVA 0x8D3AB8U
#define BETTER_TOGETHER_SPAWN_RECORD_INIT_RVA 0x145E110U
#define BETTER_TOGETHER_SPAWN_RECORD_COPY_RVA 0x145E370U
#define BETTER_TOGETHER_SPAWN_CHECKPOINT_RVA 0x145F5A0U
#define BETTER_TOGETHER_SPAWN_FIND_RVA 0x17B9310U
#define BETTER_TOGETHER_SPAWN_GAME_CHECKPOINT_OFF 0x350U
#define BETTER_TOGETHER_SPAWN_GAME_OWNER_OFF 0x358U
#define BETTER_TOGETHER_SPAWN_ENTITY_POS_OFF 0x5CU
#define BETTER_TOGETHER_SPAWN_OWNER_BYTES 0xB0U
#define BETTER_TOGETHER_SPAWN_OWNER_ENTITY_OFF 0x20U
#define BETTER_TOGETHER_SPAWN_OWNER_CTX_OFF 0x18U
#define BETTER_TOGETHER_SPAWN_CTX_ENTITY_OFF 0x140U
#define BETTER_TOGETHER_SPAWN_WORLD_MNGR_OFF 0xB8U
#define BETTER_TOGETHER_SPAWN_MNGR_WORLD_OFF 0x58U
#define BETTER_TOGETHER_SPAWN_ENTITY_HASH_OFF 0x08U
#define BETTER_TOGETHER_SPAWN_RECORD_WORDS 32U
#define BETTER_TOGETHER_SPAWN_OWNER_WORDS 5U
#define BETTER_TOGETHER_SPAWN_ALIAS_CUNT 3U
#define BETTER_TOGETHER_SPAWN_NAME_CAP 24U
#define BETTER_TOGETHER_SPAWN_CODE_SLOT_BYTES 128U
#define BETTER_TOGETHER_SPAWN_HOOK_CUNT 2U
#define BETTER_TOGETHER_SPAWN_CODE_BYTES (BETTER_TOGETHER_SPAWN_CODE_SLOT_BYTES * BETTER_TOGETHER_SPAWN_HOOK_CUNT)
#define BETTER_TOGETHER_SPAWN_MUT_CAP 96U
#define BETTER_TOGETHER_SPAWN_CODE_EXEC_ONLY 0U
#define BETTER_TOGETHER_SPAWN_HASH_UNSET 0U
#define BETTER_TOGETHER_SPAWN_RENAMED_NONE 0U
#define BETTER_TOGETHER_SPAWN_SETUP_ARG_NONE 0U
#define BETTER_TOGETHER_SPAWN_FIND_FLAGS 0U
#define BETTER_TOGETHER_SPAWN_ROOT_NONE 0U
#define BETTER_TOGETHER_SPAWN_THREAD_NONE 0U
#define BETTER_TOGETHER_SPAWN_NATIVE_NONE 0U
#define BETTER_TOGETHER_SPAWN_QWORD_ALIGN_MASK (sizeof(uint64_t) - 1U)
#define BETTER_TOGETHER_SPAWN_HOST_X_OFFSET -0.197623
#define BETTER_TOGETHER_SPAWN_POS_CUNT 3U
#define BETTER_TOGETHER_SPAWN_RECORD_ANCHOR_OFF 0x48U

typedef struct better_together_spawn_name
{
    uint64_t data;
    uint64_t len;
} better_together_spawn_name_t;

typedef uint64_t (*better_together_spawn_entry_func_t)(uint64_t owner, uint64_t world, uint64_t record);
typedef uint32_t (*better_together_spawn_hash_func_t)(const better_together_spawn_name_t *name);
typedef uint64_t (*better_together_spawn_alloc_func_t)(uint64_t bytes);
typedef uint64_t (*better_together_spawn_record_init_func_t)(uint64_t record);
typedef uint64_t (*better_together_spawn_record_copy_func_t)(uint64_t dst, uint64_t src);
typedef uint8_t (*better_together_spawn_checkpoint_func_t)(uint64_t checkpoint, uint64_t hook_ctx, uint64_t record);
typedef uint64_t (*better_together_spawn_find_func_t)(uint64_t mngr, uint32_t hash, uint32_t scope);

typedef struct better_together_spawn_alias
{
    char src[BETTER_TOGETHER_SPAWN_NAME_CAP];
    char dst[BETTER_TOGETHER_SPAWN_NAME_CAP];
    uint64_t len;
    uint32_t hash;
    uint32_t renamed;
} better_together_spawn_alias_t;

typedef struct better_together_spawn_ctx
{
    uint64_t magic;
    uint64_t base;
    uint64_t record[BETTER_TOGETHER_SPAWN_RECORD_WORDS];
    uint64_t manual_record[BETTER_TOGETHER_SPAWN_RECORD_WORDS];
    uint64_t orig_fields[BETTER_TOGETHER_SPAWN_OWNER_WORDS];
    uint64_t native_ret;
    uint64_t orig_tgt;
    void *setup;
    better_together_spawn_roots_cb_t roots;
    void *roots_user;
    void *code;
    HMODULE module;
    HANDLE singleton;
    SRWLOCK lock;
    RUNTIME_FUNCTION unwind[BETTER_TOGETHER_SPAWN_HOOK_CUNT];
    better_together_spawn_entry_func_t entry;
    better_together_spawn_hash_func_t hash;
    better_together_spawn_record_init_func_t record_init;
    better_together_spawn_checkpoint_func_t checkpoint;
    better_together_spawn_find_func_t find;
    better_together_spawn_alias_t aliases[BETTER_TOGETHER_SPAWN_ALIAS_CUNT];
    better_together_spawn_report_t report;
    uint32_t tls;
    uint32_t scope_thread;
    uint32_t scope_res;
    uint8_t unwind_up;
    uint8_t record_up;
    uint8_t setup_up;
    uint8_t manual_record_up;
    LONG en;
} better_together_spawn_ctx_t;

static uint32_t better_together_spawn_mem_rd(uint64_t addr, void *out, uint64_t len)
{
    SIZE_T copied = 0;

    if (LDG_UNLIKELY(!addr || !out)) { return LDG_ERR_FUNC_ARG_NULL; }

    if (LDG_UNLIKELY(!len || addr > UINT64_MAX - len)) { return LDG_ERR_FUNC_ARG_INVALID; }

    if (LDG_UNLIKELY(!ReadProcessMemory(GetCurrentProcess(), (const void *)(uintptr_t)addr, out, (SIZE_T)len, &copied) || copied != len)) { return LDG_ERR_MEM_BAD; }

    return LDG_ERR_AOK;
}

static uint32_t better_together_spawn_code_validate(uint64_t addr, const uint8_t *bytes, uint64_t len)
{
    MEMORY_BASIC_INFORMATION info = LDG_STRUCT_ZERO_INIT;
    uint8_t actual[32] = LDG_ARR_ZERO_INIT;
    uint32_t res = LDG_ERR_AOK;

    if (LDG_UNLIKELY(!VirtualQuery((const void *)(uintptr_t)addr, &info, sizeof(info)) || info.State != MEM_COMMIT || (info.Protect & (PAGE_GUARD | PAGE_NOACCESS)) || !(info.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)))) { return LDG_ERR_DENIED; }

    if (!len) { return LDG_ERR_AOK; }

    if (LDG_UNLIKELY(!bytes || len > sizeof(actual))) { return LDG_ERR_FUNC_ARG_INVALID; }

    res = better_together_spawn_mem_rd(addr, actual, len);
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    return ldg_mem_secure_eq_is(actual, bytes, len) == LDG_TRUTH_TRUE ? LDG_ERR_AOK : LDG_ERR_UNSUPPORTED;
}

static uint32_t better_together_spawn_profile_validate(uint64_t base)
{
    IMAGE_DOS_HEADER dos = LDG_STRUCT_ZERO_INIT;
    IMAGE_NT_HEADERS64 pe = LDG_STRUCT_ZERO_INIT;
    const uint8_t owner[] = { 0x48, 0x89, 0x5C, 0x24, 0x08, 0x48, 0x89, 0x74, 0x24, 0x10, 0x57, 0x48, 0x83, 0xEC, 0x20, 0x31 };
    const uint8_t publish[] = { 0x48, 0x89, 0x0D, 0x37, 0xC4, 0x48, 0xFF };
    const uint8_t cont[] = { 0x48, 0x89, 0x79, 0x08, 0x48, 0x89, 0x79, 0x10 };
    const uint8_t alloc[] = { 0xFF, 0x25, 0x4E, 0x47, 0x2E, 0x00 };
    const uint8_t record_init[] = { 0x48, 0x89, 0x4C, 0x24, 0x08, 0x53, 0x48, 0x83, 0xEC, 0x30 };
    const uint8_t record_copy[] = { 0x48, 0x89, 0x5C, 0x24, 0x08, 0x57, 0x48, 0x83, 0xEC, 0x20, 0x8B, 0x02 };
    const uint8_t hash[] = { 0x48, 0x89, 0x5C, 0x24, 0x08, 0x48, 0x89, 0x74, 0x24, 0x10, 0x48, 0x89, 0x7C, 0x24, 0x18, 0x4C };
    uint32_t res = LDG_ERR_AOK;

    res = better_together_spawn_mem_rd(base, &dos, sizeof(dos));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(dos.e_magic != IMAGE_DOS_SIGNATURE || (uint32_t)dos.e_lfanew > BETTER_TOGETHER_SPAWN_IMG_SIZE - sizeof(pe))) { return LDG_ERR_UNSUPPORTED; }

    res = better_together_spawn_mem_rd(base + (uint32_t)dos.e_lfanew, &pe, sizeof(pe));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(pe.Signature != IMAGE_NT_SIGNATURE || pe.FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64 || pe.FileHeader.TimeDateStamp != BETTER_TOGETHER_SPAWN_PE_TIMESTAMP || pe.OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC || pe.OptionalHeader.SizeOfImage != BETTER_TOGETHER_SPAWN_IMG_SIZE)) { return LDG_ERR_UNSUPPORTED; }

    res = better_together_spawn_code_validate(base + BETTER_TOGETHER_SPAWN_OWNER_RVA, owner, sizeof(owner));
    if (res == LDG_ERR_AOK) { res = better_together_spawn_code_validate(base + BETTER_TOGETHER_SPAWN_OWNER_PUBLISH_RVA, publish, sizeof(publish)); }

    if (res == LDG_ERR_AOK) { res = better_together_spawn_code_validate(base + BETTER_TOGETHER_SPAWN_OWNER_CONT_RVA, cont, sizeof(cont)); }

    if (res == LDG_ERR_AOK) { res = better_together_spawn_code_validate(base + BETTER_TOGETHER_SPAWN_ALLOC_RVA, alloc, sizeof(alloc)); }

    if (res == LDG_ERR_AOK) { res = better_together_spawn_code_validate(base + BETTER_TOGETHER_SPAWN_RECORD_INIT_RVA, record_init, sizeof(record_init)); }

    if (res == LDG_ERR_AOK) { res = better_together_spawn_code_validate(base + BETTER_TOGETHER_SPAWN_RECORD_COPY_RVA, record_copy, sizeof(record_copy)); }

    if (res == LDG_ERR_AOK) { res = better_together_spawn_code_validate(base + BETTER_TOGETHER_SPAWN_HASH_RVA, hash, sizeof(hash)); }

    if (res == LDG_ERR_AOK) { res = better_together_spawn_code_validate(base + BETTER_TOGETHER_SPAWN_ENTRY_RVA, 0x0, BETTER_TOGETHER_SPAWN_CODE_EXEC_ONLY); }

    if (res == LDG_ERR_AOK) { res = better_together_spawn_code_validate(base + BETTER_TOGETHER_SPAWN_CHECKPOINT_RVA, 0x0, BETTER_TOGETHER_SPAWN_CODE_EXEC_ONLY); }

    if (res == LDG_ERR_AOK) { res = better_together_spawn_code_validate(base + BETTER_TOGETHER_SPAWN_FIND_RVA, 0x0, BETTER_TOGETHER_SPAWN_CODE_EXEC_ONLY); }

    return res;
}

static uint32_t better_together_spawn_aliases_init(better_together_spawn_ctx_t *ctx)
{
    const better_together_spawn_alias_t aliases[BETTER_TOGETHER_SPAWN_ALIAS_CUNT] = { { "Yarny", "Boxie", 5, BETTER_TOGETHER_SPAWN_HASH_UNSET, BETTER_TOGETHER_SPAWN_RENAMED_NONE }, { "YarnyTopCollider", "BoxieTopCollider", 16, BETTER_TOGETHER_SPAWN_HASH_UNSET, BETTER_TOGETHER_SPAWN_RENAMED_NONE }, { "YarnyBottomCollider", "BoxieBottomCollider", 19, BETTER_TOGETHER_SPAWN_HASH_UNSET, BETTER_TOGETHER_SPAWN_RENAMED_NONE } };
    better_together_spawn_name_t name = LDG_STRUCT_ZERO_INIT;
    uint32_t idx = 0;
    uint32_t res = LDG_ERR_AOK;

    res = ldg_mem_secure_cpy(ctx->aliases, aliases, sizeof(aliases));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    for (idx = 0; idx < BETTER_TOGETHER_SPAWN_ALIAS_CUNT; idx++)
    {
        name.data = (uint64_t)(uintptr_t)ctx->aliases[idx].dst;
        name.len = ctx->aliases[idx].len;
        ctx->aliases[idx].hash = ctx->hash(&name);
    }
    name.data = (uint64_t)(uintptr_t)ctx->aliases[0].src;
    name.len = ctx->aliases[0].len;
    if (LDG_UNLIKELY(ctx->hash(&name) != BETTER_TOGETHER_YARNY_HASH || ctx->aliases[0].hash != BETTER_TOGETHER_BOXIE_HASH)) { return LDG_ERR_UNSUPPORTED; }

    return LDG_ERR_AOK;
}

static uint32_t better_together_spawn_hash_cb(const better_together_spawn_name_t *name, uint64_t unused_a __attribute__((unused)), uint64_t unused_b __attribute__((unused)), better_together_spawn_ctx_t *ctx)
{
    better_together_spawn_name_t input = LDG_STRUCT_ZERO_INIT;
    uint8_t text[BETTER_TOGETHER_SPAWN_NAME_CAP] = LDG_ARR_ZERO_INIT;
    uint32_t idx = 0;
    uint32_t res = LDG_ERR_AOK;

    if (TlsGetValue(ctx->tls) != ctx || ctx->scope_thread != GetCurrentThreadId()) { return ctx->hash(name); }

    res = better_together_spawn_mem_rd((uint64_t)(uintptr_t)name, &input, sizeof(input));
    if (res != LDG_ERR_AOK) { ctx->scope_res = res; return ctx->hash(name); }

    for (idx = 0; idx < BETTER_TOGETHER_SPAWN_ALIAS_CUNT; idx++)
    {
        if (input.len != ctx->aliases[idx].len) { continue; }

        res = better_together_spawn_mem_rd(input.data, text, input.len);
        if (res != LDG_ERR_AOK) { ctx->scope_res = res; return ctx->hash(name); }

        if (ldg_mem_secure_eq_is(text, ctx->aliases[idx].src, input.len) != LDG_TRUTH_TRUE) { continue; }

        if (ctx->aliases[idx].renamed == UINT32_MAX) { ctx->scope_res = LDG_ERR_OVERFLOW; }
        else { ctx->aliases[idx].renamed++; }

        return ctx->aliases[idx].hash;
    }
    return ctx->hash(name);
}

static uint32_t better_together_spawn_orig_validate(better_together_spawn_ctx_t *ctx, uint64_t owner, uint64_t world)
{
    uint64_t global = 0;
    uint64_t mngr = 0;
    uint64_t back = 0;
    uint32_t hash = 0;
    uint32_t res = LDG_ERR_AOK;

    res = better_together_spawn_mem_rd(ctx->base + BETTER_TOGETHER_SPAWN_GLOBAL_RVA, &global, sizeof(global));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(!owner || owner != global || !world)) { return LDG_ERR_DENIED; }

    res = better_together_spawn_mem_rd(owner, ctx->orig_fields, sizeof(ctx->orig_fields));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(ctx->orig_fields[0] != world || !ctx->orig_fields[1] || !ctx->orig_fields[2] || !ctx->orig_fields[3] || !ctx->orig_fields[4])) { return LDG_ERR_NOT_INIT; }

    res = better_together_spawn_mem_rd(world + BETTER_TOGETHER_SPAWN_WORLD_MNGR_OFF, &mngr, sizeof(mngr));
    if (res == LDG_ERR_AOK) { res = better_together_spawn_mem_rd(mngr + BETTER_TOGETHER_SPAWN_MNGR_WORLD_OFF, &back, sizeof(back)); }

    if (res == LDG_ERR_AOK) { res = better_together_spawn_mem_rd(ctx->orig_fields[4] + BETTER_TOGETHER_SPAWN_ENTITY_HASH_OFF, &hash, sizeof(hash)); }

    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(back != world || hash != BETTER_TOGETHER_YARNY_HASH)) { return LDG_ERR_INVALID; }

    return better_together_spawn_mem_rd(ctx->orig_fields[BETTER_TOGETHER_SPAWN_OWNER_CTX_OFF / sizeof(uint64_t)] + BETTER_TOGETHER_SPAWN_CTX_ENTITY_OFF, &ctx->orig_tgt, sizeof(ctx->orig_tgt));
}

static uint32_t better_together_spawn_record_prepare(uint64_t *record)
{
    libresteam_fp32_t pos[BETTER_TOGETHER_SPAWN_POS_CUNT] = LDG_ARR_ZERO_INIT;
    uint32_t idx = 0;
    uint32_t res = LDG_ERR_AOK;

    if (LDG_UNLIKELY(!record)) { return LDG_ERR_FUNC_ARG_NULL; }

    res = ldg_mem_secure_cpy(pos, record, sizeof(pos));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    for (idx = 0; idx < BETTER_TOGETHER_SPAWN_POS_CUNT; idx++) { if (LDG_UNLIKELY(!isfinite(pos[idx]))) { return LDG_ERR_INVALID; } }
    pos[0] = (libresteam_fp32_t)((double)pos[0] + BETTER_TOGETHER_SPAWN_HOST_X_OFFSET);
    if (LDG_UNLIKELY(!isfinite(pos[0]))) { return LDG_ERR_INVALID; }

    res = ldg_mem_secure_cpy(record, pos, sizeof(pos[0]));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    record[BETTER_TOGETHER_SPAWN_RECORD_ANCHOR_OFF / sizeof(uint64_t)] = 0;
    return LDG_ERR_AOK;
}

static uint32_t better_together_spawn_record_create(better_together_spawn_ctx_t *ctx, uint64_t src)
{
    better_together_spawn_record_init_func_t init = (better_together_spawn_record_init_func_t)(uintptr_t)(ctx->base + BETTER_TOGETHER_SPAWN_RECORD_INIT_RVA);
    better_together_spawn_record_copy_func_t copy = (better_together_spawn_record_copy_func_t)(uintptr_t)(ctx->base + BETTER_TOGETHER_SPAWN_RECORD_COPY_RVA);
    uint64_t dst = (uint64_t)(uintptr_t)ctx->record;
    uint32_t res = LDG_ERR_AOK;

    if (!ctx->record_up)
    {
        if (LDG_UNLIKELY(init(dst) != dst)) { return LDG_ERR_INVALID; }

        ctx->record_up = LDG_TRUTH_TRUE;
    }

    if (LDG_UNLIKELY(copy(dst, src) != dst)) { return LDG_ERR_INVALID; }

    res = better_together_spawn_record_prepare(ctx->record);
    return res;
}

static uint32_t better_together_spawn_owner_create(better_together_spawn_ctx_t *ctx)
{
    better_together_spawn_alloc_func_t alloc = (better_together_spawn_alloc_func_t)(uintptr_t)(ctx->base + BETTER_TOGETHER_SPAWN_ALLOC_RVA);
    uint64_t constructed = 0;
    uint64_t global = 0;
    uint32_t res = LDG_ERR_AOK;

    ctx->report.owner = alloc(BETTER_TOGETHER_SPAWN_OWNER_BYTES);
    if (LDG_UNLIKELY(!ctx->report.owner)) { return LDG_ERR_ALLOC_NULL; }

    res = better_together_owner_construct(ctx->report.owner, ctx->base + BETTER_TOGETHER_SPAWN_OWNER_CONT_RVA, &constructed);
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(constructed != ctx->report.owner)) { return LDG_ERR_INVALID; }

    res = better_together_spawn_mem_rd(ctx->base + BETTER_TOGETHER_SPAWN_GLOBAL_RVA, &global, sizeof(global));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(global != ctx->report.orig)) { return LDG_ERR_MEM_CORRUPTION; }

    if (ctx->setup_up) { return better_together_setup_owner_bind(ctx->setup, ctx->report.owner); }

    res = better_together_setup_init(ctx->setup, better_together_setup_size_get(), ctx->report.owner);
    if (res == LDG_ERR_AOK) { ctx->setup_up = LDG_TRUTH_TRUE; }

    return res;
}

static uint32_t better_together_spawn_attachment_validate(better_together_spawn_ctx_t *ctx)
{
    uint64_t hook_ctx = 0;
    uint64_t tgt = 0;
    uint32_t res = LDG_ERR_AOK;

    res = better_together_spawn_mem_rd(ctx->orig_fields[BETTER_TOGETHER_SPAWN_OWNER_CTX_OFF / sizeof(uint64_t)] + BETTER_TOGETHER_SPAWN_CTX_ENTITY_OFF, &tgt, sizeof(tgt));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(tgt != ctx->orig_tgt)) { return LDG_ERR_MEM_CORRUPTION; }

    res = better_together_spawn_mem_rd(ctx->report.owner + BETTER_TOGETHER_SPAWN_OWNER_CTX_OFF, &hook_ctx, sizeof(hook_ctx));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(!hook_ctx || hook_ctx == ctx->orig_fields[BETTER_TOGETHER_SPAWN_OWNER_CTX_OFF / sizeof(uint64_t)])) { return LDG_ERR_INVALID; }

    res = better_together_spawn_mem_rd(hook_ctx + BETTER_TOGETHER_SPAWN_CTX_ENTITY_OFF, &tgt, sizeof(tgt));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(tgt && tgt != ctx->report.entity)) { return LDG_ERR_INVALID; }

    return LDG_ERR_AOK;
}

static uint32_t better_together_spawn_after_validate(better_together_spawn_ctx_t *ctx)
{
    uint64_t fields[BETTER_TOGETHER_SPAWN_OWNER_WORDS] = LDG_ARR_ZERO_INIT;
    uint64_t global = 0;
    uint32_t hash = 0;
    uint32_t res = LDG_ERR_AOK;

    res = better_together_spawn_mem_rd(ctx->base + BETTER_TOGETHER_SPAWN_GLOBAL_RVA, &global, sizeof(global));
    if (res == LDG_ERR_AOK) { res = better_together_spawn_mem_rd(ctx->report.orig, fields, sizeof(fields)); }

    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(global != ctx->report.orig || ldg_mem_secure_eq_is(fields, ctx->orig_fields, sizeof(fields)) != LDG_TRUTH_TRUE)) { return LDG_ERR_MEM_CORRUPTION; }

    res = better_together_spawn_mem_rd(ctx->report.owner + BETTER_TOGETHER_SPAWN_OWNER_ENTITY_OFF, &ctx->report.entity, sizeof(ctx->report.entity));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(!ctx->report.entity || ctx->report.entity == fields[4])) { return LDG_ERR_INVALID; }

    res = better_together_spawn_mem_rd(ctx->report.entity + BETTER_TOGETHER_SPAWN_ENTITY_HASH_OFF, &hash, sizeof(hash));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(hash != BETTER_TOGETHER_BOXIE_HASH)) { return LDG_ERR_INVALID; }

    res = better_together_spawn_attachment_validate(ctx);
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    return better_together_setup_validate(ctx->setup, ctx->report.world);
}

static uint32_t better_together_spawn_run(better_together_spawn_ctx_t *ctx, uint64_t owner, uint64_t world, uint64_t record)
{
    uint32_t res = LDG_ERR_AOK;
    uint32_t idx = 0;
    MH_STATUS status = MH_OK;

    res = better_together_spawn_orig_validate(ctx, owner, world);
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    ctx->report.orig = owner;
    ctx->report.world = world;
    ctx->report.thread = GetCurrentThreadId();
    for (idx = 0; idx < BETTER_TOGETHER_SPAWN_ALIAS_CUNT; idx++) { ctx->aliases[idx].renamed = 0; }
    res = better_together_spawn_record_create(ctx, record);
    if (res == LDG_ERR_AOK) { res = better_together_spawn_owner_create(ctx); }

    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    status = MH_EnableHook((void *)(uintptr_t)(ctx->base + BETTER_TOGETHER_SPAWN_HASH_RVA));
    ctx->report.hook_res = (uint32_t)status;
    if (LDG_UNLIKELY(status != MH_OK && status != MH_ERROR_ENABLED)) { return LDG_ERR_INVALID; }

    ctx->scope_thread = GetCurrentThreadId();
    ctx->scope_res = LDG_ERR_AOK;
    if (LDG_UNLIKELY(!TlsSetValue(ctx->tls, ctx))) { return LDG_ERR_DENIED; }

    res = ctx->roots(ctx->roots_user, owner, ctx->report.owner, ctx->report.thread);
    if (res == LDG_ERR_AOK) { res = better_together_setup_enter(ctx->setup, world, BETTER_TOGETHER_SPAWN_SETUP_ARG_NONE); }

    if (res == LDG_ERR_AOK) { ctx->native_ret = ctx->entry(ctx->report.owner, world, (uint64_t)(uintptr_t)ctx->record); }

    if (!TlsSetValue(ctx->tls, 0x0)) { res = LDG_ERR_DENIED; }

    ctx->scope_thread = 0;
    if (res == LDG_ERR_AOK) { res = ctx->scope_res; }

    for (idx = 0; idx < BETTER_TOGETHER_SPAWN_ALIAS_CUNT; idx++)
    {
        ctx->report.renamed[idx] = ctx->aliases[idx].renamed;
        if (res == LDG_ERR_AOK && !ctx->aliases[idx].renamed) { res = LDG_ERR_NOT_FOUND; }
    }
    if (res == LDG_ERR_AOK) { res = better_together_spawn_after_validate(ctx); }

    return res;
}

static uint32_t better_together_spawn_manual_record_get(better_together_spawn_ctx_t *ctx, uint64_t game)
{
    libresteam_fp32_t pos[BETTER_TOGETHER_SPAWN_POS_CUNT] = LDG_ARR_ZERO_INIT;
    uint64_t checkpoint = 0;
    uint64_t record = (uint64_t)(uintptr_t)ctx->manual_record;
    uint32_t idx = 0;
    uint32_t res = LDG_ERR_AOK;

    res = better_together_spawn_mem_rd(game + BETTER_TOGETHER_SPAWN_GAME_CHECKPOINT_OFF, &checkpoint, sizeof(checkpoint));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(!checkpoint)) { return LDG_ERR_NOT_INIT; }

    res = better_together_spawn_mem_rd(ctx->orig_fields[BETTER_TOGETHER_SPAWN_OWNER_ENTITY_OFF / sizeof(uint64_t)] + BETTER_TOGETHER_SPAWN_ENTITY_POS_OFF, pos, sizeof(pos));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    for (idx = 0; idx < BETTER_TOGETHER_SPAWN_POS_CUNT; idx++) { if (LDG_UNLIKELY(!isfinite(pos[idx]))) { return LDG_ERR_INVALID; } }
    if (!ctx->manual_record_up)
    {
        if (LDG_UNLIKELY(ctx->record_init(record) != record)) { return LDG_ERR_INVALID; }

        ctx->manual_record_up = LDG_TRUTH_TRUE;
    }

    if (LDG_UNLIKELY(ctx->checkpoint(checkpoint, ctx->orig_fields[BETTER_TOGETHER_SPAWN_OWNER_CTX_OFF / sizeof(uint64_t)], record) != LDG_TRUTH_TRUE)) { return LDG_ERR_NOT_FOUND; }

    return ldg_mem_secure_cpy(ctx->manual_record, pos, sizeof(pos));
}

static uint32_t better_together_spawn_manual_run(better_together_spawn_ctx_t *ctx, uint64_t game)
{
    uint64_t owner = 0;
    uint64_t world = 0;
    uint64_t mngr = 0;
    uint64_t entity = 0;
    uint32_t hash = 0;
    uint32_t res = LDG_ERR_AOK;

    res = better_together_spawn_mem_rd(game + BETTER_TOGETHER_SPAWN_GAME_OWNER_OFF, &owner, sizeof(owner));
    if (res == LDG_ERR_AOK && owner) { res = better_together_spawn_mem_rd(owner, &world, sizeof(world)); }

    if (res == LDG_ERR_AOK) { res = better_together_spawn_orig_validate(ctx, owner, world); }

    if (res == LDG_ERR_AOK) { res = better_together_spawn_mem_rd(world + BETTER_TOGETHER_SPAWN_WORLD_MNGR_OFF, &mngr, sizeof(mngr)); }

    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    entity = ctx->find(mngr, BETTER_TOGETHER_BOXIE_HASH, BETTER_TOGETHER_SPAWN_FIND_FLAGS);
    if (entity)
    {
        res = better_together_spawn_mem_rd(entity + BETTER_TOGETHER_SPAWN_ENTITY_HASH_OFF, &hash, sizeof(hash));
        if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

        if (LDG_UNLIKELY(hash != BETTER_TOGETHER_BOXIE_HASH)) { return LDG_ERR_INVALID; }

        ctx->report.req_existing = LDG_TRUTH_TRUE;
        return LDG_ERR_AOK;
    }

    if (LDG_UNLIKELY(ctx->report.state == BETTER_TOGETHER_SPAWN_FAILED)) { return LDG_ERR_DENIED; }

    res = better_together_spawn_manual_record_get(ctx, game);
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    ctx->report.state = BETTER_TOGETHER_SPAWN_STALE;
    res = ctx->roots(ctx->roots_user, BETTER_TOGETHER_SPAWN_ROOT_NONE, BETTER_TOGETHER_SPAWN_ROOT_NONE, BETTER_TOGETHER_SPAWN_THREAD_NONE);
    if (res == LDG_ERR_AOK)
    {
        ctx->report.state = BETTER_TOGETHER_SPAWN_RUNNING;
        res = better_together_spawn_run(ctx, owner, world, (uint64_t)(uintptr_t)ctx->manual_record);
    }

    ctx->report.res = res;
    ctx->report.state = res == LDG_ERR_AOK ? BETTER_TOGETHER_SPAWN_COMPLETE : BETTER_TOGETHER_SPAWN_FAILED;
    if (res != LDG_ERR_AOK || !InterlockedCompareExchange(&ctx->en, LDG_TRUTH_FALSE, LDG_TRUTH_FALSE))
    {
        res = ctx->roots(ctx->roots_user, BETTER_TOGETHER_SPAWN_ROOT_NONE, BETTER_TOGETHER_SPAWN_ROOT_NONE, BETTER_TOGETHER_SPAWN_THREAD_NONE);
        if (res != LDG_ERR_AOK) { ctx->report.res = res; }
    }

    return ctx->report.res;
}

uint32_t better_together_spawn_req(void *storage)
{
    better_together_spawn_ctx_t *ctx = (better_together_spawn_ctx_t *)storage;

    if (LDG_UNLIKELY(!ctx)) { return LDG_ERR_FUNC_ARG_NULL; }

    if (LDG_UNLIKELY(ctx->magic != BETTER_TOGETHER_SPAWN_MAGIC)) { return LDG_ERR_NOT_INIT; }

    if (LDG_UNLIKELY(!TryAcquireSRWLockExclusive(&ctx->lock))) { return LDG_ERR_BUSY; }

    if (!InterlockedCompareExchange(&ctx->en, LDG_TRUTH_FALSE, LDG_TRUTH_FALSE)) { ReleaseSRWLockExclusive(&ctx->lock); return LDG_ERR_NOT_INIT; }

    ctx->report.req_pending = LDG_TRUTH_TRUE;
    ReleaseSRWLockExclusive(&ctx->lock);
    return LDG_ERR_AOK;
}

uint32_t better_together_spawn_tick(void *storage, uint64_t game)
{
    better_together_spawn_ctx_t *ctx = (better_together_spawn_ctx_t *)storage;
    uint32_t res = LDG_ERR_AOK;

    if (LDG_UNLIKELY(!ctx || !game)) { return LDG_ERR_FUNC_ARG_NULL; }

    if (LDG_UNLIKELY(ctx->magic != BETTER_TOGETHER_SPAWN_MAGIC)) { return LDG_ERR_NOT_INIT; }

    if (LDG_UNLIKELY(game > UINT64_MAX - BETTER_TOGETHER_SPAWN_GAME_OWNER_OFF - sizeof(uint64_t))) { return LDG_ERR_RANGE; }

    if (LDG_UNLIKELY(!TryAcquireSRWLockExclusive(&ctx->lock))) { return LDG_ERR_BUSY; }

    if (ctx->report.req_pending && InterlockedCompareExchange(&ctx->en, LDG_TRUTH_FALSE, LDG_TRUTH_FALSE))
    {
        ctx->report.req_pending = LDG_TRUTH_FALSE;
        ctx->report.req_existing = LDG_TRUTH_FALSE;
        ctx->report.req_res = better_together_spawn_manual_run(ctx, game);
        ctx->report.req_cunt++;
        res = ctx->report.req_res;
    }

    ReleaseSRWLockExclusive(&ctx->lock);
    return res;
}

static uint64_t better_together_spawn_entry_cb(uint64_t owner, uint64_t world, uint64_t record, better_together_spawn_ctx_t *ctx)
{
    void *previous = TlsGetValue(ctx->tls);
    uint64_t ret = 0;
    uint64_t global = 0;
    uint32_t res = LDG_ERR_AOK;

    if (previous == ctx && !TlsSetValue(ctx->tls, 0x0)) { ctx->scope_res = LDG_ERR_DENIED; return BETTER_TOGETHER_SPAWN_NATIVE_NONE; }

    if (previous != ctx && InterlockedCompareExchange(&ctx->en, LDG_TRUTH_FALSE, LDG_TRUTH_FALSE))
    {
        res = better_together_spawn_mem_rd(ctx->base + BETTER_TOGETHER_SPAWN_GLOBAL_RVA, &global, sizeof(global));
        if (res == LDG_ERR_AOK && owner && owner == global && TryAcquireSRWLockExclusive(&ctx->lock))
        {
            if (ctx->report.state == BETTER_TOGETHER_SPAWN_COMPLETE)
            {
                ctx->report.state = BETTER_TOGETHER_SPAWN_STALE;
                ctx->report.res = ctx->roots(ctx->roots_user, BETTER_TOGETHER_SPAWN_ROOT_NONE, BETTER_TOGETHER_SPAWN_ROOT_NONE, BETTER_TOGETHER_SPAWN_THREAD_NONE);
            }

            ReleaseSRWLockExclusive(&ctx->lock);
        }
    }

    ret = ctx->entry(owner, world, record);
    if (previous == ctx)
    {
        if (!TlsSetValue(ctx->tls, previous)) { ctx->scope_res = LDG_ERR_DENIED; }

        return ret;
    }

    if (!InterlockedCompareExchange(&ctx->en, LDG_TRUTH_FALSE, LDG_TRUTH_FALSE)) { return ret; }

    res = better_together_spawn_mem_rd(ctx->base + BETTER_TOGETHER_SPAWN_GLOBAL_RVA, &global, sizeof(global));
    if (res != LDG_ERR_AOK || !owner || owner != global) { return ret; }

    if (!TryAcquireSRWLockExclusive(&ctx->lock)) { return ret; }

    if ((ctx->report.state == BETTER_TOGETHER_SPAWN_WAITING || (ctx->report.state == BETTER_TOGETHER_SPAWN_STALE && ctx->report.res == LDG_ERR_AOK)) && InterlockedCompareExchange(&ctx->en, LDG_TRUTH_FALSE, LDG_TRUTH_FALSE))
    {
        ctx->report.state = BETTER_TOGETHER_SPAWN_RUNNING;
        ctx->report.res = better_together_spawn_run(ctx, owner, world, record);
        ctx->report.state = ctx->report.res == LDG_ERR_AOK ? BETTER_TOGETHER_SPAWN_COMPLETE : BETTER_TOGETHER_SPAWN_FAILED;
        if (ctx->report.state == BETTER_TOGETHER_SPAWN_FAILED || !InterlockedCompareExchange(&ctx->en, LDG_TRUTH_FALSE, LDG_TRUTH_FALSE))
        {
            res = ctx->roots(ctx->roots_user, BETTER_TOGETHER_SPAWN_ROOT_NONE, BETTER_TOGETHER_SPAWN_ROOT_NONE, BETTER_TOGETHER_SPAWN_THREAD_NONE);
            if (res != LDG_ERR_AOK) { ctx->report.res = res; }
        }
    }

    ReleaseSRWLockExclusive(&ctx->lock);
    return ret;
}

static uint32_t better_together_spawn_code_init(better_together_spawn_ctx_t *ctx)
{
    better_together_setup_layout_t layout[BETTER_TOGETHER_SETUP_THUNK_CUNT] = LDG_ARR_ZERO_INIT;
    uint64_t hook_cb[BETTER_TOGETHER_SPAWN_HOOK_CUNT] = { (uint64_t)(uintptr_t)better_together_spawn_entry_cb, (uint64_t)(uintptr_t)better_together_spawn_hash_cb };
    uint64_t hook_ctx = (uint64_t)(uintptr_t)ctx;
    uint8_t *dst = 0x0;
    uint32_t idx = 0;
    uint32_t off = 0;
    uint32_t res = LDG_ERR_AOK;
    DWORD old = 0;

    res = better_together_setup_layout_get(layout);
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(!layout[0].src || layout[0].len < sizeof(uint64_t) || layout[0].len > BETTER_TOGETHER_SPAWN_CODE_SLOT_BYTES || layout[0].code_end > layout[0].len || layout[0].ctx_off > layout[0].len - sizeof(uint64_t) || layout[0].cb_off > layout[0].len - sizeof(uint64_t) || layout[0].unwind_off > layout[0].len - sizeof(uint32_t) || (layout[0].unwind_off & 3))) { return LDG_ERR_INVALID; }

    ctx->code = VirtualAlloc(0x0, BETTER_TOGETHER_SPAWN_CODE_BYTES, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (LDG_UNLIKELY(!ctx->code)) { return LDG_ERR_ALLOC_NULL; }

    for (idx = 0; idx < BETTER_TOGETHER_SPAWN_HOOK_CUNT; idx++)
    {
        off = idx * BETTER_TOGETHER_SPAWN_CODE_SLOT_BYTES;
        dst = (uint8_t *)ctx->code + off;
        res = ldg_mem_secure_cpy(dst, (const void *)(uintptr_t)layout[0].src, layout[0].len);
        if (res == LDG_ERR_AOK) { res = ldg_mem_secure_cpy(dst + layout[0].ctx_off, &hook_ctx, sizeof(hook_ctx)); }

        if (res == LDG_ERR_AOK) { res = ldg_mem_secure_cpy(dst + layout[0].cb_off, &hook_cb[idx], sizeof(hook_cb[idx])); }

        if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

        ctx->unwind[idx].BeginAddress = off;
        ctx->unwind[idx].EndAddress = off + (uint32_t)layout[0].code_end;
        ctx->unwind[idx].UnwindData = off + (uint32_t)layout[0].unwind_off;
    }
    if (LDG_UNLIKELY(!VirtualProtect(ctx->code, BETTER_TOGETHER_SPAWN_CODE_BYTES, PAGE_EXECUTE_READ, &old) || !FlushInstructionCache(GetCurrentProcess(), ctx->code, BETTER_TOGETHER_SPAWN_CODE_BYTES))) { return LDG_ERR_DENIED; }

    if (LDG_UNLIKELY(!RtlAddFunctionTable(ctx->unwind, BETTER_TOGETHER_SPAWN_HOOK_CUNT, (DWORD64)(uintptr_t)ctx->code))) { return LDG_ERR_INVALID; }

    ctx->unwind_up = LDG_TRUTH_TRUE;
    return LDG_ERR_AOK;
}

static uint32_t better_together_spawn_hooks_init(better_together_spawn_ctx_t *ctx)
{
    void *entry = 0x0;
    void *hash = 0x0;
    uint32_t res = LDG_ERR_AOK;
    MH_STATUS status = MH_Initialize();

    if (status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED) { ctx->report.hook_res = (uint32_t)status; return LDG_ERR_INVALID; }

    status = MH_CreateHook((void *)(uintptr_t)(ctx->base + BETTER_TOGETHER_SPAWN_ENTRY_RVA), ctx->code, &entry);
    if (status != MH_OK) { ctx->report.hook_res = (uint32_t)status; return LDG_ERR_INVALID; }

    res = ldg_mem_secure_cpy(&ctx->entry, &entry, sizeof(entry));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    status = MH_CreateHook((void *)(uintptr_t)(ctx->base + BETTER_TOGETHER_SPAWN_HASH_RVA), (uint8_t *)ctx->code + BETTER_TOGETHER_SPAWN_CODE_SLOT_BYTES, &hash);
    if (status != MH_OK) { ctx->report.hook_res = (uint32_t)status; return LDG_ERR_INVALID; }

    res = ldg_mem_secure_cpy(&ctx->hash, &hash, sizeof(hash));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    status = MH_EnableHook((void *)(uintptr_t)(ctx->base + BETTER_TOGETHER_SPAWN_ENTRY_RVA));
    ctx->report.hook_res = (uint32_t)status;
    return status == MH_OK ? LDG_ERR_AOK : LDG_ERR_INVALID;
}

uint64_t better_together_spawn_size_get(void)
{
    return sizeof(better_together_spawn_ctx_t) + better_together_setup_size_get();
}

uint32_t better_together_spawn_init(void *storage, uint64_t cap, better_together_spawn_roots_cb_t roots, void *user)
{
    better_together_spawn_ctx_t *ctx = (better_together_spawn_ctx_t *)storage;
    HMODULE game = GetModuleHandleA("Unravel.exe");
    LONG previous = 0;
    uint32_t res = LDG_ERR_AOK;
    DWORD error = 0;
    char singleton_name[BETTER_TOGETHER_SPAWN_MUT_CAP] = LDG_ARR_ZERO_INIT;
    uint64_t len = 0;
    uint64_t magic = 0;

    if (LDG_UNLIKELY(!storage || !roots || !user)) { return LDG_ERR_FUNC_ARG_NULL; }

    if (LDG_UNLIKELY(cap < better_together_spawn_size_get() || ((uintptr_t)storage & (BETTER_TOGETHER_SPAWN_QWORD_ALIGN_MASK)))) { return LDG_ERR_FUNC_ARG_INVALID; }

    res = better_together_spawn_mem_rd((uint64_t)(uintptr_t)storage, &magic, sizeof(magic));
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    if (LDG_UNLIKELY(magic == BETTER_TOGETHER_SPAWN_MAGIC)) { return LDG_ERR_EXISTS; }

    if (LDG_UNLIKELY(!game || game != GetModuleHandleA(0x0))) { return LDG_ERR_NOT_FOUND; }

    res = better_together_spawn_profile_validate((uint64_t)(uintptr_t)game);
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    res = ldg_mem_secure_zero(storage, better_together_spawn_size_get());
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    ctx->magic = BETTER_TOGETHER_SPAWN_MAGIC;
    ctx->base = (uint64_t)(uintptr_t)game;
    ctx->setup = (uint8_t *)storage + sizeof(*ctx);
    ctx->roots = roots;
    ctx->roots_user = user;
    ctx->report.state = BETTER_TOGETHER_SPAWN_FAILED;
    ctx->hash = (better_together_spawn_hash_func_t)(uintptr_t)(ctx->base + BETTER_TOGETHER_SPAWN_HASH_RVA);
    ctx->record_init = (better_together_spawn_record_init_func_t)(uintptr_t)(ctx->base + BETTER_TOGETHER_SPAWN_RECORD_INIT_RVA);
    ctx->checkpoint = (better_together_spawn_checkpoint_func_t)(uintptr_t)(ctx->base + BETTER_TOGETHER_SPAWN_CHECKPOINT_RVA);
    ctx->find = (better_together_spawn_find_func_t)(uintptr_t)(ctx->base + BETTER_TOGETHER_SPAWN_FIND_RVA);
    ctx->tls = TLS_OUT_OF_INDEXES;
    InitializeSRWLock(&ctx->lock);
    res = ldg_str_fmt(singleton_name, sizeof(singleton_name), &len, "Local\\LibreSteam.Boxie.Spawn.%u32", (uint32_t)GetCurrentProcessId());
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    ctx->singleton = CreateMutexA(0x0, FALSE, singleton_name);
    error = GetLastError();
    if (LDG_UNLIKELY(!ctx->singleton)) { return LDG_ERR_ALLOC_NULL; }

    if (LDG_UNLIKELY(error == ERROR_ALREADY_EXISTS)) { return LDG_ERR_EXISTS; }

    res = better_together_spawn_aliases_init(ctx);
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    ctx->tls = TlsAlloc();
    if (LDG_UNLIKELY(ctx->tls == TLS_OUT_OF_INDEXES)) { return LDG_ERR_FULL; }

    if (LDG_UNLIKELY(!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN, (const char *)(uintptr_t)better_together_spawn_entry_cb, &ctx->module))) { return LDG_ERR_DENIED; }

    res = better_together_spawn_code_init(ctx);
    if (res == LDG_ERR_AOK) { res = better_together_spawn_hooks_init(ctx); }

    ctx->report.res = res;
    if (LDG_UNLIKELY(res != LDG_ERR_AOK)) { return res; }

    ctx->report.state = BETTER_TOGETHER_SPAWN_WAITING;
    previous = InterlockedExchange(&ctx->en, LDG_TRUTH_TRUE);
    return previous ? LDG_ERR_EXISTS : LDG_ERR_AOK;
}

uint32_t better_together_spawn_report(void *storage, better_together_spawn_report_t *out)
{
    better_together_spawn_ctx_t *ctx = (better_together_spawn_ctx_t *)storage;
    uint32_t res = LDG_ERR_AOK;

    if (LDG_UNLIKELY(!ctx || !out)) { return LDG_ERR_FUNC_ARG_NULL; }

    if (LDG_UNLIKELY(ctx->magic != BETTER_TOGETHER_SPAWN_MAGIC)) { return LDG_ERR_NOT_INIT; }

    if (LDG_UNLIKELY(!TryAcquireSRWLockShared(&ctx->lock))) { return LDG_ERR_BUSY; }

    res = ldg_mem_secure_cpy(out, &ctx->report, sizeof(*out));
    ReleaseSRWLockShared(&ctx->lock);
    return res;
}

uint32_t better_together_spawn_stop(void *storage)
{
    better_together_spawn_ctx_t *ctx = (better_together_spawn_ctx_t *)storage;
    LONG previous = 0;

    if (LDG_UNLIKELY(!ctx)) { return LDG_ERR_FUNC_ARG_NULL; }

    if (LDG_UNLIKELY(ctx->magic != BETTER_TOGETHER_SPAWN_MAGIC)) { return LDG_ERR_NOT_INIT; }

    previous = InterlockedExchange(&ctx->en, LDG_TRUTH_FALSE);
    AcquireSRWLockExclusive(&ctx->lock);
    ctx->report.req_pending = LDG_TRUTH_FALSE;
    ReleaseSRWLockExclusive(&ctx->lock);
    return previous ? ctx->roots(ctx->roots_user, BETTER_TOGETHER_SPAWN_ROOT_NONE, BETTER_TOGETHER_SPAWN_ROOT_NONE, BETTER_TOGETHER_SPAWN_THREAD_NONE) : LDG_ERR_NOT_INIT;
}
