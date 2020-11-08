#version 330 core

in vec3 in_position;
in vec3 in_normal;
in vec2 in_uv;

out vec3 out_position;
out vec3 out_normal;
out vec2 out_uv;

uniform mat4 g_modelTransform;
uniform mat4 g_view;
uniform mat4 g_projection;

void main(){
	gl_Position = g_projection * g_view * g_modelTransform * vec4(in_position, 1.0);
	out_position = vec3(g_modelTransform * vec4(in_position, 1.0));
	out_normal = mat3(transpose(inverse(g_modelTransform))) * in_normal;
}