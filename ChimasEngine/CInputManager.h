#pragma once
#include "CEngine.h"
#include "ChimasInput.h"
#include <unordered_map>

class CInputManager : public CEngine
{
private:
    const bool* keyboardState;
    int numKeys;

    std::unordered_map<Key, bool> keyPressed;
    std::unordered_map<Key, bool> keyReleased;

    void* gamepad; // SDL_Gamepad* stored opaquely; defined/used in .cpp only
    std::unordered_map<GamepadButton, bool> buttonPressed;
    std::unordered_map<GamepadButton, bool> buttonReleased;

    static CInputManager* instance;

public:
    CInputManager();
    ~CInputManager();

    static CInputManager* GetInstance();

    void Initialize();
    void Update();
    void ProcessNativeEvent(const void* nativeEvent);
    void Cleanup();

    // Keyboard
    bool IsKeyDown(Key key) const;
    bool IsKeyPressed(Key key) const;
    bool IsKeyReleased(Key key) const;

    // Gamepad
    bool IsButtonDown(GamepadButton button) const;
    bool IsButtonPressed(GamepadButton button) const;
    bool IsButtonReleased(GamepadButton button) const;
    float GetAxis(GamepadAxis axis) const;
};
