#include "renderwindow.h"
#include <QTimer>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLDebugLogger>
#include <QKeyEvent>
#include <QStatusBar>
#include <QDebug>
#include <string>
#include "toolbox.h"
#include "trianglesurface.h"
#include "octahedronball.h"
#include "physicsobject.h"
#include "camera.h"
#include "gridplane.h"
#include "shadercoordinator.h"
#include "mesh.h"
#include "shader.h"
#include "logger.h"
#include "mainwindow.h"
#include "cubemap.h"


#define FOV 50.f

RenderWindow::RenderWindow(const QSurfaceFormat &format, MainWindow *mainWindow)
    : mContext(nullptr), mInitialized(false), mMainWindow(mainWindow)

{
    //This is sent to QWindow:
    setSurfaceType(QWindow::OpenGLSurface);
    setFormat(format);
    //Make the OpenGL context
    mContext = new QOpenGLContext(this);
    //Give the context the wanted OpenGL format (v4.1 Core)
    mContext->setFormat(requestedFormat());
    if (!mContext->create())
    {
        delete mContext;
        mContext = nullptr;
        qDebug() << "Context could not be made - quitting this application";
    }

    mRenderTimer = new QTimer(this);
}

RenderWindow::~RenderWindow()
{

}

void RenderWindow::init()
{
    //Get the instance of the utility Output logger
    //Have to do this, else program will crash (or you have to put in nullptr tests...)
    mLogger = Logger::getInstance();

    //Connect the gameloop timer to the render function:
    //This makes our render loop
    connect(mRenderTimer, SIGNAL(timeout()), this, SLOT(render()));
    //********************** General OpenGL stuff **********************

    //The OpenGL context has to be set.
    //The context belongs to the instanse of this class!
    if (!mContext->makeCurrent(this)) {
        mLogger->logText("makeCurrent() failed", LogType::REALERROR);
        return;
    }
    if (!mInitialized)
        mInitialized = true;

    //must call this to use OpenGL functions
    initializeOpenGLFunctions();
    mLogger->logText("The active GPU and API:", LogType::HIGHLIGHT);
    std::string tempString;
    tempString += std::string("  Vendor: ") + std::string((char*)glGetString(GL_VENDOR)) + "\n" +
            std::string("  Renderer: ") + std::string((char*)glGetString(GL_RENDERER)) + "\n" +
            std::string("  Version: ") + std::string((char*)glGetString(GL_VERSION));
    mLogger->logText(tempString);
    startOpenGLDebugger();

    //general OpenGL stuff:
    glEnable(GL_DEPTH_TEST);            //enables depth sorting - must then use GL_DEPTH_BUFFER_BIT in glClear
    //gimglEnable(GL_CULL_FACE);       //draws only front side of models - usually what you want - test it out!
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);    //gray color used in glClear GL_COLOR_BUFFER_BIT

    //Lager Cubemappet og initialiserer det.
    mCubemap = new Cubemap();
    mCubemap->init();

    //Lager kameraet, setter perspektiv og gir en posisjon som den skal se mot
    mCamera = new Camera();
    mCamera->perspective(FOV, 1920/1080, 0.01, 1000.0);
    mCamera->lookAt( {0, 0, 0});

    //Her er kalles funksjonen som lager nesten alle objektene, utenom de vi har gått igjennom ovenfor
    createObjects();

    //Vi må derimot kalle init her siden alle her er visualObjects som enda ikke er initialisert.
    //auto her er "std::pair<std::string, VisualObject*>"
    for (auto& iter : mMap)
    {
        iter.second->init(0);
    }
}

void RenderWindow::createObjects()
{
    mMap.insert(std::pair<std::string,VisualObject*>{"Grid", new GridPlane(240)});
    //mMap["Grid"]->Translate(0,2,0);

    mMap.insert(std::pair<std::string,VisualObject*>{"Triangles", new TriangleSurface("Fjell2_compressed.txt")});
    mMap.insert(std::pair<std::string,VisualObject*>{"Ball", new PhysicsObject(3,"plainshader")});
    mMap["Ball"]->Translate(15.0,10.0, 15.0);
}


