
#include <gou.hpp>

#include <cr.h>
#include <entt/core/type_info.hpp>

/********************************************************************************
 * Plugin entrypoint and setup
 ********************************************************************************/

gou::api::detail::type_context* gou::ctx::ref;
gou::api::Module* gou::ctx::gou_module;


CR_EXPORT int cr_main(cr_plugin* ctx, cr_op operation)
{
    if(!gou::ctx::ref) {
        auto engine = static_cast<gou::api::Engine*>(ctx->userdata);
        gou::ctx::ref = engine->type_context();
        gou::ctx::gou_module = gou::module_init(engine);
        engine->registerModule(gou::ctx::gou_module->on_load(), gou::ctx::gou_module);
    }
    switch (operation) {
        case CR_LOAD:
        {
            gou::ctx::gou_module->on_after_reload();
            break;
        }
        case CR_UNLOAD:
        {
            gou::ctx::gou_module->on_before_reload();
            break;
        }
        case CR_STEP:
        {
            break;
        }
        case CR_CLOSE:
        {
            gou::ctx::gou_module->on_unload();
            break;
        }
    }
    return 0;
}
