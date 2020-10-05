#version 330 core

in vec3 position;
in vec3 color;
out vec3 outColor;
out vec3 outPosition;
void main()
{
  gl_Position = vec4(position.x, position.y, position.z, 1.0);
  outColor = color;
}