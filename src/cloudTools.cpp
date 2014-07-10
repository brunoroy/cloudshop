#include "cloudTools.h"

CloudTools::CloudTools()
{
    static const float sphere[] = {16,2,77,29};
    _calibration.shapes.push_back(Shape("sphere", true, std::vector<float>(sphere, sphere + sizeof(sphere)/sizeof(sphere[0]))));
    static const float cylinder[] = {16,2,77,29};
    _calibration.shapes.push_back(Shape("cylinder", true, std::vector<float>(cylinder, cylinder + sizeof(cylinder)/sizeof(cylinder[0]))));

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

void CloudTools::clip(Object& object)
{
    std::lock_guard<std::mutex> lock(_mutex);

    //PointCloud<PointXYZRGBA>::Ptr clipped;
    //clipped = boost::make_shared<PointCloud<PointXYZRGBA>>(*(cloud.get()));
    //CalibrationParams calibration = _calibrationParams[0];

    //PointCloud<PointXYZRGBA>::Ptr newCloud(new PointCloud<PointXYZRGBA>(_emptyCloud));
    //pcl::PointXYZ p;
    glm::vec3 p;
    for (uint i = 0; i < object.getVertexCount(); ++i)
    {
        Vertex v = object.getVertex(i);

        bool keep = false;
        p.x = v.position.x;
        p.y = v.position.y;
        p.z = v.position.z;

        std::for_each(_calibration.shapes.begin(), _calibration.shapes.end(), [&] (Shape shape)
        {
            if (!shape.include || keep == true)
                return;

            if (_distanceFunctions.find(shape.name) == _distanceFunctions.end())
                return;

            if (_distanceFunctions[shape.name](p, shape.params) < 0.f)
                keep = true;
        });

        if (keep == false)
            continue;

        std::for_each (_calibration.shapes.begin(), _calibration.shapes.end(), [&] (Shape shape)
        {
            if (shape.include)
                return;

            if (_distanceFunctions.find(shape.name) == _distanceFunctions.end())
                return;

            if (_distanceFunctions[shape.name](p, shape.params) < 0.f)
                keep = false;
        });

        v.clipped = !keep;
        //if (keep)
            //v.clipped =
            //std::clog << "test." << std::endl;
            //newCloud->push_back(clipped->points[i]);
    }

    if (_calibration.shapes.size() > 0)
        object.setClipped(true);
        //std::clog << "test." << std::endl;
        //clipped = newCloud;

    //return clipped;
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
