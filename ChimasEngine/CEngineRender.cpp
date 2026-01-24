#include "pch.h"
#include "CEngineRender.h"
#include "CEngineWindow.h"
#include "CShader.h"
#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <cmath>
#include <cstring>

// 2D matrix implementation
struct Mat4 {
    float m[16];
    Mat4();
    static Mat4 Ortho(float left, float right, float bottom, float top);
    static Mat4 Translate(float x, float y);
    static Mat4 Scale(float x, float y);
    static Mat4 Rotate(float angleDeg);
    Mat4 operator*(const Mat4& other) const;
};

Mat4::Mat4()
{
    memset(m, 0, sizeof(m));
    m[0] = m[5] = m[10] = m[15] = 1.0f; // Identity
}

Mat4 Mat4::Ortho(float left, float right, float bottom, float top)
{
    Mat4 result;
    result.m[0] = 2.0f / (right - left);
    result.m[5] = 2.0f / (top - bottom);
    result.m[10] = -1.0f;
    result.m[12] = -(right + left) / (right - left);
    result.m[13] = -(top + bottom) / (top - bottom);
    return result;
}

Mat4 Mat4::Translate(float x, float y)
{
    Mat4 result;
    result.m[12] = x;
    result.m[13] = y;
    return result;
}

Mat4 Mat4::Scale(float x, float y)
{
    Mat4 result;
    result.m[0] = x;
    result.m[5] = y;
    return result;
}

Mat4 Mat4::Rotate(float angleDeg)
{
    float rad = angleDeg * 3.14159265f / 180.0f;
    float c = cosf(rad);
    float s = sinf(rad);

    Mat4 result;
    result.m[0] = c;
    result.m[1] = s;
    result.m[4] = -s;
    result.m[5] = c;
    return result;
}

Mat4 Mat4::operator*(const Mat4& other) const
{
    Mat4 result;
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            result.m[row + col * 4] = 0;
            for (int i = 0; i < 4; i++) {
                result.m[row + col * 4] += m[row + i * 4] * other.m[i + col * 4];
            }
        }
    }
    return result;
}

// PIMPL Implementation
struct CEngineRender::Impl
{
    SDL_GLContext glContext;
    SDL_Window* window;

    CShader* shader;
    GLuint quadVAO, quadVBO;

    Color currentColor;
    int viewportWidth;
    int viewportHeight;
    Mat4 projectionMatrix;

    Impl() : glContext(nullptr), window(nullptr), shader(nullptr),
        quadVAO(0), quadVBO(0), currentColor{ 0, 0, 0, 255 },
        viewportWidth(0), viewportHeight(0) {
    }

    bool CreateShaders();
    bool CreateQuadBuffers();
    void UpdateProjectionMatrix();
};

bool CEngineRender::Impl::CreateShaders()
{
    shader = new CShader();
    if (!shader->Initialize(nullptr, nullptr)) {
        delete shader;
        shader = nullptr;
        return false;
    }
    return true;
}

