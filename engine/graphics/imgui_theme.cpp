
#include "gou_engine.hpp"
#include <imgui.h>
#include "utils/parser.hpp"
#include "utils/colors.hpp"

namespace imgui {
    void initTheme ();
}

spp::sparse_hash_map<std::string, ImGuiCol_> theme_names {
    {"window.base",         ImGuiCol_WindowBg},
    {"header.base",         ImGuiCol_Header},
    {"header.hovered",      ImGuiCol_HeaderHovered},
    {"header.active",       ImGuiCol_HeaderActive},
    {"button.base",         ImGuiCol_Button},
    {"button.hovered",      ImGuiCol_ButtonHovered},
    {"button.active",       ImGuiCol_ButtonActive},
    {"frame.base",          ImGuiCol_FrameBg},
    {"frame.hovered",       ImGuiCol_FrameBgHovered},
    {"frame.active",        ImGuiCol_FrameBgActive},
    {"tab.base",            ImGuiCol_Tab},
    {"tab.hovered",         ImGuiCol_TabHovered},
    {"tab.active",          ImGuiCol_TabActive},
    {"tab.unfocused",       ImGuiCol_TabUnfocused},
    {"tab.unfocused-active",ImGuiCol_TabUnfocusedActive},
    {"title.base",          ImGuiCol_TitleBg},
    {"title.active",        ImGuiCol_TitleBgActive},
    {"title.collapsed",     ImGuiCol_TitleBgCollapsed},
};

void imgui::initTheme ()
{
    const std::string& themeFilename = entt::monostate<"ui/theme-file"_hs>();
    if (themeFilename != "") {
        ImGuiStyle& style = ImGui::GetStyle();
        auto& style_colors = style.Colors;

        const auto theme = parser::parse_toml(themeFilename);

        std::string base_theme = "classic";
        if (theme.contains("imgui")) {
            const auto& imgui = theme.at("imgui");
            base_theme = toml::find_or(imgui, "default", "classic");
        }
        if (base_theme == "light") {
            ImGui::StyleColorsLight();
        } else if (base_theme == "dark") {
            ImGui::StyleColorsDark();
        } else {
            ImGui::StyleColorsClassic();
        }

        if (theme.contains("colors")) {
            const auto& colors = theme.at("colors");
            for (const auto& what : {"window", "header", "button", "frame", "tab", "title"}) {
                if (colors.contains(what)) {
                    for (const auto& [key, value] : colors.at(what).as_table()) {
                        auto key_name = what + std::string{"."} + key;
                        auto it = theme_names.find(key_name);
                        if (it != theme_names.end()) {
                            float opacity = 1.0f;
                            glm::vec3 rgb;
                            if (value.is_table()) {
                                if (value.contains("rgb")) {
                                    rgb = colors::parse_rgb(toml::get<std::string>(value.at("rgb")));
                                } else {
                                    spdlog::warn("Theme setting \"{}\" is missing \"rgb\" value", key_name);
                                    continue;
                                }
                                if (value.contains("opacity")) {
                                    opacity = float(value.at("opacity").as_floating());
                                }
                            } else if (value.is_string()) {
                                rgb = colors::parse_rgb(value.as_string().str);
                            } else {
                                spdlog::warn("Theme setting \"{}\" has invalid data type, expected hex string or [hex string, alpha]", key_name);
                                continue;
                            }
                            style_colors[it->second] = ImVec4{ rgb.r, rgb.g, rgb.b, opacity};
                        }
                    }
                }
            }
        }
    }
}
