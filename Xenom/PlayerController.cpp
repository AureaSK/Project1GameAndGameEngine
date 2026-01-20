#include "pch.h"
#include "PlayerController.h"
#include "SpaceshipPawn.h"
#include "CInputManager.h"
#include "ChimasInput.h"

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
    if (input->IsKeyDown(Key::Left) || input->IsKeyDown(Key::A))
    {
        moveDirection.x = -1.0f;
    }
    else if (input->IsKeyDown(Key::Right) || input->IsKeyDown(Key::D))
    {
        moveDirection.x = 1.0f;
    }

    if (input->IsKeyDown(Key::Up) || input->IsKeyDown(Key::W))
    {
        moveDirection.y = -1.0f;
    }
    else if (input->IsKeyDown(Key::Down) || input->IsKeyDown(Key::S))
    {
        moveDirection.y = 1.0f;
    }

    // Gamepad input (overrides keyboard)
    float axisX = input->GetAxis(GamepadAxis::LeftX);
    float axisY = input->GetAxis(GamepadAxis::LeftY);

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
    if (input->IsKeyPressed(Key::Space) || input->IsButtonPressed(GamepadButton::South))
    {
        spaceship->FireWeapon();
    }
}