#version 330 core
in vec3 vTexCoords;

out vec4 color;

uniform samplerCube cube_texture;

void main()
{
  color = texture(cube_texture, vTexCoords);
}
