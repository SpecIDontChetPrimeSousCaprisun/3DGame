CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude 

TARGET = game

SRC = src/main.cpp src/Window.cpp src/FileLoader.cpp src/Mesh.cpp src/Camera.cpp src/Player.cpp src/glad.c

OBJ_DIR = build

# convert src/xxx.cpp -> build/src/xxx.o
OBJ = $(SRC:src/%.cpp=$(OBJ_DIR)/src/%.o)

LIBS = -lassimp -lglfw -lGL -ldl -lpthread -lX11 -lXrandr -lXi -lXxf86vm -lXcursor

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) $(LIBS)

$(OBJ_DIR)/src/%.o: src/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

test:
	make
	./$(TARGET)

.PHONY: all clean
