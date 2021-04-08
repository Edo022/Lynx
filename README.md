# Lynx
A volume rendering and physics engine with runtime procedural generation and advanced physics.  


&nbsp;

&nbsp;

&nbsp;

# Software requirements
The engine only supports developement on Linux systems using g++ and Python,
but can deploy applications for both Windows10 and Linux.  
Support for other C++ compilers and editors will be added in future versions.

| Software   | Version   | Path           | Required |
|------------|-----------|----------------|----------|
| Linux      | 4.4       | -              | Yes      |
| Python     | 3.8       | /bin/python3.8 | Yes      |
| g++        | 10.7      | /bin/g++       | Yes      |
| Vulkan SDK | 1.2.170.0 | any            | Yes      |
| Mingw      | 8.0.0     | /bin/mingw     | No       |
| VSCode     | 1.53      | -              | No       |



Later versions may work as well, but we haven't tested them.  
Mingw is only required when building applications for Windows.  
Visual Studio Code is not required, but it might be much easier to setup than other editors, as the engine can automatically generate configuration files for it.  
If you use a different editor, you will have to manually write include paths and commands.

&nbsp;

&nbsp;

&nbsp;

# Setup
Clone the repository from GitHub.  
It contains everything required to modify, build and use the engine.  
`git clone https://github.com/Edo022/LynxEngine.git`  

You can then link your projects to the repository and build applications with it.
Each version can be used by multiple projects at the same time.  
`cd MyProject`  
`python3.8 <path_to_repo>/Setup/Setup.py`

&nbsp;

&nbsp;

&nbsp;

# Build applications
Applications are built using the `<path_to_repo>/Build/lynxg++` executable, 
which is a g++ wrapper that automatically adds everything the engine needs to your options and forwards them to g++.   

You can use -d[\<option\>], -r[\<option\>], -l[\<opttion\>] and -w[\<option\>] to use different options based on the platform or configuration.  
 
- Debug mode  
    Turns off all the optimizations and adds any available debug, gdb and gprof information.
    It also activates Vulkan validation layers and Lynx runtime validation tests.
    Any error reported by those systems must be fixed, as they indicate a bug in the application.
    In release mode, they are not checked and have undefined behaviour.  
- Release mode  
    Turns on -Ofast and other compiler specific optimizations, and removes all the unnecessary informations.  
 
Examples are located in `<path_to_repo>/Examples/` and contain the source code and the build commands for both VSCode and command line.  

&nbsp;

## Build from command line
Platform and configuration can be specified with the -mode=\<mode\> option.  
| Mode |                                   |
|------|-----------------------------------|
| ld   | Build in debug mode for Linux     |
| lr   | Build in release mode for Linux   |
| wd   | Build in debug mode for Windows   |
| wr   | Build in release mode for Windows |

e.g. `<path_to_repo>/Build/lynxg++ -mode=ld, -r[-O3], main.cpp`  

&nbsp;

## Build from VSCode
Write the luxg++ command options in the arguments of the first task in `<path_to_project>/.vscode/tasks.json`.  
The second task is used by the engine builds, the others are used to switch between platform and configuratios.  
e.g.  
`"args": [`  
&emsp;`"-mode=ld", //Automatically set. Don't change it`  
&emsp;`"-r[O3]", "main.cpp"`  
`],`  

&nbsp;

&nbsp;

&nbsp;

# Support the developers
nyanyame nyanyajyu nyanya-do no nyarabi de nyakunyaku inyanyaku nyanyahan nyanya-dai nyannyaku nyarabete nyaganyagame
