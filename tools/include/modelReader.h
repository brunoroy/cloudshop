#ifndef MODELREADER_H
#define MODELREADER_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <mutex>

#include <GL/glew.h>
#include <GL/gl.h>
#include <QGLViewer/qglviewer.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <libpng/png.h>

#include <QProgressBar>

#include "config.h"

struct Shape
{
    Shape(std::string name, bool include, std::vector<float> params):
        name(name), include(include), params(params) {}

    std::string name;
    bool include;
    std::vector<float> params;
};

struct Calibration
{
    std::vector<Shape> shapes;
};

class Texture
{
public:
    Texture() {}
    Texture(GLubyte* image, const std::string name, const int id = -1, const ull ts = 0): _image(image), _name(name)
    {
        if (id == -1)
            _id = std::rand();
        else
            _id = id;
        _ts = ts;
    }
    ~Texture() {}

    uint getId() {return _id;}
    ull getTimestamp() {return _ts;}
    std::string getName() {return _name;}
    GLubyte* getImage() {return _image;}
    GLuint getTextureId() {return _textureId;}

    void setImage(GLubyte* image) {_image = image;}
    void setName(const std::string name) {_name = name;}
    void setId(const uint id) {_id = id;}
    void setTimestamp(const ull ts) {_ts = ts;}

    static bool sortIdTimestamp(const Texture& t1, const Texture& t2)
    {
        if (t1._id < t2._id)
            return true;
        if (t1._id > t2._id)
            return false;

        if (t1._ts < t2._ts)
            return true;

        return false;
    }

private:
    GLuint loadTexture()
    {
        glGenTextures(1, &_textureId);
        glBindTexture( GL_TEXTURE_2D, _textureId);
        glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 640, 480, 0, GL_RGB, GL_UNSIGNED_BYTE, _image);

        return _textureId;
    }

    GLuint _textureId;
    GLubyte* _image;
    std::string _name;
    uint _id;
    ull _ts;
};

struct Vertex
{
    Vertex() {}
    Vertex(glm::vec3 position, glm::vec3 color, glm::vec3 normal):
        position(position), color(color), normal(normal), clipped(false) {}

    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 normal;
    bool clipped;
};

class Object
{
public:
    Object(const int id = -1, const ull ts = 0):
        _vertexCount(0),
        _clipped(false)
    {
        if (id == -1)
            _id = std::rand();
        else
            _id = id;
        _ts = ts;
    }
    ~Object(){}

    Vertex& getVertex(const uint index)
    {
        return _vertices.at(index);
        //return Vertex(_positions.at(index), _colors.at(index), _normals.at(index));
    }
    Vertex& getMeshVertex(const uint index)
    {
        return _meshVertices.at(index);
        //return Vertex(_positions.at(index), _colors.at(index), _normals.at(index));
    }

    void addVertex(Vertex vertex)
    {
        _positions.push_back(vertex.position);
        _colors.push_back(vertex.color);
        _normals.push_back(vertex.normal);
        _vertices.push_back(vertex);
        _vertexCount++;
    }

    void setTexture(Texture texture) {_texture = texture;}
    Texture getTexture() {return _texture;}

    uint getVertexCount() {return _vertexCount;}
    uint getMeshVertexCount() {return _meshPositions.size();}

    std::vector<glm::vec3> getPositions() {return _positions;}
    std::vector<glm::vec3> getColors() {return _colors;}
    std::vector<glm::vec3> getNormals() {return _normals;}
    std::vector<Vertex> getVertices() {return _vertices;}
    std::vector<glm::vec3> getMeshPositions() {return _meshPositions;}
    std::vector<glm::vec3> getMeshColors() {return _meshColors;}
    std::vector<glm::vec3> getMeshNormals() {return _meshNormals;}
    uint getId() {return _id;}
    ull getTimestamp() {return _ts;}
    glm::mat4 getTransforms() {return _transforms;}
    void setTransforms(glm::mat4 transforms) {_transforms = transforms;}

    void setClipped(const bool clipped)
    {
        _clipped = clipped;
        if (_clipped)
        {
            /*std::clog << "before sorting..." << std::endl;
            for (uint i = _vertices.size()-10; i < _vertices.size(); ++i)
                std::clog << "v[" << i << "]: " << _vertices.at(i).clipped << std::endl;*/

            std::sort(_vertices.begin(), _vertices.end(), sortClipped);

            /*std::clog << "after sorting..." << std::endl;
            for (uint i = _vertices.size()-10; i < _vertices.size(); ++i)
                std::clog << "v[" << i << "]: " << _vertices.at(i).clipped << std::endl;*/

            _positions.clear();
            _colors.clear();
            _normals.clear();
            for (uint i = 0; i < _vertices.size(); ++i)
            {
                Vertex vertex = _vertices.at(i);
                if (vertex.clipped && i < _vertexCount) //mark first clipped vertex
                    _vertexCount = i-1;

                _positions.push_back(vertex.position);
                _colors.push_back(vertex.color);
                _normals.push_back(vertex.normal);
            }
        }
        else
            _vertexCount = _vertices.size();
    }
    bool isClipped() {return _clipped;}

