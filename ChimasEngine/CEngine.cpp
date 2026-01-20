#include "pch.h"
#include "CEngine.h"
#include "CEngineWindow.h"
#include "CEngineRender.h"
#include "CEngineTexture.h"
#include "CWorld.h"
#include "CInputManager.h"
#include <vector>
#include <SDL3/SDL.h>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

CEngine::CEngine()
    : bInitialized(false), bRunning(false), window(nullptr), renderer(nullptr), world(nullptr), Width(0), Height(0) {
}

CEngine::~CEngine()
{
    Cleanup();
}

std::string CEngine::ResolveAssetPath(const char* relativePath)
{
    std::vector<std::string> possiblePaths = {
        relativePath,
        std::string("../../../") + relativePath,
        std::string("../../") + relativePath,
        std::string("../") + relativePath,
    };

    const char* basePath = SDL_GetBasePath();
    if (basePath) {
        possiblePaths.insert(possiblePaths.begin(), std::string(basePath) + relativePath);
    }

    for (const auto& path : possiblePaths) {
        if (fs::exists(path)) {
            SDL_Log(" Found asset at: %s", path.c_str());
            return path;
        }
    }

    SDL_Log(" Could not find asset: %s", relativePath);
    return relativePath;
}

void CEngine::StartGame(const char* title, int width, int height, const char* assetPath)
{
    std::cout << "=== Game Starting ===" << std::endl;

    Initialize(title, width, height, assetPath);

    if (!bInitialized) {
        std::cout << "Failed to initialize game!" << std::endl;
        return;
    }
}

void CEngine::Run()
{
    if (!bInitialized) {
        SDL_Log("Cannot run: Engine not initialized!");
        return;
    }

    // Call BeginPlay on world BEFORE starting game loop
    if (world) {
        world->BeginPlay();
    }

    GameLoop();
    Cleanup();
    std::cout << "=== Game Ended ===" << std::endl;
}

void CEngine::Initialize(const char* title, int width, int height, const char* assetPath)
{
    Width = width;
    Height = height;

    bRunning = true;

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return;
    }

    SDL_Log("=== Engine Initialization ===");

    // Initialize Input Manager
    CInputManager::GetInstance()->Initialize();

    // Create Window
    window = new CEngineWindow();
    if (!window->CreateWindow(title, width, height)) {
        SDL_Log("Failed to create window!");
        return;
    }

    // Create Renderer
    renderer = new CEngineRender();
    if (!renderer->Initialize(window)) {
        SDL_Log("Failed to initialize renderer!");
        return;
    }


    ;
    world = new CWorld(this, width, height);

    
    world->InitializePhysics(Vector2(0.0f, 0.0f));
    world->CreateBoundaryWalls(static_cast<float>(width), static_cast<float>(height));

    // Load assets
    std::string resolvedPath = ResolveAssetPath(assetPath);

    CEngineTexture* background = new CEngineTexture();
    if (background->LoadFromFile(resolvedPath)) {
        if (background->CreateTexture(static_cast<void*>(renderer->GetSDLRenderer()))) {
            textures.push_back(background);
        }
        else {
            delete background;
        }
    }
    else {
        delete background;
    }

    bInitialized = true;
    SDL_Log("=== Engine Initialized ===");
}

void CEngine::GameLoop()
{
    //bRunning = true;
    Uint64 lastTime = SDL_GetTicks();
    float deltaTime = 0.0f;

    std::cout << "Game loop started! Press ESC to quit." << std::endl;

    while (bRunning) {
        Uint64 currentTime = SDL_GetTicks();
        deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        if (deltaTime > 0.05f) {
            deltaTime = 0.05f;
        }

        // Unity execution order:
        ProcessInput();      // Input handling
        Update(deltaTime);   // Update phase
        Render();            // Render phase

        SDL_Delay(16); // ~60 FPS
    }
}

void CEngine::ProcessInput()
{
    CInputManager* input = CInputManager::GetInstance();
    input->Update();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        input->ProcessNativeEvent(&event);

        if (event.type == SDL_EVENT_QUIT) {
            bRunning = false;
        }

        if (event.type == SDL_EVENT_KEY_DOWN) {
            if (event.key.key == SDLK_ESCAPE) {
                bRunning = false;
            }
        }
    }
}

void CEngine::Update(float deltaTime)
{
    if (world)
    {
        world->Tick(deltaTime);
    }
}

void CEngine::Render()
{
    renderer->Clear();

    // Draw background
    if (!textures.empty() && textures[0]) {
        CEngineTexture* bg = textures[0];
        if (bg->GetNativeTexture()) {
            SDL_FRect destRect = { 0.0f, 0.0f, Width, Height};
            renderer->DrawTexture(static_cast<SDL_Texture*>(bg->GetNativeTexture()), nullptr, &destRect);
        }
    }

    // Render world actors
    if (world) {
        world->Render(renderer);
    }

    renderer->Present();
}

void CEngine::Cleanup()
{
    std::cout << "Cleaning up engine..." << std::endl;

    for (auto texture : textures) {
        delete texture;
    }
    textures.clear();
    bInitialized = false;
}

void CEngine::EndGame()
{
    bRunning = false;

    if (world)
    {
        world->CleanupDestroyedActors();
        delete world;
        world = nullptr;
    }

    if (renderer) {
        renderer->Destroy();
        delete renderer;
        renderer = nullptr;
    }

    if (window) {
        window->DestroyWindow();
        delete window;
        window = nullptr;
    }

    CInputManager::GetInstance()->Cleanup();

    std::cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" << std::endl;
    SDL_Quit();
    textures.clear();
    bInitialized = false;
}