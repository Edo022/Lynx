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

# Software requirements

## Shipping

Applications can be built for both Linux and Windows.  
Mac is not supported yet, but it may be in future versions.

&nbsp;

&nbsp;

## Developement

The engine is meant to be used on Linux systems only.  
We do not provide any support for developement on Windows or Mac.  


| Software     | Version   | Required            |
|--------------|-----------|---------------------|
| Linux        | 4.4       | Yes                 |
| Python       | 3.9       | Yes //TODO remove python requirement. Compile an executable file for every script                 |
| g++          | 11.1.1    | Yes                 |
| Vulkan SDK   | 1.2.170.0 | Yes //TODO remove   |
| glslang      | 10:11.0.0 | Yes                 | 
| spirv-tools  | 2020.7    | Yes                 | 
| Mingw        | 8.0.0     | Windows builds only |
| **Lynx SDK** | 1.0.0     | Yes                 |

The location of those softwares can be changed in the `.engine/Build_config.hpp` file. //TODO

&nbsp;

The engine can automatically generate configuration files for Visual Studio Code.  
Support for other C++ compilers and editors will be added in future versions.

&nbsp;

&nbsp;

## Download the required softwares and libraries

Some developement libraries are required in order to build applications.

Fedora  
`dnf install gcc-c++ glslang spirv-tools`
`dnf install vulkan-devel glfw-devel libX11-devel libXcursor-devel libXrandr-devel libXinerama-devel libXi-devel freeglut-devel libasan libubsan`
//TODO

&nbsp;

&nbsp;

## Download the SDK

Clone the repository from GitHub:  
`git clone https://github.com/Edo022/LynxEngine.git`  
The same system can use multiple versions of the SDK.

&nbsp;

A project can be linked to an SDK by running its Setup script:  
`mkdir MyProject`  
`cd MyProject`  
`<path_to_sdk>/Tools/Setup/Setup`  

Each version can be used by multiple projects at the same time.  
Any modification to a version will affect all the projects linked to it.

&nbsp;

&nbsp;

&nbsp;

# Build applications

Applications are built using the `<path_to_sdk>/Tools/Build/lynxg++` executable, 
which is a g++ wrapper that parses the provided arguments, adds everything the engine needs and forwards them to g++.

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

Examples are located in `<path_to_sdk>/Examples/` and contain the source code, the executable and the commands used to build it.  

&nbsp;

&nbsp;

&nbsp;

# Support the developers

uwu