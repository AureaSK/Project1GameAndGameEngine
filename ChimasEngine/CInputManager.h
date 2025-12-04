#pragma once
#include "CEngine.h"
#include <SDL3/SDL.h>
#include <unordered_map>

class CInputManager : public CEngine
{
private:
    const bool* keyboardState;
    int numKeys;
    std::unordered_map<SDL_Keycode, bool> keyPressed;
    std::unordered_map<SDL_Keycode, bool> keyReleased;

    SDL_Gamepad* gamepad;
    std::unordered_map<SDL_GamepadButton, bool> buttonPressed;
    std::unordered_map<SDL_GamepadButton, bool> buttonReleased;

    static CInputManager* instance;

public:
    CInputManager();
    ~CInputManager();

    static CInputManager* GetInstance();

    void Initialize();
    void Update();
    void ProcessEvent(const SDL_Event& event);
    void Cleanup();

    // Keyboard
    bool IsKeyDown(SDL_Keycode key) const;
    bool IsKeyPressed(SDL_Keycode key) const;
    bool IsKeyReleased(SDL_Keycode key) const;

    // Gamepad
    bool IsButtonDown(SDL_GamepadButton button) const;
    bool IsButtonPressed(SDL_GamepadButton button) const;
    bool IsButtonReleased(SDL_GamepadButton button) const;
    float GetAxis(SDL_GamepadAxis axis) const;
};
