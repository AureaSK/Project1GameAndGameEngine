#include "pch.h"
#include "CWidget.h"
#include <algorithm>

CWidget::CWidget()
    : parent(nullptr), position(0.0f, 0.0f), size(100.0f, 100.0f),
    isVisible(true), isEnabled(true), zOrder(0)
{
}

CWidget::~CWidget()
{
    // Delete all children
    for (auto child : children)
    {
        delete child;
    }
    children.clear();
}

void CWidget::AddChild(CWidget* child)
{
    if (!child) return;

    // Remove from previous parent if any
    if (child->parent)
    {
        child->parent->RemoveChild(child);
    }

    child->parent = this;
    children.push_back(child);
}

void CWidget::RemoveChild(CWidget* child)
{
    if (!child) return;

    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end())
    {
        (*it)->parent = nullptr;
        children.erase(it);
    }
}

void CWidget::Tick(float deltaTime)
{
    if (!isEnabled) return;

    // Tick all children
    for (auto child : children)
    {
        if (child && child->IsEnabled())
        {
            child->Tick(deltaTime);
        }
    }
}

Vector2 CWidget::GetAbsolutePosition() const
{
    if (parent)
    {
        Vector2 parentPos = parent->GetAbsolutePosition();
        return Vector2(parentPos.x + position.x, parentPos.y + position.y);
    }
    return position;
}