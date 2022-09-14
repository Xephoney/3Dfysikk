QT          += core gui widgets opengl

TEMPLATE    = app
CONFIG      += c++17

TARGET      = 3D-programmering

SOURCES += main.cpp \
    Vertex.cpp \
    bounds.cpp \
    camera.cpp \
    cubemap.cpp \
    gridplane.cpp \
    logger.cpp \
    mainwindow.cpp \
    mesh.cpp \
    octahedronball.cpp \
    physicsobject.cpp \
    quadtree.cpp \
    renderwindow.cpp \
    shader.cpp \
    src/vendor/stb_image/stb_image.cpp \
    texture.cpp \
    trianglesurface.cpp \
    visuallineobject.cpp \
    visualobject.cpp


HEADERS += \
    Vertex.h \
    WorldCoordinator.h \
    bounds.h \
    camera.h \
    cubemap.h \
    gridplane.h \
    logger.h \
    mainwindow.h \
    mesh.h \
    octahedronball.h \
    physicsobject.h \
    quadtree.h \
    renderwindow.h \
    shader.h \
    shadercoordinator.h \
    src/vendor/stb_image/stb_image.h \
    texture.h \
    toolbox.h \
    trianglesurface.h \
    visuallineobject.h \
    visualobject.h

# external libs
    glm/glm.hpp
    src/vendor/stb_image/stb_image.h

FORMS += \
    mainwindow.ui

DISTFILES += \
    phongshader.frag \
    phongshader.vert \
    plainshader.frag \
    plainshader.vert \
    shaders/blinn_phongshader.frag \
    shaders/blinn_phongshader.vert \
    shaders/blinn_phongtextureshader.frag \
    shaders/blinn_phongtextureshader.vert \
    shaders/phongshader.frag \
    shaders/phongshader.vert \
    shaders/plainshader.frag \
    shaders/plainshader.vert \
    shaders/skybox.frag \
    shaders/skybox.vert \
    shaders/unlit_texture.frag \
    shaders/unlit_texture.vert

win32 {
    INCLUDEPATH += ./assimp/include
    LIBS += $$PWD/assimp/libs/libassimp.lib

# Copy required DLL to output directory
        CONFIG(debug, debug|release) {
            AssImp.commands = copy /Y \"$$PWD\\assimp\\dlls\\libassimp-5.dll\" debug
            AssImp.target = debug/libassimp-5.dll

            QMAKE_EXTRA_TARGETS += AssImp
            PRE_TARGETDEPS += debug/libassimp-5.dll
        } else:CONFIG(release, debug|release) {
            AssImp.commands = copy /Y \"$$PWD\\assimp\\dlls\\libassimp-5.dll\" release
            AssImp.target = release/libassimp-5.dll

            QMAKE_EXTRA_TARGETS += AssImp
            PRE_TARGETDEPS += release/libassimp-5.dll
        } else {
            error(Unknown set of dependencies.)
        }
}
