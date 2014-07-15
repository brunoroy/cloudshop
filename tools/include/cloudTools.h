#ifndef CLOUDTOOLS_H
#define CLOUDTOOLS_H

#include <functional>
#include <map>
#include <vector>

#include "modelReader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class CloudTools
{
public:
    CloudTools();
    ~CloudTools();
    void initializeDistanceFunctions(const float radius);

    void clip(Object& object);
    Object merge(std::initializer_list<Object>);

    void translate(Object& object, glm::vec3 matrix);
    void rotate(Object& object, glm::vec3 matrix);
    void scale(Object &object, glm::vec3 matrix);

private:
    void instanciateDistanceFunctions();

    std::map< std::string, std::function< float(glm::vec3&, std::vector<float>&) > > _distanceFunctions;
    Calibration _calibration;
    mutable std::mutex _mutex;
};

#endif // CLOUDTOOLS_H
