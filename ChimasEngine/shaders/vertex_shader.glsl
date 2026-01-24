#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 projection;
uniform mat4 model;
uniform vec2 texOffset;    // Texture coordinate offset (for sprite sheets)
uniform vec2 texScale;     // Texture coordinate scale (for sprite sheets)

void main()
{
    gl_Position = projection * model * vec4(aPos, 0.0, 1.0);
    // Transform texture coordinates for sprite sheets
    TexCoord = aTexCoord * texScale + texOffset;
}
