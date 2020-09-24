#version 330 core
in vec3 ourColor;
in vec3 ourPosition;
out vec4 color;
void main()
{
  float radius = 0.5;
  vec3 origin = vec3(0.0);
  float dist = distance(origin, ourPosition);
  float alpha = step(dist, radius);
  color = vec4(ourColor, alpha);
}