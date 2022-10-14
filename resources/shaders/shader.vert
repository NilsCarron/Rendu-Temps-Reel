#version 450

in vec3 position;
in vec3 normal;
uniform mat4 trans;
out vec3 positionOut;

out vec3 normalOut;
void main()
{

    gl_Position = trans * vec4(position * 0.01, 1.0);
    positionOut = position;
    normalOut = normal;
}
