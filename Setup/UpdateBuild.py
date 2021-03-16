import os




def run(dir):

	def getPf():
		return "Linux" if opts.pf() == "l" else "Windows"
	def getTp():
		return "Debug" if opts.tp() == "d" else "Release"


	os.chdir(dir)
	import BuildOptions as opts
	#import importlib.util
	# spec = importlib.util.spec_from_file_location("BuildOptions.BuildOptions", enginePath_ + '/Build/BuildOptions/BuildOptions.py')
	# foo = importlib.util.module_from_spec(spec)
	# spec.loader.exec_module(foo)

	#Open tasks file
	with open("./.vscode/tasks.json", "r") as f:

		#Find user build
		while(f.read(1) != '['): pass
		while(f.read(1) != '['): pass

		#read user build
		userBuild = ''
		n = 1
		while(n > 0):
		    c = f.read(1)
		    if(c == '['): n += 1
		    elif(c == ']'): n -= 1
		    userBuild += c

	try:
		plf = getPf()
	except FileNotFoundError:
		plf = ''
	try:
		typ = getTp()
	except FileNotFoundError:
		typ = ''



	with open("./.vscode/tasks.json", "w") as f:
		f.write(
		'//This file was generated by the engine' 																	+'\n'+\
		'//Only modify the options inside the "Build application" task args' 										+'\n'+\
		'//Other changes could be overwritten without any advise or cause the program to behave uncorreclty' 		+'\n'+\
		'{' 																										+'\n'+\
		'    "version": "2.0.0",'																					+'\n'+\
		'    "tasks": ['																							+'\n'+\
		'        {'																									+'\n'+\
		'            "type": "shell",'																				+'\n'+\
		'            "label": "' + plf + '  |  ' + typ + '  |  Build Application",'									+'\n'+\
		'            "command": "' + opts.enginePath() + '/Build/lux_g++",'											+'\n'+\
		'            "args": [' + userBuild + ','																	+'\n'+\
		'            "problemMatcher": [ "$gcc" ],'																	+'\n'+\
		'            "options": { "cwd": "${workspaceFolder}" },'													+'\n'+\
		'            "group": { "kind": "build", "isDefault": true }'												+'\n'+\
		'        },'																								+'\n'+\
		'        {'																									+'\n'+\
		'            "type": "shell",' 																				+'\n'+\
		'            "label": "' + plf + '  |  ' + typ + '  |  Build LuxEngine\",' 									+'\n'+\
		'            "command": "/usr/bin/g++",'																	+'\n'+\
		'            "args": ['																				 		+'\n'+\
		'                //Source files' 																			+'\n'+\
		'		            "-xc++", "' + opts.enginePath() + '/LuxEngine/LuxEngine_build.cpp",' 					+'\n'+\
		                    (opts.getDebugOptions() if opts.tp() == "d" else opts.getReleaseOptions()) + ',' 		+'\n'+\
		'                //Engine' 																					+'\n'+\
		'                    "-std=c++2a", "-mavx", "-pipe", "-pthread",' 											+'\n'+\
		'                    "-I' + opts.enginePath() + '",' 														+'\n'+\
		# '                    "-Wall",'																				+'\n'+\
		'"-Wall", ' +\

		'"-Wclobbered", ' +\
		'"-Wcast-function-type", ' +\
		'"-Wdeprecated-copy", ' +\
		'"-Wempty-body", ' +\
		'"-Wignored-qualifiers", ' +\
		'"-Wimplicit-fallthrough=3", ' +\
		'"-Woverride-init", ' +\
		'"-Wstring-compare", ' +\
		'"-Wredundant-move", ' +\
		'"-Wtype-limits", ' +\
		'"-Wuninitialized", ' +\
		'"-Wshift-negative-value", ' +\
		'"-Wunused-parameter", ' +\
		'"-Wunused-but-set-parameter", ' +\



		'"-Wcast-align", ' +\
		'"-Wcast-qual", ' +\
		'"-Wctor-dtor-privacy", ' +\
		'"-Wdisabled-optimization", ' +\
		'"-Wformat=2", ' +\
		'"-Winit-self", ' +\
		'"-Wlogical-op", ' +\
		'"-Wmissing-declarations", ' +\
		'"-Wmissing-include-dirs", ' +\
		'"-Wnoexcept", ' +\
		'"-Woverloaded-virtual", ' +\
		'"-Wredundant-decls", ' +\
		'"-Wshadow", ' +\
		'"-Wsign-conversion", ' +\
		'"-Wsign-promo", ' +\
		'"-Wstrict-null-sentinel", ' +\
		'"-Wstrict-overflow=5", ' +\
		'"-Wswitch-default", ' +\
		'"-Wundef", ' +'\n'+\
		                	opts.getEngineDeps() + ',' 																+'\n'+\
		'                //Output'																					+'\n'+\
		'					"-c", "-o", "' + opts.enginePath() + '/Build/' + plf + '/LuxEngine' + typ + '"' 		+'\n'+\
		'            ],' 																							+'\n'+\
		'            "problemMatcher": [ "$gcc" ],' 																+'\n'+\
		'            "options": { "cwd": "${workspaceFolder}" },' 													+'\n'+\
		'            "group": { "kind": "build", "isDefault": true }' 												+'\n'+\
		'        },' 																								+'\n'+\
		'        {' 																								+'\n'+\
		'            "type": "shell",' 																				+'\n'+\
		'            "label": " > Switch to ' + ("Windows" if opts.pf() == "l" else "Linux") + '",' 				+'\n'+\
		'            "command": "python3",' 																		+'\n'+\
		'            "args": [' 																					+'\n'+\
		'                "' + opts.enginePath() + '/Setup/SetPlatform.py",' 										+'\n'+\
		'                "' + ("w" if opts.pf() == "l" else "l") + '",'												+'\n'+\
		'            ],' 																							+'\n'+\
		'            "problemMatcher": [ ],'																		+'\n'+\
		'            "options": { "cwd": "${workspaceFolder}" },' 													+'\n'+\
		'            "group": { "kind": "build", "isDefault": true }' 												+'\n'+\
		'        },' 																								+'\n'+\
		'        {' 																								+'\n'+\
		'            "type": "shell",' 																				+'\n'+\
		'            "label": " > Switch to ' + ("Release" if opts.tp() == "d" else "Debug") + '",' 				+'\n'+\
		'            "command": "python3",' 																		+'\n'+\
		'            "args": [' 																					+'\n'+\
		'                "' + opts.enginePath() + '/Setup/SetType.py",' 											+'\n'+\
		'                "' + ("r" if opts.tp() == "d" else "d") + '"' 												+'\n'+\
		'            ],' 																							+'\n'+\
		'            "problemMatcher": [ ],' 																		+'\n'+\
		'            "options": { "cwd": "${workspaceFolder}" },' 													+'\n'+\
		'            "group": { "kind": "build", "isDefault": true }' 												+'\n'+\
		'        }' 																								+'\n'+\
		'    ]' 																									+'\n'+\
		'}\n')