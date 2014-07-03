#ifndef CLOUDTOOLS_H
#define CLOUDTOOLS_H

#include <functional>
#include <map>
#include <vector>

#include <glm/glm.hpp>

class CloudTools
{
public:
    CloudTools();
    ~CloudTools();

private:
    void initializeDistanceFunctions();

    std::map< std::string, std::function< float(glm::vec3&, std::vector<float>&) > > _distanceFunctions;
};

#endif // CLOUDTOOLS_H
