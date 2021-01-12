#type vertex
#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color;

uniform mat4 u_ProjMat;

out vec3 v_color;

void main()
{
    gl_Position = u_ProjMat * vec4(position, 0.0, 1.0);
    v_color = color;
}

#type fragment
#version 330 core

layout(location = 0) out vec4 outColor;

in vec3 v_color;

void main()
{
    outColor = vec4(v_color, 1.0);
}