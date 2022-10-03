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
    //TriangleSurface* kart= new TriangleSurface("kart.txt");

    //Vi må derimot kalle init her siden alle her er visualObjects som enda ikke er initialisert.
    //auto her er "std::pair<std::string, VisualObject*>"
    for (auto& iter : mMap)
    {
        iter.second->init(0);
    }
}

void RenderWindow::createObjects()
{
    //mMap.insert(std::pair<std::string,VisualObject*>{"Grid", new GridPlane(240)});
    //mMap["Grid"]->Translate(0,2,0);

    mMap.insert(std::pair<std::string,VisualObject*>{"Triangles", new TriangleSurface("compressed.txt")});
    mMap.insert(std::pair<std::string,VisualObject*>{"Ball", new PhysicsObject(3,"plainshader")});
    mMap["Ball"]->Translate(80.0,80.0, 80.0);
}


// Called each frame - doing the rendering!!!
void RenderWindow::render()
{
    //Henter tiden mellom nå og forrige frame og lagrer den til senere
    //  "Denne tar jeg vare på :)" - Askeladden
    deltaTime = calculateFramerate();

    mContext->makeCurrent(this); //must be called every frame (every time mContext->swapBuffers is called)

    initializeOpenGLFunctions();    //must call this every frame it seems...

    //Oppdaterer kameraet (Sender kamera-matrisene videre
    //   til shaderCoordinatoren som oppdaterer alle lastede shadere).
    mCamera->update();
    mCubemap->draw(mCamera->mPmatrix, mCamera->mVmatrix);

    //Her gjør vi inputProcessing. Som blant annet ta mus-input og keyboard trykk
    inputProcessing();
    mCamera->tick(deltaTime);
    //Her oppdaterer vi spesifikke shadere for hånd, hovedsaklig for å sette lys og kamera posisjonene.
    updateShaders();

    //Nedenfor har vi de tre spillstadiene. Editor, Gameplay og Result.
    // Selv om det er mye copy-paste mellom dem, så er det nødvendig ettersom det er ingen god løsning
    //    å få isolert kun noen funksjon-kall uavhengig av gamestate.

    //Oppgave 13
    // når GAMESTATE er i editor modus så blir aldri "tick" kalt,
    // derfor står alt stille.
    if(GAMESTATE == STATE::EDITOR)
    {
        //Kaller tick (Sånn at spilleren kan fly rundt)
        // Tegner debugKameraet, (rosa/lilla kjeglen) for å vise hvor kameraet er i Gameplay staten.
        //Går gjennom alle objektene i hashmappet og tegner dem,
        // to ting her :
        //   1. setter DebugDisplay til true,
        //   2. Sender inn en 4x4 matrise som fungerer som "world-space";
        for(auto& it : mMap)
        {
            it.second->ToggleDebugDisplay(true);
            it.second->draw(glm::mat4{1.f});
            checkForGLerrors();
        }
    }
    else if (GAMESTATE == STATE::GAMEPLAY)
    {
        //Gjøre et spill-tick for alle i hashmappet (dynamiske objekter)

        for(auto& it : mMap)
            it.second->tick(deltaTime);

        //Sjekker kollisjon
//        checkCollision();

        //Tegne objektene i Hash-map
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

//This function is called from Qt when window is exposed (shown)
// and when it is resized
//exposeEvent is a overridden function from QWindow that we inherit from
void RenderWindow::exposeEvent(QExposeEvent *)
{
    if (!mInitialized)
        init();

    //This is just to support modern screens with "double" pixels (Macs and some 4k Windows laptops)
    const qreal retinaScale = devicePixelRatio();

    //Set viewport width and height to the size of the QWindow we have set up for OpenGL
    glViewport(0, 0, static_cast<GLint>(width() * retinaScale), static_cast<GLint>(height() * retinaScale));

    if(height() > 0.5)
        mCamera->cameraResize(FOV,(width()*retinaScale)/(height()*retinaScale),0.01,1000.0);

    //If the window actually is exposed to the screen we start the main loop
    //isExposed() is a function in QWindow
    if (isExposed())
    {
        //This timer runs the actual MainLoop == the render()-function
        //2 means 2ms = 500 Frames pr second (not that it actually runs at 500 fps :X)
        mRenderTimer->start(2);
    }
}

//Starter spillet. Denne blir kalt fra Qt GUI
void RenderWindow::StartGame()
{
    if(GAMESTATE != STATE::GAMEPLAY)
    {
        GAMESTATE = STATE::GAMEPLAY;
    }
}

//Går inn i Editoren. Denne blir kalt fra Qt GUI og fra Reset()
void RenderWindow::EnterEditor()
{
    if(GAMESTATE != STATE::EDITOR)
    {
        GAMESTATE = STATE::EDITOR;
    }
}

void RenderWindow::ResetGame()
{
    // Sette alle myntene og mynteverdiene tilbake.

    //Denne er her  så du kan se alle objektene resettes uten at kameraet går tilbake til sin "resatte" posisjon
    if(GAMESTATE != STATE::EDITOR)
        mCamera->Reset();

    //EnterEditor();

    //reset alle objekter i hashmap.
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

    float movementInput = 0;
    float rotationInput = 0;

    if(mKeyboard[Qt::Key_A])
    {
        inputDirection -= right;
        rotationInput += 1;
    }

    if(mKeyboard[Qt::Key_S])
    {
        inputDirection -= forward;
        movementInput -= 1;
    }

    if(mKeyboard [Qt::Key_W])
    {
        inputDirection += forward;
        movementInput += 1;
    }

    if(mKeyboard [Qt::Key_D])
    {
        inputDirection += right;
        rotationInput -= 1;
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
        movementInput *= 2;
        rotationInput *= 0.25;
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
    // Gir alle et physics-tick
    for(auto& obj : mMap)
    {
        // Teste distansen mellom punktet fra barysentriske til ballen er mindre enn radiusen til ballen

        // dir = bCoord - obj.position
        // glm::length(dir) <= obj.radius

        {
            //
        }

    }
}

void RenderWindow::updateShaders()
{
    //Dobbeltsjekker at lyset(NPC) er satt inn i scenen før jeg sender det til shaderen.

    //Sender inn kamera posisjonen. Denne trenger ikke en sjekk fordi det skal alltid være et kamera til stedet! ;)
    ShaderCoordinator::getInstance().updateShaderUniformVec3("blinn_phongshader", "cameraPosition", mCamera->getViewPosition());
    ShaderCoordinator::getInstance().updateShaderUniformVec3("blinn_phongtextureshader", "cameraPosition", mCamera->getViewPosition());
}

//The way this function is set up is that we start the clock before doing the draw call,
// and check the time right after it is finished (done in the render function)
//This will approximate what framerate we COULD have.
//The actual frame rate on your monitor is limited by the vsync and is probably 60Hz
double RenderWindow::calculateFramerate()
{
    //static int64_t elapsedTime;
    static double elapsedTime{0};
    //Brukt Sten Rune sin kode som utgangspunkt, endret noen små greier for å tilpasse min applikasjon
    static auto lastFrame = std::chrono::high_resolution_clock::now();
    const auto thisFrame = std::chrono::high_resolution_clock::now();

    const auto deltaTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(thisFrame - lastFrame).count(); //milliseconds
    const auto deltaTimeS = deltaTimeMs/1000.0;

    elapsedTime += deltaTimeS;
    //    elapsedTime+=nsecElapsed/1000000.f;
    static int frameCount{0};                       //counting actual frames for a quick "timer" for the statusbar

    static long AVGframerate;
    if (mMainWindow)            //if no mainWindow, something is really wrong...
    {
        ++frameCount;
        if (frameCount > 30)    //once pr 30 frames = update the message == twice pr second (on a 60Hz monitor)
        {
            AVGframerate = 1E3 / deltaTimeMs;
            frameCount = 0;     //reset to show a new message in 30 frames
        }
        //showing some statistics in status bar
        mMainWindow->statusBar()->showMessage(" Time pr FrameDraw: " + QString::number(deltaTimeMs, 'g', 4) + " ms "    //Draw cycle (limited by mRenderTimer at 2ms)
                                              "| Runtime: " + QString::number((int)elapsedTime) + " sec " +             //Elapsed Time
                                              "| FPS (approximated): " + QString::number(AVGframerate, 'g', 7));        //Framerate calculation
    }
    lastFrame = thisFrame;

    // Deltatime in seconds
    return deltaTimeS;
}

//Uses QOpenGLDebugLogger if this is present
//Reverts to glGetError() if not
void RenderWindow::checkForGLerrors()
{
    if(mOpenGLDebugLogger)  //if our machine got this class to work
    {
        const QList<QOpenGLDebugMessage> messages = mOpenGLDebugLogger->loggedMessages();
        for (const QOpenGLDebugMessage &message : messages)
        {
            if (!(message.type() == message.OtherType)) // get rid of uninteresting "object ...
                // will use VIDEO memory as the source for
                // buffer object operations"
                // valid error message:
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

//Tries to start the extended OpenGL debugger that comes with Qt
//Usually works on Windows machines, but not on Mac...
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

//Disse to funksjonene, Wireframe og Shaded blir kalt fra QT GUI-en.
void RenderWindow::Wireframe()
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}
void RenderWindow::Shaded()
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

//Event sent from Qt when program receives a keyPress
// NB - see renderwindow.h for signatures on keyRelease and mouse input
void RenderWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        mMainWindow->close();       //Shuts down the whole program
        return;
    }

    //Her setter jeg key en(Qt::key) inn i et hashmap med en boolean som verdi
    mKeyboard[event->key()] = true;
    QWindow::keyPressEvent(event);

}
void RenderWindow::keyReleaseEvent(QKeyEvent *event)
{
    //Når tasten slippes setter jeg verdien i hashmappen til false.
    mKeyboard[event->key()] = false;
    QWindow::keyReleaseEvent(event);
}

void RenderWindow::mousePressEvent(QMouseEvent *event)
{
    // Samme gjør jeg her og i funksjonen under med mus knappene.
    mMouse[event->button()] = true;
    QWindow::mousePressEvent(event);
}
void RenderWindow::mouseReleaseEvent(QMouseEvent *event)
{
    // bFirstClick blir brukt til i inputProcessing for å
    //  stoppe kameraet fra å "skippe" når man trykker gjentatte ganger.
    if(event->button() == Qt::RightButton)
        bFirstClick = true;
    mMouse[event->button()] = false;
    QWindow::mouseReleaseEvent(event);
}
