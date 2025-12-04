#pragma once
#include "CEngine.h"
#include <SDL3/SDL.h>
#include <string>


class CEngineWindow : public CEngine
{
private:

    SDL_Renderer* renderTarget;
    SDL_Window* window;
    int width;
    int height;


public:
    CEngineWindow();
    ~CEngineWindow();


	bool CreateWindow(const char* title, int XWindowSize, int YWindowSize);
    void DestroyWindow();

    SDL_Window* GetSDLWindow() const { return window; }
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }

};

