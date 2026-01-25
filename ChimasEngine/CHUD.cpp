#include "pch.h"
#include "CHUD.h"
#include "CEngineRender.h"
#include <algorithm>

CHUD::CHUD(CWorld* worldRef)
    : world(worldRef)
{
}

CHUD::~CHUD()
{
    ClearAllWidgets();
}

void CHUD::AddWidget(CWidget* widget)
{
    if (!widget) return;
    rootWidgets.push_back(widget);
}

void CHUD::RemoveWidget(CWidget* widget)
{
    if (!widget) return;

    auto it = std::find(rootWidgets.begin(), rootWidgets.end(), widget);
    if (it != rootWidgets.end())
    {
        delete* it;
        rootWidgets.erase(it);
    }
}

void CHUD::ClearAllWidgets()
{
    for (auto widget : rootWidgets)
    {
        delete widget;
    }
    rootWidgets.clear();
}

void CHUD::Tick(float deltaTime)
{
    // Update all root widgets (they'll update their children)
    for (auto widget : rootWidgets)
    {
        if (widget && widget->IsEnabled())
        {
            widget->Tick(deltaTime);
        }
    }
}

void CHUD::Render(CEngineRender* renderer)
{
    if (!renderer) return;

    // Sort widgets by Z-order before rendering
    std::vector<CWidget*> sortedWidgets = rootWidgets;
    std::sort(sortedWidgets.begin(), sortedWidgets.end(),
        [](CWidget* a, CWidget* b) {
            return a->GetZOrder() < b->GetZOrder();
        });

    // Render all widgets
    for (auto widget : sortedWidgets)
    {
        if (widget && widget->IsVisible())
        {
            widget->Render(renderer);
        }
    }
}