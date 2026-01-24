#include "pch.h"
#include "CShader.h"
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

static std::string LoadShaderFile(const char* filepath)
{
    std::string shaderCode;
    std::ifstream shaderFile;

    // Try multiple possible paths
    std::vector<std::string> possiblePaths = {
        filepath,
        std::string("../../../") + filepath,
        std::string("../../") + filepath,
        std::string("../") + filepath,
    };

    const char* basePath = SDL_GetBasePath();
    if (basePath) {
        possiblePaths.insert(possiblePaths.begin(), std::string(basePath) + filepath);
    }

    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    
    for (const auto& path : possiblePaths) {
        try {
            shaderFile.open(path);
            std::stringstream shaderStream;
            shaderStream << shaderFile.rdbuf();
            shaderFile.close();
            shaderCode = shaderStream.str();
            SDL_Log("Loaded shader from: %s", path.c_str());
            return shaderCode;
        }
        catch (std::ifstream::failure& e) {
            if (shaderFile.is_open()) {
                shaderFile.close();
            }
            // Try next path
        }
    }

    SDL_Log("ERROR: Could not load shader file: %s", filepath);
    return "";
}

CShader::CShader()
    : shaderProgram(0), uProjection(-1), uModel(-1), uColor(-1), uUseTexture(-1)
{
}

CShader::~CShader()
{
    if (shaderProgram != 0) {
        glDeleteProgram(shaderProgram);
        shaderProgram = 0;
    }
}

GLuint CShader::CompileShader(GLenum type, const char* source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        SDL_Log("Shader compilation failed: %s", infoLog);
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

bool CShader::LinkProgram(GLuint vertexShader, GLuint fragmentShader)
{
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        SDL_Log("Shader linking failed: %s", infoLog);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(shaderProgram);
        shaderProgram = 0;
        return false;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Get uniform locations
    uProjection = glGetUniformLocation(shaderProgram, "projection");
    uModel = glGetUniformLocation(shaderProgram, "model");
    uColor = glGetUniformLocation(shaderProgram, "color");
    uUseTexture = glGetUniformLocation(shaderProgram, "useTexture");

    return true;
}

bool CShader::Initialize(const char* vertexSource, const char* fragmentSource)
{
    std::string vertexCode;
    std::string fragmentCode;

    // If nullptr is passed, load from default shader files
    if (!vertexSource) {
        vertexCode = LoadShaderFile("shaders/vertex_shader.glsl");
        if (vertexCode.empty()) {
            SDL_Log("Failed to load vertex shader from file");
            return false;
        }
        vertexSource = vertexCode.c_str();
    }

    if (!fragmentSource) {
        fragmentCode = LoadShaderFile("shaders/fragment_shader.glsl");
        if (fragmentCode.empty()) {
            SDL_Log("Failed to load fragment shader from file");
            return false;
        }
        fragmentSource = fragmentCode.c_str();
    }

    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
    if (!vertexShader) {
        SDL_Log("Failed to compile vertex shader");
        return false;
    }

    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (!fragmentShader) {
        SDL_Log("Failed to compile fragment shader");
        glDeleteShader(vertexShader);
        return false;
    }

    if (!LinkProgram(vertexShader, fragmentShader)) {
        SDL_Log("Failed to link shader program");
        return false;
    }

    SDL_Log("Shader program created successfully");
    return true;
}

void CShader::Use() const
{
    if (shaderProgram != 0) {
        glUseProgram(shaderProgram);
    }
}

void CShader::SetProjectionMatrix(const float* matrix) const
{
    if (uProjection >= 0 && shaderProgram != 0) {
        glUniformMatrix4fv(uProjection, 1, GL_FALSE, matrix);
    }
}

void CShader::SetModelMatrix(const float* matrix) const
{
    if (uModel >= 0 && shaderProgram != 0) {
        glUniformMatrix4fv(uModel, 1, GL_FALSE, matrix);
    }
}

void CShader::SetColor(float r, float g, float b, float a) const
{
    if (uColor >= 0 && shaderProgram != 0) {
        glUniform4f(uColor, r, g, b, a);
    }
}

void CShader::SetUseTexture(bool useTexture) const
{
    if (uUseTexture >= 0 && shaderProgram != 0) {
        glUniform1i(uUseTexture, useTexture ? 1 : 0);
    }
}
