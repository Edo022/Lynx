# Lynx Engine

A 3D software engine with volume rendering and advanced physics.  
Features:  
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

&nbsp;

&nbsp;

&nbsp;

# Software requirements

## Shipping

Applications can be built for both Linux and Windows.  
Mac is not supported yet, but it may be in future versions.

## Developement

The engine is meant to be used on Linux systems only.  
We do not provide any support for developement on Windows or Mac.  


| Software     | Version   | Path                                     | Required |
|--------------|-----------|------------------------------------------|----------|
| Linux        | 4.4       | -                                        | Yes      |
| Python       | 3.8       | `/bin/python3.8`<br />`/bin64/python3.8` | Yes      |
| g++          | 10.7      | `/bin/g++`<br />`/bin64/g++`             | Yes      |
| Vulkan SDK   | 1.2.170.0 | any                                      | Yes      |
| Mingw        | 8.0.0     | `/bin/mingw`<br />`/bin64/mingw`         | No       |
| VSCode       | 1.53      | -                                        | No       |
| **Lynx SDK** | 1.0.0     | any                                      | Yes      |


Different versions may work as well, but we haven't tested them.
Mingw is only required when building applications for Windows.

Visual Studio Code is not required, but it might be much easier to setup than other editors, as the engine can automatically generate configuration files for it.  
Support for other C++ compilers and editors will be added in future versions.

&nbsp;

&nbsp;

&nbsp;

## Download the SDK

Clone the repository from GitHub:   
`git clone https://github.com/Edo022/LynxEngine.git`  
The same system can use multiple versions of the SDK.

&nbsp;

## How to use the SDK
Projects can be linked to an SDK by running its "Setup.py" script:  
`mkdir MyProject`  
`cd MyProject`  
`python3.8 <path_to_sdk>/Setup/Setup.py`  

Each version can be used by multiple projects at the same time.  
Any modification to a version will affect all the projects linked to it.

&nbsp;

## Build applications
Applications are built using the `<path_to_sdk>/Build/lynxg++` executable, 
which is a g++ wrapper that parses the provided options, adds everything the engine needs to the compiler and linker ones and forwards them to g++.   

`-l[ \<options\> ]` and `-w[ \<options\> ]`, respectively linux and windows, can be used to enable different options based on the target platform.  
 
- Debug mode  
    - `-d[ \<options\> ]` can be used to activate specific options when building in debug mode.
    - Turns off all the optimizations and adds any available debug, gdb and gprof information.
    - Activates Vulkan validation layers and Lynx runtime validation tests.  
    Any error reported by those systems must be fixed before shipping, as they indicate a bug in the application.  

- Release mode  
    - `-r[ \<options\> ]` can be used to activate specific options when building in release mode.
    - Optimizes the code for speed by using `-Ofast` and other compiler specific optimizations.
    - Removes any unnecessary information from the executable.  

- Shipping mode  
    - Packs the application in a .zip file.
    - The output zip file can be specified with the `-so "<path_to_zip>.zip"` option.
    - Files used by the application can be included in the .zip by using the `-sf "<path_to_file>" "<path_inside_zip>"` option.


Target platform and configuration can be specified with the `-mode=\<mode\>` option.  

| Mode | Action                     | Active macros    |
|------|----------------------------|------------------|
| `ld` | Debug build for Linux      | `_dbg`<br>`_lnx` |
| `lr` | Release build for Linux    | `_rls`<br>`_lnx` |
| `ls` | Shipping build for Linux   |
| `wd` | Bebug build for Windows    | `_dbg`<br>`_wds` |
| `wr` | Release build for Windows  | `_rls`<br>`_wds` |
| `ws` | Shipping build for Windows |

e.g. `<path_to_sdk>/Build/lynxg++ -mode=ld -r[ -O3 -g0 ] -d[ -O0 -g3 ] main.cpp`  
Examples are located in `<path_to_sdk>/Examples/` and contain both the source code and the build commands.  

&nbsp;

&nbsp;

&nbsp;

# Support the developers

nyanyame nyanyajyu nyanya-do no nyarabi de nyakunyaku inyanyaku nyanyahan nyanya-dai nyannyaku nyarabete nyaganyagame
