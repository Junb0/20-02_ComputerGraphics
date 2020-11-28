#version 330 core

layout(location=0) in vec3 vPos;
layout(location=1) in vec3 dis_color;
layout(location=2) in vec2 vTexCoord;

uniform mat4 g_modelTransform;
uniform mat4 g_view;
uniform mat4 g_projection;
uniform int Trans;

out vec3 outColor;
out vec2 TexCoord;

void main(){
	gl_Position = g_projection * g_view * g_modelTransform * vec4(vPos, 1.0);
	if(Trans == 0)
		gl_Position = vec4(vPos, 1.0);
	outColor = dis_color;
	TexCoord = vTexCoord;
}