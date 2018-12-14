#ifndef GF3D_WORLD_H
#define GF3D_WORLD_H

#include <game/entity/entity.h>
#include <game/chunk.h>
#include <game/coords.h>

#define MAX_CHUNKS_PLAYER_DISTANCE 3
#define MAX_NUM_LOADED_CHUNKS 128

extern entity_t *world_entity;

world_chunk_t *world_chunk_get(const chunk_location *cl);

void entity_world_init(entity_t *entity, void *metadata);

/**
 * Height of the world at a specific coordinate.
 * @param l
 * @return
 */
long world_height(location *l);

#endif
