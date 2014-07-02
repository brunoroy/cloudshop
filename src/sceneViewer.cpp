#include "sceneViewer.h"
#include "definitions.h"

#include <QKeyEvent>
#include <QFrame>
#include <QDir>
#include <QVector>

#include <limits>

#define OPENGL_TEST 1

SceneViewer::SceneViewer(Ui_MainWindow *userInterface)
{
    _userInterface = userInterface;
    _sceneCamera.reset(this->camera());
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

/*void SceneViewer::animate()
{
}*/

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

void SceneViewer::drawGeometry()
{
    float min = -0.5f;
    float max = 0.5f;

    std::vector<glm::vec3> points;
    std::vector<glm::vec4> colors;
    glm::vec4 white(1.0f, 1.0f, 1.0f, 1.0f);

    points.push_back(glm::vec3(min, 0.0f, min));
    colors.push_back(white);
    points.push_back(glm::vec3(max, 0.0f, min));
    colors.push_back(white);
    points.push_back(glm::vec3(max, 0.0f, max));
    colors.push_back(white);
    points.push_back(glm::vec3(min, 0.0f, max));
    colors.push_back(white);

    glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, points.size() * 3 * sizeof(GLfloat), &points.at(0), GL_STREAM_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, _colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * 4 * sizeof(GLfloat), &colors.at(0), GL_STREAM_DRAW);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glPointSize(5.0f);

    glUseProgram(_programID);

    GLdouble matrix[16];
    _sceneCamera->getModelViewProjectionMatrix(matrix);
    _matrix = glm::make_mat4(matrix);

    //printMat(_matrix);

    glUniformMatrix4fv(_matrixID, 1, GL_FALSE, &_matrix[0][0]);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, _colorBuffer);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

    //glDrawArrays(GL_LINES, 0, 2);
    //glDrawArrays(GL_TRIANGLES, 0, 3);
    glDrawArrays(GL_QUADS, 0, points.size());

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
}

void SceneViewer::draw()
{
    drawGeometry();
    /*if (OPENGL_TEST)
    {
        if (_scenePlayer->isReady())
            drawPlane();
            //drawNoise();
    }
    else
    {
        if (_scenePlayer->isReady())
        {
            if (!_frames.at(_scenePlayer->getCurrentFrame()).selectedPoints.isEmpty())
                updateTrackingMarkers();

            if (_currentDisplayMode == MODE_POSITION)
                drawPosition();
            else if (_currentDisplayMode == MODE_REGRESSION)
                drawRegressionOutput();
            else if (_currentDisplayMode == MODE_ATTRIBUTE)
                drawAttributes(_attributeList);
        }
    }*/
}

void SceneViewer::centerCamera()
{
    /*
    q.w = cos(angle / 2)
    q.x = axis.x * sin( angle / 2)
    q.y = axis.y * sin( angle / 2)
    q.z = axis.z * sin( angle / 2)
    */

    Quaternion q;
    q.setAxisAngle(Vec(1.0, 0.0, 0.0), -45.0 * (M_PI/180.0));
    _sceneCamera->setOrientation(q);
    _sceneCamera->setPosition(Vec(0.0, 5.0, 5.0));
    _userInterface->statusBar->showMessage("Center view", 3000);
    update();
}

void SceneViewer::frontCameraView()
{
    Quaternion q;
    q.setAxisAngle(Vec(0.0, 0.0, 0.0), 0.0 * (M_PI/180.0));
    _sceneCamera->setOrientation(q);
    _sceneCamera->setPosition(Vec(0.0, 1.5, 5.0));
    _userInterface->statusBar->showMessage("Front view", 3000);
    update();
}

void SceneViewer::rightCameraView()
{
    Quaternion q;
    q.setAxisAngle(Vec(0.0, 1.0, 0.0), 90.0 * (M_PI/180.0));
    _sceneCamera->setOrientation(q);
    _sceneCamera->setPosition(Vec(5.0, 1.5, 0.0));
    _userInterface->statusBar->showMessage("Right view", 3000);
    update();
}

void SceneViewer::topCameraView()
{
    Quaternion q;
    q.setAxisAngle(Vec(1.0, 0.0, 0.0), -90.0 * (M_PI/180.0));
    _sceneCamera->setOrientation(q);
    _sceneCamera->setPosition(Vec(0.0, 5.0, 0.0));
    _userInterface->statusBar->showMessage("Top view", 3000);
    update();
}

void SceneViewer::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
        case Qt::Key_C:
            centerCamera();
        break;
        case Qt::Key_1:
            frontCameraView();
        break;
        case Qt::Key_3:
            rightCameraView();
        break;
        case Qt::Key_7:
            topCameraView();
        break;
        case Qt::Key_Space:
        {
            /*if (_scenePlayer->isPaused())
            {
                _scenePlayer->play();
                _userInterface->bPlayPause->setIcon(QIcon(PATH_PAUSE_ICON));
            }
            else
            {
                _scenePlayer->pause();
                _userInterface->bPlayPause->setIcon(QIcon(PATH_PLAY_ICON));
            }*/
        }
        break;
    }
}

bool SceneViewer::isReady()
{
    //return _scenePlayer->isReady();
}
