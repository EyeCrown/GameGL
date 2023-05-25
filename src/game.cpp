#include <algorithm>
#include <sstream>
#include <iostream>

#include "game.h"
#include "sprite_renderer.h"
#include "shader.h"
#include "resource_manager.h"

// Game-related State data
SpriteRenderer          *Renderer;

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

    ResourceManager::LoadTexture(faceFile, true, "face");
    ResourceManager::LoadTexture(blockFile, false, "block");
    ResourceManager::LoadTexture(blockSolidFile, false, "block_solid");
    ResourceManager::LoadTexture(bgFile, false, "background");

    // load levels
    GameLevel lvl1; lvl1.Load("./levels/1.lvl", Width, Height / 2);
    GameLevel lvl2; lvl2.Load("./levels/2.lvl", Width, Height / 2);
    GameLevel lvl3; lvl3.Load("./levels/3.lvl", Width, Height / 2);
    GameLevel lvl4; lvl4.Load("./levels/4.lvl", Width, Height / 2);

    Levels.push_back(lvl1);
    Levels.push_back(lvl2);
    Levels.push_back(lvl3);
    Levels.push_back(lvl4);
    Level = 0;
}

void Game::ProcessInput(float dt)
{

}

void Game::Update(float dt)
{

}

void Game::Render()
{
    /*
    glm::vec2 position = glm::vec2(200.0f, 200.0f);
    glm::vec2 size = glm::vec2(300.0f, 400.0f);
    float rotate = 45.0f;
    glm::vec3 color = glm::vec3(0.0f, 1.0f, 0.0f);
    Renderer->DrawSprite(ResourceManager::GetTexture("face"), position, size, rotate, color);
    */
   if (State == GAME_ACTIVE)
   {
        // draw background
        Renderer->DrawSprite(
            ResourceManager::GetTexture("bacckground"), 
            glm::vec2(0.0f, 0.0f), 
            glm::vec2(Width, Height), 
            0.0f
        );

        // draw level
        Levels[Level].Draw(*Renderer);
   }
}