#include <malloc.h>
#include "chunk.h"
#include "noise.h"
#include <game/structures/bprune.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <simple_logger.h>
#include <errno.h>

void file_read_data(FILE *f, char *data, size_t num_bytes)
{
    fseek(f, 0, SEEK_SET);
    while (num_bytes > 0) {
        size_t read = fread(data, sizeof(char), num_bytes, f);
        num_bytes -= read;
        data += read;
    }
}

void file_save_data(FILE *f, char *data, size_t num_bytes)
{
    fseek(f, 0, SEEK_SET);
    while (num_bytes > 0) {
        size_t written = fwrite(data, sizeof(char), num_bytes, f);
        num_bytes -= written;
        data += written;
    }
    fflush(f);
}

bool file_system_entry_exists(char *name)
{
    static struct stat chunk_folder_stats = {0};

    // Chunk folder doesn't exist.
    if (stat(name, &chunk_folder_stats) == -1) {
        return false;
    }

    return true;
}

void world_chunk_reindex(world_chunk_t *chunk)
{
    chunk->index.num_blocks = 0;
    for (size_t i = 0; i < SIZE_CHUNK_BLOCKS; i++) {
        if (chunk->blocks[i]) {
            chunk->index.num_blocks += 1;
        }
    }

    chunk->rendering.num_visible_blocks = bprune_update_chunk_index(chunk);
}

void world_chunk_reserialize(world_chunk_t *chunk)
{
    gpublock tmp;
    location l;

    for (size_t i = 0; i < chunk->rendering.num_visible_blocks; i++) {
        block_location *bl = &chunk->rendering.visible_blocks[i];
        // convert local location to world_entity space location
        location_from_chunk_block(&chunk->location, bl, &l);

        long block_idx = block_location_to_index(bl);
        char block_type = chunk->blocks[block_idx];

        // Set positions of tmp block
        tmp.position[0] = (float) l.x;
        tmp.position[1] = (float) l.y;
        tmp.position[2] = (float) l.z;
        tmp.position[3] = (float) block_type;

        // Send into serialization cache
        memcpy(&chunk->rendering.serialized_blocks[i], &tmp, sizeof(tmp));
    }
}

void world_chunk_persist(world_chunk_t *chunk)
{
    if (!chunk->persistence) {
        slog("Cannot persist a chunk. File was never opened.");
        return;
    }

    file_save_data(chunk->persistence, chunk->blocks, SIZE_CHUNK_BLOCKS);
}

void world_chunk_update(world_chunk_t *chunk)
{
    if (chunk->tainted) {
        world_chunk_reindex(chunk);
        world_chunk_reserialize(chunk);
        world_chunk_persist(chunk);
        chunk->tainted = false;
    }
}

world_chunk_t *world_chunk_allocate(const chunk_location cl)
{

    world_chunk_t *chunk = malloc(sizeof(world_chunk_t));
    chunk->location = cl;
    chunk->persistence = NULL;
    return chunk;
}

void world_chunk_generate(long seed, world_chunk_t *chunk)
{
    memset(chunk->blocks, 0, sizeof(char) * SIZE_CHUNK_BLOCKS);

    block_location bl;
    location l;
    for (bl.x = 0; bl.x < SIZE_CHUNK_X; bl.x++) {
        for (bl.y = 0; bl.y < SIZE_CHUNK_Y; bl.y++) {
            for (bl.z = 0; bl.z < SIZE_CHUNK_Z; bl.z++) {
                location_from_chunk_block(&chunk->location, &bl, &l);
                bool block_should_be_placed = noise_block_exists(seed, l.x, l.y, l.z);

                if (!block_should_be_placed && !(bl.x == 0 && bl.y == 0 && bl.z == 0)) {
                    continue;
                }

                const block_t *const block_type = noise_block_type(seed, l.x, l.y, l.z);

                long location = block_location_to_index(&bl);
                chunk->blocks[location] = block_type->id;
            }
        }
    }
    chunk->tainted = true;
}

void world_chunk_mmap_file(world_chunk_t *chunk, const chunk_location cl)
{

#define CHUNK_FOLDER_NAME "chunks/"
#define MAX_CHUNK_FILE_NAME_LENGTH 128

    static bool has_created_folder = false;

    // Attempt to make folder to hold them
    if (!has_created_folder) {
        // Chunk folder doesn't exist.
        if (!file_system_entry_exists(CHUNK_FOLDER_NAME)) {
            slog("Attempted to create chunks folder: %d", mkdir(CHUNK_FOLDER_NAME, 0700));
        }
        has_created_folder = true;
    }

    // Make file name
    static char chunk_file_name[MAX_CHUNK_FILE_NAME_LENGTH];
    sprintf(chunk_file_name, CHUNK_FOLDER_NAME "%li_%li.bin", cl.x, cl.z);

    chunk->file_previously_existed = file_system_entry_exists(chunk_file_name);

    if (!chunk->file_previously_existed) {
        chunk->persistence = fopen(chunk_file_name, "wb+");
    } else {
        chunk->persistence = fopen(chunk_file_name, "rb+");
    }
    if (chunk->persistence) {
        fseek(chunk->persistence, 0, SEEK_SET);
    } else {
        slog("Failed to open chunk %s, %d\n", chunk_file_name, errno);
    }
}

bool world_chunk_attempt_file_load(world_chunk_t *chunk, const chunk_location cl)
{
    if (chunk->persistence == NULL) {
        return false;
    }

    if (chunk->file_previously_existed) {
        file_read_data(chunk->persistence, chunk->blocks, SIZE_CHUNK_BLOCKS);
        chunk->tainted = true;
    }

    return chunk->file_previously_existed;
}

world_chunk_t *world_chunk_load(long seed, const chunk_location cl)
{
    world_chunk_t *chunk = world_chunk_allocate(cl);
    world_chunk_mmap_file(chunk, cl);
    if (!world_chunk_attempt_file_load(chunk, cl)) {
        world_chunk_generate(seed, chunk);
    }
    return chunk;
}

bool world_chunk_block_location_exists(const world_chunk_t *chunk, const block_location *bl)
{
    if (bl->x >= SIZE_CHUNK_X || bl->y >= SIZE_CHUNK_Y || bl->z >= SIZE_CHUNK_Z || bl->z < 0.0f) {
        return false;
    }
    long location = block_location_to_index(bl);
    return chunk->blocks[location] != 0;
}

bool world_chunk_location_exists(const world_chunk_t *chunk, location *l)
{
    block_location bl;
    location_to_block_location(l, &bl);
    return world_chunk_block_location_exists(chunk, &bl);
}


size_t world_chunk_gpu_send(const world_chunk_t *chunk, gpublock *blocks)
{
    memcpy(blocks, chunk->rendering.serialized_blocks, sizeof(gpublock) * chunk->rendering.num_visible_blocks);
    return chunk->rendering.num_visible_blocks;
}

long world_chunk_height(const world_chunk_t *chunk, location *l)
{
    block_location bl;
    location_to_block_location(l, &bl);

    for (bl.y = SIZE_CHUNK_Y; bl.y >= 0; bl.y--) {
        if (world_chunk_block_location_exists(chunk, &bl)) {
            return bl.y;
        }
    }

    return -1;
}