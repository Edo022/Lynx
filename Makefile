# This makefile is intended to be used by the python wrapper, you may not use it manually

.DEFAULT_GOAL := all

CPP=g++ # Compiler, changed by the script when building for windows
CPPFLAGS=-std=c++20 -I. # Default flags, left unchanged
CPPFLAGS+=$(flags) # extra flags passed by the script
ENGINELIB=$(APP)/.engine/bin/Engine/$(OUTPUT)/libLynxEngine.a # path to the engine static library
# APP = path/to/application, passed by the script
# SRC = path/to/engine/file1.cpp path/to/engine/file2.cpp, passed by script
# OUTPUT = platform/mode, passed by script to determine the correct output for the build mode



ifneq ($(SRC),) # Check if there are source files
EBINS=$(addprefix $(APP)/.engine/bin/Engine/$(OUTPUT),$(shell basename -a $(SRC:.cpp=.o)))

# Build libLynxEngine.a rebuilds if Lynx/Lynx_config.hpp is changed
$(ENGINELIB): Lynx/Lynx_config.hpp $(EBINS)
	ar -rcs ./bin/lynxengine.a $(filter-out $<,$^)

else # If there aren't, don't try to build
$(ENGINELIB):
	@>&2 echo "Missing source files"
endif

engine: $(ENGINELIB) # make engine forces engine to be rebuilt

all: $(ENGINELIB) application # make/make all build the engine if necessary and the application every time


ifneq ($(APP),) # check if APP is passed
application: $(ENGINELIB) # The application requires the engine static library, if not available or outdated it's rebuilt
	cd $(APP); \
	# TODO Build Commands	

clean_application: # delete all object and executable files (including Windows .exe files)
	cd $(APP)/.engine/bin/Application; \
	@-find . -type f  ! -name "*.*" -delete; \
	@-find . -type f -name "*.o" -delete; \
	@-find . -type f -name "*.exe" -delete



else # if APP is not passed don't build
application:
	@>&2 echo "Missing application path"
clean_application:
	@>&2 echo "Missing application path"
endif


# Build engine object files
$(EBINS): $(SRC)
	$(CPP) $(CPPFLAGS) --include Lynx/Lynx_config.hpp -c $< -o $@

clean: clean_engine clean_application # make clean removes every single object and executable file

# remove all engine files
clean_engine:
	cd $(APP)/.engine/bin/Engine; \
	@-find . -type f  ! -name "*.*" -delete; \
	@-find . -type f -name "*.o" -delete; \
	@-find . -type f -name "*.exe" -delete
