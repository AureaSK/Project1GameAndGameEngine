#pragma once
#include <string>
#include <unordered_map>
#include "ChimasUtilities.h"

class CEngineTexture;

// Character data for bitmap font
struct CharacterData
{
    int x;        // X position in sprite sheet
    int y;        // Y position in sprite sheet
    int width;    // Character width
    int height;   // Character height
    int xOffset;  // X offset when rendering
    int yOffset;  // Y offset when rendering
    int xAdvance; // How much to advance cursor after this char
};

class CBitmapFont
{
private:
    CEngineTexture* fontTexture;
    std::unordered_map<char, CharacterData> characters;
    int lineHeight;
    int baseHeight;
    bool ownsTexture;

public:
    CBitmapFont();
    ~CBitmapFont();

    // Load font from bitmap (texture) and definition file
    bool LoadFromFile(const std::string& texturePath, const std::string& dataPath);

    // Manual setup (if you want to define characters in code)
    bool LoadTexture(const std::string& texturePath);
    void SetupGridFont(int charWidth, int charHeight, const std::string& charSet);
    void AddCharacter(char c, const CharacterData& data);

    // Getters
    CEngineTexture* GetTexture() const { return fontTexture; }
    const CharacterData* GetCharacter(char c) const;
    int GetLineHeight() const { return lineHeight; }
    int GetBaseHeight() const { return baseHeight; }

    // Calculate text dimensions
    Vector2 MeasureText(const std::string& text, float scale = 1.0f) const;

    // Setters
    void SetLineHeight(int height) { lineHeight = height; }
    void SetBaseHeight(int height) { baseHeight = height; }
};