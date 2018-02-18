#version 330 core
in vec3 vNormal;
in vec3 vFragPosition;
in vec2 vTexCoords;
in vec3 pos_eye;
in vec3 n_eye;

out vec4 color;

uniform mat4 view;
uniform sampler2D texture_water;
uniform samplerCube cube_texture;
uniform vec3 light_src;

void main()
{
  vec3 incident_eye = normalize(pos_eye);
  vec3 normal = normalize(n_eye);

  vec3 reflected = reflect(incident_eye, normal);
  reflected = vec3(inverse(view) * vec4(reflected, 0.0f)); 

  vec3 col = texture(cube_texture, reflected).rgb;
  col = mix(col, texture(texture_water, vTexCoords).rgb, 0.5f);
  
  vec3 ambient = vec3(0.1f, 0.1f, 0.1f) * col;
  vec3 lightDir = normalize(light_src - vFragPosition);
  float cosTheta = clamp(dot(vNormal, lightDir), 0, 1);

  color = vec4(col * cosTheta + ambient, 0.75f);
}
