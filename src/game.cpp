#include <algorithm>
#include <sstream>
#include <iostream>

#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "game_object.h"
#include "ball_object.h"

// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
// Initial velocity of the player paddle
const float PLAYER_VELOCITY(500.0f);
// Initial velocity of the Ball
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
// Radius of the ball object
const float BALL_RADIUS = 12.5f;

// Game-related State data
SpriteRenderer          *Renderer;
GameObject              *Player;
BallObject              *Ball;

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{
    this->Width = width;
    this->Height = height;
}

Game::~Game()
{
    delete Renderer;
}

void Game::Init()
{
    std::cout << "Starting Game Initialisation" << std::endl;
    const char *vertexShaderFile = "./src/shaders/sprite.vert";
    const char *fragmentShaderFile = "./src/shaders/sprite.frag";
    
    // build and compile our shader program
    // ------------------------------------
    ResourceManager::LoadShader(vertexShaderFile, fragmentShaderFile, nullptr, "sprite");
    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), 
        static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").use();
    ResourceManager::GetShader("sprite").setMat4("projection", projection);
    // set render-specific controls
    Shader shader = ResourceManager::GetShader("sprite");
    Renderer = new SpriteRenderer(shader);
    
    // load textures
    const char *faceFile = "./resources/textures/awesomeface.png";
    const char *blockFile = "./resources/textures/block.png";
    const char *blockSolidFile = "./resources/textures/block_solid.png";
    const char *bgFile = "./resources/textures/background.jpg";
    const char *playerFile = "./resources/textures/paddle.png";
        
    std::cout << "  Begin loading textures" << std::endl;
    ResourceManager::LoadTexture(faceFile, true, "face");               //std::cout << "1st loading textures" << std::endl;
    ResourceManager::LoadTexture(blockFile, false, "block");            //std::cout << "2nd loading textures" << std::endl;
    ResourceManager::LoadTexture(blockSolidFile, false, "block_solid"); //std::cout << "3rd loading textures" << std::endl;
    ResourceManager::LoadTexture(bgFile, false, "background");          //std::cout << "4st loading textures" << std::endl;
    ResourceManager::LoadTexture(playerFile, true, "paddle");           //std::cout << "5st loading textures" << std::endl;
    std::cout << "  End loading textures " << std::endl;
    
    // load player
    glm::vec2 playerPos = glm::vec2(Width / 2.0f - PLAYER_SIZE.x / 2.0f, Height - (PLAYER_SIZE.y * 2));
    Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

    // load ball
    glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, - BALL_RADIUS * 2.0f);
    Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));

    // load levels
    GameLevel lvl1; lvl1.Load("./levels/1.lvl", Width, Height / 2);
    GameLevel lvl2; lvl2.Load("./levels/2.lvl", Width, Height / 2);
    GameLevel lvl3; lvl3.Load("./levels/3.lvl", Width, Height / 2);
    GameLevel lvl4; lvl4.Load("./levels/4.lvl", Width, Height / 2);

    Levels.push_back(lvl1);
    Levels.push_back(lvl2);
    Levels.push_back(lvl3);
    Levels.push_back(lvl4);
    Level = 1;

    std::cout << "Finishing Game Initialisation" << std::endl;
}

void Game::ProcessInput(float dt)
{
    if (State == GAME_ACTIVE)
    {
        float velocity = PLAYER_VELOCITY * dt;
        // move playerboard
        if (this->Keys[GLFW_KEY_A])
        {
            if (Player->Position.x >= 0.0f)
            {
                Player->Position.x -= velocity;
                if (Ball->Stuck)
                    Ball->Position.x -= velocity;
            }
        }
        if (this->Keys[GLFW_KEY_D])
        {
            if (Player->Position.x <= Width - Player->Size.x)
            {
                Player->Position.x += velocity;
                if (Ball->Stuck)
                    Ball->Position.x += velocity;
            }
        }
        if (this->Keys[GLFW_KEY_SPACE])
        {
            Ball->Stuck = false;
        }
    }
}

void Game::Update(float dt)
{
    Ball->Move(dt, Width);
}

void Game::Render()
{    
    if (State == GAME_ACTIVE)
    {
        // draw background
        Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(Width, Height), 0.0f);

        // draw level
        Levels[Level].Draw(*Renderer);

        // draw player
        Player->Draw(*Renderer);

        // draw ball
        Ball->Draw(*Renderer);
    }
}