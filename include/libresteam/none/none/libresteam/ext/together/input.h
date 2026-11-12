#ifndef BETTER_TOGETHER_INPUT_H
#define BETTER_TOGETHER_INPUT_H

#include <stdint.h>
#include <libresteam/ext/together/pad.h>
#include <libresteam/ext/together/tex.h>

#define BETTER_TOGETHER_INPUT_ACTION_CAP 16U
#define BETTER_TOGETHER_INPUT_QUERY_CUNT 3U

typedef struct better_together_input_sample
{
    uint32_t action;
    uint32_t reserved;
    double current;
    double prior;
} better_together_input_sample_t;

typedef struct better_together_input_packet
{
    uint64_t seq;
    uint32_t cunt;
    uint32_t reserved;
    better_together_input_sample_t samples[BETTER_TOGETHER_INPUT_ACTION_CAP];
} better_together_input_packet_t;

#define BETTER_TOGETHER_ATTACH_WAITING 0U
#define BETTER_TOGETHER_ATTACH_STARTED 1U
#define BETTER_TOGETHER_ATTACH_COMPLETE 2U
#define BETTER_TOGETHER_ATTACH_FAILED 3U

typedef struct better_together_input_report
{
    uint64_t owner;
    uint64_t seq;
    uint64_t routed[BETTER_TOGETHER_INPUT_QUERY_CUNT];
    uint64_t passed;
    uint64_t unknown;
    uint64_t scan_passes;
    uint32_t installed[BETTER_TOGETHER_INPUT_QUERY_CUNT];
    uint32_t group;
    uint32_t res;
    uint32_t ready;
    better_together_pad_report_t pads;
    uint64_t host_routed;
    uint64_t frame_time;
    uint32_t hook_res;
    uint32_t unknown_action;
    uint64_t root_calls[BETTER_TOGETHER_PAD_PLAYER_CUNT];
    uint64_t root_errors;
    uint32_t root_res;
    uint32_t root_thread;
    uint64_t cam_samples;
    double cam_dist;
    uint32_t cam_state;
    uint32_t cam_res;
    uint32_t cam_host;
    uint32_t cam_guest;
    char cam_err[128];
    better_together_tex_report_t tex;
    uint32_t attach_state;
    uint32_t attach_epoch;
    char attach_err[128];
} better_together_input_report_t;

uint64_t better_together_input_size_get(void);
typedef uint32_t (*better_together_input_spawn_cb_t)(void *user, uint64_t game);
uint32_t better_together_input_spawn_bind(void *storage, better_together_input_spawn_cb_t cb, void *user);
uint32_t better_together_input_init(void *storage, uint64_t cap);
uint32_t better_together_input_roots_bind(void *storage, uint64_t host, uint64_t guest, uint32_t thread);
uint32_t better_together_input_tick(void *storage, uint64_t owner, uint32_t thread);
uint32_t better_together_input_submit(void *storage, const better_together_input_packet_t *packet);
uint32_t better_together_input_controllers_sel(void *storage, uint32_t host, uint32_t guest);
uint32_t better_together_input_report(void *storage, better_together_input_report_t *out);
uint32_t better_together_input_stop(void *storage);

#endif
