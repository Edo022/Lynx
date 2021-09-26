.SECONDEXPANSION:


# get-dep(targets,dependencies)
# Returns the dependency element corresponding to the target of the current rule
get-dep=$$(word 2,$$(subst ^, ,$$(filter $$@^$$(percent),$(join $1,$(addprefix ^,$2)))))
percent=%

# gen-cpp-deps(flags,objects,sources)
# Generates a rule for each object file and writes the corresponding source file and the required headers in its dependencies
gen-cpp-deps=$2:%:$$(wordlist 2,999999,$$(shell g++ -M -MG $1 $(call get-dep,$2,$3) | tr -d "\\n\\\\"))




run:$(OBJ)

    # Build object files
    $(call gen-cpp-deps,$(FLG),$(OBJ),$(CPP))
	    @echo Compiling $@
	    @$(EXEC) $(FLG) -c -xc++ $< -o $@