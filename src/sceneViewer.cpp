#include "sceneViewer.h"

#include <QKeyEvent>
#include <QFrame>
#include <QDir>
#include <QVector>

#include <limits>

#include "timer.h"

SceneViewer::SceneViewer(Ui_MainWindow *userInterface, Ui_TransformsDialog *transformsDialog):
    _matching(false),
    _shapeClipping(false),
    _merge(false),
    _objectSelected(-1)
{
    _userInterface = userInterface;
    _transformsDialog = transformsDialog;
    _sceneCamera.reset(this->camera());
    _modelReader.reset(new ModelReader());
    _cloudTools.reset(new CloudTools());
    _scenePlayer.reset(new ScenePlayer());

    _dialog.reset(new QDialog());
    _transformsDialog->setupUi(_dialog.get());
}

SceneViewer::~SceneViewer()
{
    glDeleteBuffers(1, &_vertexBuffer);
    glDeleteBuffers(1, &_colorBuffer);
}

void SceneViewer::init()
{
    setSceneRadius(10.0);
    setSceneCenter(Vec(0.0, 0.0, 0.0));
    setBackgroundColor(QColor(0.0, 0.0, 0.0));
    _sceneCamera->setType(Camera::ORTHOGRAPHIC);
    topCameraView();
    _sceneCamera->setZNearCoefficient(0.01f);
    //setMouseBinding(Qt::ShiftModifier | Qt::RightButton, CAMERA, TRANSLATE);
    //setMouseBinding(Qt::NoModifier | Qt::RightButton, SELECT);

    glewExperimental = GL_TRUE;
    glewInit();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LESS);

    glGenVertexArrays(1, &_vertexArrayID);
    glBindVertexArray(_vertexArrayID);

    glGenBuffers(1, &_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);

    glGenBuffers(1, &_colorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _colorBuffer);

    loadShaders();
}

void SceneViewer::initScenePlayer(const uint frameCount)
{
    _scenePlayer->setFrameCount(frameCount);
}

std::shared_ptr<ScenePlayer> SceneViewer::getScenePlayer()
{
    return _scenePlayer;
}

void SceneViewer::animate()
{
    if (_scenePlayer->isReady() && !_scenePlayer->isPaused())
        _scenePlayer->getNextFrame();
}

