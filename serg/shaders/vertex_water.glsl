#version 330 core
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

out vec3 vFragPosition;
out vec3 vNormal;
out vec2 vTexCoords;
out vec3 pos_eye;
out vec3 n_eye;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
  gl_Position = projection * view * model * vec4(vertex, 1.0f);

  vFragPosition = vec3(model * vec4(vertex, 1.0f));
  vNormal = mat3(transpose(inverse(model))) * normal;
  vTexCoords = texCoords;
  pos_eye = vec3(view * model * vec4(vertex, 1.0f));
  n_eye = vec3(view * model * vec4(normal, 0.0f));
}
