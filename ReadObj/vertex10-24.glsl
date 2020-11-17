#version 330

in vec3 in_position;
in vec3 in_normal;
in vec2 in_uv;
in vec4 in_color;

out vec3 v2f_normal;
out vec3 v2f_worldPos;
out vec4 outColor;

uniform mat4 g_modelTransform;
uniform mat4 g_view;
uniform mat4 g_projection;
uniform int vIsAlpha;

void main(void)
{
	vec4 worldPos = g_modelTransform * vec4(in_position, 1.0);
	gl_Position = g_projection * g_view * worldPos;
	outColor = vec4(1.0, 1.0, 1.0, 1.0);
	if(vIsAlpha == 1)
		outColor = in_color;
	vec4 worldNormal = g_modelTransform * vec4(in_normal, 0.0);
	v2f_normal = vec3(worldNormal.x, worldNormal.y, worldNormal.z);
	v2f_worldPos = vec3(worldPos.x, worldPos.y, worldPos.z);
}
