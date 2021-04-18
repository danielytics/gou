
#include <cxxopts.hpp>
#include <toml.hpp>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include <filesystem>
#include <cctype>
#include <tsl/ordered_map.h>

using TomlValue = typename toml::basic_value<toml::discard_comments, tsl::ordered_map, std::vector>;
using TomlTable = typename toml::basic_value<toml::discard_comments, tsl::ordered_map, std::vector>::table_type;
using TomlArray = typename toml::basic_value<toml::discard_comments, tsl::ordered_map, std::vector>::array_type;

// TODO: Probably not needed once cxxopts PR #256 is merged
void expectOptions (const cxxopts::ParseResult& results, const std::vector<std::string>& options) {
    for (auto& option : options) {
        if (results.count(option) == 0) {
            throw cxxopts::option_has_no_value_exception(option);
        }
    }
}

enum class GenerationType {
    Component,
    Loader,
    Registration,
};

class OutWrapper {
public:
    OutWrapper (std::ofstream& ofs) : stream(ofs) {}
    OutWrapper (OutWrapper& other) :
        current_indent(other.current_indent),
        stream(other.stream) {
    }
    ~OutWrapper() {}

    void newline () { stream << "\n"; }
    void indent () { ++current_indent; }
    void dedent () { --current_indent; }
    std::ofstream& operator() (bool whitespace=true) {
        if (whitespace) {
            stream << "\n";
            for (auto i = 0; i < current_indent; ++i) {
                stream << "\t";
            }
        }
        return stream;
    }

private:
    int current_indent = 0;
    std::ofstream& stream;
};

enum class CaseStyle {
    Pascal,
    Snake,
};


std::map<std::string, std::string> data_types{
    {"vec2", "glm::vec2"},
    {"vec3", "glm::vec3"},
    {"vec4", "glm::vec4"},
    {"uint64", "std::uint64_t"},
    {"uint32", "std::uint32_t"},
    {"uint16", "std::uint16_t"},
    {"uint8", "std::uint8_t"},
    {"int64", "std::int64_t"},
    {"int32", "std::int32_t"},
    {"int16", "std::int16_t"},
    {"int8", "std::int8_t"},
    {"byte", "std::byte"},
    {"resource", "gou::resources::Handle"},
    {"entity", "entt::entity"},
    {"entity-set", "gou::resources::EntitySetHandle"},
    {"float", "float"},
    {"double", "double"},
    {"bool", "bool"},
    {"event", "gou::events::Event"},
    {"ref", "entt::hashed_string::hash_type"}
};

std::string load_vec2 (const std::string& attribute) {
    std::ostringstream sstr;
    sstr << "glm::vec2{";
    sstr <<   "float(toml::find<toml::floating>(" << attribute << ", \"x\")), ";
    sstr <<   "float(toml::find<toml::floating>(" << attribute << ", \"y\"))";
    sstr << "}";
    return sstr.str();
}
std::string load_vec3 (const std::string& attribute) {
    std::ostringstream sstr;
    sstr << "glm::vec3{";
    sstr <<   "float(toml::find<toml::floating>(" << attribute << ", \"x\")), ";
    sstr <<   "float(toml::find<toml::floating>(" << attribute << ", \"y\")), ";
    sstr <<   "float(toml::find<toml::floating>(" << attribute << ", \"z\"))";
    sstr << "}";
    return sstr.str();
}
std::string load_vec4 (const std::string& attribute) {
    std::ostringstream sstr;
    sstr << "glm::vec4{";
    sstr <<   "float(toml::find<toml::floating>(" << attribute << ", \"x\")), ";
    sstr <<   "float(toml::find<toml::floating>(" << attribute << ", \"y\")), ";
    sstr <<   "float(toml::find<toml::floating>(" << attribute << ", \"z\")), ";
    sstr <<   "float(toml::find<toml::floating>(" << attribute << ", \"w\"))";
    sstr << "}";
    return sstr.str();
}
std::string load_event (const std::string& attribute) {
    std::ostringstream sstr;
    sstr << "gou::events::Event{";
    sstr <<   "entt::hashed_string::value(toml::find<std::string>(" << attribute << ", \"type\").c_str()), ";
    sstr <<   "entity, ";
    sstr <<   "entt::null, ";
    sstr <<   "glm::vec3{";
    sstr <<     "float(toml::find<toml::floating>(" << attribute << ", \"x\")), ";
    sstr <<     "float(toml::find<toml::floating>(" << attribute << ", \"y\")), ";
    sstr <<     "float(toml::find<toml::floating>(" << attribute << ", \"z\"))";
    sstr <<    "}";
    sstr << "}";
    return sstr.str();
}
std::string load_hashed_string (const std::string& attribute) {
    std::ostringstream sstr;
    sstr << "entt::hashed_string::value(";
    sstr <<   "toml::find<std::string>(table, \"" << attribute << "\").c_str()";
    sstr << ")";
    return sstr.str();
}
std::string load_resource_handle (const std::string& attribute) {
    std::ostringstream sstr;
    sstr << "engine->findResource(";
    sstr <<   "entt::hashed_string::value(";
    sstr <<     "toml::find<std::string>(table, \"" << attribute << "\").c_str()";
    sstr <<   ")";
    sstr << ")";
    return sstr.str();
}

