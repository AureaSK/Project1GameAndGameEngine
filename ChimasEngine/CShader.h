#pragma once
#include <glad/glad.h>
#include <SDL3/SDL.h>

// Shader class for managing OpenGL shader programs
class CShader
{
private:
    GLuint shaderProgram;
    GLint uProjection;
    GLint uModel;
    GLint uColor;
    GLint uUseTexture;

    GLuint CompileShader(GLenum type, const char* source);
    bool LinkProgram(GLuint vertexShader, GLuint fragmentShader);

public:
    CShader();
    ~CShader();

    // Initialize shader with vertex and fragment shader source code
    bool Initialize(const char* vertexSource, const char* fragmentSource);

    // Use this shader program
    void Use() const;

    // Set uniform values
    void SetProjectionMatrix(const float* matrix) const;
    void SetModelMatrix(const float* matrix) const;
    void SetColor(float r, float g, float b, float a) const;
    void SetUseTexture(bool useTexture) const;

    // Get uniform locations (for direct access if needed)
    GLint GetProjectionLocation() const { return uProjection; }
    GLint GetModelLocation() const { return uModel; }
    GLint GetColorLocation() const { return uColor; }
    GLint GetUseTextureLocation() const { return uUseTexture; }

    // Get the shader program ID
    GLuint GetProgramID() const { return shaderProgram; }

    // Check if shader is valid
    bool IsValid() const { return shaderProgram != 0; }
};
