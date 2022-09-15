 #pragma once

#include <QWindow>
#include <QOpenGLFunctions_4_1_Core>
#include <QTimer>
#include <QElapsedTimer>
#include "visualobject.h"
#include "quadtree.h"
#include "texture.h"
#include "PhysicsObject.h"
#include "octahedronball.h"

class QOpenGLContext;
class Shader;
class MainWindow;

enum class STATE
{
    EDITOR = 0,     //Debug modus, Alt er synlig
    GAMEPLAY = 1,   //Gameplay, er tar spilleren input, og alle gameplay mekanikker kjøres
    RESULT = 2,     //Her tegnes alt i tillegg til billboardet som viser resultatet.
};

/// This inherits from QWindow to get access to the Qt functionality and
// OpenGL surface.
// We also inherit from QOpenGLFunctions, to get access to the OpenGL functions
// This is the same as using "glad" and "glw" from general OpenGL tutorials
class RenderWindow : public QWindow, protected QOpenGLFunctions_4_1_Core
{
    Q_OBJECT
public:
    RenderWindow(const QSurfaceFormat &format, MainWindow *mainWindow);
    ~RenderWindow() override;

    QOpenGLContext *context() { return mContext; }

    void exposeEvent(QExposeEvent *) override;  //gets called when app is shown and resized

    //Oppgave 6. disse funksjonene kalles fra knappene i QT UI-en
    void StartGame();
    void EnterEditor();
    void ResetGame();
    void checkGameCompletion();


    void inputProcessing();
    void checkCollision();

    void updateShaders();

    bool mTogglePath{true};

    //Camera variables and functions
    void CameraTopDown(const bool&);
    bool mCameraRotate{false};
    bool bDoorOpen {false};

    class Camera* mCamera               { nullptr };
    class Cubemap* mCubemap             { nullptr };


    void Wireframe();
    void Shaded();

    STATE GAMESTATE = STATE::EDITOR;

    double deltaTime = 0;
private slots:
    void render();          //the actual render - function

private:
    bool bDebugMode = true;
    bool bFirstClick = true;

    glm::vec2 mousePos;
    glm::vec2 center{1920/2, 1080/2};
    void init();            //initialize things we need before rendering
    void createObjects();

    std::vector<VisualObject*> mObjects;
    std::unordered_map<std::string, VisualObject*> mMap;
    gsml::QuadTree<std::string,class InteractableObject*> mTre;

    QOpenGLContext *mContext{nullptr};  //Our OpenGL context
    bool mInitialized{false};

    GLint  mModelMatrixUniform {0} ;              //OpenGL reference to the Uniform in the shader program

    QTimer *mRenderTimer{nullptr};          //timer that drives the gameloop
    QTimer *mPhysicsTimer{nullptr};          //timer that drives the gameloop
    QElapsedTimer mTimeStart;               //time variable that reads the calculated FPS
    std::unordered_map<int, bool> mKeyboard;
    std::unordered_map<int, bool> mMouse;

    MainWindow *mMainWindow{nullptr};        //points back to MainWindow to be able to put info in StatusBar

    class QOpenGLDebugLogger *mOpenGLDebugLogger{nullptr};  //helper class to get some clean debug info from OpenGL
    class Logger *mLogger{nullptr};         //logger - Output Log in the application

    ///Helper function that uses QOpenGLDebugLogger or plain glGetError()
    void checkForGLerrors();

    double calculateFramerate();          //as name says

    ///Starts QOpenGLDebugLogger if possible
    void startOpenGLDebugger();

protected:
    //The QWindow that we inherit from have these functions to capture
    // - mouse and keyboard.
    // Uncomment to use (you also have to make the definitions of
    // these functions in the cpp-file to use them of course!)
    //
    //    void mousePressEvent(QMouseEvent *event) override{}
    //    void mouseMoveEvent(QMouseEvent *event) override{}
    void keyPressEvent(QKeyEvent *event) override;              //the only one we use now
    void keyPressAction();
    void keyReleaseEvent(QKeyEvent *event) override;
    //void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    //    void wheelEvent(QWheelEvent *event) override{}
};
