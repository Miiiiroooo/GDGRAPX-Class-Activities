#version 330 core

uniform vec3 newColor;
out vec4 FragColor;

void main()
{
	FragColor = vec4(newColor.x, newColor.y, newColor.z, 1.f);
}