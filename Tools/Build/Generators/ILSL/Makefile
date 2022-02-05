CC=gcc
CFLAGS=-std=c11 -ggdb3 -g3 -O0 -I.

all: ../GenGlsl

alloy_build/%home%Edo%Desktop%Lynx-T%Lynx%Tools%Build%Generators%ILSL%ILSL.c.o: /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/ILSL/ILSL.c /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/ILSL/Utils.h /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/ILSL/Preprocessor/Files.h /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/ILSL/Tokenizer/Operators.h /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/ILSL/Tokenizer/Types.h Tokenizer/Tokens.h /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/ILSL/Tokenizer/Keywords.h /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/ILSL/Tokenizer/Literals.h /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/ILSL/Preprocessor/Preprocessor.h /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/ILSL/Tokenizer/Tokenizer.h /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/ILSL/SyntaxAnalyzer/SyntaxAnalyzer.h SyntaxAnalyzer/Constructs.h 
	$(CC) $(CFLAGS) -c $< -o $@

alloy_build/%home%Edo%Desktop%Lynx-T%Lynx%Tools%Build%Generators%ILSL%SyntaxAnalyzer%SyntaxAnalyzer.c.o: /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/ILSL/SyntaxAnalyzer/SyntaxAnalyzer.c Utils.h Preprocessor/Files.h SyntaxAnalyzer/SyntaxAnalyzer.h SyntaxAnalyzer/Constructs.h Tokenizer/Tokens.h Tokenizer/Operators.h 
	$(CC) $(CFLAGS) -c $< -o $@

alloy_build/%home%Edo%Desktop%Lynx-T%Lynx%Tools%Build%Generators%ILSL%Preprocessor%Preprocessor.c.o: /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/ILSL/Preprocessor/Preprocessor.c Utils.h Preprocessor/Files.h Preprocessor/Preprocessor.h 
	$(CC) $(CFLAGS) -c $< -o $@

alloy_build/%home%Edo%Desktop%Lynx-T%Lynx%Tools%Build%Generators%ILSL%Preprocessor%Files.c.o: /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/ILSL/Preprocessor/Files.c /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/ILSL/Preprocessor/Files.h 
	$(CC) $(CFLAGS) -c $< -o $@

alloy_build/%home%Edo%Desktop%Lynx-T%Lynx%Tools%Build%Generators%ILSL%Tokenizer%Tokenizer.c.o: /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/ILSL/Tokenizer/Tokenizer.c Utils.h Preprocessor/Files.h Tokenizer/Tokenizer.h Tokenizer/Tokens.h Tokenizer/Operators.h Tokenizer/Types.h Tokenizer/Keywords.h Tokenizer/Literals.h 
	$(CC) $(CFLAGS) -c $< -o $@

alloy_build/%home%Edo%Desktop%Lynx-T%Lynx%Tools%Build%Generators%ILSL%Utils.c.o: /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/ILSL/Utils.c /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/ILSL/Utils.h /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/ILSL/Preprocessor/Files.h 
	$(CC) $(CFLAGS) -c $< -o $@

../GenGlsl: alloy_build/%home%Edo%Desktop%Lynx-T%Lynx%Tools%Build%Generators%ILSL%ILSL.c.o alloy_build/%home%Edo%Desktop%Lynx-T%Lynx%Tools%Build%Generators%ILSL%SyntaxAnalyzer%SyntaxAnalyzer.c.o alloy_build/%home%Edo%Desktop%Lynx-T%Lynx%Tools%Build%Generators%ILSL%Preprocessor%Preprocessor.c.o alloy_build/%home%Edo%Desktop%Lynx-T%Lynx%Tools%Build%Generators%ILSL%Preprocessor%Files.c.o alloy_build/%home%Edo%Desktop%Lynx-T%Lynx%Tools%Build%Generators%ILSL%Tokenizer%Tokenizer.c.o alloy_build/%home%Edo%Desktop%Lynx-T%Lynx%Tools%Build%Generators%ILSL%Utils.c.o 
	$(CC) $(CFLAGS) $^ -o $@

clean:
	@-rm alloy_build/*.o
	@-rm ../GenGlsl
