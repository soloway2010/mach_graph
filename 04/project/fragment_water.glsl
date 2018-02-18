#version 330 core
in vec3 vNormal;
in vec3 vFragPosition;
in vec2 vTexCoords;
in vec3 pos_eye;
in vec3 n_eye;

out vec4 color;

uniform mat4 view;
uniform sampler2D tex;
uniform samplerCube texCube;
uniform vec3 lightSource;

uniform float size;

void main()
{
  vec3 incident_eye = normalize(pos_eye);
  vec3 normal = normalize(n_eye);

  vec3 reflected = reflect(incident_eye, normal);
  reflected = vec3(inverse(view) * vec4(reflected, 0.0f)); 

  vec3 col = texture(texCube, reflected).rgb;
  col *= texture(tex, vTexCoords).rgb;
  
  vec3 ambient = vec3(0.1f, 0.1f, 0.1f) * col;
  vec3 lightDir = normalize(lightSource - vFragPosition);
  float dist = length(lightSource - vFragPosition);  
  float cosTheta = clamp(dot(vNormal, lightDir), 0, 1);

  color = vec4(5.0f * size * col * cosTheta / (dist*dist) + ambient, 0.8f);
}
