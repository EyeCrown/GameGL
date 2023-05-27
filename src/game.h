#ifndef GAME_H
#define GAME_H
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "game_level.h"

// represents the current state of the game
enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

// Represents the four possible (collision) directions
enum Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
};
// Defines a Collision typedef that represents collision data
typedef std::tuple<bool, Direction, glm::vec2> Collision; // <collision?, what direction?, difference vector center - closest point>


class Game
{
	public:
	// game state
	GameState	State;
	bool 		Keys[1024];
	unsigned int Width, Height;
	std::vector<GameLevel> Levels;

	unsigned int Level;
	// constructor/destructor
	Game(unsigned int width, unsigned int height);
	~Game();
	// initialize game state (load all shaders/textures/levels)
	void Init();
	//game loop
	void ProcessInput(float dt);
	void Update(float dt);
	void Render();
	void DoCollisions();
	// reset
    void ResetLevel();
    void ResetPlayer();
};
#endif