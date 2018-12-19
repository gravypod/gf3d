#include <game/rendering/ui.h>
#include <gf3d_vgraphics.h>
#include "ui.h"
#include "player.h"
#include <SDL_ttf.h>

rendering_pipeline_ui_t *pipeline_ui;

SDL_Surface *surface;
SDL_Renderer *renderer;
int lastPosition = 0;
Texture *interface_texture ;

TTF_Font *font;


SDL_Surface *entity_ui_sdl_surface_init(uint32_t width, uint32_t height)
{
    Uint32 rmask, gmask, bmask, amask;

    /* SDL interprets each pixel as a 32-bit number, so our masks must depend
       on the endianness (byte order) of the machine */
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif

    return SDL_CreateRGBSurface(
            0,
            width, height,
            32,
            rmask, gmask, bmask, amask
    );
}

void entity_ui_sdl_render_text(char *message, int x, int y, int scale)
{
    SDL_Color color = { 255, 255, 255 };
    SDL_Surface * surface = TTF_RenderText_Solid(
            font,
            message,
            color
    );
    SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);
    {
        int texW = 0;
        int texH = 0;
        SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
        SDL_Rect dstrect = { x, y, texW*scale, texH*scale };

        SDL_RenderCopy(renderer, texture, NULL, &dstrect);
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void entity_ui_sdl_render_sceen_clear()
{
    // Set everything to clear
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
}

void entity_ui_sdl_create()
{
    surface = entity_ui_sdl_surface_init(
            gf3d_vgraphics_get_view_extent().width,
            gf3d_vgraphics_get_view_extent().height
    );


    // Setup renderer
    renderer = SDL_CreateSoftwareRenderer(surface);

    interface_texture = gf3d_texture_surface_create(surface);
}

void entity_ui_sdl_draw()
{
    static char message_buffer[512];

    entity_ui_sdl_render_sceen_clear();

    // Creat a rect at pos ( 50, 50 ) that's 50 pixels wide and 50 pixels high.
    SDL_Rect r;
    r.x = lastPosition;
    r.y = 50;
    r.w = 50;
    r.h = 50;

    // Set render color to blue ( rect will be rendered in this color )
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

    // Render rect
    SDL_RenderFillRect(renderer, &r);

    // Render the rect to the screen
    SDL_RenderPresent(renderer);


    sprintf(message_buffer, "Num Souls Captured: %d", collected_souls);
    entity_ui_sdl_render_text(message_buffer, 20, 20, 1);

    sprintf(message_buffer, "HP Remaining: %li", player_entity == NULL ? 0 : player_entity->health);
    entity_ui_sdl_render_text(message_buffer, 20, 46, 1);

}

void entity_ui_update(entity_t *entity, void *metadata)
{
    static int lastUpdate = 10;
    if (lastPosition > surface->w) {
        lastPosition = 0;
    }

    lastPosition++;

    if (lastUpdate >= 10) {
        entity_ui_sdl_draw();
        gf3d_texture_surface_update(interface_texture, surface);
        lastUpdate = 0;
    }
    lastUpdate++;
}

void entity_ui_draw(entity_t *entity, const entity_render_pass_t *pass)
{
    rendering_pipeline_ui_renderpass(pipeline_ui, pass->commandBuffer, pass->bufferFrame);
}

void entity_ui_free(entity_t *entity, void *metadata)
{
    TTF_CloseFont(font);
    TTF_Quit();
}

void entity_ui_init(entity_t *entity, void *metadata)
{
    TTF_Init();
    font = TTF_OpenFont("./fonts/ComicNeue-Regular.ttf", 32);

    entity_ui_sdl_create();
    entity_ui_sdl_draw();


    pipeline_ui = rendering_pipeline_ui_init(
            interface_texture, (uint32_t) windowWidth, (uint32_t) windowHeight
    );
    entity->draw = entity_ui_draw;
    entity->update = entity_ui_update;
    entity->free = entity_ui_free;
}