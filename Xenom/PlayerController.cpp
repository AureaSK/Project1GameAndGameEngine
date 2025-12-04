#include "pch.h"
#include "PlayerController.h"
#include "SpaceshipPawn.h"
#include "CInputManager.h"

PlayerController::PlayerController(CWorld* world) : CController(world){}

PlayerController::~PlayerController()
{
}
void PlayerController::BeginPlay()
{
    CController::BeginPlay();
}

void PlayerController::Tick(float deltaTime)
{
    CController::Tick(deltaTime);

    // Only process input if we have a pawn to control
    if (!HasPawn())
    {
        return;
    }

    SpaceshipPawn* spaceship = dynamic_cast<SpaceshipPawn*>(possessedPawn);
    if (!spaceship)
    {
        return;
    }

    CInputManager* input = CInputManager::GetInstance();

    // Handle movement input
    Vector2 moveDirection(0.0f, 0.0f);

    // Keyboard input
    if (input->IsKeyDown(SDLK_LEFT) || input->IsKeyDown(SDLK_A))
    {
        moveDirection.x = -1.0f;
    }
    else if (input->IsKeyDown(SDLK_RIGHT) || input->IsKeyDown(SDLK_D))
    {
        moveDirection.x = 1.0f;
    }

    if (input->IsKeyDown(SDLK_UP) || input->IsKeyDown(SDLK_W))
    {
        moveDirection.y = -1.0f;
    }
    else if (input->IsKeyDown(SDLK_DOWN) || input->IsKeyDown(SDLK_S))
    {
        moveDirection.y = 1.0f;
    }

    // Gamepad input (overrides keyboard)
    float axisX = input->GetAxis(SDL_GAMEPAD_AXIS_LEFTX);
    float axisY = input->GetAxis(SDL_GAMEPAD_AXIS_LEFTY);

    if (std::abs(axisX) > 0.2f)
    {
        moveDirection.x = axisX;
    }

    if (std::abs(axisY) > 0.2f)
    {
        moveDirection.y = axisY;
    }

    // Tell the pawn to move
    spaceship->MoveInDirection(moveDirection);

    // Handle fire input
    if (input->IsKeyPressed(SDLK_SPACE) || input->IsButtonPressed(SDL_GAMEPAD_BUTTON_SOUTH))
    {
        spaceship->FireWeapon();
    }
}