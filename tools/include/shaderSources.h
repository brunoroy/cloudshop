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
        //uniform sampler2D texCoord;


        out vec4 vertexColor;
        smooth out vec3 smoothNormal;
        //out vec2 texCoordUV;
        smooth out vec2 texCoordUV;

        void main()
        {
            vertexColor = vec4(color, alpha);
            gl_Position =  pmvMatrix * objectMatrix * vec4(position, 1.0);

            mat4 normalMatrix = transpose(inverse(mvMatrix));
            vec4 result = normalMatrix * vec4(normal, 0.0);
            smoothNormal = result.xyz;

            //vec2 texSample = textureQueryLod(texCoord, VertexIn.texCoord.xy);
            texCoordUV = texCoord;
        }
    )"};

    /**
    * Default fragment shader
    */
    const std::string FRAGMENT_SHADER_DEFAULT {R"(
        #version 330 core

        in vec4 vertexColor;
        in vec2 texCoord;

        out vec4 fragmentColor;
        uniform sampler2D textureMap;

        void main()
        {
            //fragmentColor = vertexColor;
            //fragmentColor = texture2D(textureColor, gl_TexCoord[0].st);
            //fragmentColor = vec4(textureColor, 0.0, 0.0);
            fragmentColor = texture(textureMap, texCoord);
            //textureQueryLevels
        }
    )"};
};

#endif // SHADERSOURCES_H
