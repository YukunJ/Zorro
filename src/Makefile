#   Compiler, g++ for C++ program
CXX := g++

#   We provide the following Flags
#     optimization level 0
#     debugger
#     turn all optional warnings
#     turn all warnings into errors
#     set standard c++ to c11
CFLAGS := -O3 -Wall -Werror -std=c++17 -lpthread
 
#   for simplicity, wrap all files
SOURCES := *.cpp
HEADERS := *.h
TARGET  := run_pool

# default target when we only type "make"
all: $(TARGET)
	@echo "Building run_pool"

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) -o $@ $(CFLAGS) $(SOURCES)

# format command
.PHONY: format
format:
	# clang-format all the source code files
	clang-format -style=google -i $(SOURCES) $(HEADERS)

# clean up command
.PHONY: clean
clean:
	# remove all the files with extension .o or .s or .so and executable with tar
	rm -f $(TARGET) *.o *.s *.so

