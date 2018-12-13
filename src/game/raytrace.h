#ifndef GF3D_RAYTRACE_H
#define GF3D_RAYTRACE_H

#include <stdbool.h>
#include <linmath.h>
#include <game/entity/entity.h>

/**
 * Iteratively step through points via modification of point and max_distance
 * @param step_size - How much we should increment with each step
 * @param max_distance - Pointer to a number noting the remaining distance in this raytrace.
 * @param point - Starting point
 * @param direction - Direction we want to travel
 * @return true if we generated a new point. false if there are no more points.
 */
bool raytrace_point_step(float step_size, float *max_distance, vec3 point, vec3 direction);

/**
 * Find an entity in a path. Only checks against touchable entities
 * @param step_size - How much we should increment with each step
 * @param max_distance - Pointer to a number noting the remaining distance in this raytrace.
 * @param point - Starting point
 * @param direction - Direction we want to travel
 * @return
 */
entity_t *raytrace_contacts_entity(entity_t *source_entity, float step_size, float *max_distance, vec3 point, vec3 direction);

#endif
