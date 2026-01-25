#pragma once
#include "ChimasUtilities.h"
#include <vector>
#include <string>

class CEngineRender;

// Base class for all UI widgets (similar to UE's UWidget)
class CWidget
{
protected:
    CWidget* parent;
    std::vector<CWidget*> children;

    Vector2 position;
    Vector2 size;
    bool isVisible;
    bool isEnabled;
    int zOrder; // For rendering order

public:
    CWidget();
    virtual ~CWidget();

    // Widget hierarchy
    void AddChild(CWidget* child);
    void RemoveChild(CWidget* child);
    CWidget* GetParent() const { return parent; }
    const std::vector<CWidget*>& GetChildren() const { return children; }

    // Lifecycle
    virtual void Tick(float deltaTime);
    virtual void Render(CEngineRender* renderer) = 0; // Pure virtual

    // Transform
    void SetPosition(const Vector2& pos) { position = pos; }
    void SetSize(const Vector2& s) { size = s; }
    Vector2 GetPosition() const { return position; }
    Vector2 GetSize() const { return size; }
    Vector2 GetAbsolutePosition() const; // World position considering parent

    // Visibility
    void SetVisibility(bool visible) { isVisible = visible; }
    bool IsVisible() const { return isVisible; }
    void SetEnabled(bool enabled) { isEnabled = enabled; }
    bool IsEnabled() const { return isEnabled; }

    // Rendering order
    void SetZOrder(int order) { zOrder = order; }
    int GetZOrder() const { return zOrder; }
};