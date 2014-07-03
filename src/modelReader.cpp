#include "modelReader.h"

#include <sstream>
#include <iterator>

ModelReader::ModelReader()
{
}

ModelReader::~ModelReader()
{

}

bool ModelReader::importModel(std::string path)
{
    if (isFormatPCD(path))
        return importPCD(path);
    else if (isFormatPLY(path))
        return importPLY(path);

    return false;
}

bool ModelReader::importPCD(const std::string filename)
{
    return false;
}

bool ModelReader::importPLY(const std::string filename)
{
    bool isVertexProperties = false;
    std::string line;
    std::ifstream inputFile(filename);
    std::vector<std::string> values;

    if (inputFile.is_open())
    {
        Object object;
        //_points.clear();
        while (std::getline(inputFile, line))
        {
            if (line.compare(PLY_END_HEADER) == 0)
            {
                isVertexProperties = true;
            }
            else if (isVertexProperties)
            {
                values = split(line);
                glm::vec3 point(std::stof(values.at(0)), std::stof(values.at(1)), std::stof(values.at(2)));
                //glm::vec3 normal(std::stof(values.at(4)), std::stof(values.at(5)), std::stof(values.at(6)));
                glm::vec3 normal(0.0f, 0.0f, 0.0f);
                glm::vec4 color(std::stof(values.at(3))/255.0f, std::stof(values.at(4))/255.0f, std::stof(values.at(5))/255.0f, 1.0f);
                object.addVertex(Vertex(point, color, normal));
                //std::clog << "vertex added." << std::endl;
                //_points.push_back(point);
                //_normals.push_back(normal);
            }
        }
        inputFile.close();
        _sceneObjects.addObject(object);
        //std::clog << _sceneObjects.getSceneSize() << " objects imported to scene." << std::endl;
        return true;
    }

    return false;
}

bool ModelReader::isFormatPCD(const std::string filename)
{
    std::string fileExtension = getFileExtension(filename);
    if (fileExtension.compare(PCD_FILE_EXTENSION) == 0)
        return true;
    else
        return false;
}

bool ModelReader::isFormatPLY(const std::string filename)
{
    std::string fileExtension = getFileExtension(filename);
    if (fileExtension.compare(PLY_FILE_EXTENSION) == 0)
        return true;
    else
        return false;
}

std::string ModelReader::getFileExtension(const std::string filename)
{
    std::string fileExtension = filename.substr(filename.find_last_of('.')+1, filename.length()-filename.find_last_of('.'));
    std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), (int(*)(int))std::toupper);
    return fileExtension;
}

std::vector<std::string> ModelReader::split(const std::string input)
{
    std::istringstream streamInput(input);
    return {std::istream_iterator<std::string>{streamInput}, std::istream_iterator<std::string>{}};
}
