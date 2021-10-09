# Lynx Engine

A 3D software engine with volume rendering and advanced physics.
<br>The engine features:

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

<br>//TODO add cool images
<br>
<br>
<br>
<br>

# Getting started

<details>
<summary><b>Software requirements</b></summary>
The engine is meant to be used on Linux systems only.
<br>We do not provide any support for developement on Windows or MacOS.

| Software     | Version   |
|--------------|-----------|
| Linux        | 4.4       |
| g++          | 11.1.1    |
| glslang      | 10:11.0.0 |
| spirv-tools  | 2020.7    |

<br>
</details>
<details>
<summary><b>Install the required packages</b></summary>
Some softwares and libraries are required in order to build applications with the engine.
<br>You can install them with those commands:
<br>
<br>Fedora:
<br><code>sudo dnf install gcc-c++ glslang spirv-tools</code>
<br><code>sudo dnf install vulkan-devel glfw-devel libX11-devel libXcursor-devel libXrandr-devel libXinerama-devel libXi-devel freeglut-devel libasan libubsan</code>
<br>
<br>Arch:
<br><code>sudo pacman -S gcc glslang</code>
<br><code>sudo pacman -S vulkan-devel glfw-x11 libx11 libxcursor libxrandr libxinerama libxi freeglut gcc-libs</code>
<br>//TODO add software for wayland
<br>
<br>Additionally, Windows builds require the mingw executable
<br>//TODO add mingw commands
<br>
<br>
</details>
<details>
<summary><b>Download the LynxEngine SDK</b></summary>
Clone the repository from GitHub:<br>
<code>git clone https://github.com/Edo022/Lynx.git</code>
</details>
<br>
<br>
<br>
<br>

# Build applications

<details>
<summary><b>Application project structure</b></summary>

The minimal application project contains one source file and a generated .engine directory.
<br>The .engine directory is used to build the application and is generated when linking the project to an SDK.
<br>An application project can be linked to the SDK by running its Setup script:
<br>
<br><code>mkdir MyApp; cd MyApp</code>
<br><code>touch main.cpp</code> //TODO add an option to the setup script to make it generate a main.cpp
<br><code>&lt;path_to_sdk&gt;/Tools/Setup/Setup -vsc</code>
<br><code>code .</code>
<br>
<br>The -vsc option tells the setup script to generate configuration files for Visual Studio Code.
<br>Support for other editors will be added in future versions.
![a](./doc/ReadmeMinimalApplication.png)
<br>//TODO update image. use a rendered output for the hello world
<br>//TODO link to include paths details
<br>The source file must define a main function, like any C++ program.
<br>
<br>
</details>
<details>
<summary><b>Build system</b></summary>
Applications are built using the <code>&lt;path_to_sdk&gt;/Tools/Build/Build</code> executable, which is an <a href="https://github.com/d-Qw4w9WgXc-Q/Alloy">Alloy</a> wrapper that parses the provided arguments, adds everything the engine needs and forwards them to the build system.
<br>The source files and other flags can be specified in the <code>.engine/Build.Application</code> file. See //TODO link to syntax details
<br>
<br>//TODO show vscode build tasks
<br>The Build executable allows 2 build configurations:

- Debug  
  - The code is not optimized and contains any available debug, gdb and gprof information.
  - The engine enables Vulkan validation layers and Lynx runtime validation tests.  
    Any error reported by those systems must be fixed before shipping, as they indicate a bug in the application.  
  - Additional tools and key bindings are built into your application to help debugging it

- Release  
  - The code is optimized for speed by using <code>-Ofast</code> and other compiler specific options and contains no debug informations.
  - No validation layers, no runtime tests.
<br>
<br>Selectors can be used to enable different arguments based on the target platform and active configuration.
<br>See <code>&lt;path_to_sdk&gt;/Tools/Build/Build -h</code> for more informations.
<br>
<br>e.g. <code>&lt;path_to_sdk&gt;/Tools/Build/Build -m=ld -r: -O3 -g0 -d: -O0 -g3 -a: main.cpp</code>
//TODO RENAME LYNXG++ AS "Build"
<br>Examples are located in <code>&lt;path_to_sdk&gt;/Examples/</code> and contain the source code, the executable and the commands used to build it.
</details>
<br>
<br>
<br>
<br>

# Configure and modify an SDK

//TODO
The same system can use multiple versions of the SDK.

Each version can be used by multiple applications at the same time.  
Any modification to a version will affect all the applications linked to it.
<br>
<br>
<br>
<br>

# Support the developers

uwu
