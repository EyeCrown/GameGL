CXX=g++
CXXFLAGS=-ldl -lglfw
OTHERFILES=./src/texture.cpp ./src/sprite_renderer.cpp ./src/game.cpp ./src/resource_manager.cpp ./src/game_object.cpp ./src/game_level.cpp ./src/ball_object.cpp
EXEC=window

window: 
	$(CXX) -std=c++17 -o ./target/window.out ./src/window.cpp $(OTHERFILES) thirdparty/glad.c $(CXXFLAGS)

run:
	./target/window.out