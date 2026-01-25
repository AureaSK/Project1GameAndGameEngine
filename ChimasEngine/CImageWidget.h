#pragma once
#include "CWidget.h"
#include "ChimasUtilities.h"

class CEngineTexture;

// Image widget for displaying textures (similar to UE's UImage)
class CImageWidget : public CWidget
{
private:
    CEngineTexture* texture;
    bool ownsTexture;

    // Visual properties
    Color tintColor;
    float opacity;

    // UV coordinates for sprite sheets
    RectF sourceRect;
    bool useSourceRect;

public:
    CImageWidget();
    virtual ~CImageWidget();

    // Texture loading
    bool LoadImage(const std::string& path);
    void SetTexture(CEngineTexture* tex);

    // Visual properties
    void SetTintColor(const Color& color) { tintColor = color; }
    void SetOpacity(float alpha) { opacity = alpha; }

    // Source rect for sprite sheets
    void SetSourceRect(float x, float y, float w, float h);
    void ClearSourceRect();

    // Getters
    CEngineTexture* GetTexture() const { return texture; }
    Color GetTintColor() const { return tintColor; }
    float GetOpacity() const { return opacity; }

    // Lifecycle
    virtual void Tick(float deltaTime) override;
    virtual void Render(CEngineRender* renderer) override;
};