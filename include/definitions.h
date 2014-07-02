#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QVector>
#include <GL/glew.h>
#include <GL/gl.h>

#define SPLIT_CHAR " "
#define END_CHAR "\n"

#define HOUDINI_FILE_SUFFIX ".hclassic"
#define DATA_FILE_SUFFIX ".data"
#define TRANSFORMED_FILE_SUFFIX ".transformed"
#define SCALING_PARAMS_FILE_SUFFIX ".scaling.params"
#define SCALED_FILE_SUFFIX ".scaled"
#define VALIDATION_FILE_SUFFIX ".test"
#define MODEL_FILE_SUFFIX ".model"
#define PREDICT_FILE_SUFFIX ".predict"

#define PATH_PLAY_ICON "icons/play.ico"
#define PATH_PAUSE_ICON "icons/pause.ico"

#define VORTICITY_THRESHOLD 10.0
#define Malloc(type,n) (type *)malloc((n)*sizeof(type))

//Draw modes
#define MODE_POSITION 0
#define MODE_REGRESSION 1
#define MODE_ATTRIBUTE 2

#define GRID_WIDTH 3.0
#define GRID_DEPTH 3.0
#define GRID_HEIGHT 5.0

struct SelectedPoint
{
    SelectedPoint(): cellIndex(-1), pointIndex(-1) {}
    SelectedPoint(const int cellIndex, const int pointIndex): cellIndex(cellIndex), pointIndex(pointIndex) {}

    int cellIndex;
    int pointIndex;
};

struct VertexColor
{
public:
    VertexColor(const GLfloat red, const GLfloat green, const GLfloat blue, const GLfloat alpha):
        _red(red), _green(green), _blue(blue), _alpha(alpha){}

    GLfloat getRed(){return _red;}
    GLfloat getGreen(){return _green;}
    GLfloat getBlue(){return _blue;}
    GLfloat getAlpha(){return _alpha;}

private:
    GLfloat _red;
    GLfloat _green;
    GLfloat _blue;
    GLfloat _alpha;
};

struct Vertices
{
    QVector<GLfloat> positions;
    QVector<GLfloat> colors;
};

#endif // CONSTANTS_H
