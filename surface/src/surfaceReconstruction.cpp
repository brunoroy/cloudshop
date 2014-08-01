#include "surfaceReconstruction.h"
#include "ui_mainWindow.h"
#include "timer.h"

#include <iostream>
#include <fstream>

#include <QFile>

SurfaceReconstruction::SurfaceReconstruction()
{
}

SurfaceReconstruction::~SurfaceReconstruction()
{
}

CloudVolume SurfaceReconstruction::getCloudVolume(std::vector<glm::vec3> points)
{
    float resolution = 0.05f;

    CloudVolume cloudVolume;
    cloudVolume.minimum = points.at(0);
    cloudVolume.maximum = points.at(0);
    cloudVolume.resolution = resolution;
    //std::clog << "resolution: " << resolution << std::endl;

    int nbPoints = points.size();
    for (int i = 0; i < nbPoints; ++i)
    {
        glm::vec3 point(points.at(i));

        if (point.x < cloudVolume.minimum.x)
            cloudVolume.minimum.x = point.x;
        if (point.y < cloudVolume.minimum.y)
            cloudVolume.minimum.y = point.y;
        if (point.z < cloudVolume.minimum.z)
            cloudVolume.minimum.z = point.z;

        if (point.x > cloudVolume.maximum.x)
            cloudVolume.maximum.x = point.x;
        if (point.y > cloudVolume.maximum.y)
            cloudVolume.maximum.y = point.y;
        if (point.z > cloudVolume.maximum.z)
            cloudVolume.maximum.z = point.z;
    }

    float influenceRadius = 4.0f * resolution;
    float borderSize = influenceRadius + resolution;
     glm::vec3 offset(borderSize, borderSize, borderSize);
    cloudVolume.minimum -= offset;
    cloudVolume.maximum += offset;

    return cloudVolume;
}

void SurfaceReconstruction::writeHeaderOutput(std::ofstream& outputFile, const unsigned int nbPoints, const unsigned int nbFaces)
{
    outputFile << PLY_HEADER_FIRST_PART << nbPoints << std::endl;
    outputFile << PLY_HEADER_SECOND_PART << std::endl;
    outputFile << PLY_HEADER_THIRD_PART << nbFaces << std::endl;
    outputFile << PLY_HEADER_LAST_PART << std::endl;
}

void SurfaceReconstruction::writeMeshOutput(Mesh mesh, const std::string filename)
{
    std::ofstream meshFile(filename);

    if (meshFile.is_open())
    {
        writeHeaderOutput(meshFile, mesh.points().size(), mesh.triangles().size());

        for (int i = 0; i < mesh.points().size(); ++i)
            meshFile << mesh.points().at(i).x << SPLIT_CHAR << mesh.points().at(i).y << SPLIT_CHAR << mesh.points().at(i).z << " 1 " << \
                        mesh.normals().at(i).x << SPLIT_CHAR << mesh.normals().at(i).y << SPLIT_CHAR << mesh.normals().at(i).z << std::endl;

        for (int i = 0; i < mesh.triangles().size(); ++i)
            meshFile << "3" << SPLIT_CHAR << mesh.triangles().at(i).v[0] << SPLIT_CHAR << mesh.triangles().at(i).v[1] << SPLIT_CHAR << mesh.triangles().at(i).v[2] << std::endl;

        meshFile.close();
    }
}

void SurfaceReconstruction::writeTextureOutput(Object &object, const std::string filename)
{
    std::string textureFilename(filename.substr(0, filename.find_last_of(".")));
    textureFilename.append(".png");

    std::string imageFilename(object.getTexture().getName());
    QFile image(QString(imageFilename.c_str()));
    image.rename(QString(textureFilename.c_str()));
    //png_write_png();
}

void SurfaceReconstruction::generateMesh(Mesh mesh, Object& object)
{
    for (int i = 0; i < mesh.triangles().size(); ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            uint triangleIndex = mesh.triangles().at(i).v[j];

            glm::vec3 position(mesh.points().at(triangleIndex).x, mesh.points().at(triangleIndex).y, mesh.points().at(triangleIndex).z);
            glm::vec3 normal(mesh.normals().at(triangleIndex).x, mesh.normals().at(triangleIndex).y, mesh.normals().at(triangleIndex).z);
            object.addMeshVertex(Vertex(position, glm::vec3(0.7f, 0.7f, 0.7f), normal));
        }
    }
}

void SurfaceReconstruction::reconstruct(SceneObjects& objects, Ui_MainWindow userInterface)
{
    userInterface.progressBar->setMaximum(objects.getSceneSize());
    for (uint i = 0; i < objects.getSceneSize(); ++i)
    {
        Object& object = objects.getObject(i);
        std::vector<glm::vec3> points = object.getPositions();
        std::vector<glm::vec3> normals = object.getNormals();
        //std::clog << points.size() << " points have been read." << std::endl;
        CloudVolume cloudVolume = getCloudVolume(points);
        //std::clog << "volume: minimum[" << cloudVolume.minimum.x << "," << cloudVolume.minimum.y << "," << cloudVolume.minimum.z << "]";
        //std::clog << "; maximum[" << cloudVolume.maximum.x << "," << cloudVolume.maximum.y << "," << cloudVolume.maximum.z << "]";
        //std::clog << "; resolution(" << cloudVolume.resolution << ")." << std::endl;

        //Timer buildSpatialGrid(true);
        _surfaceTriangulation.reset(new SurfaceTriangulation(cloudVolume, points, normals));
        //auto elapsed = buildSpatialGrid.elapsed();
        //std::cout << "spatial grid: " << std::fixed << elapsed.count() << " ms." << std::endl;
        std::shared_ptr<MarchingCubeGrid> grid = _surfaceTriangulation->getMarchingCubeGrid();

        Mesh mesh;
        Timer computerIsoValuesTimer(true);
        grid->computeIsoValues(points, normals, cloudVolume.resolution);
        auto elapsed = computerIsoValuesTimer.elapsed();
        std::cout << "compute isovalues: " << std::fixed << elapsed.count() << " ms." << std::endl;

        //Timer triangulateTimer(true);
        grid->triangulate(mesh);
        //elapsed = triangulateTimer.elapsed();
        //std::cout << "triangulation: " << std::fixed << elapsed.count() << " ms." << std::endl;

        //Timer writeMeshTimer(true);

        std::string meshFilename("output/");
        meshFilename.append("cam0_").append(std::to_string(i)).append(".ply");
        writeMeshOutput(mesh, meshFilename);
        writeTextureOutput(object, meshFilename);
        generateMesh(mesh, object);
        //elapsed = writeMeshTimer.elapsed();
        //std::cout << "generating mesh: " << std::fixed << elapsed.count() << " ms." << std::endl;

        userInterface.progressBar->setValue(userInterface.progressBar->value()+1);
        //std::cout << "writing mesh file: " << std::fixed << elapsed.count() << " ms." << std::endl;
    }
 }


