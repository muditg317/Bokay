# This file was inspired by:
#   my CS 2110 homework
#   https://stackoverflow.com/questions/53136024/makefile-to-compile-all-c-files-without-needing-to-specify-them/53138757

MKDIR := mkdir -p
RM    := rm -f
RMDIR := $(RM) -r
CC    := g++
BIN   := .
OBJ   := ./obj
SRC   := ./src
INC   := ./inc

# Flag info credit: https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
# CFLAGS = -std=c++98 # Using the C++98 standard
CFLAGS += -Wall # This enables all the warnings about constructions that some users consider questionable, and that are easy to avoid (or modify to prevent the warning), even in conjunction with macros
CFLAGS += -pedantic # Issue all the warnings demanded by strict ISO C and ISO C++; reject all programs that use forbidden extensions, and some other programs that do not follow ISO C and ISO C++
CFLAGS += -Wextra # This enables some extra warning flags that are not enabled by -Wall
CFLAGS += -Werror # Make all warnings into errors
CFLAGS += -O0 # Optimize even more. GCC performs nearly all supported optimizations that do not involve a space-speed tradeoff.
CFLAGS += -g # Generate debugging information

CFLAGS += -I$(INC) # Add header files to include path for c compilation

LDFLAGS +=

# LDLIBS += -lm

# Source files to be compiled
CFILES  := $(wildcard $(SRC)/*.cpp)
HEADERS := $(patsubst $(SRC)/%.cpp,$(INC)/%.hpp,$(CFILES))
# HEADERS := $(wildcard $(INC)/%.hpp) $(wildcard $(INC)/%.h)
OBJECTS := $(patsubst $(SRC)/%.cpp,$(OBJ)/%.o,$(CFILES))

# Executable name
EXECUTABLE := bokay

.PHONY: all run clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) | $(BIN)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

$(OBJ)/%.o: $(SRC)/%.cpp | $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN) $(OBJ):
	$(MKDIR) $@

run: $(EXECUTABLE)
	$<

clean:
	$(RM) $(EXECUTABLE)
	$(RMDIR) $(OBJ) $(filter-out .,$(BIN))