void SceneViewer::loadShaders()
{
    _shaderID[0] = glCreateShader(GL_VERTEX_SHADER);
    _shaderID[1] = glCreateShader(GL_FRAGMENT_SHADER);

    GLint result = GL_FALSE;
    int infoLogLength;

    ShaderSources shaderSources;

    const char* vertexSourcePointer = shaderSources.VERTEX_SHADER_DEFAULT.c_str();
    glShaderSource(_shaderID[0], 1, &vertexSourcePointer, NULL);
    glCompileShader(_shaderID[0]);

    glGetShaderiv(_shaderID[0], GL_COMPILE_STATUS, &result);
    glGetShaderiv(_shaderID[0], GL_INFO_LOG_LENGTH, &infoLogLength);
    std::vector<char> vertexShaderErrorMessage(infoLogLength);
    glGetShaderInfoLog(_shaderID[0], infoLogLength, NULL, &vertexShaderErrorMessage[0]);

    if (vertexShaderErrorMessage[0] != NULL)
    {
        std::vector<char> vertexShaderErrorMessage(infoLogLength+1);
        glGetShaderInfoLog(_shaderID[0], infoLogLength, NULL, &vertexShaderErrorMessage[0]);
        std::cout << &vertexShaderErrorMessage[0] << std::endl;
    }

    char const* fragmentSourcePointer = shaderSources.FRAGMENT_SHADER_DEFAULT.c_str();
    glShaderSource(_shaderID[1], 1, &fragmentSourcePointer , NULL);
    glCompileShader(_shaderID[1]);

    glGetShaderiv(_shaderID[1], GL_COMPILE_STATUS, &result);
    glGetShaderiv(_shaderID[1], GL_INFO_LOG_LENGTH, &infoLogLength);
    std::vector<char> fragmentShaderErrorMessage(infoLogLength);
    glGetShaderInfoLog(_shaderID[1], infoLogLength, NULL, &fragmentShaderErrorMessage[0]);

    if (fragmentShaderErrorMessage[0] != NULL)
    {
        std::vector<char> fragmentShaderErrorMessage(infoLogLength+1);
        glGetShaderInfoLog(_shaderID[1], infoLogLength, NULL, &fragmentShaderErrorMessage[0]);
        std::cout << &fragmentShaderErrorMessage[0] << std::endl;
    }

    _programID = glCreateProgram();
    glAttachShader(_programID, _shaderID[0]);
    glAttachShader(_programID, _shaderID[1]);
    glLinkProgram(_programID);

    glGetProgramiv(_programID, GL_LINK_STATUS, &result);
    glGetProgramiv(_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
    std::vector<char> programErrorMessage(std::max(infoLogLength, int(1)));
    glGetProgramInfoLog(_programID, infoLogLength, NULL, &programErrorMessage[0]);

    if (programErrorMessage[0] != NULL)
    {
        std::vector<char> programErrorMessage(infoLogLength+1);
        glGetShaderInfoLog(_programID, infoLogLength, NULL, &programErrorMessage[0]);
        std::cout << &programErrorMessage[0] << std::endl;
    }

    _matrixID = glGetUniformLocation(_programID, "matrix");
    _alphaID = glGetUniformLocation(_programID, "alpha");

    glDeleteShader(_shaderID[0]);
    glDeleteShader(_shaderID[1]);
}

/*void printMat(glm::mat4  mat)
{
    int i,j;
    for (j=0; j<4; j++){
        for (i=0; i<4; i++){
            printf("%f ",mat[i][j]);
        }
        printf("\n");
    }
}*/

void SceneViewer::setMatching(const bool matching)
{
    _matching = matching;
    if (_matching)
    {
        uint frameCount = _scenePlayer->getFrameCount()/3;
        _userInterface->hsCurrentFrame->setMaximum(frameCount);
        _scenePlayer->init(frameCount);
    }
    else
    {
        uint frameCount = _modelReader->getSceneObjects().getSceneSize();
        _userInterface->hsCurrentFrame->setMaximum(frameCount);
        _scenePlayer->init(frameCount);
        _objectSelected = -1;
    }
}

void SceneViewer::setShapeClipping(const bool clipping)
{
    _shapeClipping = clipping;

    uint currentFrame = _scenePlayer->getCurrentFrame();
    Object object = _modelReader->getObject(currentFrame);
    _cloudTools->clip(object);
}

void SceneViewer::drawGeometry(const uint povSize)
{
    for (uint i = 0; i < povSize; ++i)
    {
        Object object = _modelReader->getObject(_scenePlayer->getCurrentFrame(), i);

        glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, object.getVertexCount() * 3 * sizeof(GLfloat), &object.getPositions().at(0), GL_STREAM_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, _colorBuffer);
        glBufferData(GL_ARRAY_BUFFER, object.getVertexCount() * 3 * sizeof(GLfloat), &object.getColors().at(0), GL_STREAM_DRAW);

        glUseProgram(_programID);

        GLdouble matrix[16];
        _sceneCamera->getModelViewProjectionMatrix(matrix);
        _matrix = glm::make_mat4(matrix);

        glUniformMatrix4fv(_matrixID, 1, GL_FALSE, &_matrix[0][0]);

        if (_objectSelected == -1)
            glUniform1f(_alphaID, 1.0f);
        else
        {
            if (i == _objectSelected)
                glUniform1f(_alphaID, 1.0f);
            else
                glUniform1f(_alphaID, 0.25f);
        }

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, _colorBuffer);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glDrawArrays(GL_POINTS, 0, object.getVertexCount());
    }

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
}

void SceneViewer::mergeObjects()
{
    _userInterface->statusBar->showMessage("Merging matching clouds...");
    uint povSize = _modelReader->getSceneObjects().getIdSize();

    uint frameCount = _scenePlayer->getFrameCount() / povSize;
    SceneObjects sceneObjects;
    for (uint f = 0; f < frameCount; ++f)
    {
        std::vector<Object> objects;
        for (uint i = 0; i < povSize; ++i)
            objects.push_back(_modelReader->getObject(f, i));

        Object object;
        if (povSize == 3)
            object = _cloudTools->merge({objects.at(0), objects.at(1), objects.at(2)});
        else
            object = _cloudTools->merge({objects.at(0)});

        sceneObjects.addObject(object);
    }

    _modelReader->setSceneObjects(sceneObjects);
    frameCount = _modelReader->getSceneObjects().getSceneSize();
    _scenePlayer->init(frameCount);
    _userInterface->hsCurrentFrame->setMaximum(frameCount);
    _userInterface->statusBar->showMessage("Merging done.", 3000);
}

