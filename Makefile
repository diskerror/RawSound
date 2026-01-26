# Name of application.
APP = rawsound

# Detect Operating System
UNAME_S := $(shell uname -s)

# Boost version
BOOSTV = 1.81

ifeq ($(UNAME_S),Darwin)
	# macOS Configuration (MacPorts)
	# Compiler (not clang, alias to gcc-mp-15)
	CC = g++ -std=c++17 -Wall -Wextra -Winvalid-pch -Wno-macro-redefined

	CXXFLAGS = -I/opt/local/libexec/gcc15/libc++/include \
		-I/opt/local/libexec/boost/$(BOOSTV)/include \
		-I/opt/local/include

	LDFLAGS = -lz -lpthread -L/opt/local/libexec/boost/$(BOOSTV)/lib -L/opt/local/lib -lssl -lcrypto
else
	# Debian 13 / Linux Configuration
	# Standard GCC
	CC = g++ -std=c++17 -Wall -Wextra

	# Standard include paths (usually /usr/include)
	CXXFLAGS = 

	# Link against zlib and pthread (often needed for Boost/Crow on Linux)
	LDFLAGS = -lz -lpthread -lssl -lcrypto
endif

SRCS=$(wildcard *.cpp)
HDRS=$(wildcard *.h)

.PHONY: all test clean pre

all: $(APP)

# Create Precompiled Header
precompile.pch: precompile.hh
	@rm -f $@
	$(CC) -O3 $(CXXFLAGS) -x c++-header $< -o $@

$(APP): $(SRCS) $(HDRS) Makefile precompile.pch
	@rm -f $(APP)
	$(CC) -O1 $(CXXFLAGS) $(LDFLAGS) $(SRCS) -o $@

pre:
	precompile.pch

clean:
	@rm -f *.o $(APP)

test: $(APP)
	./$(APP)