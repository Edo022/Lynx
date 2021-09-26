.SECONDEXPANSION:


# get-dep(targets,dependencies)
# Returns the dependency element corresponding to the target of the current rule
get-dep=$$(word 2,$$(subst ^, ,$$(filter $$@^$$(percent),$(join $1,$(addprefix ^,$2)))))
percent=%

# target-tuple(targets,dependencies)
# Generates a rule for each target and writes the corresponding dependency in its dependencies
target-tuple=$1:%:$$(filter $$@$$(percent),$(join $1,$(addprefix ^,$2)))#FIXME filter deletes the whole string
# target-tuple=$1:%:$(call get-dep,$1,$2)




run:$(SPV) $(GSI)

   # Build spir-v files
   $(call target-tuple,$(SPV),$(GLS))
	    @echo Compiling shader $@ WWWWWW $^
	    glslangValidator -V $^ -o $@


    # Generate shader interfaces
    $(call target-tuple,$(GSI),$(GLS))
	    @echo Generating interface files for shader $@
	    python3 Tools/Build/GlslToCpp.py $^ $(APP) e
