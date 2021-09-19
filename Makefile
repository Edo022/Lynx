# This makefile is intended to be used by the python wrapper, you may not use it manually

.DEFAULT_GOAL := all

CPP=g++ # Engine compiler, may be changed by the script for building on windows
ECPPFLAGS=-std=c++20 -I. # Default engine flags, left unchanged
ACPPFLAGS=-std=c++20 # Default application flags, left unchanged
ECPPFLAGS+=$(EFLAGS) # extra flags passed by the script
ACPPFLAGS+=$(AFLAGS) # extra flags passed by the script
ENGINELIB=$(APP)/.engine/bin/Engine/$(OUTPUT)/libLynxEngine.a # path to the engine static library
# APP = path/to/application, passed by the script
# ESRC = path/to/engine/file1.cpp path/to/engine/file2.cpp, passed by the script
# ASRC = path/to/application/file1.cpp path/to/application/file2.cpp, passed by the script
# OUTPUT = platform/mode, passed by the script to determine the correct output for the build mode
PLATFORM=$(shell printf $(TEST) | sed "s/\(.\+\)\/.\+/\1/g") # Get platform from output
# ECOMP = path/to/shader1.comp path/to/shader2.comp, passed by the script
ESHADERS=$(ECOMP:.comp=.spv) # 
ESHADERSO=$(ECOMP:.comp=.o)
# ACOMP = path/to/shader1.comp path/to/shader2.comp (application shaders), passed by the script
ASHADERS=$(ACOMP:.comp=.spv) # 
ASHADERSO=$(ACOMP:.comp=.o)


# ----------------------------------------- ENGINE ------------------------------------------ 


ifneq ($(ESRC),) # Check if there are engine source files
EBINS=$(addprefix $(APP)/.engine/bin/Engine/$(OUTPUT),$(shell basename -a $(ESRC:.cpp=.o))) # Get the binary path from each cpp file

engine: $(ENGINELIB)

# Build libLynxEngine.a rebuilds if Lynx/Lynx_config.hpp is changed
$(ENGINELIB): Lynx/Lynx_config.hpp $(EBINS) $(ESHADERS) $(ESHADERSO)
	ar -rcs $(ENGINELIB) $(filter-out $<,$^)


# Build engine object files
$(EBINS): $(ESRC)
	$(CPP) $(ECPPFLAGS) --include Lynx/Lynx_config.hpp -c $< -o $@


# generate .spv, .cpp and .hpp files from the shaders
$(ESHADERS): $(ECOMP)
	glslangValidator -V $< -o $@
	python3 Tools/Build/GlslToCpp.py $< $(PLATFORM) . e

$(ESHADERSO): $(ESHADERS)
	$(CPP) $(ECPPFLAGS) --include Lynx/Lynx_config.hpp -c $(<:.spv=.gsi.cpp) -o $@


# remove all engine files
clean_engine:
	cd $(APP)/.engine/bin/Engine; \
	@-find . -type f  ! -name "*.*" -delete; \
	@-find . -type f -name "*.o" -delete; \
	@-find . -type f -name "*.exe" -delete


endif




# ------------------------------------------ APPLICATION ------------------------------------------ 


ifneq ($(APP),) # check if APP is passed
ifneq ($(ASRC),) # Check if there are application source files
ABINS=$(addprefix $(APP)/.engine/bin/Application/$(OUTPUT),$(shell basename -a $(ASRC:.cpp=.o))) # Get the binary path from each cpp file


application: $(ENGINELIB) $(ABINS) $(ASHADERS) # The application requires the engine static library, if not available or outdated it's rebuilt


# Build application object files
$(ABINS): $(ASRC)
	$(CPP) $(ACPPFLAGS) -c $< -o $@



$(ASHADERS): $(ACOMP)
	glslangValidator -V $< -o $@
	python3 Tools/Build/GlslToCpp.py $< $(PLATFORM) . a



$(ASHADERSO): $(ASHADERS)
	$(CPP) $(ACPPFLAGS) -c $(<:.spv=.gsi.cpp) -o $@




clean_application: # delete all object and executable files (including Windows .exe files)
	cd $(APP)/.engine/bin/Application; \
	@-find . -type f  ! -name "*.*" -delete; \
	@-find . -type f -name "*.o" -delete; \
	@-find . -type f -name "*.exe" -delete





endif
endif



all: $(ENGINELIB) application # make/make all builds the engine if necessary and the application every time

clean: clean_engine clean_application # make clean removes every single object and executable file


