#pragma once
#include <string>
#include <vector>

// Forward declarations (NO includes of engine components)
class CEngineWindow;
class CEngineRender;
class CEngineTexture;
class CWorld;

class CEngine
{
private:
    bool bInitialized;
    bool bRunning;

    // Engine subsystems - owned by CEngine
    CEngineWindow* window;
    CEngineRender* renderer;
    CWorld* world;

    float Width;
    float Height;

    // Textures managed by engine
    std::vector<CEngineTexture*> textures;

    // Internal methods
    void Initialize(const char* title, int width, int height, const char* assetPath);
    void GameLoop();
    void ProcessInput();
    void Update(float deltaTime);
    void Render();
    void Cleanup();

    

public:
    CEngine();
    ~CEngine();

    // Simple API
    void StartGame(const char* title, int width, int height, const char* assetPath);
    void Run();
    void EndGame();

    float GetWidthMesure() { return Width; }
    float GetHeightMesure() { return Height; }

    // Access to subsystems (read-only)
    CEngineRender* GetRenderer() const { return renderer; }
    CEngineWindow* GetWindow() const { return window; }
    CWorld* GetWorld() { return world; }

    

    // Helper to resolve asset paths
    std::string ResolveAssetPath(const char* relativePath);

};