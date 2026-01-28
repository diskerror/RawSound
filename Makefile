# Name of application.
APP = rawsound

# Detect Operating System
UNAME_S := $(shell uname -s)

# Language standard
STD = c++20

# Boost version
BOOSTV = 1.88

# Precompiled header
PCH_SRC = precompile.h
PCH_OUT = precompile.gch

ifeq ($(UNAME_S),Darwin)
	# macOS Configuration (MacPorts)
	CXX = g++-mp-15
	CXXFLAGS = -std=$(STD) -Wall -Wextra -Winvalid-pch -Wno-macro-redefined \
		-I/opt/local/libexec/gcc15/libc++/include \
		-I/opt/local/libexec/boost/$(BOOSTV)/include \
		-I/opt/local/include

	LDFLAGS = -L/opt/local/libexec/boost/$(BOOSTV)/lib \
		-L/opt/local/lib \
		-lz -lpthread -lssl -lcrypto
else
	# Debian 13 / Linux Configuration
	CXX = g++
	CXXFLAGS = -std=$(STD) -Wall -Wextra -Winvalid-pch \
		-I/usr/include

	LDFLAGS = -L/usr/lib \
		-lz -lpthread -lssl -lcrypto -ldl -lboost_system
endif

SRCS = $(wildcard *.cp)
HDRS = $(wildcard *.hh *.h)

.PHONY: all clean pre test debug release

all: release

debug: OPTFLAGS = -O0 -g
debug: $(APP)

release: OPTFLAGS = -O3 -DNDEBUG
release: $(APP)

# Create Precompiled Header
$(PCH_OUT): $(PCH_SRC) $(HDRS)
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -x c++-header $< -o $@

# Build application using precompiled header
$(APP): $(SRCS) $(HDRS) $(PCH_OUT) Makefile
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -include $(PCH_SRC) $(SRCS) $(LDFLAGS) -o $@

# Build just the precompiled header (uses release flags)
pre: OPTFLAGS = -O3 -DNDEBUG
pre: $(PCH_OUT)
	@echo "Precompiled header built: $(PCH_OUT)"

clean:
	rm -f $(APP) $(PCH_OUT) *.o

test: $(APP)
	./$(APP)
