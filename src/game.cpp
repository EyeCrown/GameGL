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

void Game::Init(Texture2D texture)
{
    const char *vertexShaderFile = "./src/shaders/sprite.vert";
    const char *fragmentShaderFile = "./src/shaders/sprite.frag";
    const char *textureFile = "./resources/textures/awesomeface.png";

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
    ResourceManager::LoadTexture(textureFile, true, "face");
}

void Game::ProcessInput(float dt)
{

}

void Game::Update(float dt)
{

}

void Game::Render(Texture2D texture)
{
    glm::vec2 position = glm::vec2(200.0f, 200.0f);
    glm::vec2 size = glm::vec2(300.0f, 400.0f);
    float rotate = 45.0f;
    glm::vec3 color = glm::vec3(0.0f, 1.0f, 0.0f);
    Renderer->DrawSprite(ResourceManager::GetTexture("face"), position, size, rotate, color);
}