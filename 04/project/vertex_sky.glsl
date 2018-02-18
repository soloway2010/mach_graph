#version 330 core
layout(location = 0) in vec3 vertex;

out vec3 vTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
  vec4 WVP_Pos = projection * view * model * vec4(vertex, 1.0f); 
  gl_Position = WVP_Pos.xyww;

  vTexCoords = vertex;
}
