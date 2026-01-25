#pragma once
#include "CWidget.h"
#include <vector>
#include <string>

class CEngineRender;
class CWorld;

// HUD class to manage all UI widgets (similar to UE's AHUD)
class CHUD
{
private:
    CWorld* world;
    std::vector<CWidget*> rootWidgets;

public:
    CHUD(CWorld* worldRef);
    ~CHUD();

    // Widget management
    template<typename T>
    T* CreateWidget()
    {
        T* widget = new T();
        rootWidgets.push_back(widget);
        return widget;
    }

    void AddWidget(CWidget* widget);
    void RemoveWidget(CWidget* widget);
    void ClearAllWidgets();

    // Lifecycle
    void Tick(float deltaTime);
    void Render(CEngineRender* renderer);

    // Getters
    CWorld* GetWorld() const { return world; }
};