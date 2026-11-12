#ifndef BETTER_TOGETHER_OWNER_H
#define BETTER_TOGETHER_OWNER_H

#include <stdint.h>

uint32_t better_together_owner_construct(uint64_t owner, uint64_t cont, uint64_t *out);
uint32_t better_together_owner_test(void);

#endif
