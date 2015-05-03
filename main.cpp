#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "pipeline.h"
#include "camera.h"
#include "texture.h"
#include "frame_technique.h"
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
        m_pFrameEffect = NULL;
        m_scale = 0.0f;
        m_directionalLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
        m_directionalLight.AmbientIntensity = 0.2f;
        m_directionalLight.DiffuseIntensity = 0.7f;
        m_directionalLight.Direction = Vector3f(1.0f, 0.0f, 1.0f);
        CreateVertexBuffer();
        CreateIndexBuffer();
        model_dx = -2.05f;

        wireflame_mode = false;
        camera_on_mouse = true;

        k = 3.935f;
        CouplingAColor = Vector3f(0.8f, 0.0f, 0.4f);
        CouplingBColor = Vector3f(0.0f, 0.8f, 0.0f);
        CouplingInitColor = Vector3f(0.0f, 0.0f, 0.8f);
        rotateStep = 0.3f;
    }

    virtual ~Main()
    {
        delete m_pFrameEffect;
        delete m_pGradientEffect;
        delete m_pEffect;
        delete m_pGameCamera;
        delete m_pBase;
        delete m_pCouplingA;
        delete m_pCouplingB;
        delete m_pCouplingInt;
    }

    bool Init()
    {
        Vector3f Pos(5.0f, 30.0f, 2.0f);
        Vector3f Target(0.0f, 0.0f, 1.0f);
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
            return false;
        }

        m_pFrameEffect = new FrameTechnique();

        if (!m_pFrameEffect->Init()) {
            printf("Error initializing the frame technique\n");
            return false;
        }

        m_pEffect->Enable();

        m_pBase = new Mesh();
        m_pCouplingA = new Mesh();
        m_pCouplingB = new Mesh();
        m_pCouplingInt = new Mesh();
        return m_pBase->LoadMesh("PEDYKTOP.stl") && m_pCouplingA->LoadMesh("STUPEN PROM.stl") && m_pCouplingB->LoadMesh("STUPEN B.stl") && m_pCouplingInt->LoadMesh("STUPEN TIX.stl");
    }

    void Run()
    {
        GLUTBackendRun(this);
    }

    void InitTwBar(TwBar *GUI){

        TwSetParam(GUI, NULL, "refresh", TW_PARAM_CSTRING, 1, "0.1");
        TwAddVarRW(GUI, "Angular velocity", TW_TYPE_DOUBLE, &rotateStep, " label='Ang. vel.' min=0.1 max=10 step=0.05 keyIncr=w keyDecr=W help='Angular velocity of a cogwheel.' ");
        TwAddVarRW(GUI, "Kof", TW_TYPE_FLOAT, &k, " label='Ang. vel.' min=0.01 max=10 step=0.005 help='Angular velocity of a cogwheel.' ");


        TwAddVarRW(GUI, "AmbientIntensity", TW_TYPE_FLOAT, &m_directionalLight.AmbientIntensity, " label='AmbientIntensity' step=0.05 help='AmbientIntensity' ");
        TwAddVarRW(GUI, "DiffuseIntensity", TW_TYPE_FLOAT, &m_directionalLight.DiffuseIntensity, " label='DiffuseIntensity' step=0.05 help='DiffuseIntensity' ");
        TwAddVarRW(GUI, "Coupling A", TW_TYPE_COLOR3F, &CouplingAColor, " group='Colors' ");
        TwAddVarRW(GUI, "Coupling B", TW_TYPE_COLOR3F, &CouplingBColor, " group='Colors' ");
        TwAddVarRW(GUI, "Coupling Init", TW_TYPE_COLOR3F, &CouplingInitColor, " group='Colors' ");

//        TwAddVarRW(GUI, "", TW_TYPE_FLOAT, &k, " label='' step=0.05 help='' ");

    }

    virtual void RenderSceneCB()
    {
        m_pGameCamera->OnRender();
        m_directionalLight.Direction = m_pGameCamera->GetTarget();

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        m_scale += rotateStep;

        Pipeline p;
        p.Scale(0.005f, 0.005f, 0.005f);
        p.Rotate(0.0f, m_scale, 0.0f);
        p.WorldPos(0.0f + model_dx, 0.0f, 0.0f);
        p.SetCamera(m_pGameCamera->GetPos(), m_pGameCamera->GetTarget(), m_pGameCamera->GetUp());
        p.SetPerspectiveProj(60.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 0.1f, 100.0f);


        glDepthFunc(GL_LEQUAL);

        // Render background
        m_pGradientEffect->Enable();
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glDisableVertexAttribArray(0);


        glDepthFunc(GL_LESS);

        p.Scale(0.005f, 0.005f, 0.005f);
        p.Rotate(90.0f, 0.0f, 0.0f);
        RenderMesh(m_pBase, Vector3f(0.8f, 0.8f, 0.8f), p);


        p.Scale(0.005f, 0.005f, 0.005f);
        p.Rotate(0.0f, 270.0f, - m_scale * k);
        p.WorldPos(2.55f + model_dx, 1.15f, 0.625f);
        RenderMesh(m_pCouplingA, CouplingAColor, p);

        p.WorldPos(3.55f + model_dx, 1.15f, 1.20f);

        float k2 = 4.5f;
        p.Rotate(0.0f, 270.0f, m_scale * k * k2);
        RenderMesh(m_pCouplingB, CouplingBColor, p);

        p.WorldPos(1.30 + model_dx, 1.15f, 0.645f);
        p.Rotate(0.0f, 270.0f, m_scale);
        RenderMesh(m_pCouplingInt, CouplingInitColor, p);

        TwDraw();
        glutSwapBuffers();
    }

    virtual void RenderMesh(Mesh* p_mesh, const Vector3f &color, Pipeline &p) {
        if(wireflame_mode) {
            m_pFrameEffect->Enable();
            m_pFrameEffect->SetWVP(p.GetWVPTrans());

            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
            p_mesh->Render();
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        }


        // Render mesh
        m_pEffect->Enable();

        m_pEffect->SetColor(color);
        m_pEffect->SetWVP(p.GetWVPTrans());
        m_pEffect->SetWorldMatrix(p.GetWorldTrans());
        m_pEffect->SetDirectionalLight(m_directionalLight);
        m_pEffect->SetEyeWorldPos(m_pGameCamera->GetPos());
        m_pEffect->SetMatSpecularIntensity(0.0f);
        m_pEffect->SetMatSpecularPower(0);

        p_mesh->Render();
    }

    virtual void IdleCB()
    {
        RenderSceneCB();
    }

    virtual void SpecialKeyboardCB(int Key, int x, int y)
    {
        TwEventSpecialGLUT(Key, x, y);
        m_pGameCamera->OnKeyboard(Key);
    }


    virtual void KeyboardCB(unsigned char Key, int x, int y)
    {
        TwEventKeyboardGLUT(Key, x,y);
        switch (Key) {
            case 'q':
                glutLeaveMainLoop();
                break;
            case 'm':
                if (!camera_on_mouse) {
                    m_pGameCamera->WarpMouse();
                }
                camera_on_mouse = !camera_on_mouse;
                break;
            case 'a':
                m_directionalLight.AmbientIntensity += 0.05f;
                break;
            case 's':
                m_directionalLight.AmbientIntensity -= 0.05f;
                break;
            case 'z':
                m_directionalLight.DiffuseIntensity += 0.05f;
                break;
            case 'x':
                m_directionalLight.DiffuseIntensity -= 0.05f;
                break;
            case 'w':
                wireflame_mode = !wireflame_mode;
                break;
        }
    }


    virtual void PassiveMouseCB(int x, int y)
    {        
        TwEventMouseMotionGLUT(x, y);
        if (camera_on_mouse) {
            m_pGameCamera->OnMouse(x, y);
        }
    }

private:

    GLuint m_VBO;
    GLuint m_IBO;
    LightingTechnique* m_pEffect;
    GradientTechnique* m_pGradientEffect;
    FrameTechnique* m_pFrameEffect;
    Mesh *m_pBase, *m_pCouplingA, *m_pCouplingB, *m_pCouplingInt;
    Vector3f CouplingAColor, CouplingInitColor, CouplingBColor;
    Camera* m_pGameCamera;
    float m_scale;
    DirectionalLight m_directionalLight;

    double rotateStep;

    double model_dx;
    bool wireflame_mode;
    bool camera_on_mouse;
    float k;
};


int main(int argc, char** argv)
{
    GLUTBackendInit(argc, argv);

    if (!GLUTBackendCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 32, false, "Simulation of two-stage reduction gear")) {
        return 1;
    }

    Main* pApp = new Main();

    if (!pApp->Init()) {
        return 1;
    }
    glLineWidth(2);

    pApp->Run();

    delete pApp;

    return 0;
}
