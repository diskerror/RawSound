# Name of application.
APP = rawsound

SRCS=$(wildcard *.cpp)
HDRS=$(wildcard *.h)

# Detect Operating System
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
	# macOS Configuration (MacPorts)
	# Compiler (not clang, alias to gcc-mp-15)
	CC = g++ -std=c++17 -Wall -Wextra -Winvalid-pch -Wno-macro-redefined -O1

	# Boost version
	BV = 1.88

	CXXFLAGS = -I/opt/local/libexec/gcc15/libc++/include \
		-I/opt/local/libexec/boost/$(BV)/include \
		-I/opt/local/include

	LDFLAGS = -lz -L/opt/local/libexec/boost/$(BV)/lib -L/opt/local/lib -lssl -lcrypto
else
	# Debian 13 / Linux Configuration
	# Standard GCC
	CC = g++ -std=c++17 -Wall -Wextra -O1

	# Standard include paths (usually /usr/include)
	CXXFLAGS = 

	# Link against zlib and pthread (often needed for Boost/Crow on Linux)
	LDFLAGS = -lz -lpthread -lssl -lcrypto
endif

.PHONY: all test clean

all: $(APP)

#pre: cleanpre precompile.pch

# Create Precompiled Header
#precompile.pch: precompile.hh
#	$(CC) $(CXXFLAGS) -x c++-header $< -o $@

$(APP): $(SRCS) $(HDRS) Makefile
	$(CC) $(CXXFLAGS) $(LDFLAGS) $(SRCS) -o $@

clean:
	@rm -f *.o $(APP)

test: $(APP)
	./$(APP)