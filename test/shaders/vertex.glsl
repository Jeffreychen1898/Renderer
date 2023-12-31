#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform mat4 u_projection;
uniform ivec3 u_sumVector;

out vec3 vColor;
void main()
{
	vec3 position = u_sumVector + aPos;
	gl_Position = u_projection * vec4(position, 1.0);
	vColor = aColor;
}