void RenderWindow::render()
{
    deltaTime = calculateFramerate();

    mContext->makeCurrent(this); //must be called every frame (every time mContext->swapBuffers is called)

    initializeOpenGLFunctions();    //must call this every frame it seems...

    mCamera->update();
    mCubemap->draw(mCamera->mPmatrix, mCamera->mVmatrix);

    inputProcessing();
    mCamera->tick(deltaTime);
    updateShaders();

    if(GAMESTATE == STATE::EDITOR)
    {
        for(auto& it : mMap)
        {
            it.second->ToggleDebugDisplay(true);
            it.second->draw(glm::mat4{1.f});
            checkForGLerrors();
        }
    }
    else if (GAMESTATE == STATE::GAMEPLAY)
    {
        for(auto& it : mMap)
            it.second->tick(deltaTime);

        for(auto& it : mMap)
        {
            it.second->ToggleDebugDisplay(true);
            it.second->draw(glm::mat4{1.f});
        }

        checkForGLerrors();
    }
    else
    {
        mCamera->tick(deltaTime);
        for(auto& it : mMap)
        {
            it.second->ToggleDebugDisplay(false);
            it.second->draw(glm::mat4{1.f});
        }
    }

    mContext->swapBuffers(this);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderWindow::exposeEvent(QExposeEvent *)
{
    if (!mInitialized)
        init();

    const qreal retinaScale = devicePixelRatio();

    glViewport(0, 0, static_cast<GLint>(width() * retinaScale), static_cast<GLint>(height() * retinaScale));

    if(height() > 0.5)
        mCamera->cameraResize(FOV,(width()*retinaScale)/(height()*retinaScale),0.01,1000.0);

    if (isExposed())
    {
        mRenderTimer->start(2);
    }
}

void RenderWindow::StartGame()
{
    if(GAMESTATE != STATE::GAMEPLAY)
    {
        GAMESTATE = STATE::GAMEPLAY;
    }
}

void RenderWindow::EnterEditor()
{
    if(GAMESTATE != STATE::EDITOR)
    {
        GAMESTATE = STATE::EDITOR;
    }
}

void RenderWindow::ResetGame()
{
    if(GAMESTATE != STATE::EDITOR)
        mCamera->Reset();

    //EnterEditor();
    for(auto& vo : mMap)
    {
        vo.second->reset();
    }
}

void RenderWindow::checkGameCompletion()
{

}

void RenderWindow::CameraTopDown(const bool& t)
{
    if(mCamera)
        mCamera->TopDown(t);
}

//Oppgave 4 og Oppgave 5
void RenderWindow::inputProcessing()
{
    glm::vec3 inputDirection{0.f,0.f,0.f};
    glm::vec3 forward = mCamera->forward();
    glm::vec3 right = mCamera->right();
    glm::vec3 upward = mCamera->up();

    if(mKeyboard[Qt::Key_A])
    {
        inputDirection -= right;
    }

    if(mKeyboard[Qt::Key_S])
    {
        inputDirection -= forward;
    }

    if(mKeyboard [Qt::Key_W])
    {
        inputDirection += forward;
    }

    if(mKeyboard [Qt::Key_D])
    {
        inputDirection += right;
    }

    if(mKeyboard [Qt::Key_E])
        inputDirection += upward;

    if(mKeyboard [Qt::Key_Q])
        inputDirection -= upward;

    if(glm::length(inputDirection) > 0)
        inputDirection = glm::normalize(inputDirection);

    if(mKeyboard [Qt::Key_Shift])
    {
        inputDirection *= 2;
    }
    if(mKeyboard [Qt::Key_Control])
    {
        inputDirection *= 0.5;
    }

    if(true)
    {
        if(mMouse[Qt::RightButton])
        {
            this->setCursor((QCursor(Qt::BlankCursor)));

            if(bFirstClick)
            {
                bFirstClick = false;
                QCursor::setPos(1920/2, 1080/2);
                return;
            }
            mousePos.x = QCursor::pos().x();
            mousePos.y = QCursor::pos().y();
            mCamera->LookAround(mousePos - center);
            mCamera->movementVector = inputDirection;
            QCursor::setPos(1920/2, 1080/2);
        }
        else
        {
            this->setCursor((QCursor(Qt::ArrowCursor)));
        }
    }
}


void RenderWindow::checkCollision()
{
    for(auto& obj : mMap)
    {

    }
}

void RenderWindow::updateShaders()
{
    ShaderCoordinator::getInstance().updateShaderUniformVec3("blinn_phongshader", "cameraPosition", mCamera->getViewPosition());
    ShaderCoordinator::getInstance().updateShaderUniformVec3("blinn_phongtextureshader", "cameraPosition", mCamera->getViewPosition());
}

double RenderWindow::calculateFramerate()
{
    static double elapsedTime{0};
    //Brukt Sten Rune sin kode som utgangspunkt, endret noen små greier for å tilpasse min applikasjon
    static auto lastFrame = std::chrono::high_resolution_clock::now();
    const auto thisFrame = std::chrono::high_resolution_clock::now();

    const auto deltaTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(thisFrame - lastFrame).count(); //milliseconds
    const auto deltaTimeS = deltaTimeMs/1000.0;

    elapsedTime += deltaTimeS;
    static int frameCount{0};

    static long AVGframerate;
    if (mMainWindow)
    {
        ++frameCount;
        if (frameCount > 30)
        {
            AVGframerate = 1E3 / deltaTimeMs;
            frameCount = 0;
        }
        mMainWindow->statusBar()->showMessage(" Time pr FrameDraw: " + QString::number(deltaTimeMs, 'g', 4) + " ms "    //Draw cycle (limited by mRenderTimer at 2ms)
                                              "| Runtime: " + QString::number((int)elapsedTime) + " sec " +             //Elapsed Time
                                              "| FPS (approximated): " + QString::number(AVGframerate, 'g', 7));        //Framerate calculation
    }
    lastFrame = thisFrame;
    return deltaTimeS;
}

void RenderWindow::checkForGLerrors()
{
    if(mOpenGLDebugLogger)
    {
        const QList<QOpenGLDebugMessage> messages = mOpenGLDebugLogger->loggedMessages();
        for (const QOpenGLDebugMessage &message : messages)
        {
            if (!(message.type() == message.OtherType))
            {
                if(message.PerformanceType)
                    qDebug() << message.message().toStdString().c_str();
                else
                    mLogger->logText(message.message().toStdString(), LogType::REALERROR);
            }
        }
    }
    else
    {
        GLenum err = GL_NO_ERROR;
        while((err = glGetError()) != GL_NO_ERROR)
        {
            mLogger->logText("glGetError returns " + std::to_string(err), LogType::REALERROR);
            switch (err) {
            case 1280:
                mLogger->logText("GL_INVALID_ENUM - Given when an enumeration parameter is not a "
                                 "legal enumeration for that function.");
                break;
            case 1281:
                mLogger->logText("GL_INVALID_VALUE - Given when a value parameter is not a legal "
                                 "value for that function.");
                break;
            case 1282:
                mLogger->logText("GL_INVALID_OPERATION - Given when the set of state for a command "
                                 "is not legal for the parameters given to that command. "
                                 "It is also given for commands where combinations of parameters "
                                 "define what the legal parameters are.");
                break;
            }
        }
    }
}

void RenderWindow::startOpenGLDebugger()
{
    QOpenGLContext * temp = this->context();
    if (temp)
    {
        QSurfaceFormat format = temp->format();
        if (! format.testOption(QSurfaceFormat::DebugContext))
            mLogger->logText("This system can not use QOpenGLDebugLogger, so we revert to glGetError()",
                             LogType::HIGHLIGHT);

        if(temp->hasExtension(QByteArrayLiteral("GL_KHR_debug")))
        {
            mLogger->logText("This system can log extended OpenGL errors", LogType::HIGHLIGHT);
            mOpenGLDebugLogger = new QOpenGLDebugLogger(this);
            if (mOpenGLDebugLogger->initialize()) // initializes in the current context
                mLogger->logText("Started Qt OpenGL debug logger");
        }
    }
}

void RenderWindow::Wireframe()
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}
void RenderWindow::Shaded()
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void RenderWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        mMainWindow->close();
        return;
    }
    mKeyboard[event->key()] = true;
    QWindow::keyPressEvent(event);

}
void RenderWindow::keyReleaseEvent(QKeyEvent *event)
{
    mKeyboard[event->key()] = false;
    QWindow::keyReleaseEvent(event);
}

void RenderWindow::mousePressEvent(QMouseEvent *event)
{
    mMouse[event->button()] = true;
    QWindow::mousePressEvent(event);
}
void RenderWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
        bFirstClick = true;
    mMouse[event->button()] = false;
    QWindow::mouseReleaseEvent(event);
}
