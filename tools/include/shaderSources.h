#ifndef SHADERSOURCES_H
#define SHADERSOURCES_H

#include <string>

struct ShaderSources
{
    /**
     * Default vertex shader
     */
    const std::string VERTEX_SHADER_DEFAULT {R"(
        #version 330 core

        layout(location = 0) in vec3 position;
        layout(location = 1) in vec3 color;
        layout(location = 2) in vec3 normal;

        uniform mat4 pmvMatrix;
        uniform mat4 objectMatrix;
        uniform mat4 mvMatrix;

        uniform float alpha;
        out vec4 vertexColor;
        smooth out vec3 smoothNormal;

        void main()
        {
            vertexColor = vec4(color, alpha);
            gl_Position =  pmvMatrix * objectMatrix * vec4(position, 1.0);

            mat4 normalMatrix = transpose(inverse(mvMatrix));
            vec4 result = normalMatrix * vec4(normal, 0.0);
            smoothNormal = result.xyz;
        }
    )"};

    /**
    * Default fragment shader
    */
    const std::string FRAGMENT_SHADER_DEFAULT {R"(
        #version 330 core

        in vec4 vertexColor;
        out vec4 fragmentColor;

        void main()
        {
            fragmentColor = vertexColor;
        }
    )"};
};

#endif // SHADERSOURCES_H
