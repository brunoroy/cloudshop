#ifndef MODELREADER_H
#define MODELREADER_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <mutex>

#include <glm/glm.hpp>

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
    Vertex(glm::vec3 position, glm::vec4 color, glm::vec3 normal):
        position(position), color(color), normal(normal), clipped(false) {}

    glm::vec3 position;
    glm::vec4 color;
    glm::vec3 normal;
    bool clipped;
};

class Object
{
public:
    Object(const int id = -1, const ull ts = 0):
        _clipped(false)
    {
        if (id == -1)
            _id = std::rand();
        else
            _id = id;
        _ts = ts;
    }
    ~Object(){}

    Vertex getVertex(const uint index)
    {
        return Vertex(_positions.at(index), _colors.at(index), _normals.at(index));
    }
    void addVertex(Vertex vertex)
    {
        _positions.push_back(vertex.position);
        _colors.push_back(vertex.color);
        _normals.push_back(vertex.normal);
    }
    uint getVertexCount() {return _positions.size();}

    std::vector<glm::vec3> getPositions() {return _positions;}
    std::vector<glm::vec4> getColors() {return _colors;}
    std::vector<glm::vec3> getNormals() {return _normals;}
    uint getId() {return _id;}

    void setClipped(const bool clipped)
    {
        _clipped = clipped;
        //if (_clipped)
        //    std::sort(_)
    }
    bool isClipped() {return _clipped;}

    static bool sortId(const Object& o1, const Object& o2) {return (o1._id < o2._id);}
    static bool sortClipped(const Object& o1, const Object& o2) {return (o1._clipped == true);}
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

private:
    std::vector<glm::vec3> _positions;
    std::vector<glm::vec4> _colors;
    std::vector<glm::vec3> _normals;
    uint _id;
    ull _ts;

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
    Object getObject(const uint frame, const uint id)
    {
        uint stride = _objects.size()/getIdSize();
        uint objectIndex = (stride * id) + frame;
        //std::clog << "objectIndex: " << objectIndex << std::endl;
        //std::clog << "stride: " << stride << std::endl;
        //std::clog << "idSize: " << _idSize << std::endl;
        return _objects.at(objectIndex);
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

    bool isSceneEmpty() {return (_sceneObjects.getSceneSize() == 0);}
    SceneObjects getSceneObjects() {return _sceneObjects;}
    Object getObject(const uint frame, const uint id = 0) {return _sceneObjects.getObject(frame, id);}
    void resetScene() {_sceneObjects.reset();}

private:
    std::vector<std::string> split(const std::string input);

    SceneObjects _sceneObjects;
};

#endif // MODELREADER_H
