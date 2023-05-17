//
// Created by alx on 5/16/23.
//

#ifndef OPENCL_CGOL_OGLSHADER_H
#define OPENCL_CGOL_OGLSHADER_H

const char* vertexShaderSource = R"(
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

out vec2 fragTexCoord;

void main()
{
    gl_Position = vec4(position, 1.0);
    fragTexCoord = texCoord;
}
)";

// Fragment shader source code
const char* fragmentShaderDEBUGSource = R"(
#version 330 core

in vec2 fragTexCoord;

out vec4 fragColor;

void main()
{
    fragColor = vec4(fragTexCoord.x, 0.0, fragTexCoord.y, 1.0);
}
)";

const char* fragmentShaderTextureSource = R"(
#version 330 core

in vec2 fragTexCoord;
out vec4 fragColor;

uniform bool useTexture;
uniform sampler2D textureSampler;

void main()
{
    if (useTexture)
    {
        fragColor = texture(textureSampler, fragTexCoord);
    }
    else
    {
        fragColor = vec4(fragTexCoord, 0.0, 1.0);
    }
}
)";

#endif //OPENCL_CGOL_OGLSHADER_H
