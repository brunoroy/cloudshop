#ifndef SCENEVIEWER_H
#define SCENEVIEWER_H

#include <QVector3D>
#include <QFile>
#include <QTextStream>

#include "modelReader.h"
#include "shaderSources.h"
#include "ui_mainWindow.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GL/glew.h>
#include <QGLViewer/qglviewer.h>

#include <thread>

using namespace qglviewer;

class SceneViewer : public QGLViewer
{
public:
    SceneViewer(Ui_MainWindow *userInterface);
    ~SceneViewer();

    void drawGeometry();
    bool isReady();
    void importGeometry(const std::string filename);

public slots:

protected:
    void init();
    //void animate();
    void draw();
    void keyPressEvent(QKeyEvent* event);

private:
    //Camera
    void centerCamera();
    void frontCameraView();
    void rightCameraView();
    void topCameraView();

    //OpenGL and shaders
    void loadShaders();

    //OpenGL
    GLint _shaderLocation[2];
    GLuint _vertexArrayID;
    GLuint _vertexBuffer;
    GLuint _colorBuffer;
    GLuint _shaderID[2];
    GLuint _programID;
    GLuint _matrixID;
    GLuint _pickingColorID;
    glm::mat4 _matrix = glm::mat4(1.0f);
    glm::vec4 _pickingColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    qglviewer::Vec _cameraOrigin;
    qglviewer::Vec _cameraDirection;

    Ui_MainWindow *_userInterface;
    std::shared_ptr<Camera> _sceneCamera;
    std::shared_ptr<ModelReader> _modelReader;
};

#endif // SCENEVIEWER_H
