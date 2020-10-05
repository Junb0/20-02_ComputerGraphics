#version 330 core

layout(location=0) in vec3 position;

out vec3 outColor;

void main()
{
  gl_Position = vec4(position.x, position.y, position.z, 1.0);
  outColor = vec3(0.2f, 0.2f, 1.0f);
}