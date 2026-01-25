#include "pch.h"
#include "CBitmapFont.h"
#include "CEngineTexture.h"
#include "ChimasLog.h"
#include <fstream>
#include <sstream>

CBitmapFont::CBitmapFont()
    : fontTexture(nullptr), lineHeight(0), baseHeight(0), ownsTexture(false)
{
}

CBitmapFont::~CBitmapFont()
{
    if (ownsTexture && fontTexture)
    {
        delete fontTexture;
    }
}

bool CBitmapFont::LoadTexture(const std::string& texturePath)
{
    if (ownsTexture && fontTexture)
    {
        delete fontTexture;
    }

    fontTexture = new CEngineTexture();
    if (!fontTexture->LoadFromFile(texturePath))
    {
        ChimasLog::Info("CBitmapFont: Failed to load texture '%s'", texturePath.c_str());
        delete fontTexture;
        fontTexture = nullptr;
        return false;
    }

    if (!fontTexture->CreateTexture(nullptr))
    {
        ChimasLog::Info("CBitmapFont: Failed to create GPU texture");
        delete fontTexture;
        fontTexture = nullptr;
        return false;
    }

    ownsTexture = true;
    ChimasLog::Info("CBitmapFont: Loaded texture '%s'", texturePath.c_str());
    return true;
}

bool CBitmapFont::LoadFromFile(const std::string& texturePath, const std::string& dataPath)
{
    // Load texture first
    if (!LoadTexture(texturePath))
    {
        return false;
    }

    // Load character data from file
    std::ifstream file(dataPath);
    if (!file.is_open())
    {
        ChimasLog::Info("CBitmapFont: Failed to open data file '%s'", dataPath.c_str());
        return false;
    }

    std::string line;
    while (std::getline(file, line))
    {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#')
            continue;

        std::istringstream iss(line);
        std::string tag;
        iss >> tag;

        if (tag == "char")
        {
            char c;
            CharacterData data;

            // Format: char [character] x y width height xOffset yOffset xAdvance
            iss >> c >> data.x >> data.y >> data.width >> data.height
                >> data.xOffset >> data.yOffset >> data.xAdvance;

            characters[c] = data;
        }
        else if (tag == "lineHeight")
        {
            iss >> lineHeight;
        }
        else if (tag == "baseHeight")
        {
            iss >> baseHeight;
        }
    }

    file.close();
    ChimasLog::Info("CBitmapFont: Loaded %zu characters from '%s'", characters.size(), dataPath.c_str());
    return true;
}

void CBitmapFont::SetupGridFont(int charWidth, int charHeight, const std::string& charSet)
{
    if (!fontTexture)
    {
        ChimasLog::Info("CBitmapFont: Cannot setup grid font - no texture loaded");
        return;
    }

    int texWidth = fontTexture->GetWidth();
    int charsPerRow = texWidth / charWidth;

    for (size_t i = 0; i < charSet.length(); i++)
    {
        char c = charSet[i];

        CharacterData data;
        data.x = (i % charsPerRow) * charWidth;
        data.y = (i / charsPerRow) * charHeight;
        data.width = charWidth;
        data.height = charHeight;
        data.xOffset = 0;
        data.yOffset = 0;
        data.xAdvance = charWidth;

        characters[c] = data;
    }

    lineHeight = charHeight;
    baseHeight = charHeight;

    ChimasLog::Info("CBitmapFont: Setup grid font with %zu characters", characters.size());
}

void CBitmapFont::AddCharacter(char c, const CharacterData& data)
{
    characters[c] = data;
}

const CharacterData* CBitmapFont::GetCharacter(char c) const
{
    auto it = characters.find(c);
    if (it != characters.end())
    {
        return &it->second;
    }
    return nullptr;
}

Vector2 CBitmapFont::MeasureText(const std::string& text, float scale) const
{
    float width = 0.0f;
    float maxHeight = 0.0f;

    for (char c : text)
    {
        const CharacterData* charData = GetCharacter(c);
        if (charData)
        {
            width += charData->xAdvance * scale;
            float charHeight = charData->height * scale;
            if (charHeight > maxHeight)
                maxHeight = charHeight;
        }
    }

    return Vector2(width, maxHeight > 0 ? maxHeight : lineHeight * scale);
}