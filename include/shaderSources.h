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
        layout(location = 1) in vec4 color;
        uniform mat4 matrix;
        out vec4 vertexColor;

        void main()
        {
            vertexColor = color;
            gl_Position =  matrix * vec4(position, 1.0);
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
