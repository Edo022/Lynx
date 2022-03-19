CC=gcc
CFLAGS=-std=c11 -ggdb3 -g3 -O0 -I.

all: ../GenGlsl

.bin/%home%Edo%Desktop%Lynx-T%Lynx%Tools%Build%Generators%Lux%SyntaxAnalyzer%Constructs.c.o: /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/Lux/SyntaxAnalyzer/Constructs.c SyntaxAnalyzer/Constructs.h Tokenizer/Tokens.h Preprocessor/Files.h Tokenizer/Operators.h Utils.h 
	$(CC) $(CFLAGS) -c $< -o $@

.bin/%home%Edo%Desktop%Lynx-T%Lynx%Tools%Build%Generators%Lux%SyntaxAnalyzer%SyntaxAnalyzer.c.o: /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/Lux/SyntaxAnalyzer/SyntaxAnalyzer.c Utils.h Preprocessor/Files.h SyntaxAnalyzer/SyntaxAnalyzer.h SyntaxAnalyzer/Constructs.h Tokenizer/Tokens.h Tokenizer/Operators.h 
	$(CC) $(CFLAGS) -c $< -o $@

.bin/%home%Edo%Desktop%Lynx-T%Lynx%Tools%Build%Generators%Lux%Translator.c.o: /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/Lux/Translator.c /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/Lux/Translator.h /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/Lux/SyntaxAnalyzer/Constructs.h Tokenizer/Tokens.h Preprocessor/Files.h Tokenizer/Operators.h /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/Lux/Tokenizer/Types.h 
	$(CC) $(CFLAGS) -c $< -o $@

.bin/%home%Edo%Desktop%Lynx-T%Lynx%Tools%Build%Generators%Lux%Preprocessor%Preprocessor.c.o: /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/Lux/Preprocessor/Preprocessor.c Utils.h Preprocessor/Files.h Preprocessor/Preprocessor.h 
	$(CC) $(CFLAGS) -c $< -o $@

.bin/%home%Edo%Desktop%Lynx-T%Lynx%Tools%Build%Generators%Lux%Preprocessor%Files.c.o: /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/Lux/Preprocessor/Files.c /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/Lux/Preprocessor/Files.h 
	$(CC) $(CFLAGS) -c $< -o $@

.bin/%home%Edo%Desktop%Lynx-T%Lynx%Tools%Build%Generators%Lux%Tokenizer%Tokenizer.c.o: /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/Lux/Tokenizer/Tokenizer.c Utils.h Preprocessor/Files.h Tokenizer/Tokenizer.h Tokenizer/Tokens.h Tokenizer/Operators.h Tokenizer/Types.h Tokenizer/Keywords.h Tokenizer/Literals.h 
	$(CC) $(CFLAGS) -c $< -o $@

.bin/%home%Edo%Desktop%Lynx-T%Lynx%Tools%Build%Generators%Lux%Lux.c.o: /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/Lux/Lux.c /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/Lux/Utils.h /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/Lux/Preprocessor/Files.h /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/Lux/Tokenizer/Operators.h Tokenizer/Tokens.h /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/Lux/Tokenizer/Types.h /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/Lux/Tokenizer/Keywords.h /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/Lux/Tokenizer/Literals.h /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/Lux/Preprocessor/Preprocessor.h /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/Lux/Tokenizer/Tokenizer.h /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/Lux/SyntaxAnalyzer/SyntaxAnalyzer.h SyntaxAnalyzer/Constructs.h /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/Lux/Translator.h 
	$(CC) $(CFLAGS) -c $< -o $@

.bin/%home%Edo%Desktop%Lynx-T%Lynx%Tools%Build%Generators%Lux%Utils.c.o: /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/Lux/Utils.c /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/Lux/Utils.h /home/Edo/Desktop/Lynx-T/Lynx/Tools/Build/Generators/Lux/Preprocessor/Files.h 
	$(CC) $(CFLAGS) -c $< -o $@

../GenGlsl: .bin/%home%Edo%Desktop%Lynx-T%Lynx%Tools%Build%Generators%Lux%SyntaxAnalyzer%Constructs.c.o .bin/%home%Edo%Desktop%Lynx-T%Lynx%Tools%Build%Generators%Lux%SyntaxAnalyzer%SyntaxAnalyzer.c.o .bin/%home%Edo%Desktop%Lynx-T%Lynx%Tools%Build%Generators%Lux%Translator.c.o .bin/%home%Edo%Desktop%Lynx-T%Lynx%Tools%Build%Generators%Lux%Preprocessor%Preprocessor.c.o .bin/%home%Edo%Desktop%Lynx-T%Lynx%Tools%Build%Generators%Lux%Preprocessor%Files.c.o .bin/%home%Edo%Desktop%Lynx-T%Lynx%Tools%Build%Generators%Lux%Tokenizer%Tokenizer.c.o .bin/%home%Edo%Desktop%Lynx-T%Lynx%Tools%Build%Generators%Lux%Lux.c.o .bin/%home%Edo%Desktop%Lynx-T%Lynx%Tools%Build%Generators%Lux%Utils.c.o 
	$(CC) $(CFLAGS) $^ -o $@

clean:
	@-rm .bin/*.o
	@-rm ../GenGlsl
