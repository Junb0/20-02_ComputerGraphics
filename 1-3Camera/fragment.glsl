#version 330 core

out vec4 FragColor;
in vec3 outColor;
in vec2 TexCoord;

uniform vec3 color;
uniform int is_texture;
uniform sampler2D texture1;

void main()
{
  if(is_texture == 1)
    FragColor = texture(texture1, TexCoord);
  else
    FragColor = vec4(color, 1.0f);
}