#ifndef SURFACERECONSTRUCTION_H
#define SURFACERECONSTRUCTION_H

#include "ui_mainWindow.h"
#include "surfaceTriangulation.h"
#include "modelReader.h"

#define PLY_HEADER_FIRST_PART "ply\nformat ascii 1.0\nelement vertex "
#define PLY_HEADER_SECOND_PART "property float x\nproperty float y\nproperty float z\nproperty float w\nproperty float nx\nproperty float ny\nproperty float nz"
//#define PLY_HEADER_SECOND_PART "property float x\nproperty float y\nproperty float z\nproperty float w\nproperty uchar red\nproperty uchar green\nproperty uchar blue"
//#define PLY_HEADER_SECOND_PART "property float x\nproperty float y\nproperty float z\nproperty float w"
#define PLY_HEADER_THIRD_PART "element face "
#define PLY_HEADER_LAST_PART "property list int int vertex_indices\nend_header"
#define SPLIT_CHAR " "

class SurfaceReconstruction
{
public:
    SurfaceReconstruction();
    ~SurfaceReconstruction();

    void writeMeshOutput(Mesh mesh, const std::string filename);
    void writeTextureOutput(Object& object, const std::string filename, const uint index);
    void reconstruct(SceneObjects& objects, Ui_MainWindow userInterface);
    void generateMesh(Mesh mesh, Object& object);

private:
    std::shared_ptr<SurfaceTriangulation> _surfaceTriangulation;
    //std::shared_ptr<ModelReader> _modelReader;
    //std::shared_ptr<SpatialGridPoints> _spatialGrid;

    CloudVolume getCloudVolume(std::vector<glm::vec3> points);
    void writeHeaderOutput(std::ofstream& outputFile, const unsigned int nbPoints, const unsigned int nbFaces);
};

#endif // SURFACERECONSTRUCTION_H
