#version 330 core
in vec3 vFragPosition;

out vec4 color;

void main()
{
  vec3 col = vec3(0, 1, 0);
  
  color = vec4(col, 1.0f);
}
