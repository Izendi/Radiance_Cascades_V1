#version 420 core
layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


uniform vec3 lightRGB;

out vec3 lightColor;

void main()
{
    mat4 MVP = projection * view * model;

    lightColor = lightRGB * 3.0;

    gl_Position = MVP * vec4(aPos, 1.0);

}