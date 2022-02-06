import json
import alloy

with open('.alloy.json', 'r') as f:
	settings = json.loads(f.read())
files = {filename : alloy.auto_deps(filename, settings['flags']) for filename in alloy.auto_files(extension='.c')}
c = alloy.Compiler(settings['compiler'], files, settings['bin'], 'alloy_build', settings['flags'])
c.to_make()
