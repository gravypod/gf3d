#include <game/rendering/ui.h>
#include <gf3d_vgraphics.h>
#include "ui.h"


rendering_pipeline_ui_t *pipeline_ui;

void entity_ui_draw(entity_t *entity, const entity_render_pass_t *pass)
{
    rendering_pipeline_ui_renderpass(pipeline_ui, pass->commandBuffer, pass->bufferFrame);
}

void entity_ui_init(entity_t *entity, void *metadata)
{
    pipeline_ui = rendering_pipeline_ui_init(
            (uint32_t) windowWidth, (uint32_t) windowHeight
    );
    entity->draw = entity_ui_draw;
}