#type vertex
#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 texcoord;

uniform mat4 u_ProjMat;
uniform vec2 u_texSize;

out vec3 v_color;
out vec2 v_texcoord;

void main()
{
    gl_Position = u_ProjMat * vec4(position, 0.0, 1.0);
    v_color = color;
    v_texcoord = texcoord/u_texSize;
}

#type fragment
#version 330 core

layout(location = 0) out vec4 outColor;

uniform sampler2D u_texture;

in vec3 v_color;
in vec2 v_texcoord;

void main()
{
    outColor = texture(u_texture, v_texcoord) * vec4(v_color, 1.0);
}