std::map<std::string, std::pair<bool, std::function<std::string(const std::string&)>>> data_type_loaders{
    // type     sub-table   loader-function
    {"vec2",    {true,      load_vec2}},
    {"vec3",    {true,      load_vec3}},
    {"vec4",    {true,      load_vec4}},
    {"event",   {true,      load_event}},
    {"ref",     {false,     load_hashed_string}},
    {"resource",{false,     load_resource_handle}},
    //{"entity", "entt::entity"},
    //{"entity-set", "frenzy::resources::EntitySetHandle"},
};

std::map<std::string, std::string> toml_data_types{
    {"uint64", "toml::integer"},
    {"uint32", "toml::integer"},
    {"uint16", "toml::integer"},
    {"uint8", "toml::integer"},
    {"int64", "toml::integer"},
    {"int32", "toml::integer"},
    {"int16", "toml::integer"},
    {"int8", "toml::integer"},
    {"byte", "toml::integer"},
    {"float", "toml::floating"},
    {"double", "toml::floating"},
    {"bool", "toml::boolean"},
};


std::string fromKebabCase (const std::string& kebab_case, CaseStyle style) {
    std::ostringstream sstr;
    bool first = true;
    bool flag = false; // Flag when next character needs to be uppercased
    for (auto ch : kebab_case) {
        if (first && style == CaseStyle::Pascal) {
            sstr << char(std::toupper(ch));
            first = false;
        } else {
            if (ch == '-') {
                if (style == CaseStyle::Snake) {
                    sstr << "_";
                } else {
                    flag = true;
                }
            } else if (flag) {
                sstr << char(std::toupper(ch));
                flag = false;
            } else {
                sstr << ch;
            }
        }
    }
    return sstr.str();
}


class HeaderGenerator {
public:
    class Component {
    public:
        Component(HeaderGenerator& header, const std::string& name, const std::string& description) : header(header) {
            if (! description.empty()) {
                header.out() << "// " << description;
            }

            header.out() << "struct " << name << " {";
            header.out.indent();
            component_name = name;
        }

        void addAttribute (const std::string& attribute, const std::string& data_type) {
            auto type = data_type;
            if (data_type.substr(0, 4) == "ptr:") {
                type = header.pointer_types.at(data_type) + "*";
            } else if (data_types.find(data_type) != data_types.end()) {
                type = data_types.at(data_type);
            } else {
                std::cerr << "Invalid data type for '" << component_name << "." << attribute << "': " << data_type << "\n";
            }

            header.out() << type << " " << attribute << ";";
        }

        ~Component() {
            header.out.dedent();
            header.out() << "};\n";
        }

    private:
        HeaderGenerator& header;
        std::string component_name;
    };

    HeaderGenerator (std::ofstream& file) : out(file) {
        out(false) << "#pragma once";
        out() << "#include <types.hpp>";
        out();
    }
    ~HeaderGenerator () {
        // Add closing brace, if needed
        if (current_namespace != "") {
            out.dedent();
            out() << "} // " << current_namespace << "\n";
        }
        out.dedent();
        out() << "} // components\n";
    }

    void addTypeForwardDeclaration (const std::string& type) {
        if (type.substr(0, 4) == "ptr:" && pointer_types.find(type) == pointer_types.end()) {
            pointer_types[type] = type.substr(4);
        }
    }

