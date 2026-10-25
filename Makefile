default_target: main

main: main.cpp
	g++ main.cpp -o anki_converter -std=c++20 -Ithird_party -lcurl 

main_debug: main.cpp 
	g++ main.cpp -o anki_converter_debug -std=c++20 -g -O0 -Wall -Wextra -lcurl

clean: 
	rm -rf anki_converter anki_converter_debug