void SceneViewer::draw()
{
    _userInterface->eCurrentFrame->setText(QString::number(_scenePlayer->getCurrentFrame()+1));

    if (isReady())
        if (_matching)
            drawGeometry(_modelReader->getSceneObjects().getIdSize());
        else
            drawGeometry();
}

void SceneViewer::centerCamera()
{
    /*q.w = cos(angle / 2)
    q.x = axis.x * sin( angle / 2)
    q.y = axis.y * sin( angle / 2)
    q.z = axis.z * sin( angle / 2)*/

    Quaternion q;
    q.setAxisAngle(Vec(1.0, 0.0, 0.0), -45.0 * (M_PI/180.0));
    _sceneCamera->setOrientation(q);
    _sceneCamera->setPosition(Vec(0.0, 5.0, 5.0));
    _userInterface->statusBar->showMessage("Center view", 3000);
}

void SceneViewer::frontCameraView()
{
    Quaternion q;
    q.setAxisAngle(Vec(0.0, 0.0, 0.0), 0.0 * (M_PI/180.0));
    _sceneCamera->setOrientation(q);
    _sceneCamera->setPosition(Vec(0.0, 1.5, 5.0));
    _userInterface->statusBar->showMessage("Front view", 3000);
}

void SceneViewer::rightCameraView()
{
    Quaternion q;
    q.setAxisAngle(Vec(0.0, 1.0, 0.0), 90.0 * (M_PI/180.0));
    _sceneCamera->setOrientation(q);
    _sceneCamera->setPosition(Vec(5.0, 1.5, 0.0));
    _userInterface->statusBar->showMessage("Right view", 3000);
}

void SceneViewer::topCameraView()
{
    Quaternion q;
    q.setAxisAngle(Vec(1.0, 0.0, 0.0), -90.0 * (M_PI/180.0));
    _sceneCamera->setOrientation(q);
    _sceneCamera->setPosition(Vec(0.0, 5.0, 0.0));
    _userInterface->statusBar->showMessage("Top view", 3000);
}

void SceneViewer::keyPressEvent(QKeyEvent* event)
{
    switch (event->modifiers())
    {
        case Qt::AltModifier:
        {
            if (_matching)
            {
                if (event->key() == Qt::Key_1 || event->key() == Qt::Key_2 || event->key() == Qt::Key_3)
                {
                    _objectSelected = event->key()-49;
                    _userInterface->statusBar->showMessage(QString("Object %1 selected.").arg(QString::number(_objectSelected)), 3000);
                }
                else if (event->key() == Qt::Key_D)
                {
                    _objectSelected = -1;
                    _userInterface->statusBar->showMessage(QString("No object selected."), 3000);
                }
                else if (event->key() == Qt::Key_R && _objectSelected != -1)
                {
                    _dialog->setWindowTitle(QString("Rotation"));
                    _dialog->show();
                }
                else if (event->key() == Qt::Key_T && _objectSelected != -1)
                {
                    _dialog->setWindowTitle(QString("Translation"));
                    _dialog->show();
                }
            }
        }
        break;
        case Qt::NoModifier:
        {
            if (event->key() == Qt::Key_R)
            {
                _userInterface->widgetScenePlayer->hide();
                _scenePlayer->init(0);
                _modelReader->resetScene();
                _userInterface->statusBar->showMessage(QString("Scene has been cleared."), 3000);
            }
            else if (event->key() == Qt::Key_C)
                centerCamera();
            else if (event->key() == Qt::Key_1)
                frontCameraView();
            else if (event->key() == Qt::Key_3)
                rightCameraView();
            else if (event->key() == Qt::Key_7)
                topCameraView();
            else if (event->key() == Qt::Key_Space)
            {
                if (_scenePlayer->isPaused())
                {
                    _scenePlayer->play();
                    _userInterface->bPlayPause->setIcon(QIcon(PATH_PAUSE_ICON));
                }
                else
                {
                    _scenePlayer->pause();
                    _userInterface->bPlayPause->setIcon(QIcon(PATH_PLAY_ICON));
                    _userInterface->hsCurrentFrame->setValue(_scenePlayer->getCurrentFrame()-1);
                }
            }
        }
        break;
    }
    update();
}

bool SceneViewer::isReady()
{
    return _scenePlayer->isReady();
}

void SceneViewer::importGeometry(const std::string filename)
{
    _modelReader->importModel(filename);
}
