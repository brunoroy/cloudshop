#ifndef MODELREADER_H
#define MODELREADER_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <glm/glm.hpp>

#define PLY_END_HEADER "end_header"
#define PCD_FILE_EXTENSION "PCD"
#define PLY_FILE_EXTENSION "PLY"

class Vertex
{
public:
    Vertex(){}
    Vertex(glm::vec3 position, glm::vec4 color, glm::vec3 normal);
    ~Vertex(){}

    glm::vec3 getPosition() {return _position;}
    glm::vec4 getColor() {return _color;}
    glm::vec3 getNormal() {return _normal;}

private:
    glm::vec3 _position;
    glm::vec4 _color;
    glm::vec3 _normal;
};

class Object
{
public:
    Object(){}
    ~Object(){}

    Vertex getVertex(const uint index) {return _vertices.at(index);}

private:
    std::vector<Vertex> _vertices;
};

class SceneObjects
{
public:
    SceneObjects(){}
    ~SceneObjects(){}

    void addObject(Object object) {_objects.push_back(object);}
    Object getObject(const uint index) {return _objects.at(index);}

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

private:
    std::vector<std::string> split(const std::string input);

    SceneObjects sceneObjects;
};

#endif // MODELREADER_H
