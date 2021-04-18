# GOU

The name **GOU** is in honour of the [Game of Ur](https://en.wikipedia.org/wiki/Royal_Game_of_Ur) (also known as *The Royal Game of Ur*), the oldest known boardgame, first played between 2600 to 2400 BC. Pronounced `gah-ow` like in `gouge`.

**GOU** is a game engine, with the following key features:

**This is an early work-in-progress, most of the below features do not yet exist!**

1. Component Entity System based game entities
2. Multi-threaded systems
3. Scene-tree based game logic
4. Lua scripts can be attached to the scene-tree
5. Modular engine, all game-specific logic is in modules, scripts or configuration
6. Modules are hot-code reloadable
7. Configuration, scenes and game data described in human-readable TOML files
8. Focused on 2.5D or 3D Action RPG games
9. Deferred-texturing based renderer
    * https://mynameismjp.wordpress.com/2016/03/25/bindless-texturing-for-deferred-rendering-and-decals/
    * https://www.reedbeta.com/blog/deferred-texturing/

# Dependencies

Engine dependencies:

* C++17 and OpenGL 4.1
* [SDL2](http://libsdl.org/) - Windowing & Input (ZLIB License)
* [EnTT](https://github.com/skypjack/entt) - Entity Component System (MIT License)
* [GLAD](https://github.com/Dav1dde/glad) - OpenGL extension generator (MIT License)
* [GLM](https://glm.g-truc.net/0.9.8/index.html) - OpenGL Mathematics library (The Happy Bunny/Modified MIT License)
* [spdlog](https://github.com/gabime/spdlog) - Fast logging library (MIT License)
* [LuaJIT](http://luajit.org/luajit.html) - Lua bindings and JIT runtime (MIT License)
* [FastNoiseSIMD](https://github.com/Auburns/FastNoiseSIMD) - Library of various SIMD-optimised noise functions (MIT License, note: Simplex noise is patent-encumbered when used for texture generation)
* [PhysicsFS](http://icculus.org/physfs/) - Library for filesystem abstraction (zlib License)
* [PhysFS++](https://github.com/danielytics/physfs-hpp) - C++ wrapper for PhysicsFS (zlib License + [notes](https://github.com/Ybalrid/physfs-hpp/blob/master/LICENSE.txt))
* [toml11](https://github.com/ToruNiina/toml11) - TOML data file loader (MIT License)
* [cr.h](https://github.com/fungos/cr) - Module live reload library (MIT License)
* [stb_image](https://github.com/nothings/stb) - Image loader (Public Domain)
* [Bullet Physics](https://github.com/bulletphysics/bullet3) - Physics engine (zlib License)
* [cxxopts](https://github.com/jarro2783/cxxopts) - Lightweight C++ command line option parser (MIT License)
* [taskflow](https://github.com/taskflow/taskflow) - Parallel task programming framework (MIT-like [Taskflow Lisence](https://github.com/taskflow/taskflow/blob/master/LICENSE))
* [Dear ImGui](https://github.com/ocornut/imgui) - Bloat-free graphical user interface library for C++ (MIT License)
* [vecmath](https://github.com/GaijinEntertainment/vecmath) - SIMD vector math HAL (BSD 3-Clause License)
* [ordered-map](https://github.com/Tessil/ordered-map) - Order-preserving hash map/set (MIT License)
* [Sparsepp](https://github.com/greg7mdp/sparsepp) - A fast, memory efficient hash map for C++ ([Custom do-almost-anything License](https://github.com/greg7mdp/sparsepp/blob/master/LICENSE))
* [Catch2](https://github.com/catchorg/Catch2) - C++ testing library (Boost Software License)

Building from surce:

* [tup](http://gittup.org/tup/) - Tup is a file-based build system for Linux, OSX, and Windows ([Dual License](http://gittup.org/tup/license.html) GPL-v2 or Commercial)

# Building

Make sure you have Git, Clang, SDL2 and Tup installed.

```
git clone git@github.com:danielytics/gou.git
cd gou
git submodule update --init
./generate_components.sh
tup
```

Now you can run the engine using one of:

* `./debug.sh` to run the debug build (no optimisations, debug and development features)
* `./dev.sh` to run the development build (optimisations, development features)
* `./release.sh` to run the release build (optimisations, development features & debugger symbols stripped)

The development version is intended for developing games with GOU and includes access to editor tools.

The debug version is intended for developing the engine itself, or when debugging a game. May run slowly.

When developing GOU modules (or the engine itself), always building all three builds at once may be too slow. You can build each version in isolation with:

* `tup build-debug` to build the debug build
* `tup build-default` to build the dev build
* `tup build-release` to build the release build

Components are specified in `components.toml` files (in `engine/components.toml` for core engine components, or in `modules/*/components.toml` for module-specific components). If these files are modified, it is necessary to rerun `./generate_components.sh` to regenerate the component header and source files. The initial run also compiles the generator, but subsequent runs should be very quick.

# License

GOU is released under the terms of the [MIT License](https://github.com/danielytics/gou/blob/master/LICENSE).
