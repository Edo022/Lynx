# Lux Engine
A volume rendering and physics engine with runtime procedural generation and advanced particle physics.  
The engine only supports developement on Linux systems using g++ and VSCode,
but can deploy applications for both Windows10 and Linux.  
Support for other C++ compilers and editors will be added in future versions.

# Minimum requirements
| Software|Version|
|---------|-------|
| g++     | 10.7  |
| VSCode  | 1.53  |
| Python  | 3.8   |
| Linux   | 4.4   |

Older versions may work as well, but we haven't tested them and don't know if they can be used safely

//TODO

# Setup
Clone the repository from GitHub.  
It contains everything required to modify, build and use the engine.  
`git clone https://github.com/Edo022/LuxEngine.git`  
You can then link your projects to the repository and build applications with it.
Any version can be used by multiple project at the same time.

# Build applications
cd into your project directory and link it to the desired engine version.  
`cd MyProject`  
`python3.8 <path_to_engine_repo>/Setup/Setup.py`

You can configure the g++ command to compile your application in the .vscode/tasks.json file, as you wold normally do with any VSCode project.  
The tasks are set to run lux_g++, which is a g++ wrapper that automatically adds everything the engine needs to your options and forwards them to g++.  
C++20 is required.

The Vulkan validation layers library "libVkLayer_khronos_validation.so" must be placed in the default library directory, or bad things could happen

# Build modes
The task.json file has 2 build configurations: debug, and release.   
Debug mode turns off all the optimizations and adds any available debug, gdb and gprof information.
It also activates Vulkan validation layers and Lux Engine runtime validation tests.
Any error reported by those systems must be resolved, as they indicate a bug in the application.
In release mode, they are not checked and have undefined behaviour.  
Release mode turns on -Ofast and other compiler specific optimizations, and removes all the unnecessary informations.


# Support the developers
chu  
//TODO
