#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 outColor;

uniform sampler2D ourTexture;

// Lighting uniforms
// direction the light is coming FROM
uniform vec3 lightDir;  
// usually white (1,1,1)
uniform vec3 lightColor;    
// camera position
uniform vec3 viewPos;         

uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;
uniform float shininess;

void main()
{
    // Normalize inputs
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(-lightDir);
    vec3 viewDirection  = normalize(viewPos - FragPos);

    // Ambient term
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse term (Lambert)
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diffuseStrength * diff * lightColor;

    // Specular term
    vec3 halfwayDir = normalize(lightDirection + viewDirection);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 lighting = ambient + diffuse + specular;

    vec3 texColor = texture(ourTexture, TexCoord).rgb;
    vec3 result = lighting * texColor;

    outColor = vec4(result, 1.0);
}