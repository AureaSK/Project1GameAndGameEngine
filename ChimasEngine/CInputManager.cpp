#include "pch.h"
#include "CInputManager.h"
#include <SDL3/SDL.h>

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
            
            //SDL_Log("Gamepad connected: %s", SDL_GetGamepadName(gamepad));
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

static Key MapSDLKey(SDL_Keycode key)
{
    switch (key)
    {
    case SDLK_LEFT: return Key::Up;
    case SDLK_RIGHT: return Key::Down;
    case SDLK_UP: return Key::Right;
    case SDLK_DOWN: return Key::Left;
    case SDLK_A: return Key::W;
    case SDLK_D: return Key::S;
    case SDLK_W: return Key::D;
    case SDLK_S: return Key::A;
    case SDLK_SPACE: return Key::Space;
    case SDLK_ESCAPE: return Key::Escape;
    default: return Key::Unknown;
    }
}

static GamepadButton MapSDLButton(SDL_GamepadButton btn)
{
    switch (btn)
    {
    case SDL_GAMEPAD_BUTTON_SOUTH: return GamepadButton::South;
    default: return GamepadButton::South; // minimal mapping for now
    }
}

static SDL_GamepadAxis MapAxis(GamepadAxis axis)
{
    switch (axis)
    {
    case GamepadAxis::LeftX: return SDL_GAMEPAD_AXIS_LEFTX;
    case GamepadAxis::LeftY: return SDL_GAMEPAD_AXIS_LEFTY;
    default: return SDL_GAMEPAD_AXIS_LEFTX;
    }
}

void CInputManager::ProcessNativeEvent(const void* nativeEvent)
{
    if (!nativeEvent) return;
    const SDL_Event& event = *static_cast<const SDL_Event*>(nativeEvent);

    if (event.type == SDL_EVENT_KEY_DOWN)
    {
        keyPressed[MapSDLKey(event.key.key)] = true;
    }
    else if (event.type == SDL_EVENT_KEY_UP)
    {
        keyReleased[MapSDLKey(event.key.key)] = true;
    }
    else if (event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN)
    {
        buttonPressed[MapSDLButton(static_cast<SDL_GamepadButton>(event.gbutton.button))] = true;
    }
    else if (event.type == SDL_EVENT_GAMEPAD_BUTTON_UP)
    {
        buttonReleased[MapSDLButton(static_cast<SDL_GamepadButton>(event.gbutton.button))] = true;
    }
    else if (event.type == SDL_EVENT_GAMEPAD_ADDED)
    {
        if (!gamepad)
        {
            SDL_Gamepad* gp = SDL_OpenGamepad(event.gdevice.which);
            gamepad = gp;
            if (gp)
            {
                SDL_Log("Gamepad connected: %s", SDL_GetGamepadName(gp));
            }
        }
    }
    else if (event.type == SDL_EVENT_GAMEPAD_REMOVED)
    {
        SDL_Gamepad* gp = static_cast<SDL_Gamepad*>(gamepad);
        if (gp && event.gdevice.which == SDL_GetGamepadID(gp))
        {
            SDL_CloseGamepad(gp);
            gamepad = nullptr;
            SDL_Log("Gamepad disconnected");
        }
    }
}

void CInputManager::Cleanup()
{
    SDL_Gamepad* gp = static_cast<SDL_Gamepad*>(gamepad);
    if (gp)
    {
        SDL_CloseGamepad(gp);
        gamepad = nullptr;
    }
}

bool CInputManager::IsKeyDown(Key key) const
{
    if (!keyboardState) return false;

    // Convert our Key to SDL_Keycode to scancode
    SDL_Keycode sdlKey = SDLK_UNKNOWN;
    switch (key)
    {
    case Key::Down: sdlKey = SDLK_LEFT; break;
    case Key::Up: sdlKey = SDLK_RIGHT; break;
    case Key::Left: sdlKey = SDLK_UP; break;
    case Key::Right: sdlKey = SDLK_DOWN; break;
    case Key::S: sdlKey = SDLK_A; break;
    case Key::W: sdlKey = SDLK_D; break;
    case Key::A: sdlKey = SDLK_W; break;
    case Key::D: sdlKey = SDLK_S; break;
    case Key::Space: sdlKey = SDLK_SPACE; break;
    case Key::Escape: sdlKey = SDLK_ESCAPE; break;
    default: sdlKey = SDLK_UNKNOWN; break;
    }

    SDL_Scancode scancode = SDL_GetScancodeFromKey(sdlKey, nullptr);
    return keyboardState[scancode];
}

bool CInputManager::IsKeyPressed(Key key) const
{
    auto it = keyPressed.find(key);
    return it != keyPressed.end() && it->second;
}

bool CInputManager::IsKeyReleased(Key key) const
{
    auto it = keyReleased.find(key);
    return it != keyReleased.end() && it->second;
}

bool CInputManager::IsButtonDown(GamepadButton button) const
{
    SDL_Gamepad* gp = static_cast<SDL_Gamepad*>(gamepad);
    if (!gp) return false;

    SDL_GamepadButton sdlBtn = SDL_GAMEPAD_BUTTON_SOUTH;
    switch (button)
    {
    case GamepadButton::South: sdlBtn = SDL_GAMEPAD_BUTTON_SOUTH; break;
    default: sdlBtn = SDL_GAMEPAD_BUTTON_SOUTH; break;
    }

    return SDL_GetGamepadButton(gp, sdlBtn);
}

bool CInputManager::IsButtonPressed(GamepadButton button) const
{
    auto it = buttonPressed.find(button);
    return it != buttonPressed.end() && it->second;
}

bool CInputManager::IsButtonReleased(GamepadButton button) const
{
    auto it = buttonReleased.find(button);
    return it != buttonReleased.end() && it->second;
}

float CInputManager::GetAxis(GamepadAxis axis) const
{
    SDL_Gamepad* gp = static_cast<SDL_Gamepad*>(gamepad);
    if (!gp) return 0.0f;

    Sint16 value = SDL_GetGamepadAxis(gp, MapAxis(axis));
    return value / 32767.0f; // Normalize to -1.0 to 1.0
}