#include "pch.h"
#include "CTextWidget.h"
#include "CEngineRender.h"
#include "CEngineTexture.h"
#include "ChimasLog.h"
#include <SDL3/SDL.h>

CTextWidget::CTextWidget()
    : CWidget(), font(nullptr), text(""), scale(1.0f), color(Color::White()),
    ownsFont(false), horizontalAlignment(TextAlign::Left), verticalAlignment(TextAlign::Left)
{
}

CTextWidget::~CTextWidget()
{
    if (ownsFont && font)
    {
        delete font;
    }
}

bool CTextWidget::LoadFont(const std::string& texturePath, const std::string& dataPath)
{
    if (ownsFont && font)
    {
        delete font;
    }

    font = new CBitmapFont();
    if (!font->LoadFromFile(texturePath, dataPath))
    {
        delete font;
        font = nullptr;
        return false;
    }

    ownsFont = true;
    return true;
}

bool CTextWidget::LoadGridFont(const std::string& texturePath, int charWidth, int charHeight, const std::string& charSet)
{
    if (ownsFont && font)
    {
        delete font;
    }

    font = new CBitmapFont();
    if (!font->LoadTexture(texturePath))
    {
        delete font;
        font = nullptr;
        return false;
    }

    font->SetupGridFont(charWidth, charHeight, charSet);
    ownsFont = true;
    return true;
}

void CTextWidget::SetFont(CBitmapFont* newFont)
{
    if (ownsFont && font)
    {
        delete font;
    }

    font = newFont;
    ownsFont = false;
}

void CTextWidget::SetText(const std::string& newText)
{
    text = newText;
}

Vector2 CTextWidget::MeasureText() const
{
    if (!font)
        return Vector2(0.0f, 0.0f);

    return font->MeasureText(text, scale);
}

void CTextWidget::Tick(float deltaTime)
{
    CWidget::Tick(deltaTime);
    // Add any text-specific logic here
}

void CTextWidget::Render(CEngineRender* renderer)
{
    if (!isVisible || !font || !font->GetTexture() || text.empty())
    {
        return;
    }

    CEngineTexture* texture = font->GetTexture();
    if (!texture->GetNativeTexture())
        return;

    // Get absolute position in screen space
    Vector2 renderPos = GetAbsolutePosition();

    // Calculate alignment offset
    Vector2 textSize = MeasureText();

    if (horizontalAlignment == TextAlign::Center)
    {
        renderPos.x += (size.x - textSize.x) / 2.0f;
    }
    else if (horizontalAlignment == TextAlign::Right)
    {
        renderPos.x += size.x - textSize.x;
    }

    if (verticalAlignment == TextAlign::Center)
    {
        renderPos.y += (size.y - textSize.y) / 2.0f;
    }
    else if (verticalAlignment == TextAlign::Right)
    {
        renderPos.y += size.y - textSize.y;
    }

    float cursorX = renderPos.x;
    float cursorY = renderPos.y;

    // Render each character
    for (char c : text)
    {
        if (c == '\n')
        {
            cursorX = renderPos.x;
            cursorY += font->GetLineHeight() * scale;
            continue;
        }

        const CharacterData* charData = font->GetCharacter(c);
        if (!charData)
            continue;

        SDL_FRect srcRect;
        srcRect.x = static_cast<float>(charData->x);
        srcRect.y = static_cast<float>(charData->y);
        srcRect.w = static_cast<float>(charData->width);
        srcRect.h = static_cast<float>(charData->height);

        SDL_FRect dstRect;
        dstRect.x = cursorX + (charData->xOffset * scale);
        dstRect.y = cursorY + (charData->yOffset * scale);
        dstRect.w = charData->width * scale;
        dstRect.h = charData->height * scale;

        renderer->DrawTexture(
            texture->GetNativeTexture(),
            &srcRect,
            &dstRect,
            0.0f,
            texture->GetWidth(),
            texture->GetHeight()
        );

        cursorX += charData->xAdvance * scale;
    }

    // Render children
    for (auto child : children)
    {
        if (child && child->IsVisible())
        {
            child->Render(renderer);
        }
    }
}