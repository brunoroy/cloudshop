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
        uniform float alpha;
        uniform mat4 matrix;
        uniform mat4 transforms;
        out vec4 vertexColor;

        void main()
        {
            vertexColor = vec4(color, alpha);
            gl_Position =  matrix * transforms * vec4(position, 1.0);
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