    static bool sortId(const Object& o1, const Object& o2) {return (o1._id < o2._id);}
    static bool sortClipped(const Vertex& v1, const Vertex& v2) {return (v1.clipped < v2.clipped);}
    static bool sortIdTimestamp(const Object& o1, const Object& o2)
    {
        if (o1._id < o2._id)
            return true;
        if (o1._id > o2._id)
            return false;

        if (o1._ts < o2._ts)
            return true;

        return false;
        //return (o1._ts < o2._ts);
    }

    void merge(Object object)
    {
        Vertex vertex;
        for (uint i = 0; i < object.getVertexCount(); ++i)
        {
            vertex = object.getVertex(i);
            glm::vec4 transformedPosition = object.getTransforms() * glm::vec4(vertex.position, 1.0f);
            _positions.push_back(glm::vec3(transformedPosition));
            _colors.push_back(vertex.color);
            _normals.push_back(vertex.normal);
            _vertices.push_back(Vertex(glm::vec3(transformedPosition), vertex.color, vertex.normal));
        }
        _vertexCount += object.getVertexCount();
        _id = object.getId();
        _ts = object.getTimestamp();
    }

    void addMeshVertex(Vertex vertex)
    {
        _meshPositions.push_back(vertex.position);
        _meshColors.push_back(vertex.color);
        _meshNormals.push_back(vertex.normal);
        _meshVertices.push_back(vertex);
    }

private:
    Texture _texture;

    std::vector<Vertex> _meshVertices;
    std::vector<glm::vec3> _meshPositions;
    std::vector<glm::vec3> _meshColors;
    std::vector<glm::vec3> _meshNormals;

    std::vector<Vertex> _vertices;
    std::vector<glm::vec3> _positions;
    std::vector<glm::vec3> _colors;
    std::vector<glm::vec3> _normals;
    uint _id;
    ull _ts;

    uint _vertexCount;
    glm::mat4 _transforms = glm::mat4(1.0f);
    bool _clipped;
};

class SceneTextures
{
public:
    SceneTextures() {}
    ~SceneTextures() {}

    void addTexture(Texture texture)
    {
        _textures.push_back(texture);
        std::sort(_textures.begin(), _textures.end(), Texture::sortIdTimestamp);

        uint lastId = _textures.at(_textures.size()-1).getId();
        if (lastId > _idSize)
            _idSize = lastId;
    }

    Texture getTexture(const uint index) {return _textures.at(index);}

    uint getTextureSize() {return _textures.size();}

private:
    std::vector<Texture> _textures;
    uint _idSize;
};

class SceneObjects
{
public:
    SceneObjects(): _idSize(0) {}
    ~SceneObjects(){}

    void addObject(Object object)
    {
        _objects.push_back(object);
        //std::sort(_objects.begin(), _objects.end(), Object::sortIdTimestamp);

        uint lastId = _objects.at(_objects.size()-1).getId();
        if (lastId > _idSize)
            _idSize = lastId;
    }

    Object& getObject(const uint frame, const uint id = 0)
    {
        uint stride = _objects.size()/getIdSize();
        uint objectIndex = (stride * id) + frame;
        return _objects.at(objectIndex);
    }
    uint getSceneSize() {return _objects.size();}
    uint getIdSize() {return (_idSize+1);}
    void reset()
    {
        _objects.clear();
        _idSize = 0;
    }

    void sort() {std::sort(_objects.begin(), _objects.end(), Object::sortIdTimestamp);}

private:

    std::vector<Object> _objects;
    uint _idSize;
};

class ModelReader
{
public:
    ModelReader();
    ~ModelReader();

    bool importPCD(const std::string filename);
    bool importPLY(const std::string filename);
    bool importPNG(const std::string filename);
    bool isFormatPCD(const std::string filename);
    bool isFormatPLY(const std::string filename);
    bool isFormatPNG(const std::string filename);
    std::string getFileExtension(const std::string filename);
    bool importModel(std::string path);
    bool importTexture(std::string path);
    bool loadImagePNG(const std::string filename);
    void assignObjectTexture();

    bool exportPLY(const std::string filename, const uint frame, const uint pov = 1);
    bool exportFrames(const std::string filename, QProgressBar* progressBar, const uint pov = 1);

    bool isSceneEmpty() {return (_sceneObjects.getSceneSize() == 0);}
    SceneObjects& getSceneObjects() {return _sceneObjects;}
    Object& getObject(const uint frame, const uint id = 0) {return _sceneObjects.getObject(frame, id);}
    void resetScene() {_sceneObjects.reset();}
    void addObject(Object object) {_sceneObjects.addObject(object);}
    void setSceneObjects(SceneObjects sceneObjects) {_sceneObjects = sceneObjects;}
    void unclip()
    {
        for (uint i = 0; i < _sceneObjects.getSceneSize(); ++i)
        {
            _sceneObjects.getObject(i).setClipped(false);
        }
    }
    void sortSceneObjects() {_sceneObjects.sort();}

private:
    std::vector<std::string> split(const std::string input);

    bool _hasPosition, _hasColor, _hasNormal;
    SceneObjects _sceneObjects;
    SceneTextures _sceneTextures;
};

#endif // MODELREADER_H
