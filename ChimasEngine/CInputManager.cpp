#include "pch.h"
#include "CInputManager.h"

CInputManager* CInputManager::instance = nullptr;

CInputManager::CInputManager()
    : keyboardState(nullptr), numKeys(0), gamepad(nullptr)
{
}

CInputManager::~CInputManager()
{
    Cleanup();
}

CInputManager* CInputManager::GetInstance()
{
    if (!instance)
    {
        instance = new CInputManager();
    }
    return instance;
}

void CInputManager::Initialize()
{
    keyboardState = SDL_GetKeyboardState(&numKeys);

    // Try to open first gamepad
    int numJoysticks = 0;
    SDL_JoystickID* joysticks = SDL_GetJoysticks(&numJoysticks);

    if (numJoysticks > 0)
    {
        gamepad = SDL_OpenGamepad(joysticks[0]);
        if (gamepad)
        {
            SDL_Log("Gamepad connected: %s", SDL_GetGamepadName(gamepad));
        }
    }

    SDL_free(joysticks);
    SDL_Log("Input Manager initialized");
}

void CInputManager::Update()
{
    // Clear pressed/released states from previous frame
    keyPressed.clear();
    keyReleased.clear();
    buttonPressed.clear();
    buttonReleased.clear();
}

void CInputManager::ProcessEvent(const SDL_Event& event)
{
    if (event.type == SDL_EVENT_KEY_DOWN)
    {
        keyPressed[event.key.key] = true;
    }
    else if (event.type == SDL_EVENT_KEY_UP)
    {
        keyReleased[event.key.key] = true;
    }
    else if (event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN)
    {
        buttonPressed[static_cast<SDL_GamepadButton>(event.gbutton.button)] = true;
    }
    else if (event.type == SDL_EVENT_GAMEPAD_BUTTON_UP)
    {
        buttonReleased[static_cast<SDL_GamepadButton>(event.gbutton.button)] = true;
    }
    else if (event.type == SDL_EVENT_GAMEPAD_ADDED)
    {
        if (!gamepad)
        {
            gamepad = SDL_OpenGamepad(event.gdevice.which);
            if (gamepad)
            {
                SDL_Log("Gamepad connected: %s", SDL_GetGamepadName(gamepad));
            }
        }
    }
    else if (event.type == SDL_EVENT_GAMEPAD_REMOVED)
    {
        if (gamepad && event.gdevice.which == SDL_GetGamepadID(gamepad))
        {
            SDL_CloseGamepad(gamepad);
            gamepad = nullptr;
            SDL_Log("Gamepad disconnected");
        }
    }
}

void CInputManager::Cleanup()
{
    if (gamepad)
    {
        SDL_CloseGamepad(gamepad);
        gamepad = nullptr;
    }
}

bool CInputManager::IsKeyDown(SDL_Keycode key) const
{
    if (!keyboardState) return false;
    SDL_Scancode scancode = SDL_GetScancodeFromKey(key, nullptr);
    return keyboardState[scancode];
}

bool CInputManager::IsKeyPressed(SDL_Keycode key) const
{
    auto it = keyPressed.find(key);
    return it != keyPressed.end() && it->second;
}

bool CInputManager::IsKeyReleased(SDL_Keycode key) const
{
    auto it = keyReleased.find(key);
    return it != keyReleased.end() && it->second;
}

bool CInputManager::IsButtonDown(SDL_GamepadButton button) const
{
    if (!gamepad) return false;
    return SDL_GetGamepadButton(gamepad, button);
}

bool CInputManager::IsButtonPressed(SDL_GamepadButton button) const
{
    auto it = buttonPressed.find(button);
    return it != buttonPressed.end() && it->second;
}

bool CInputManager::IsButtonReleased(SDL_GamepadButton button) const
{
    auto it = buttonReleased.find(button);
    return it != buttonReleased.end() && it->second;
}

float CInputManager::GetAxis(SDL_GamepadAxis axis) const
{
    if (!gamepad) return 0.0f;
    Sint16 value = SDL_GetGamepadAxis(gamepad, axis);
    return value / 32767.0f; // Normalize to -1.0 to 1.0
}