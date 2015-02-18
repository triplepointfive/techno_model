#ifndef FRAME_TECHNIQUE_H
#define FRAME_TECHNIQUE_H

#include "technique.h"
#include "math_3d.h"

class FrameTechnique : public Technique {
public:
    FrameTechnique();

    virtual bool Init();
    void SetWVP(const Matrix4f& WVP);

private:
    GLuint m_WVPLocation;
};

#endif // FRAME_TECHNIQUE_H
