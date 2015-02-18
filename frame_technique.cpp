#include <string.h>

#include "frame_technique.h"

static const char* pVS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
layout (location = 0) in vec3 Position;                                             \n\
                                                                                    \n\
uniform mat4 gWVP;                                                                  \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    gl_Position = gWVP * vec4(Position, 1.0);                                       \n\
}";

static const char* pFS = "                                                          \n\
#version 330                                                                        \n\
out vec4 FragColor;                                                                 \n\
uniform sampler2D gSampler;                                                                 \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    FragColor = vec4(0.0, 0.0, 0.0, 1.0);                                           \n\
}";

FrameTechnique::FrameTechnique() {
}

bool FrameTechnique::Init() {
    if (!Technique::Init()) {
        return false;
    }

    if (!AddShader(GL_VERTEX_SHADER, pVS)) {
        return false;
    }

    if (!AddShader(GL_FRAGMENT_SHADER, pFS)) {
        return false;
    }

    if (!Finalize()) {
        return false;
    }

    m_WVPLocation = GetUniformLocation("gWVP");

    return m_WVPLocation != INVALID_UNIFORM_LOCATION;
}

void FrameTechnique::SetWVP(const Matrix4f& WVP) {
    glUniformMatrix4fv(m_WVPLocation, 1, GL_TRUE, (const GLfloat*)WVP.m);
}
