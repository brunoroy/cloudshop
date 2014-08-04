#include "modelReader.h"

#include <sstream>
#include <iterator>

#include <iostream>
#include <fstream>

ModelReader::ModelReader():
    _hasPosition(false),
    _hasNormal(false),
    _hasColor(false)
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

bool ModelReader::importTexture(std::string path)
{
    if (isFormatPNG(path))
        return importPNG(path);

    return false;
}

bool ModelReader::importPCD(const std::string filename)
{
    return false;
}

bool ModelReader::importPLY(const std::string filename)
{
    _hasPosition = false;
    _hasNormal = false;
    _hasColor = false;
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
                    _hasPosition = true;
                }

                if (values.at(2).compare("red") == 0)
                    colorComp[0] = compIndex;
                else if (values.at(2).compare("green") == 0)
                    colorComp[1] = compIndex;
                else if (values.at(2).compare("blue") == 0)
                {
                    colorComp[2] = compIndex;
                    _hasColor = true;
                }

                if (values.at(2).compare("nx") == 0)
                    normalComp[0] = compIndex;
                if (values.at(2).compare("ny") == 0)
                    normalComp[1] = compIndex;
                else if (values.at(2).compare("nz") == 0)
                {
                    normalComp[2] = compIndex;
                    _hasNormal = true;
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

                if (_hasPosition)
                    point = glm::vec3(std::stof(values.at(positionComp[0])), std::stof(values.at(positionComp[1])), std::stof(values.at(positionComp[2])));
                else
                    point = glm::vec3(0.0f, 0.0f, 0.0f);

                if (_hasNormal)
                    normal = glm::vec3(std::stof(values.at(normalComp[0])), std::stof(values.at(normalComp[1])), std::stof(values.at(normalComp[2])));
                else
                    normal = glm::vec3(0.0f, 0.0f, 0.0f);

                if (_hasColor)
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

bool ModelReader::importPNG(const std::string filename)
{
    return loadImagePNG(filename);
}

bool ModelReader::loadImagePNG(const std::string filename)
{
    png_structp png_ptr;
    png_infop info_ptr;
    unsigned int sig_read = 0;
    int color_type, interlace_type;
    FILE *fp;

    if ((fp = fopen(filename.c_str(), "rb")) == NULL)
        return false;

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (png_ptr == NULL) {
        fclose(fp);
        return false;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
        fclose(fp);
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return false;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        return false;
    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, sig_read);
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);

    png_uint_32 width, height;
    int bit_depth;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
                 &interlace_type, NULL, NULL);

    unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
    GLubyte* image = (unsigned char*) malloc(row_bytes * height);

    png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

    for (int i = 0; i < height; i++)
        memcpy(image+(row_bytes * (height-1-i)), row_pointers[i], row_bytes);

    //std::clog << "image [" << width << " x " << height << "]" << std::endl;

    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(fp);

    int textureId = -1;
    ull timestamp;
    if (filename.find("cam") != std::string::npos)
        textureId = std::stoi(filename.substr(filename.find("cam")+3, 1));
    if (filename.find("_") != std::string::npos)
    {
        try
        {
            timestamp = std::stoull(filename.substr(filename.find("_")+1, filename.length()-5));
        }
        catch (const std::exception& e)
        {
            timestamp = _sceneTextures.getTextureSize();
        }
    }

    std::clog << filename << std::endl;
    Texture texture(image, filename, textureId, timestamp);
    _sceneTextures.addTexture(texture);
    //std::clog << "textures: " << _sceneTextures.getTextureSize() << std::endl;

    return true;
}

void ModelReader::assignObjectTexture()
{
    //std::clog << ((_sceneObjects.getSceneSize()-1) * 3) << " == " << _sceneTextures.getTextureSize() << std::endl;
    /*if (_sceneObjects.getSceneSize() == _sceneTextures.getTextureSize())
    {*/
        for (uint i = 0; i < _sceneObjects.getSceneSize(); ++i)
        {
            for (uint j = 0; j < 3; ++j)
            {
                uint textureId = i * 3 + j;
                std::clog << "textureId: " << textureId << std::endl;
                _sceneObjects.getObject(i).addTexture(_sceneTextures.getTexture(textureId));
            }
        }
    /*}
    else
        std::clog << "texture set does not match object set." << std::endl;*/
}

bool ModelReader::isFormatPNG(const std::string filename)
{
    std::string fileExtension = getFileExtension(filename);
    if (fileExtension.compare(PNG_FILE_EXTENSION) == 0)
        return true;
    else
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

bool ModelReader::exportPLY(const std::string filename, const uint frame, const uint pov)
{
    if (!isSceneEmpty())
    {
        for (uint i = 0; i < pov; ++i)
        {
            Object object = _sceneObjects.getObject(frame, i);

            std::string frameFilename(filename.substr(0, filename.find_last_of(".")));
            if (pov == 1)
                frameFilename.append("_").append(std::to_string(frame)).append(".ply");
            else
                frameFilename.append("_").append(std::to_string(frame)).append("_pov").append(std::to_string(i)).append(".ply");
            //std::clog << frameFilename << std::endl;

            std::ofstream exportFile(frameFilename);
            if (exportFile.is_open())
            {
                uint vertexCount = object.getVertexCount();
                exportFile << "ply\nformat ascii 1.0\nelement vertex " << std::to_string(vertexCount) << std::endl;
                if (_hasPosition)
                    exportFile << "property float x\nproperty float y\nproperty float z" << std::endl;
                if (_hasNormal)
                    exportFile << "property float nx\nproperty float ny\nproperty float nz" << std::endl;
                if (_hasColor)
                    exportFile << "property uchar red\nproperty uchar green\nproperty uchar blue" << std::endl;
                exportFile << "end_header" << std::endl;

                for (uint i = 0; i < vertexCount; ++i)
                {
                    Vertex v = object.getVertex(i);

                    if (_hasPosition)
                        exportFile << std::to_string(v.position.x) << " " << std::to_string(v.position.y) << " " << std::to_string(v.position.z) << " ";
                    if (_hasNormal)
                        exportFile << std::to_string(v.normal.x) << " " << std::to_string(v.normal.y) << " " << std::to_string(v.normal.z) << " ";
                    if (_hasColor)
                        exportFile << std::to_string(v.color.x) << " " << std::to_string(v.color.y) << " " << std::to_string(v.color.z) << " ";
                    exportFile << std::endl;
                }
                exportFile.close();
            }
        }
    }
}

bool ModelReader::exportFrames(const std::string filename, QProgressBar* progressBar, const uint pov)
{
    uint frameCount;
    if (pov == 1)
        frameCount = _sceneObjects.getSceneSize();
    else
        frameCount = _sceneObjects.getSceneSize() / pov;

    for (uint i = 0; i < frameCount; ++i)
    {
        //for (uint j = 0; j < pov; ++j)
        exportPLY(filename, i, pov);
        progressBar->setValue(i+1);
    }
}
