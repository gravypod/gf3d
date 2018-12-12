#ifndef GF3D_WORLD_H
#define GF3D_WORLD_H

#include <game/entity/entity.h>
#include <game/chunk.h>
#include <game/coords.h>

#define MAX_CHUNKS_PLAYER_DISTANCE 3
#define MAX_NUM_LOADED_CHUNKS (MAX_CHUNKS_PLAYER_DISTANCE * MAX_CHUNKS_PLAYER_DISTANCE)

extern entity_t *world_entity;

world_chunk_t *world_chunk_get(chunk_location *cl);

bool world_collision_find(const vec3 center, float x_radius, float y_radius, float z_radius, location *blocking_block);

void entity_world_init(entity_t *entity, void *metadata);

/**
 * Height of the world at a specific coordinate.
 * @param l
 * @return
 */
long world_height(location *l);

#endif
