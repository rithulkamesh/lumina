# Compiler and compiler flags
CXX = g++-14
CXXFLAGS = -Wall -Wextra -g -std=c++17 -I/usr/local/include -I/opt/homebrew/include -fpic -Iinclude -Ilumina -L/usr/local/lib
# Determine the operating system
UNAME_S := $(shell uname -s)

# Set platform-specific linker flags
ifeq ($(UNAME_S),Linux)
	LDFLAGS = -lglfw3 -lvulkan -lGL -lm -lpthread -lXrandr -lXinerama -lXi -lXxf86vm -lX11 -ldl
else ifeq ($(UNAME_S),Darwin)
	LDFLAGS = -L/usr/local/lib -lglfw3 -lvulkan -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
else
	$(error Unsupported operating system: $(UNAME_S))
endif

SRCDIR = src
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp, build/%.o, $(SOURCES))


ENGINE_SRCDIR = lumina
ENGINE_SOURCES = $(wildcard $(ENGINE_SRCDIR)/*.cpp) $(wildcard $(ENGINE_SRCDIR)/util/*.cpp) $(wildcard $(ENGINE_SRCDIR)/engine/*.cpp)
ENGINE_OBJECTS = $(patsubst $(ENGINE_SRCDIR)/%.cpp, build/%.o, $(ENGINE_SOURCES))

TARGET = build/game
LIB_TARGET = build/liblumina.so

all: $(TARGET)

run: $(TARGET)
	@ ./build/game

$(TARGET): $(OBJECTS) $(LIB_TARGET)
	@$(CXX) $(OBJECTS) -o $@ $(LDFLAGS) -Lbuild -llumina

$(LIB_TARGET): $(ENGINE_OBJECTS)
	@$(CXX) -shared $(ENGINE_OBJECTS) -o $@ $(LDFLAGS)

build/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

build/%.o: $(ENGINE_SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf build

.PHONY: all clean
