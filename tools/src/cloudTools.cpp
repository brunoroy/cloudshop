#include "cloudTools.h"

CloudTools::CloudTools()
{
    instanciateDistanceFunctions();
}

CloudTools::~CloudTools()
{
}

void CloudTools::initializeDistanceFunctions(const float radius)
{
    _calibration.shapes.clear();
    static const float sphere[] = {2.0f * radius, 0.0f, 0.0f, 0.0f};
    _calibration.shapes.push_back(Shape("sphere", false, std::vector<float>(sphere, sphere + sizeof(sphere)/sizeof(sphere[0]))));
    static const float cylinder[] = {radius, 0.0f, 0.0f, 0.0f, 2.0f};
    _calibration.shapes.push_back(Shape("cylinder", true, std::vector<float>(cylinder, cylinder + sizeof(cylinder)/sizeof(cylinder[0]))));
}

void CloudTools::instanciateDistanceFunctions()
{
    // Sphere distance function
    _distanceFunctions[std::string("sphere")] = [] (glm::vec3& p, std::vector<float>& s)
    {
        if (s.size() < 4)
            return 0.f;

        float distance = sqrtf(pow(p.x - s[1], 2.f) + pow(p.y - s[2], 2.f) + pow(p.z - s[3], 2.f));
        distance -= s[0];

        return distance;
    };

    // Infinite cylinder distance function
    _distanceFunctions[std::string("cylinder")] = [] (glm::vec3& p, std::vector<float>& s)
    {
        if (s.size() < 4)
            return 0.f;

        // s[4] contains the axis index of the cylinder
        if (s[4] == 0.f)
            return sqrtf(pow(p.y - s[2], 2.f) + pow(p.z - s[3], 2.f)) - s[0];
        else if (s[4] == 1.f)
            return sqrtf(pow(p.x - s[1], 2.f) + pow(p.z - s[3], 2.f)) - s[0];
        else if (s[4] == 2.f)
            return sqrtf(pow(p.x - s[1], 2.f) + pow(p.y - s[2], 2.f)) - s[0];
        else
            return 0.f;
    };
}

void CloudTools::clip(Object& object)
{
    std::lock_guard<std::mutex> lock(_mutex);

    glm::vec3 p;
    for (uint i = 0; i < object.getVertexCount(); ++i)
    {
        Vertex& v = object.getVertex(i);

        bool keep = false;
        p.x = v.position.x;
        p.y = v.position.y;
        p.z = v.position.z;

        std::for_each(_calibration.shapes.begin(), _calibration.shapes.end(), [&] (Shape shape)
        {
            //std::clog << "shape: " << shape.name << std::endl;
            if (_distanceFunctions[shape.name](p, shape.params) < 0.0f)
                keep = true;
        });

        if (keep == false)
            v.clipped = true;
    }

    if (_calibration.shapes.size() > 0)
        object.setClipped(true);
}

Object CloudTools::merge(std::initializer_list<Object> objects)
{
    Object mergeObject;

    for (auto& object : objects)
        mergeObject.merge(object);

    return mergeObject;
}

void CloudTools::translate(Object &object, glm::vec3 matrix)
{
    glm::mat4 transformsObject = object.getTransforms();
    glm::mat4 transforms = glm::translate(transformsObject, matrix);
    object.setTransforms(transforms);
}

void CloudTools::rotate(Object &object, glm::vec3 matrix)
{
    glm::mat4 transformsObject = object.getTransforms();
    glm::mat4 transforms = glm::rotate(transformsObject, matrix.x, glm::vec3(1.0, 0.0, 0.0));
    transforms = glm::rotate(transforms, matrix.y, glm::vec3(0.0, 1.0, 0.0));
    transforms = glm::rotate(transforms, matrix.z, glm::vec3(0.0, 0.0, 1.0));
    object.setTransforms(transforms);
}

void CloudTools::scale(Object &object, glm::vec3 matrix)
{
    glm::mat4 transformsObject = object.getTransforms();
    glm::mat4 transforms = glm::scale(transformsObject, matrix);
    object.setTransforms(transforms);
}
