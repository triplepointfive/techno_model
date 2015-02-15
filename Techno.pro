TEMPLATE = app
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += /usr/include/bullet/ /usr/include/ImageMagick
LIBS += -lGL -lGLU -lXrandr -lXext -lX11 -lrt -lglut -lGLEW -lBulletDynamics -lBulletCollision -lLinearMath -lAntTweakBar -lMagick++ -lMagickCore -lassimp

SOURCES += main.cpp \
    glut_backend.cpp \
    lighting_technique.cpp \
    math_3d.cpp \
    mesh.cpp \
    pipeline.cpp \
    technique.cpp \
    texture.cpp \
    camera.cpp

OTHER_FILES += \
    phoenix_ugv.stl \
    phoenix_ugv1.md2 \
    phoenix.pcx \
    tutorial22 \
    Techno.config \
    Techno.files \
    Techno.includes \
    test.png \
    white.png \
    Techno.creator.user \
    Techno.pro.user \
    Techno.creator \
    Makefile \

HEADERS += \
    glut_backend.h \
    lighting_technique.h \
    math_3d.h \
    mesh.h \
    pipeline.h \
    technique.h \
    texture.h \
    util.h \
    callbacks.h \
    camera.h

