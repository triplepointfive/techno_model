#include <string.h>

#include "math_3d.h"
#include "gradient_technique.h"

static const char* pVS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
layout (location = 0) in vec2 Position;                                             \n\
                                                                                    \n\
out vec4 Color;                                                                     \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    gl_Position = vec4(Position, 1.0, 1.0);                                         \n\
    if (Position.y == 1.0) {                                                        \n\
        Color = vec4(0.411, 0.65, 0.88, 1.0);                                       \n\
    } else {                                                                        \n\
        Color = vec4(1.0, 1.0, 1.0, 1.0);                                           \n\
    }                                                                               \n\
}";

static const char* pFS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
in vec4 Color;                                                                      \n\
                                                                                    \n\
out vec4 FragColor;                                                                 \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    FragColor = Color;                                                              \n\
}";

GradientTechnique::GradientTechnique(){
}

bool GradientTechnique::Init(){
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

    return true;
}
