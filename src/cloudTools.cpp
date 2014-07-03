#include "cloudTools.h"

CloudTools::CloudTools()
{
    initializeDistanceFunctions();
}

CloudTools::~CloudTools()
{
}

void CloudTools::initializeDistanceFunctions()
{
    // Sphere distance function
    _distanceFunctions[std::string("sphere")] = [] (glm::vec3& p, std::vector<float>& s)
    {
        if (s.size() < 4)
            return 0.f;

        float distance = sqrtf(pow(p.x - s[1], 2.f) + pow(p.y - s[2], 2.f) + pow(p.z - s[3], 2.f));
        return distance - s[0];
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
