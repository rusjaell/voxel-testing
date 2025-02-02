#version 460 core

layout (location = 0) in vec3 in_Pos;
layout (location = 1) in vec4 in_Color;
layout (location = 2) in vec2 in_UV;
layout (location = 3) in vec3 in_Normal;

uniform mat4 u_View;
uniform mat4 u_Projection;
uniform mat4 u_Model;

out vec4 v_Color;
out vec2 v_UV;
out vec3 v_Normal;

void main()
{
    v_Color = in_Color;
    v_UV = in_UV;

    mat3 normalMatrix = transpose(inverse(mat3(u_Model)));
    v_Normal = normalize(normalMatrix * in_Normal);

    gl_Position = u_Projection * u_View * u_Model * vec4(in_Pos, 1.0f);
}