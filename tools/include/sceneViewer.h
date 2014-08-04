#ifndef SCENEVIEWER_H
#define SCENEVIEWER_H

#include <QVector3D>
#include <QFile>
#include <QTextStream>

#include "scenePlayer.h"
#include "cloudTools.h"
#include "modelReader.h"
#include "shaderSources.h"
#include "ui_mainWindow.h"
#include "ui_transformsDialog.h"
#include "ui_clippingDialog.h"
#include "config.h"

#include <thread>

using namespace qglviewer;

class SceneViewer : public QGLViewer
{
public:
    SceneViewer(Ui_MainWindow *userInterface, Ui_TransformsDialog *transformsDialog, Ui_ClippingDialog *clippingDialog);
    ~SceneViewer();

    void drawGeometry(const uint povSize = 1);
    bool isReady();
    void importGeometry(const std::string filename);
    void importTexture(const std::string filename);
    void assignObjectTexture();
    void initScenePlayer(const uint frameCount);
    std::shared_ptr<ScenePlayer> getScenePlayer();
    void mergeObjects();
    void resetTransforms();
    void exportGeometry(const std::string filename, const uint currentFrame = -1);
    void surfaceReconstruction(Ui_MainWindow _userInterface);

    void setViewMode(uint mode) {_viewMode = mode;}
    void sortSceneObjects();
    void sortObjectTextures();

    void printTextures();

public slots:
    void setMatching(const bool matching);
    void setShapeClipping(const bool clipping);

protected:
    void init();
    void animate();
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
    GLuint _normalBuffer;
    GLuint _textureBuffer;
    GLuint _shaderID[2];

    GLuint _programID;
    GLuint _pmvMatrixID;
    GLuint _alphaID;
    GLuint _objectMatrixID;
    GLuint _mvMatrixID;
    GLuint _texCoordID;

    glm::mat4 _pmvMatrix = glm::mat4(1.0f);
    glm::mat4 _mvMatrix = glm::mat4(1.0f);

    qglviewer::Vec _cameraOrigin;
    qglviewer::Vec _cameraDirection;
    float _pointSize;

    Ui_MainWindow *_userInterface;
    Ui_TransformsDialog *_transformsDialog;
    Ui_ClippingDialog *_clippingDialog;
    std::shared_ptr<QDialog> _transformsWindow;
    std::shared_ptr<QDialog> _clippingWindow;

    std::shared_ptr<Camera> _sceneCamera;
    std::shared_ptr<ScenePlayer> _scenePlayer;
    std::shared_ptr<ModelReader> _modelReader;
    std::shared_ptr<CloudTools> _cloudTools;

    bool _matching;
    bool _shapeClipping;
    bool _merge;
    int _objectSelected;
    uint _viewMode;
    bool _wireframe;
};

#endif // SCENEVIEWER_H
