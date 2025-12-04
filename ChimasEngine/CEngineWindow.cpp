#include "pch.h"
#include <SDL3/SDL.h>
#include "CEngineWindow.h"
#include <iostream>
#include <string>
#include <memory>



CEngineWindow::CEngineWindow()
{
	window = nullptr , width = 0.f, height = 0.f, renderTarget = nullptr;;
}

CEngineWindow::~CEngineWindow()
{
	DestroyWindow();
}

bool CEngineWindow::CreateWindow(const char* title, int XWindowSize, int YWindowSize)
{
	SDL_Texture* currentImage = nullptr;
	SDL_Texture* background = nullptr;
	window = SDL_CreateWindow(title, XWindowSize, YWindowSize, SDL_WINDOW_OPENGL);
	return true;
}

void CEngineWindow::DestroyWindow()
{
	if (window)
	{
		SDL_DestroyWindow(window);
		window = nullptr;
	}
}

