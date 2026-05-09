CXX = g++
WIN_CXX = x86_64-w64-mingw32-g++

# =======================
# Compiler flags
# =======================
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude

WINDOWS_CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude 

# =======================
# Targets
# =======================
TARGET = game
WIN_TARGET = publish/windows/game.exe

# =======================
# Source files
# =======================
SRC = \
src/stbImpl.cpp \
src/main.cpp \
src/Window.cpp \
src/FileLoader.cpp \
src/Mesh.cpp \
src/Camera.cpp \
src/Player.cpp \
src/UIElement.cpp \
src/TextElement.cpp \
src/Font.cpp \
src/Light.cpp \
src/glad.c

PUBLISH_SRC = textures \
							fonts \
							models \
							shaders

WINDOWS_DLLS = /usr/x86_64-w64-mingw32/bin/glfw3.dll \
							 /usr/x86_64-w64-mingw32/bin/libassimp.dll \
							 /usr/x86_64-w64-mingw32/bin/libgcc_s_seh-1.dll \
							 /usr/x86_64-w64-mingw32/bin/libssp-0.dll \
							 /usr/x86_64-w64-mingw32/bin/libstdc++-6.dll \
							 /usr/x86_64-w64-mingw32/bin/zlib1.dll \
							 /usr/x86_64-w64-mingw32/bin/libwinpthread-1.dll

# =======================
# Object files (Linux build)
# =======================
OBJ_DIR = build

OBJ = $(SRC:src/%.cpp=$(OBJ_DIR)/src/%.o)
OBJ := $(OBJ:src/glad.c=$(OBJ_DIR)/src/glad.o)

# =======================
# Linux libraries
# =======================
LIBS_LINUX = \
-lassimp \
-lglfw \
-lGL \
-ldl \
-lpthread \
-lX11 \
-lXrandr \
-lXi \
-lXxf86vm \
-lXcursor

# =======================
# Windows libraries
# =======================
LIBS_WINDOWS = \
-L/usr/x86_64-w64-mingw32/lib \
-lassimp \
-lglfw3 \
-lopengl32 \
-lgdi32 \
-lwinmm

# =======================
# Default Linux build
# =======================
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) $(LIBS_LINUX)	

# =======================
# Linux object compilation
# =======================
$(OBJ_DIR)/src/%.o: src/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/src/glad.o: src/glad.c
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# =======================
# Windows build
# =======================
windows:
	mkdir -p build
	mkdir -p publish/windows

	$(WIN_CXX) \
	$(WINDOWS_CXXFLAGS) \
	$(SRC) \
	-o $(WIN_TARGET) \
	$(LIBS_WINDOWS)

# =======================
# Clean
# =======================
clean:
	rm -rf $(OBJ_DIR)
	rm -f $(TARGET)
	rm -rf publish

publish:
	rm -rf publish
	mkdir -p publish/linux
	mkdir -p publish/windows

	cp -r $(PUBLISH_SRC) publish/linux
	$(MAKE)

	cp $(TARGET) publish/linux/$(TARGET)

	cp -r $(PUBLISH_SRC) publish/windows
	$(MAKE) windows
	cp $(WINDOWS_DLLS) publish/windows

	cd publish && zip -r -9 linux.zip linux && zip -r -9 windows.zip windows

# =======================
# Run Linux build
# =======================
test: all
	./$(TARGET)

.PHONY: all clean test windows publish
