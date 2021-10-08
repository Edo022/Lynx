# Lynx Engine

A 3D software engine with volume rendering and advanced physics.  
The engine features:

- Ray tracing volume rendering
  - No textures or UVs required
  - Better performance in physics simulations
  - Runtime destruction physics
  - High quality renders

- Node-based runtime procedural object generation
  - Minimal disk usage
  - Short loading time
  - Multiple outputs from a single object

- Global thread pool
- Per-thread memory pool
- Global VRAM pool

//TODO add cool images

&nbsp;

&nbsp;

&nbsp;

# Getting started

## Software requirements

The engine is meant to be used on Linux systems only.  
We do not provide any support for developement on Windows or Mac.  


| Software     | Version   |
|--------------|-----------|
| Linux        | 4.4       |
| g++          | 11.1.1    |
| glslang      | 10:11.0.0 |
| spirv-tools  | 2020.7    |

&nbsp;

## Install the required softwares and libraries

Fedora  
`sudo dnf install gcc-c++ glslang spirv-tools`  
`sudo dnf install vulkan-devel glfw-devel libX11-devel libXcursor-devel libXrandr-devel libXinerama-devel libXi-devel freeglut-devel libasan libubsan`
//TODO

Arch  
`sudo pacman -S gcc glslang`  
`sudo pacman -S vulkan-devel glfw-x11 libx11 libxcursor libxrandr libxinerama libxi freeglut gcc-libs`

//TODO add software for wayland

&nbsp;

## Download the LynxEngine SDK

Clone the repository from GitHub:  
`git clone https://github.com/Edo022/LynxEngine.git`  

&nbsp;

&nbsp;

&nbsp;

&nbsp;

# Build applications

## Project structure

Every project is composed of a directory which contains at least one source file and a generated .engine directory.  
A project can be linked to the SDK by running its Setup script:  
`mkdir MyProject`  
`cd MyProject`  
`touch main.cpp`  
`<path_to_sdk>/Tools/Setup/Setup`  

The Setup scripy can automatically generate configuration files for Visual Studio Code.  
Support for other editors will be added in future versions.

//FIXME

## Build system

Applications are built using the `<path_to_sdk>/Tools/Build/lynxg++` executable,
which is an Alloy wrapper that parses the provided arguments, adds everything the engine needs and forwards them to the build system.
//TODO write how to use the build file
lynxg++ allows 2 build configurations:

- Debug  
  - The code is not optimized and contains any available debug, gdb and gprof information.
  - The engine enables Vulkan validation layers and Lynx runtime validation tests.  
    Any error reported by those systems must be fixed before shipping, as they indicate a bug in the application.  
  - Additional tools and key bindings are built into your application to help debugging it

- Release  
  - The code is optimized for speed by using `-Ofast` and other compiler specific options and contains no debug informations.
  - No validation layers, no runtime tests.

&nbsp;

Selectors can be used to enable different arguments based on the target platform and active configuration.  
See `<path_to_sdk>/Tools/Build/lynxg++ -h` for more informations.

&nbsp;

e.g. `<path_to_sdk>/Tools/Build/lynxg++ -m=ld -r: -O3 -g0 -d: -O0 -g3 -a: main.cpp`  
//TODO RENAME LYNXG++ AS "Build"
Examples are located in `<path_to_sdk>/Examples/` and contain the source code, the executable and the commands used to build it.  

&nbsp;

&nbsp;

&nbsp;

# Configure and modify an SDK
 
//TODO

The same system can use multiple versions of the SDK.

Each version can be used by multiple projects at the same time.  
Any modification to a version will affect all the projects linked to it.


&nbsp;

&nbsp;

&nbsp;

# Support the developers

uwu
