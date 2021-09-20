# This makefile is intended to be used by the python wrapper, you may not use it manually

.DEFAULT_GOAL := all
#TODO add option to use provided compilation outputs



# CPP     = g++														# Path to the compiler. Changed by the wrapper when building for Windows #TODO use mingw for windows
# OUTPUT  = Platform/Mode											# Output path suffix based on build configuration and target platform	#! Passed by the wrapper
# APP     = path/to/application										# Path to the user application											#! Passed by the wrapper
XPLATFORM = $(shell printf $(OUTPUT) | sed "s/\(.\+\)\/.\+/\1/g") 	# Get target platform from output path
ENGINELIB = $(APP)/.engine/bin/Engine/$(OUTPUT)/libLynxEngine.a 	# Path to the engine static library

# EFLAGS  =															# User defined engine flags from the wrapper
ECPPFLAGS = 														# Default engine flags
    ECPPFLAGS += $(EFLAGS)												# Append user defined flags

# AFLAGS  =															# User defined application flags from the wrapper
ACPPFLAGS =  														# Default application flags
    ACPPFLAGS += -L/usr/lib64 -L/lib64#									# Prefer 64bit libraries						#TODO fix windows build
    ACPPFLAGS += -ldl -lrt -lXrandr -lXi -lXcursor -lXinerama -lX11#	# Link dependencies								#TODO fix windows build
    ACPPFLAGS += -lvulkan -Bstatic -lglfw#								# Link Vulkan dynamically and GLFW statically	#TODO fix windows build
    ACPPFLAGS += $(AFLAGS)												# Append user defined flags


SCPPFLAGS =															# Shared C++ default flags
    SCPPFLAGS += -pthread -I.#											# Default g++ call, pthread, include project root
    SCPPFLAGS += -std=c++20 -m64#										# Use C++20, build for 64bit environments
    SCPPFLAGS += -include Lynx/Core/VkDef.hpp#							# Include forced vulkan macros
    SCPPFLAGS += -include Lynx/Lynx_config.hpp#							# Include engine configuration macros
    SCPPFLAGS += -ffile-prefix-map=$(APP)=#								# Fix file prefix in debug infos




# ESRC      = path/to/engine/file1.cpp...	# Engine C++  source files #! Passed by the wrapper
# ECOMP     = path/to/shader1.comp...		# Engine GLSL source files #! Passed by the wrapper
ESHADERS    = $(ECOMP:.comp=.spv)			# Get output spir-v files
ESHADERSO   = $(ECOMP:.comp=.o)				# Get output .o files for generated shader interfaces

# ASRC      = path/to/app/file1.cpp...		# Application C++  source files #! Passed by the wrapper
# ACOMP     = path/to/shader1.comp...		# Application GLSL source files #! Passed by the wrapper
ASHADERS    = $(ACOMP:.comp=.spv)			# Get output spir-v files
ASHADERSO   = $(ACOMP:.comp=.o)				# Get output .o files for generated shader interfaces






$(shell mkdir -p					 		 \
	$(APP)/.engine/bin/Engine/$(OUTPUT)		 \
	$(APP)/.engine/bin/Application/$(OUTPUT) \
)



# ----------------------------------------- ENGINE ------------------------------------------



# Check if there are engine source files
ifneq ($(ESRC),)
# Get the binary path from each cpp file
EBINS = $(addprefix $(APP)/.engine/bin/Engine/$(OUTPUT),$(shell basename -a $(ESRC:.cpp=.o)))

engine: $(ENGINELIB)

# Build libLynxEngine.a rebuilds if Lynx/Lynx_config.hpp is changed
$(ENGINELIB): Lynx/Lynx_config.hpp $(EBINS) $(ESHADERS) $(ESHADERSO)
	ar -rcs $(ENGINELIB) $(filter-out $<,$^)


# Build engine object files
$(EBINS): $(ESRC)
	$(CPP) $(SCPPFLAGS) $(ECPPFLAGS) -c $< -o $@


# Build engine spir-v files and generate shader interfaces
$(ESHADERS): $(ECOMP)
	glslangValidator -V $< -o $@
	python3 Tools/Build/GlslToCpp.py $< $(PLATFORM) ${shell pwd} e

#Build engine generated shader interfaces .cpp s
$(ESHADERSO): $(ESHADERS)
	$(CPP) $(SCPPFLAGS) $(ECPPFLAGS) -c $(<:.spv=.gsi.cpp) -o $@


# Remove all the engine files
clean_engine:
	cd $(APP)/.engine/bin/Engine; \
	@-find . -type f  ! -name "*.*" -delete; \
	@-find . -type f -name "*.o" -delete; \
	@-find . -type f -name "*.exe" -delete


endif




# ------------------------------------------ APPLICATION ------------------------------------------




ifneq ($(APP),)		# check if APP is passed
ifneq ($(ASRC),) 	# Check if there are application source files
ABINS = $(addprefix $(APP)/.engine/bin/Application/$(OUTPUT),$(shell basename -a $(ASRC:.cpp=.o))) # Get the binary path from each cpp file


# The application requires the engine static library, if not available or outdated it's rebuilt
application: $(ENGINELIB) $(ABINS) $(ASHADERS)


# Build application object files
$(ABINS): $(ASRC)
	$(CPP) $(SCPPFLAGS) $(ACPPFLAGS) -c $< -o $@


# Build application spir-v files and generate shader interfaces
$(ASHADERS): $(ACOMP)
	glslangValidator -V $< -o $@
	python3 Tools/Build/GlslToCpp.py $< $(PLATFORM) ${shell pwd} a



#Build application generated shader interfaces .cpp s
$(ASHADERSO): $(ASHADERS)
	$(CPP) $(SCPPFLAGS) $(ACPPFLAGS) -c $(<:.spv=.gsi.cpp) -o $@



# Delete all the object and executable files (including Windows .exe files)
clean_application:
	cd $(APP)/.engine/bin/Application; \
	@-find . -type f  ! -name "*.*" -delete; \
	@-find . -type f -name "*.o" -delete; \
	@-find . -type f -name "*.exe" -delete





endif
endif



# make/make all builds the application and the engine, if necessary
all: $(ENGINELIB) application

# make clean removes every object and executable file
clean: clean_engine clean_application


