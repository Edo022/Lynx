##########################################################################################################

# This makefile is intended to be used by the wrapper Tools/Build/lynxg++, you may not use it manually

##########################################################################################################
#FIXME only build modified files



.DEFAULT_GOAL := abuild
# .DEFAULT_GOAL := dbg
.PHONY: ebuild abuild eclean aclean clean ABIN

#TODO add option to use provided compilation outputs
#FIXME automatically track headers
#FIXME pass tracked headers from script




EXEC     =$(strip $(_EXEC))#		_EXEC =						# Path to the compiler. Changed by the wrapper when building for Windows	#! Passed by the wrapper	#TODO use mingw for windows
OUTPUT   =$(strip $(_OUTPUT))#		_OUTPUT =					# Output path suffix based on build configuration and target platform		#! Passed by the wrapper
APP      =$(strip $(_APP))#			_APP =						# Path to the user application												#! Passed by the wrapper
PLATFORM =$(shell printf $(OUTPUT) | sed "s/\(.\+\)\/.\+/\1/g")## Get target platform from output path
ELIB     =$(APP)/.engine/bin/Lnx/$(OUTPUT)/libLynxEngine.a#		# Path to the engine static library


LINK  =#							_LINK =						# Application linker arguments
    LINK += -L/usr/lib64 -L/lib64#									# Prefer 64bit libraries						#TODO fix windows build
    LINK += -ldl -lrt -lXrandr -lXi -lXcursor -lXinerama -lX11#		# Link dependencies								#TODO fix windows build
    LINK += -lvulkan -Bstatic -lglfw#								# Link Vulkan dynamically and GLFW statically	#TODO fix windows build
    LINK += $(strip $(_LINK))#										# Append user defined link flags #! Passed by the wrapper


SFLG =#															# Shared C++ default flags
    SFLG += -pthread -I.#											# Default g++ call, pthread, include project root
    SFLG += -std=c++20 -m64#										# Use C++20, build for 64bit environments
    SFLG += -include Lynx/Core/VkDef.hpp#							# Include forced vulkan macros
    SFLG += -include Lynx/Lynx_config.hpp#							# Include engine configuration macros
    SFLG += -ffile-prefix-map=$(realpath $(APP))=#					# Fix file prefix in debug infos




EFLG  =#														# Default engine flags
EFLG += $(strip $(_EFLG))#		_EFLG =							# Append user defined flags		#! Passed by the wrapper
EGLS   =$(strip $(_EGLS))#		_EGLS =							# Engine GLSL source files		#! Passed by the wrapper
ECPP   =$(strip $(_ECPP))#		_ECPP =							# Engine C++  source files		#! Passed by the wrapper
ECPP += $(addsuffix .gsi.cpp,$(basename $(EGLS)))#				# Append shaders C++ source files to ECPP

ESPV   =$(strip $(addsuffix .spv,$(basename $(EGLS))))#			# Get output .spv files paths
EOBJ   =$(addsuffix .o,$(addprefix $(APP)/.engine/bin/Lnx/$(OUTPUT)/,$(notdir $(basename $(strip $(_ECPP))))))#		# Get output .o files of the non-generated C++ source files
EOBJ += $(addsuffix .o,$(addprefix $(APP)/.engine/bin/Lnx/$(OUTPUT)/Shaders/,$(notdir $(basename $(EGLS)))))#		# Append modified paths of shaders C++ source files
#!^ $(ECPP) is not used as it also contains the interfaces output C++ files which need to be in a different directory




AFLG  =#														# Default application flags
AFLG += $(strip $(_AFLG))#		_AFLG =							# Append user defined flags		#! Passed by the wrapper
AGLS   =$(strip $(_AGLS))#		_AGLS =							# Application GLSL source files	#! Passed by the wrapper	#! Can be empty
ACPP   =$(strip $(_ACPP))#		_ACPP =							# Application C++  source files	#! Passed by the wrapper
ACPP += $(strip $(addsuffix .gsi.cpp,$(basename $(AGLS))))#		# Append shaders C++ source files to ACPP

