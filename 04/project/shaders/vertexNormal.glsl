#version 330 core
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;

out vec3 vNormal;

void main()
{
  gl_Position = vec4(vertex, 1.0f);

  vNormal = normal;
}
