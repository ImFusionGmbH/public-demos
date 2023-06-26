#version 430
layout(location = 0) in vec4 in_Position;
layout(location = 1) in vec4 in_Tangent;

out vec4 vert_Position;
out vec4 vert_Tangent;

uniform mat4 u_modelMatrix;

void main()
{
	vert_Position = u_modelMatrix * vec4(in_Position.xyz, 1.0);
	vert_Position.w = in_Position.w;
	vert_Tangent = vec4(normalize(in_Tangent.xyz), in_Tangent.w);
}