ASPV   =$(strip $(addsuffix .spv,$(basename $(AGLS))))#			# Get output .spv files paths
AOBJ   =$(addsuffix .o,$(addprefix $(APP)/.engine/bin/App/$(OUTPUT)/,$(notdir $(basename $(strip $(_ACPP))))))#		# Get output .o files of the non-generated C++ source files
AOBJ += $(addsuffix .o,$(addprefix $(APP)/.engine/bin/App/$(OUTPUT)/Shaders/,$(notdir $(basename $(AGLS)))))#		# Append modified paths of shaders C++ source files
#!^ $(ACPP) is not used as it also contains the interfaces output C++ files which need to be in a different directory



# Create output directories
$(shell mkdir -p $(APP)/.engine/bin/Lnx/$(OUTPUT))
$(shell mkdir -p $(APP)/.engine/bin/App/$(OUTPUT))
$(shell mkdir -p $(APP)/.engine/bin/Lnx/$(OUTPUT)/Shaders)
$(shell mkdir -p $(APP)/.engine/bin/App/$(OUTPUT)/Shaders)



dbg:
	@clear
	@echo $(ACPP)
	@echo
	@echo
	@echo $(AOBJ)
	@echo
	@echo
	@echo
	@echo
	@echo $(ECPP)
	@echo
	@echo
	@echo $(EOBJ)




# ----------------------------------------- ENGINE ------------------------------------------




ifneq ($(ECPP),)

    # Build engine spir-v files and generate shader interfaces
    $(ESPV):$(EGLS)
	    glslangValidator -V $< -o $@
        # python3 Tools/Build/GlslToCpp.py $< $(PLATFORM) ${shell pwd} e


    # Build engine object files
    $(EOBJ):$(ECPP)
        # $(EXEC) $(SFLG) $(EFLG) -c $(if $(filter-out .cpp,$(suffix $<)),-xc++) $< -o $@
        # @echo -xc++ $(if $(filter-out .cpp,$(suffix $<)),-xc++) $<
	    @echo $<


    # Build engine static library
    $(ELIB):$(EOBJ)
        # ar -rcs $(ELIB) $(filter-out $<,$^)
	    ar -rcs $(ELIB) $^
    ebuild: $(ELIB) $(ESPV)

endif




# Remove all the engine files
eclean:
	-@(cd $(APP)/.engine/bin/Lnx && find . -type f -wholename "./*/*/*.o" -delete) || true
	-@(cd $(APP)/.engine/bin/Lnx && find . -type f -wholename "./*/*/libLynxEngine.a" -delete) || true




# ------------------------------------------ APPLICATION ------------------------------------------




ifneq ($(APP),)
ifneq ($(ACPP),)

    # Build application spir-v files and generate shader interfaces
    $(ASPV):$(AGLS)
	    glslangValidator -V $< -o $@
        # python3 Tools/Build/GlslToCpp.py $< $(PLATFORM) ${shell pwd} a #FIXME


    # Build application object files
    $(AOBJ):$(ACPP)
	    $(EXEC) $(SFLG) $(AFLG) -c $(if $(filter-out .cpp,$(suffix $<)),-xc++) $< -o $@


    # Buld executable #FIXME follow input options
    # ABIN: $(ELIB) $(AOBJ)
    ABIN:$(AOBJ) $(ELIB)
        # $(EXEC) $(SFLG) $(AFLG) $(LINK) $(filter-out $<,$^) -L$(APP)/.engine/bin/$(OUTPUT) -l:libLynxEngine.a -o $(APP)/tmp.out
	    $(EXEC) $(SFLG) $(AFLG) $^ $(LINK) -o $(APP)/tmp.out
    abuild: ebuild ABIN $(ASPV)

endif
endif




#TODO delete executable output file
#TODO delete generated shaders
# Delete all the object and executable files (including Windows .exe files)
aclean:
	-@(cd $(APP)/.engine/bin/App && find . -type f -wholename "./*/*/*.o" -delete) || true




# ------------------------------------------------ ALL --------------------------------------------




# make clean removes every object and executable file
clean: eclean aclean