bool CEngineRender::Impl::CreateQuadBuffers()
{
    float vertices[] = {
        0.0f, 1.0f,   0.0f, 1.0f,
        1.0f, 0.0f,   1.0f, 0.0f,
        0.0f, 0.0f,   0.0f, 0.0f,

        0.0f, 1.0f,   0.0f, 1.0f,
        1.0f, 1.0f,   1.0f, 1.0f,
        1.0f, 0.0f,   1.0f, 0.0f
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    return true;
}

void CEngineRender::Impl::UpdateProjectionMatrix()
{
    if (shader) {
        projectionMatrix = Mat4::Ortho(0.0f, (float)viewportWidth, (float)viewportHeight, 0.0f);
        shader->Use();
        shader->SetProjectionMatrix(projectionMatrix.m);
    }
}

// Public interface
CEngineRender::CEngineRender()
    : impl(new Impl())
{
}

CEngineRender::~CEngineRender()
{
    Destroy();
    delete impl;
}

bool CEngineRender::Initialize(CEngineWindow* engineWindow)
{
    if (!engineWindow) {
        SDL_Log("Cannot create renderer: window is null");
        return false;
    }

    impl->window = engineWindow->GetSDLWindow();
    if (!impl->window) {
        SDL_Log("Cannot get SDL window");
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    impl->glContext = SDL_GL_CreateContext(impl->window);
    if (!impl->glContext) {
        SDL_Log("Failed to create OpenGL context: %s", SDL_GetError());
        return false;
    }

    SDL_GL_MakeCurrent(impl->window, impl->glContext);

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        SDL_Log("Failed to initialize GLAD");
        return false;
    }

    SDL_Log("OpenGL Version: %s", glGetString(GL_VERSION));
    SDL_Log("GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    SDL_GetWindowSize(impl->window, &impl->viewportWidth, &impl->viewportHeight);
    glViewport(0, 0, impl->viewportWidth, impl->viewportHeight);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (!impl->CreateShaders()) {
        SDL_Log("Failed to create shaders");
        return false;
    }

    if (!impl->CreateQuadBuffers()) {
        SDL_Log("Failed to create quad buffers");
        return false;
    }

    impl->UpdateProjectionMatrix();
    SDL_GL_SetSwapInterval(1);

    return true;
}

void CEngineRender::Destroy()
{
    if (!impl) return;

    if (impl->quadVAO) {
        glDeleteVertexArrays(1, &impl->quadVAO);
        impl->quadVAO = 0;
    }
    if (impl->quadVBO) {
        glDeleteBuffers(1, &impl->quadVBO);
        impl->quadVBO = 0;
    }
    if (impl->shader) {
        delete impl->shader;
        impl->shader = nullptr;
    }
    if (impl->glContext) {
        SDL_GL_DestroyContext(impl->glContext);
        impl->glContext = nullptr;
    }
}

void CEngineRender::Clear()
{
    glClearColor(impl->currentColor.r / 255.0f, impl->currentColor.g / 255.0f,
        impl->currentColor.b / 255.0f, impl->currentColor.a / 255.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void CEngineRender::Present()
{
    SDL_GL_SwapWindow(impl->window);
}

void CEngineRender::SetDrawColor(const Color& color)
{
    impl->currentColor = color;
}

void CEngineRender::DrawRect(const SDL_FRect& rect, const Color& color)
{
    if (!impl->shader) return;

    impl->shader->Use();
    impl->shader->SetColor(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
    impl->shader->SetUseTexture(false);

    Mat4 translate = Mat4::Translate(rect.x, rect.y);
    Mat4 scale = Mat4::Scale(rect.w, rect.h);
    Mat4 model = translate * scale;

    impl->shader->SetModelMatrix(model.m);

    glBindVertexArray(impl->quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void CEngineRender::DrawTexture(void* texture, const SDL_FRect* srcRect, const SDL_FRect* destRect, float rotation, int textureWidth, int textureHeight)
{
    GLuint texID = static_cast<GLuint>(reinterpret_cast<uintptr_t>(texture));
    if (!texID || !destRect || !impl->shader) return;

    impl->shader->Use();
    impl->shader->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
    impl->shader->SetUseTexture(true);

    // Calculate texture coordinates for sprite sheets
    if (srcRect && textureWidth > 0 && textureHeight > 0) {
        // Convert pixel coordinates to normalized UV coordinates (0.0 to 1.0)
        float offsetX = srcRect->x / static_cast<float>(textureWidth);
        float offsetY = srcRect->y / static_cast<float>(textureHeight);
        float scaleX = srcRect->w / static_cast<float>(textureWidth);
        float scaleY = srcRect->h / static_cast<float>(textureHeight);
        
        impl->shader->SetTextureCoordinates(offsetX, offsetY, scaleX, scaleY);
    }
    else {
        // No source rect or texture dimensions - use full texture
        impl->shader->SetTextureCoordinates(0.0f, 0.0f, 1.0f, 1.0f);
    }

    Mat4 model;
    if (rotation != 0.0f) {
        Mat4 translate1 = Mat4::Translate(destRect->x + destRect->w / 2.0f, destRect->y + destRect->h / 2.0f);
        Mat4 rotate = Mat4::Rotate(rotation);
        Mat4 translate2 = Mat4::Translate(-destRect->w / 2.0f, -destRect->h / 2.0f);
        Mat4 scale = Mat4::Scale(destRect->w, destRect->h);

        model = translate1 * rotate * translate2 * scale;
    }
    else {
        Mat4 translate = Mat4::Translate(destRect->x, destRect->y);
        Mat4 scale = Mat4::Scale(destRect->w, destRect->h);
        model = translate * scale;
    }

    impl->shader->SetModelMatrix(model.m);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID);

    glBindVertexArray(impl->quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void* CEngineRender::CreateTextureFromSurface(SDL_Surface* surface)
{
    if (!surface) return nullptr;

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    GLenum format = GL_RGBA;
    if (SDL_BYTESPERPIXEL(surface->format) == 3) {
        format = GL_RGB;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0,
        format, GL_UNSIGNED_BYTE, surface->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);

    return reinterpret_cast<void*>(static_cast<uintptr_t>(texture));
}

void* CEngineRender::GetInternalContext() const
{
    return impl->glContext;
}