#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 aTex;

uniform float x;
uniform float y;
uniform float z;
uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

out vec2 texCoord;


void main()
{
	vec3 newPos = vec3(aPos.x + x, aPos.y + y, aPos.z + z);
	gl_Position = projection * view * transform * vec4(newPos, 1.0);

	texCoord = aTex;
}