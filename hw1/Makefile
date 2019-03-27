CXX = clang++ -O3 -std=c++17
COMPILE_FILES = hw1.cpp

all: $(COMPILE_FILES)
	$(CXX) -g -o main $^

.PHONY: clean
clean:
	rm -rf main
