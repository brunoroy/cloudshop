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

private:
    std::vector<glm::vec3> _positions;
    std::vector<glm::vec4> _colors;
    std::vector<glm::vec3> _normals;

    uint _id;
};

class SceneObjects
{
public:
    SceneObjects(){}
    ~SceneObjects(){}

    void addObject(Object object) {_objects.push_back(object);}
    Object getObject(const uint index) {return _objects.at(index);}
    uint getSceneSize() {return _objects.size();}

private:
    std::vector<Object> _objects;
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
    Object getObject(const uint index) {return _sceneObjects.getObject(index);}

private:
    std::vector<std::string> split(const std::string input);

    SceneObjects _sceneObjects;
};

#endif // MODELREADER_H
