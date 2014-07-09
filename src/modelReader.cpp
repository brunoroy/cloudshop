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
    bool hasPosition = false;
    bool hasNormal = false;
    bool hasColor = false;
    uint positionComp[3];
    uint colorComp[3];
    uint normalComp[3];
    uint compIndex = 0;

    bool isVertexProperties = false;
    std::string line;
    std::ifstream inputFile(filename);
    std::vector<std::string> values;

    if (inputFile.is_open())
    {        
        int objectId = -1;
        ull timestamp;
        if (filename.find("cam") != std::string::npos)
            objectId = std::stoi(filename.substr(filename.find("cam")+3, 1));
        if (filename.find("_") != std::string::npos)
        {
            try
            {
                timestamp = std::stoull(filename.substr(filename.find("_")+1, filename.length()-5));
            }
            catch (const std::exception& e)
            {
                timestamp = _sceneObjects.getSceneSize();
            }
        }

        Object object(objectId, timestamp);

        while (std::getline(inputFile, line))
        {
            if (line.find(PLY_PROPERTY) != std::string::npos)
            {
                values = split(line);
                if (values.at(2).compare("x") == 0)
                    positionComp[0] = compIndex;
                else if (values.at(2).compare("y") == 0)
                    positionComp[1] = compIndex;
                else if (values.at(2).compare("z") == 0)
                {
                    positionComp[2] = compIndex;
                    hasPosition = true;
                }

                if (values.at(2).compare("red") == 0)
                    colorComp[0] = compIndex;
                else if (values.at(2).compare("green") == 0)
                    colorComp[1] = compIndex;
                else if (values.at(2).compare("blue") == 0)
                {
                    colorComp[2] = compIndex;
                    hasColor = true;
                }

                if (values.at(2).compare("nx") == 0)
                    normalComp[0] = compIndex;
                if (values.at(2).compare("ny") == 0)
                    normalComp[1] = compIndex;
                else if (values.at(2).compare("nz") == 0)
                {
                    normalComp[2] = compIndex;
                    hasNormal = true;
                }

                compIndex++;
            }
            else if (line.compare(PLY_END_HEADER) == 0)
            {
                isVertexProperties = true;
            }
            else if (isVertexProperties)
            {
                glm::vec3 point;
                glm::vec3 color;
                glm::vec3 normal;

                values = split(line);

                if (hasPosition)
                    point = glm::vec3(std::stof(values.at(positionComp[0])), std::stof(values.at(positionComp[1])), std::stof(values.at(positionComp[2])));
                else
                    point = glm::vec3(0.0f, 0.0f, 0.0f);

                if (hasNormal)
                    normal = glm::vec3(std::stof(values.at(normalComp[0])), std::stof(values.at(normalComp[1])), std::stof(values.at(normalComp[2])));
                else
                    normal = glm::vec3(0.0f, 0.0f, 0.0f);

                if (hasColor)
                    color = glm::vec3(std::stof(values.at(colorComp[0]))/255.0f, std::stof(values.at(colorComp[1]))/255.0f, std::stof(values.at(colorComp[2]))/255.0f);
                else
                    color = glm::vec3(1.0f, 1.0f, 1.0f);


                object.addVertex(Vertex(point, color, normal));
            }
        }
        inputFile.close();
        _sceneObjects.addObject(object);
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
