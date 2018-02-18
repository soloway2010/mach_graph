#version 330 core
in vec3 vNormal;
in vec3 vFragPosition;
in vec2 vTexCoords;

out vec4 color;

uniform sampler2D texture_sand;
uniform sampler2D texture_grass;
uniform int norm;
uniform vec3 light_src;

void main()
{
  vec3 lightDir = normalize(light_src - vFragPosition);

  vec3 col; 

  if(norm == 1)
    col = vNormal;
  else if(vFragPosition.y <= 0.7f)
    col = texture(texture_sand, vTexCoords).rgb;
  else if(vFragPosition.y >= 1.7f)
    col = texture(texture_grass, vTexCoords).rgb;
  else{
    vec3 col1 = texture(texture_sand, vTexCoords).rgb;
    vec3 col2 = texture(texture_grass, vTexCoords).rgb;
    col = mix(col1, col2, (vFragPosition.y - 0.7f)); 
  }
  
  vec3 ambient = vec3(0.1f, 0.1f, 0.1f) * col;
  float cosTheta = clamp(dot(vNormal, lightDir), 0, 1);

  color = vec4(col * cosTheta + ambient, 1.0f);
}
