#version 330 core
in vec3 vTexCoords;

out vec4 color;

uniform samplerCube tex;

void main()
{
  color = texture(tex, vTexCoords);
}
