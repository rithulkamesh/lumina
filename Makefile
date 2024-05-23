# Compiler and compiler flags
CXX = g++-13
CXXFLAGS = -Wall -Wextra -g -std=c++17 -fpic -Iinclude -Ilumina -I/opt/homebrew/include -I/usr/local/include

# Determine the operating system
UNAME_S := $(shell uname -s)

# Set platform-specific linker flags
ifeq ($(UNAME_S),Linux)
	LDFLAGS = -lglfw3 -lGL -lm -lpthread -lXrandr -lXinerama -lXi -lXxf86vm -lX11 -ldl
else ifeq ($(UNAME_S),Darwin)
	LDFLAGS = -lglfw3 -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
else
	$(error Unsupported operating system: $(UNAME_S))
endif

SRCDIR = src
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp, build/%.o, $(SOURCES))

ENGINE_SRCDIR = lumina
ENGINE_SOURCES = $(wildcard $(ENGINE_SRCDIR)/*.cpp)
ENGINE_OBJECTS = $(patsubst $(ENGINE_SRCDIR)/%.cpp, build/%.o, $(ENGINE_SOURCES))

TARGET = build/game
LIB_TARGET = build/liblumina.so

all: $(TARGET)

$(TARGET): $(OBJECTS) $(LIB_TARGET)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS) -Lbuild -llumina

$(LIB_TARGET): $(ENGINE_OBJECTS)
	$(CXX) -shared $(ENGINE_OBJECTS) -o $@ $(LDFLAGS)

build/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/%.o: $(ENGINE_SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf build

.PHONY: all clean
