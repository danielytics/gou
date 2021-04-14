# GOU

The name **GOU** is in honour of the [Game of Ur](https://en.wikipedia.org/wiki/Royal_Game_of_Ur) (also known as The Royal Game of Ur), the oldest known boardgame, first played between 2600 to 2400 BC.

**GOU** is a game engine, with the following key features:

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
* [GLEW](http://glew.sourceforge.net/) - OpenGL extension library (Modified BSD & MIT Licenses) TODO: Replace with GLAD
* [GLM](https://glm.g-truc.net/0.9.8/index.html) - OpenGL Mathematics library (The Happy Bunny/Modified MIT License)
* [spdlog](https://github.com/gabime/spdlog) - Fast logging library (MIT License)
* [LuaJIT](http://luajit.org/luajit.html) - Lua bindings and JIT runtime (MIT License)
* [FastNoiseSIMD](https://github.com/Auburns/FastNoiseSIMD) - Library of various SIMD-optimised noise functions (MIT License, note: Simplex noise is patent-encumbered when used for texture generation)
* [PhysicsFS](http://icculus.org/physfs/) - Library for filesystem abstraction (zlib License)
* [PhysFS++](https://github.com/Ybalrid/physfs-hpp) - C++ wrapper for PhysicsFS (zlib License + [notes](https://github.com/Ybalrid/physfs-hpp/blob/master/LICENSE.txt))
* [toml11](https://github.com/ToruNiina/toml11) - TOML data file loader (MIT License)
* [cr.h](https://github.com/fungos/cr) - Module live reload library (MIT License)
* [stb_image](https://github.com/nothings/stb) - Image loader (Public domain)
* [Bullet Physics](https://github.com/bulletphysics/bullet3) - Physics engine (zlib license)
* [cxxopts](https://github.com/jarro2783/cxxopts) - Lightweight C++ command line option parser (MIT License)
* [taskflow](https://github.com/taskflow/taskflow) - Parallel task programming framework (MIT-like [Taskflow Licence](https://github.com/taskflow/taskflow/blob/master/LICENSE))
* [Dear ImGui](https://github.com/ocornut/imgui) - Bloat-free graphical user interface library for C++ (MIT License)
* [vecmath](https://github.com/GaijinEntertainment/vecmath) - SIMD vector math HAL (BSD 3-Clause License)
* [ordered-map](https://github.com/Tessil/ordered-map) - Order-preserving hash map/set (MIT License)
* [Catch2](https://github.com/catchorg/Catch2) - C++ testing library (Boost Software License)

Building from surce:

* [tup](http://gittup.org/tup/) - Tup is a file-based build system for Linux, OSX, and Windows ([Dual License](http://gittup.org/tup/license.html) GPL-v2 or Commercial)

# Building

Make sure you have Git, Clang, SDL2 and Tup installed.

```
git clone git@github.com:danielytics/gou.git
cd gou
git submodule update --init
tup build-default
```

Now you can run the engine using `./release.sh`

To build a debug build, use `tup build-debug` and run with `./debug.sh`


# License

GOU is released under the terms of the MIT License.
