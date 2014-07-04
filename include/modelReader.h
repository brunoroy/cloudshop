#ifndef MODELREADER_H
#define MODELREADER_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <glm/glm.hpp>

#include "config.h"

struct Vertex
{
    Vertex(glm::vec3 position, glm::vec4 color, glm::vec3 normal):
        position(position), color(color), normal(normal) {}

    glm::vec3 position;
    glm::vec4 color;
    glm::vec3 normal;
};

class Object
{
public:
    Object(const int id = -1)
    {
        if (id == -1)
            _id = std::rand();
        else
            _id = id;
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

private:
    std::vector<glm::vec3> _positions;
    std::vector<glm::vec4> _colors;
    std::vector<glm::vec3> _normals;

    uint _id;
};

class SceneObjects
{
public:
    SceneObjects(): _idSize(0) {}
    ~SceneObjects(){}

    void addObject(Object object)
    {
        _objects.push_back(object);

        uint lastId = _objects.at(_objects.size()-1).getId();
        std::clog << "lastId: " << lastId << std::endl;
        if (lastId > _idSize)
            _idSize = lastId;
        //_idSize = _objects.at(_objects.size()-1).getId() + 1;
        std::clog << "idSize: " << _idSize << std::endl;
    }
    Object getObject(const uint index, const uint id = 0)
    {
        uint stride = ((_idSize+1) * id);
        uint objectIndex = (_idSize * id) + id;
        std::clog << "draw id " << objectIndex << std::endl;
        std::clog << "stride: " << stride << std::endl;
        return _objects.at(objectIndex);
    }
    uint getSceneSize() {return _objects.size();}
    uint getIdSize() {return (_idSize+1);}

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
    Object getObject(const uint index, const uint id = 0) {return _sceneObjects.getObject(index, id);}

private:
    std::vector<std::string> split(const std::string input);

    SceneObjects _sceneObjects;
};

#endif // MODELREADER_H
