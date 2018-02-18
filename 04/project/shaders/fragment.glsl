#version 330 core
in vec3 vNormal;
in vec3 vFragPosition;
in vec2 vTexCoords;

out vec4 color;

uniform sampler2D sampler0;
uniform sampler2D sampler1;
uniform int NORMAL_FLAG;
uniform vec3 lightSource;

uniform float size;

void main()
{
  vec3 lightDir = normalize(lightSource - vFragPosition);

  float dist = length(lightSource - vFragPosition);

  vec3 col; 

  if(NORMAL_FLAG == 1)
    col = vNormal;
  else if(vFragPosition.y <= 0.01f * size)
    col = texture(sampler0, vTexCoords).rgb;
  else
    col = texture(sampler1, vTexCoords).rgb;
  
  vec3 ambient = vec3(0.1f, 0.1f, 0.1f) * col;
  float cosTheta = clamp(dot(vNormal, lightDir), 0, 1);

  color = vec4(10.0f * size * col * cosTheta / (dist*dist) + ambient, 1.0f);
}
