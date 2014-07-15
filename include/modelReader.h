#ifndef MODELREADER_H
#define MODELREADER_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <mutex>

#include <glm/glm.hpp>
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
    void addVertex(Vertex vertex)
    {
        _positions.push_back(vertex.position);
        _colors.push_back(vertex.color);
        _normals.push_back(vertex.normal);
        _vertices.push_back(vertex);
        _vertexCount++;
    }
    uint getVertexCount() {return _vertexCount;}

    std::vector<glm::vec3> getPositions(){return _positions;}
    std::vector<glm::vec3> getColors() {return _colors;}
    std::vector<glm::vec3> getNormals() {return _normals;}
    std::vector<Vertex> getVertices() {return _vertices;}
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

private:
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

class SceneObjects
{
public:
    SceneObjects(): _idSize(0) {}
    ~SceneObjects(){}

    void addObject(Object object)
    {
        _objects.push_back(object);
        std::sort(_objects.begin(), _objects.end(), Object::sortIdTimestamp);

        uint lastId = _objects.at(_objects.size()-1).getId();
        if (lastId > _idSize)
            _idSize = lastId;
    }

    Object& getObject(const uint frame, const uint id = 0)
    {
        uint stride = _objects.size()/getIdSize();
        uint objectIndex = (stride * id) + frame;
        //std::clog << "objectIndex: " << objectIndex << std::endl;
        //std::clog << "stride: " << stride << std::endl;
        //std::clog << "idSize: " << _idSize << std::endl;
        return _objects.at(objectIndex);
        //return _objects.at(id);
    }
    uint getSceneSize() {return _objects.size();}
    uint getIdSize() {return (_idSize+1);}
    void reset()
    {
        _objects.clear();
        _idSize = 0;
    }

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
    bool isFormatPCD(const std::string filename);
    bool isFormatPLY(const std::string filename);
    std::string getFileExtension(const std::string filename);
    bool importModel(std::string path);

    bool exportPLY(const std::string filename, const uint frame);
    bool exportFrames(const std::string filename, QProgressBar* progressBar);

    bool isSceneEmpty() {return (_sceneObjects.getSceneSize() == 0);}
    SceneObjects getSceneObjects() {return _sceneObjects;}
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

private:
    std::vector<std::string> split(const std::string input);

    bool _hasPosition, _hasColor, _hasNormal;
    SceneObjects _sceneObjects;
};

#endif // MODELREADER_H
