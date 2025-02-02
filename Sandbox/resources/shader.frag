#version 460 core

layout (location = 0) out vec4 o_Color;

in vec4 v_Color;
in vec2 v_UV;
in vec3 v_Normal;

uniform sampler2D u_Texture;
uniform vec3 u_LightDirection;

void main()
{
    vec4 textureColor = texture(u_Texture, v_UV);

    vec3 normal = normalize(v_Normal);

    float diff = max(dot(normal, -normalize(u_LightDirection)), 0.0f);

    vec4 diffuse = diff * textureColor * v_Color;

    vec3 ambient = 0.1f * textureColor.rgb;

    o_Color = vec4(ambient + diffuse.rgb, v_Color.a);
}