    void beginComponents () {
        for (const auto& [_, type] : pointer_types) {
            out() << "class " << type << ";";
        }
        out.newline();
        out() << "namespace components {";
        out.indent();
    }

    Component addComponent (const std::string& ns, const std::string& name, const std::string& description) {
        if (ns != current_namespace) {
            // Add closing brace to previous namespace, if needed
            if (current_namespace != "") {
                out.dedent();
                out() << "} // " << current_namespace << "\n";
            }
            // Start new namespace
            current_namespace = ns;
            out() << "namespace " << current_namespace << " {";
            out.indent();
        }
        return Component(*this, name, description);
    }

private:
    OutWrapper out;
    std::map<std::string, std::string> pointer_types;
    std::string current_namespace = "";
    std::string component_name;

    friend class Component;
};


class LoaderGenerator {
public:
    LoaderGenerator(OutWrapper& file) : out(file) {
    }
    ~LoaderGenerator() {
    }

    class Component {
    public:
        Component(LoaderGenerator& loader, const std::string& ns, const std::string& name, const std::string& struct_name) : loader(loader), ns(ns), name(name), struct_name(struct_name) {}
        ~Component() {
            loader.out() << "engine->registerLoader(\"" << name << "\"_hs, [](gou::api::Engine* engine, entt::registry& registry, const void* tableptr, entt::entity entity) {";
            loader.out.indent();
            bool empty = true;
            for (auto attribute : attributes) {
                if (attribute.first != "_name_" && attribute.first != "_description_" && attribute.first != "_namespace_") {
                    empty = false;
                    break;
                }
            }
            if (! empty) {
                loader.out() << "const auto& table = *reinterpret_cast<const toml::value*>(tableptr);";
            }
            
            for (auto attribute : attributes) {
                auto it = data_type_loaders.find(attribute.second);
                if (it != data_type_loaders.end()) {
                    const auto& [table, _] = it->second;
                    if (table) {
                        loader.out() << "auto " << attribute.first << " = table.at(\"" << attribute.first << "\");";
                    }
                }
            }
            loader.out() << "registry.emplace_or_replace<components::";
            if (! ns.empty()) {
                loader.out(false) << ns << "::";
            }
            loader.out(false) << struct_name << ">(entity";
            for (auto attribute : attributes) {
                auto it = data_type_loaders.find(attribute.second);
                if (it != data_type_loaders.end()) {
                    const auto& [_, generator_function] = it->second;
                    loader.out(false) << ", " << generator_function(attribute.first);
                } else {
                    auto it = data_types.find(attribute.second);
                    if (it != data_types.end()) {
                        auto tdt = toml_data_types.find(attribute.second);
                        if (tdt != toml_data_types.end()) {
                            loader.out(false) << ", " << it->second << "(";
                            loader.out(false) << "toml::find<" << tdt->second << ">(table, \"" << attribute.first << "\"))";
                        }
                    } else {
                        // Unknown type, pass through
                        loader.out(false) << ", " << attribute.second;
                        if (attribute.second != "nullptr") {
                            std::cerr << "Unknown data type: " << attribute.second << "\n";
                        }
                    }
                }
            }
            loader.out(false) << ");";
            loader.out.dedent();
            loader.out() << "});";
        }

        void addAttribute (const std::string& attribute, const std::string& data_type) {
            if (data_type.substr(0, 4) == "ptr:") {
                attributes.push_back({attribute, "nullptr"});
            } else {
                attributes.push_back({attribute, data_type});
            }
        }
    private:
        LoaderGenerator& loader;
        const std::string& ns;
        const std::string& name;
        const std::string& struct_name;
        std::vector<std::pair<std::string, std::string>> attributes;
    };

    Component add (const std::string& ns, const std::string& name, const std::string& struct_name) {
        return Component(*this, ns, name, struct_name);
    }

private:
    OutWrapper out;
    friend class Component;
};


class RegistrationGenerator {
public:
    RegistrationGenerator(OutWrapper& file) : out(file)  {
    }
    ~RegistrationGenerator() {
    }

    void add (const std::string& ns, const std::string& component) {
        out();
        out() << "registry.prepare<components::";
        if (! ns.empty()) {
            out(false) << ns << "::";
        }
        out(false) << component << ">();";
    }

private:
    OutWrapper& out;
};


