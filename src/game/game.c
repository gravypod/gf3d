#include <SDL.h>
#include <zconf.h>

#include "simple_logger.h"
#include "gf3d_vgraphics.h"
#include "gf3d_pipeline.h"
#include "gf3d_swapchain.h"
#include "gf3d_model.h"
#include "gf3d_camera.h"
#include "gf3d_texture.h"
#include "entity/manager.h"
#include "entity/definitions/agumon.h"
#include "entity/definitions/player.h"
#include <game/entity/definitions/world.h>

int main(int argc,char *argv[])
{
    int done = 0;
    const Uint8 * keys;
    Uint32 bufferFrame = 0;
    VkCommandBuffer commandBuffer;
    Model *model;
    Model *model2;

    vec4 background_color = {
            0.51, 0.75, 1, 1
    };
    
    init_logger("gf3d.log");    
    slog("gf3d begin");
    int width = 1200, height = 700;
    gf3d_vgraphics_init(
        "gf3d",                 //program name
        width,                  //screen width
        height,                 //screen height
        background_color,       //background color
        0,                      //fullscreen
        1                       //validation
    );

    entity_manager_init();
    
    // main game loop
    slog("gf3d main loop begin");
    entity_t *world = entity_manager_make(entity_world_init, NULL);
    entity_t *player = entity_manager_make(entity_player_init, NULL);
    entity_t *agumon1 = entity_manager_make(entity_agumon_init, NULL);
    //entity_t *agumon2 = entity_manager_make(entity_agumon_init, NULL);

    int lastX = 0, lastY = 0, lastZ = 0;
    agumon1->position[0] = 50.0f;

    agumon1->scale[0] = 0.5f;
    agumon1->scale[1] = 0.5f;
    agumon1->scale[2] = 0.5f;

    //entity_manager_release(agumon1);
    //entity_manager_release(agumon2);

    unsigned char c = 0;
    while(!done)
    {/*
        agumon1->rotation[2] += 0.001f;
        //agumon2->rotation[2] -= 0.003f;

        agumon1->rotation[1] += 0.001f;
        //agumon2->rotation[1] -= 0.003f;*/

        c++;
        SDL_PumpEvents();   // update SDL's internal event structures
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        //update game things here

        entity_manager_update();

        if (lastX != (int) player->position[0]  || lastY != (int) player->position[1] || lastZ != (int) player->position[2]) {
            lastX = (int) player->position[0]; lastY = (int) player->position[1]; lastZ = (int) player->position[2];
            printf("Player entered (%d, %d, %d)\n", lastX, lastY, lastZ);
        }

        if (world_collision_find(player->position, 0.5f, 0.5f, 0.5f, NULL)) {
            printf("Player colliding at (%d, %d, %d)\n", lastX, lastY, lastZ);
        }

        // configure render command for graphics command pool
        // for each mesh, get a command and configure it from the pool
        bufferFrame = gf3d_vgraphics_render_begin();
        {
            commandBuffer = gf3d_command_rendering_begin(bufferFrame);
            {

                gf3d_command_configure_render_pass(
                        commandBuffer,
                        gf3d_vgraphics_get_graphics_pipeline()->renderPass,
                        gf3d_swapchain_get_frame_buffer_by_index(bufferFrame),
                        gf3d_vgraphics_get_graphics_pipeline()->pipeline,
                        gf3d_vgraphics_get_graphics_pipeline()->pipelineLayout);
                entity_manager_draw(bufferFrame, commandBuffer);
            }
            gf3d_command_rendering_end(commandBuffer);
/*
            commandBuffer = gf3d_command_rendering_begin(bufferFrame);
            {

            }
            gf3d_command_rendering_end(commandBuffer);*/
        }
        gf3d_vgraphics_render_end(bufferFrame);

        if (keys[SDL_SCANCODE_ESCAPE])done = 1; // exit condition
    }    
    
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());    
    //cleanup
    slog("gf3d program end");
    slog_sync();
    return 0;
}

/*eol@eof*/
