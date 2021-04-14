import os, re




def run(dir):

	def getPf():
		return "Linux" if opts.pf() == "l" else "Windows"
	def getTp():
		return "Debug" if opts.tp() == "d" else "Release"


	os.chdir(dir) #FIXME REMOVE
	import BuildOptions as opts




	#Update tasks.json
	with open('./.vscode/tasks.json', 'r') as f:
		s = re.sub(r'("args"\s*:\s*\[\s*"-mode=)[l|w][d|r|s]"', r'\g<1>' + opts.pf() + opts.tp() + '"',
			re.sub(r'("label"\s*:\s*")\w+  \|  \w+(  \|  Build (\w| )+")', r'\g<1>' + getPf() + '  |  ' + getTp() + r'\g<2>',
			re.sub(r'("label"\s*:\s*" > Switch to )(Linux|Windows)("(.|\n)*?"Lynx\/Setup\/SetPlatform\.py"\s*,\s*")(l|w)"', r'\g<1>' + ("Windows" if opts.pf() == "l" else "Linux") + r'\g<3>' + ('w' if opts.pf() == 'l' else 'l') + '"',
			re.sub(r'("label"\s*:\s*" > Switch to )(Debug|Release)("(.|\n)*?"Lynx\/Setup\/SetType' r'\.py"\s*,\s*")(d|r)"', r'\g<1>' + ("Release" if opts.tp() == "d" else "Debug") + r'\g<3>' + ('r' if opts.tp() == 'd' else 'd') + '"',
		f.read()))))

	with open('./.vscode/tasks.json', 'w') as f:
		f.write(s)


	#Update debug macro
	with open('./.engine/conf.hpp', 'w') as f:
		f.write('#define ' + ('LNX_DEBUG' if opts.tp() == 'd' else 'LNX_RELEASE'))
