#version 330 core

layout(location=0) in vec3 vPos;

uniform mat4 modelTransform;
uniform mat4 viewTransform;
uniform mat4 projectionTransform;
uniform vec3 color;

out vec3 outColor;

void main()
{
  gl_Position = projectionTransform * viewTransform * modelTransform * vec4(vPos, 1.0);
  outColor = color;
}