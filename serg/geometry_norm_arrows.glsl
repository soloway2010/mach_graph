#version 330 core

layout(triangles) in;
layout(line_strip, max_vertices = 5) out;

in vec3 vNormal[];

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vec3 P = gl_in[0].gl_Position.xyz;
	vec3 N = vNormal[0];

	gl_Position = projection * view * model * vec4(P, 1.0f);
	EmitVertex();

	gl_Position = projection * view * model * vec4(P + N, 1.0f);
	EmitVertex();

	vec3 sideR = P + 3 * N / 4;
	sideR.x += length(N) / 8;
	gl_Position = projection * view * model * vec4(sideR, 1.0f);
	EmitVertex();

	gl_Position = projection * view * model * vec4(P + N, 1.0f);
	EmitVertex();

	vec3 sideL = P + 3 * N / 4;
	sideL.x -= length(N) / 8;
	gl_Position = projection * view * model * vec4(sideL, 1.0f);
	EmitVertex();

	EndPrimitive();
}
