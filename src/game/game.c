#include <SDL.h>            

#include "simple_logger.h"
#include "gf3d_vgraphics.h"
#include "gf3d_pipeline.h"
#include "gf3d_swapchain.h"
#include "gf3d_model.h"
#include "gf3d_camera.h"
#include "gf3d_texture.h"
#include "entity/manager.h"
#include "entity/definitions/agumon.h"

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
    gf3d_vgraphics_init(
        "gf3d",                 //program name
        1200,                   //screen width
        700,                    //screen height
        background_color,       //background color
        0,                      //fullscreen
        1                       //validation
    );

    entity_manager_init();
    
    // main game loop
    slog("gf3d main loop begin");

    entity_t *agumon1 = entity_manager_make(entity_agumon_init, NULL);
    entity_t *agumon2 = entity_manager_make(entity_agumon_init, NULL);

    //agumon2->position[0] -= 6.0f;

    agumon1->scale[0] = 0.5f;
    agumon1->scale[1] = 0.5f;
    agumon1->scale[2] = 0.5f;

    //entity_manager_release(agumon1);
    entity_manager_release(agumon2);

    while(!done)
    {
        agumon1->rotation[2] += 0.001f;
        //agumon2->rotation[2] -= 0.003f;

        agumon1->rotation[1] += 0.001f;
        //agumon2->rotation[1] -= 0.003f;

        SDL_PumpEvents();   // update SDL's internal event structures
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        //update game things here

        entity_manager_update();
        //gf3d_vgraphics_rotate_camera(0.001);
        
        // configure render command for graphics command pool
        // for each mesh, get a command and configure it from the pool
        bufferFrame = gf3d_vgraphics_render_begin();
        {
            commandBuffer = gf3d_command_rendering_begin(bufferFrame);
            {
                entity_manager_draw(bufferFrame, commandBuffer);
            }
            gf3d_command_rendering_end(commandBuffer);
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
