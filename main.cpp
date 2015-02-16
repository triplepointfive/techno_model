#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "pipeline.h"
#include "camera.h"
#include "texture.h"
#include "lighting_technique.h"
#include "gradient_technique.h"
#include "glut_backend.h"
#include "util.h"
#include "mesh.h"

#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 1024

#include <AntTweakBar.h>

GLuint VBO;
GLuint IBO;

static void CreateVertexBuffer()
{
    Vector2f Vertices[4];
    Vertices[0] = Vector2f(-1.0f, -1.0f);
    Vertices[1] = Vector2f(1.0f, -1.0f);
    Vertices[2] = Vector2f(1.0f, 1.0f);
    Vertices[3] = Vector2f(-1.0f, 1.0f);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
}

static void CreateIndexBuffer()
{
    unsigned int Indices[] = { 1, 0, 2,
                               3, 2, 0 };

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
}

class Main : public ICallbacks
{
public:

    Main()
    {
        m_pGameCamera = NULL;
        m_pEffect = NULL;
        m_pGradientEffect = NULL;
        m_scale = 0.0f;
        m_directionalLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
        m_directionalLight.AmbientIntensity = 1.0f;
        m_directionalLight.DiffuseIntensity = 0.01f;
        m_directionalLight.Direction = Vector3f(1.0f, 0.0f, 1.0f);
        CreateVertexBuffer();
        CreateIndexBuffer();
    }

    virtual ~Main()
    {
        delete m_pGradientEffect;
        delete m_pEffect;
        delete m_pGameCamera;
        delete m_pMesh;
    }

    bool Init()
    {
        Vector3f Pos(-10.0f, 4.0f, 0.0f);
        Vector3f Target(1.0f, 0.0f, 1.0f);
        Vector3f Up(0.0, 1.0f, 0.0f);
        m_pGameCamera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT, Pos, Target, Up);

        m_pEffect = new LightingTechnique();

        if (!m_pEffect->Init()){
            printf("Error initializing the lighting technique\n");
            return false;
        }

        m_pGradientEffect = new GradientTechnique();

        if (!m_pGradientEffect->Init()) {
            printf("Error initializing the gradient technique\n");
        }

        m_pEffect->Enable();

        m_pEffect->SetTextureUnit(0);

        m_pMesh = new Mesh();

        return m_pMesh->LoadMesh("phoenix_ugv.stl");
    }

    void Run()
    {
        GLUTBackendRun(this);
    }

    virtual void RenderSceneCB()
    {
        m_pGameCamera->OnRender();

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        m_scale += 0.01f;


        GLint OldDepthFuncMode;
        glGetIntegerv(GL_DEPTH_FUNC, &OldDepthFuncMode);
        m_pGradientEffect->Enable();
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glDisableVertexAttribArray(0);
        glDepthFunc(GL_LEQUAL);


        Pipeline p;
        p.Scale(0.005f, 0.005f, 0.005f);
        p.Rotate(0.0f, m_scale, 0.0f);
        p.WorldPos(0.0f, 0.0f, 10.0f);
        p.SetCamera(m_pGameCamera->GetPos(), m_pGameCamera->GetTarget(), m_pGameCamera->GetUp());
        p.SetPerspectiveProj(60.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f, 100.0f);

        m_pEffect->Enable();
        m_pEffect->SetWVP(p.GetWVPTrans());
        m_pEffect->SetWorldMatrix(p.GetWorldTrans());
        m_pEffect->SetDirectionalLight(m_directionalLight);
        m_pEffect->SetEyeWorldPos(m_pGameCamera->GetPos());
        m_pEffect->SetMatSpecularIntensity(0.0f);
        m_pEffect->SetMatSpecularPower(0);

        m_pMesh->Render();
//        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
//        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );



        TwDraw();
        glutSwapBuffers();
    }

    virtual void IdleCB()
    {
        RenderSceneCB();
    }

    virtual void SpecialKeyboardCB(int Key, int x, int y)
    {
        m_pGameCamera->OnKeyboard(Key);
    }


    virtual void KeyboardCB(unsigned char Key, int x, int y)
    {
        switch (Key) {
            case 'q':
                glutLeaveMainLoop();
                break;

            case 'a':
                m_directionalLight.AmbientIntensity += 0.05f;
                break;

            case 's':
                m_directionalLight.AmbientIntensity -= 0.05f;
                break;
        }
    }


    virtual void PassiveMouseCB(int x, int y)
    {
        m_pGameCamera->OnMouse(x, y);
    }

private:

    GLuint m_VBO;
    GLuint m_IBO;
    LightingTechnique* m_pEffect;
    GradientTechnique* m_pGradientEffect;
    Mesh* m_pMesh;
    Camera* m_pGameCamera;
    float m_scale;
    DirectionalLight m_directionalLight;
};


int main(int argc, char** argv)
{
    GLUTBackendInit(argc, argv);

    if (!GLUTBackendCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 32, false, "OpenGL tutors")) {
        return 1;
    }

    Main* pApp = new Main();

    if (!pApp->Init()) {
        return 1;
    }

    pApp->Run();

    delete pApp;

    return 0;
}
