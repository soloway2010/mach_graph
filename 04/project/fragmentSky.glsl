#version 330 core
in vec3 vFragPosition;
in vec2 vTexCoords;
in vec3 vNormal;

out vec4 color;

uniform sampler2D sampler;
uniform vec3 lightSource;

void main()
{
  vec3 lightDir = normalize(lightSource - vFragPosition);

  float dist = length(lightSource - vFragPosition);

  vec3 col; 

  col = texture(sampler, vTexCoords).rgb;
  
  vec3 ambient = vec3(0.1f, 0.1f, 0.1f) * col;
  float cosTheta = clamp(dot(vNormal, lightDir), 0, 1);

  color = vec4(200.0f * col * cosTheta / (dist*dist) + ambient, 1.0f);
}