void generate_components (const TomlValue& in, const std::string& module_name, std::ofstream& header_file, std::ofstream& source_file) {
    OutWrapper source(source_file);
    source(false) << "#include <components/" << module_name << ".hpp>";
    source() << "#include <gou_api.hpp>";
    source() << "#include <toml.hpp>";
    source();
    source() << "using namespace entt::literals;";
    source();
    source() << "namespace gou {";
    source.indent();
    source() << "void register_components (gou::api::Engine* engine)";
    source() << "{";
    source.indent();
    source() << "entt::registry& registry = engine->registry();";
    
    HeaderGenerator header(header_file);
    RegistrationGenerator registration(source);
    LoaderGenerator loader(source);

    auto default_namespace = toml::find<std::string>(in, "namespace");
    auto components = toml::find<TomlArray>(in, "component");

    // Find any custom types and forward declare them
    for (const auto& component_def : components) {
        for (const auto& [key, value] : component_def.as_table()) {
            std::string datatype = value.as_string();
            header.addTypeForwardDeclaration(datatype);
        }
    }

    header.beginComponents();

    for (const auto& component_def : components) {
        auto raw_name = toml::find<std::string>(component_def, "_name_");
        auto component_name = fromKebabCase(raw_name, CaseStyle::Pascal);
        auto component_namespace = toml::find_or<std::string>(component_def, "_namespace_", default_namespace);
        std::string component_description;

        if (component_def.contains("_description_")) {
            component_description = toml::find<std::string>(component_def, "_description_");
        }

        auto component = header.addComponent(component_namespace, component_name, component_description);
        registration.add(component_namespace, component_name);
        auto loader_component = loader.add(component_namespace, raw_name, component_name);

        // Add fields to component
        for (const auto& [key, value] : component_def.as_table()) {
            if (key.size() > 0 && key[0] != '_') {
                auto attribute = fromKebabCase(key, CaseStyle::Snake);
                std::string data_type = value.as_string();
                component.addAttribute(attribute, data_type);
                loader_component.addAttribute(attribute, data_type);
            }
        }
    }

    source.dedent();
    source() << "}";
    source.dedent();
    source() << "} // gou::";
}

/*
 * Component Code Generator
 * Generates code for components defined in components.toml files
 * 
 * Inputs:
 *  Generate header file from components.toml file
 *  Generate component loader code from components.toml file
 *  Generate ECS component registration code from components.toml file
 *  -t components
 *      engine/components.toml  -> sdk/components/core.h
 *      modules/X/components.toml -> sdk/components/X.h
 *      modules/X/components.toml -> modules/build/loaders/X.cpp
 *      modules/X/components.toml -> modules/build/registry/X.cpp
 */
int main (int argc, char* argv []) {    
    try {
        cxxopts::Options options("componentgen", "Component Code Generator");
        options.add_options()
            ("help", "Help")
            ("i,in", "Input", cxxopts::value<std::string>())
            ("h,header", "Header Output", cxxopts::value<std::string>())
            ("s,source", "Source Output", cxxopts::value<std::string>());
        
        options.parse_positional({"files"});
        auto result = options.parse(argc, argv);

        if (result.count("help")) {
            std::cout << options.help() << "\n";
            return 0;
        }

        expectOptions(result, {"in", "header", "source"});
        std::ofstream header_file(result["header"].as<std::string>());
        std::ofstream source_file(result["source"].as<std::string>());

        auto input_filename = result["in"].as<std::string>();
        const auto config = toml::parse<toml::discard_comments, tsl::ordered_map, std::vector>(input_filename); // Use tsl::ordered_map so order of component attributes is retained
        auto module_name = std::filesystem::path(input_filename).parent_path().filename().string();
        if (module_name == "engine") {
            module_name = "core";
        }

        generate_components(config, module_name, header_file, source_file);

    } catch (const cxxopts::option_has_no_value_exception& e) {
        std::cerr << "Mandatory option not supplied: " << e.what() << "\n";
    } catch (const cxxopts::OptionParseException& e) {
        std::cerr << "Error parsing commandline options:\n" << e.what() << "\n";
    } catch (const std::out_of_range& e) {
        std::cerr << "Invalid generator type: " << e.what() << "\n";
    }
    return 0;
}
