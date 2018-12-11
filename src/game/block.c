#include "block.h"
#include <stddef.h>

block_t grass = {
        .id = 1
};

block_t stone = {
        .id = 2
};


block_t *block_grass = &grass;
block_t *block_stone = &stone;