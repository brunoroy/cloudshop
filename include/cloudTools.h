#ifndef CLOUDTOOLS_H
#define CLOUDTOOLS_H

#include <functional>
#include <map>
#include <vector>

#include "modelReader.h"

#include <glm/glm.hpp>

class CloudTools
{
public:
    CloudTools();
    ~CloudTools();

    void clip(Object& object);
    Object merge(std::initializer_list<Object>);

private:
    void initializeDistanceFunctions();

    std::map< std::string, std::function< float(glm::vec3&, std::vector<float>&) > > _distanceFunctions;
    Calibration _calibration;
    mutable std::mutex _mutex;
};

#endif // CLOUDTOOLS_H
