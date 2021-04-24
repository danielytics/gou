
#include <gou/gou.hpp>

#include <cr.h>
#include <entt/core/type_info.hpp>
#include <spdlog/spdlog.h>

#include <imgui.h>

/********************************************************************************
 * Plugin entrypoint and setup
 ********************************************************************************/

gou::api::detail::type_context* gou::ctx::ref;
gou::api::Module* gou::ctx::gou_module;

CR_EXPORT int cr_main(cr_plugin* ctx, cr_op operation)
{
    if(!gou::ctx::ref) {
        auto info = static_cast<gou::api::detail::ModuleInfo*>(ctx->userdata);
        // Setup EnTT component type ID generator
        gou::ctx::ref = info->engine->type_context();
        // Set spdlog logger
        spdlog::set_default_logger(info->logger);
        // Set Dear ImGUI context
        ImGui::SetCurrentContext(info->imgui_context);
        // Create instance of module class
        gou::ctx::gou_module = gou::module_init(
            std::string{"["} + info->name + std::string{"] "},
            info->engine
        );
        // Call onLoad() and register module hooks with engine
        info->engine->registerModule(gou::ctx::gou_module->on_load(), gou::ctx::gou_module);
    }
    switch (operation) {
        // Hot-code reloading
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
        // Update step
        case CR_STEP:
        {
            break;
        }
        // Close and unload module
        case CR_CLOSE:
        {
            gou::ctx::gou_module->on_unload();
            break;
        }
    }
    return 0;
}
