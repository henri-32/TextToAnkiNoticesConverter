compile = g++ -std=c++20 -c -Ithird_party
.DEFAULT_GOAL:= anki_converter

anki_converter: build/main.o build/args.o
	@g++ $^ -lcurl -o anki_converter

build/main.o: main.cpp 
	@$(compile) $< -o build/main.o

args.o: third_party/args.cpp 
	@$(compile) $< -o build/args.o

main_debug: main.cpp 
	@g++ main.cpp third_party/args.cpp -o anki_converter_debug -std=c++20 -g -O0 -Wall -Wextra -lcurl

clean: 
	rm -rf anki_converter anki_converter_debug
