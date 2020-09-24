#version 330 core
in vec3 position;
in vec3 color;
out vec3 ourColor;
out vec3 ourPosition;
void main()
{
  gl_Position = vec4(position.x, position.y, position.z, 1.0);
  ourColor = color;
  float ratio = 3.0 / 2.0;
  ourPosition = vec3(position.x * ratio, position.y, position.z);
}