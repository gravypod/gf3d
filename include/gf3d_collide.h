#ifndef GF3D_GF3D_COLLIDE_H
#define GF3D_GF3D_COLLIDE_H

/**
 * Check to see if two entities AABBs are colliding/touching in any way.
 * @param a An entity to check
 * @param b An entity to check
 * @return True if they are touching
 */
bool gf3d_collide_is_entity_touching(const entity_t *a, const entity_t *b);

/**
 * Check to see if a point is within an entity's bounding box
 * @param point_radius - Size of the point (allow for error)
 * @param point - Point to check
 * @param b - Entity to check
 * @return
 */
bool gf3d_collide_is_point_touching(float point_radius, const vec3 point, const entity_t *b);

#endif
