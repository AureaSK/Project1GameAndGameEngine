#pragma once
#include "CWidget.h"
#include "CBitmapFont.h"
#include <string>


class CTextWidget : public CWidget
{
private:
    CBitmapFont* font;
    std::string text;
    float scale;
    bool ownsFont;

    

public:

    CTextWidget();
    virtual ~CTextWidget();

    // Alignment
    enum class TextAlign
    {
        Left,
        Center,
        Right
    };
    TextAlign horizontalAlignment;
    TextAlign verticalAlignment;

    // Font management
    bool LoadFont(const std::string& texturePath, const std::string& dataPath);
    bool LoadGridFont(const std::string& texturePath, int charWidth, int charHeight, const std::string& charSet);
    void SetFont(CBitmapFont* font);

    // Text properties
    void SetText(const std::string& newText);
    void SetScale(float s) { scale = s; }
    //void SetColor(const Color& c) { color = c; }
    void SetHorizontalAlignment(TextAlign align) { horizontalAlignment = align; }
    void SetVerticalAlignment(TextAlign align) { verticalAlignment = align; }

    // Getters
    const std::string& GetText() const { return text; }
    float GetScale() const { return scale; }
    //Color GetColor() const { return color; }

    // Lifecycle
    virtual void Tick(float deltaTime) override;
    virtual void Render(CEngineRender* renderer) override;

    // Utility
    Vector2 MeasureText() const;
};