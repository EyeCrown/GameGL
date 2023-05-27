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
    std::cout << "  Start loading shader" << std::endl;
    ResourceManager::LoadShader(vertexShaderFile, fragmentShaderFile, nullptr, "sprite");
    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), 
        static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").use();
    ResourceManager::GetShader("sprite").setMat4("projection", projection);
    // set render-specific controls
    Shader shader = ResourceManager::GetShader("sprite");
    Renderer = new SpriteRenderer(shader);
    std::cout << "  End loading shader" << std::endl;

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
    Level = 0;

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
    // update objects
    Ball->Move(dt, Width);
    // check for collisions
    DoCollisions();
    if (Ball->Position.y >= Height) // did ball reach bottom edge?
    {
        ResetLevel();
        ResetPlayer();
    }
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

void Game::ResetLevel()
{
    if (this->Level == 0)
        this->Levels[0].Load("levels/one.lvl", this->Width, this->Height / 2);
    else if (this->Level == 1)
        this->Levels[1].Load("levels/two.lvl", this->Width, this->Height / 2);
    else if (this->Level == 2)
        this->Levels[2].Load("levels/three.lvl", this->Width, this->Height / 2);
    else if (this->Level == 3)
        this->Levels[3].Load("levels/four.lvl", this->Width, this->Height / 2);
}

void Game::ResetPlayer()
{
    // reset player/ball stats
    Player->Size = PLAYER_SIZE;
    Player->Position = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
    Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -(BALL_RADIUS * 2.0f)), INITIAL_BALL_VELOCITY);
    Player->Color = glm::vec3(1.0f);
    Ball->Color = glm::vec3(1.0f);
}

bool CheckCollision(GameObject &one, GameObject &two); // AABB - AABB
Collision CheckCollision(BallObject &one, GameObject &two); // AABB - Circle
Direction VectorDirection(glm::vec2 target);

void Game::DoCollisions()
{
    for (GameObject &box : Levels[Level].Bricks)
    {
        if (!box.Destroyed)
        {
            Collision collision = CheckCollision(*Ball, box);
            if (std::get<0>(collision)) // if collision is true
            {
                // destroy block if not solid
                if (!box.IsSolid)
                    box.Destroyed = true;
                // collision resolution
                Direction dir = std::get<1>(collision);
                glm::vec2 diff_vector = std::get<2>(collision);
                if (dir == LEFT || dir == RIGHT) // horizontal collision
                {
                    Ball->Velocity.x = -Ball->Velocity.x; // reverse
                    // relocate
                    float penetration = Ball->Radius -
                    std::abs(diff_vector.x);
                    if (dir == LEFT)
                        Ball->Position.x += penetration; // move right
                    else
                        Ball->Position.x -= penetration; // move left;
                }
                else // vertical collision
                {
                    Ball->Velocity.y = -Ball->Velocity.y; // reverse
                    // relocate
                    float penetration = Ball->Radius -
                    std::abs(diff_vector.y);
                    if (dir == UP)
                        Ball->Position.y -= penetration; // move up
                    else
                        Ball->Position.y += penetration; // move down
                }
            }
        }
    }

    Collision result = CheckCollision(*Ball, *Player);
    if (!Ball->Stuck && std::get<0>(result))
    {
        // check where it hit the board, and change velocity
        float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
        float distance = (Ball->Position.x + Ball->Radius) - centerBoard;
        float percentage = distance / (Player->Size.x / 2.0f);
        // then move accordingly
        float strength = 2.0f;
        glm::vec2 oldVelocity = Ball->Velocity;
        Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
        //Ball->Velocity.y = -Ball->Velocity.y;
        Ball->Velocity.y = -1.0f * abs(Ball->Velocity.y);
        Ball->Velocity = glm::normalize(Ball->Velocity) *
        glm::length(oldVelocity);
    }
}

bool CheckCollision(GameObject &one, GameObject &two)
{
    // collision x-axis?
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
                        two.Position.x + two.Size.x >= one.Position.x;

    // collision y-axis?
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
                        two.Position.y + two.Size.y >= one.Position.y;

    // collision only if on both axes
    return collisionX && collisionY;
}

Collision CheckCollision(BallObject &one, GameObject &two)
{
    // get center point circle firt
    glm::vec2 center(one.Position + one.Radius);

    // calculate AABB info (center, half-extents)
    glm::vec2 aabb_half_extents(two.Size.x / 2.0f, two.Size.y / 2.0f);
    glm::vec2 aabb_center(two.Position.x + aabb_half_extents.x, two.Position.y + aabb_half_extents.y);

    // get difference vector between centers
    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped =  glm::clamp(difference, -aabb_half_extents, aabb_half_extents);

    // add clamped value to AABB_center and get the value closet to circle
    glm::vec2 closest = aabb_center + clamped;

    // vector between center circle and closest point AABB
    difference = closest - center;

    if (glm::length(difference) <= one.Radius)
        return std::make_tuple(true, VectorDirection(difference), difference);
    else
        return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}

// calculates which direction a vector is facing (N,E,S or W)
Direction VectorDirection(glm::vec2 target)
{
    glm::vec2 compass[] = {
        glm::vec2(0.0f, 1.0f),	// up
        glm::vec2(1.0f, 0.0f),	// right
        glm::vec2(0.0f, -1.0f),	// down
        glm::vec2(-1.0f, 0.0f)	// left
    };
    float max = 0.0f;
    unsigned int best_match = -1;
    for (unsigned int i = 0; i < 4; i++)
    {
        float dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}