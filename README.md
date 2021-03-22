# Lux Engine
A volume rendering and physics engine with runtime procedural generation and advanced particle physics.  

# Software requirements
The engine only supports developement on Linux systems using g++ and Python,
but can deploy applications for both Windows10 and Linux.  
Support for other C++ compilers and editors will be added in future versions.

| Software     | Version | Required |
|--------------|---------|----------|
| g++          | 10.7    | Yes      |
| Mingw        | 8.0.0   | No       |
| VSCode       | 1.53    | No       |
| Python       | 3.8     | Yes      |
| Linux kernel | 4.4     | Yes      |


Later versions may work as well, but we haven't tested them.  
Visual Studio Code is not required, but it might be much easier to setup than other editors, as the engine can automatically generate configuration files for it.  
If you use a different editor, you will have to manually write include paths and commands.



# Setup
Clone the repository from GitHub.  
It contains everything required to modify, build and use the engine.  
`git clone https://github.com/Edo022/LuxEngine.git`  

You can then link your projects to the repository and build applications with it.
Each version can be used by multiple projects at the same time.  
`cd MyProject`  
`python3.8 <path_to_repo>/Setup/Setup.py`


# Build applications
Applications are built using the` <path_to_repo>/Build/lux_g++` executable, 
which is a g++ wrapper that automatically adds everything the engine needs to your options and forwards them to g++.   

You can use -d[\<option\>], -r[\<option\>], -l[\<opttion\>] and -w[\<option\>] to use different options based on the platform or configuration.  
 
- Debug mode  
    Turns off all the optimizations and adds any available debug, gdb and gprof information.
    It also activates Vulkan validation layers and Lux Engine runtime validation tests.
    Any error reported by those systems must be fixed, as they indicate a bug in the application.
    In release mode, they are not checked and have undefined behaviour.  
- Release mode  
    Turns on -Ofast and other compiler specific optimizations, and removes all the unnecessary informations.  
 
Examples are located in `<path_to_repo>/Examples/` and contain the source code and the build commands for both VSCode and command line.  
 
## Build from command line
Platform and configuration can be specified with the -mode=\<mode\> option.  
| Mode |                                   |
|------|-----------------------------------|
| ld   | Build in debug mode for Linux     |
| lr   | Build in release mode for Linux   |
| wd   | Build in debug mode for Windows   |
| wr   | Build in release mode for Windows |


## Build from VSCode
You can switch platform and configuration by selecting specific tasks (default: ctrl+shift+b).  
Write the lux_g++ command in the `<path_to_project>/.vscode/tasks.json` file, as you would normally do with any VSCode project.




# Support the developers
nyanyame nyanyajyu nyanya-do no nyarabi de nyakunyaku inyanyaku nyanyahan nyanya-dai nyannyaku nyarabete nyaganyagame
