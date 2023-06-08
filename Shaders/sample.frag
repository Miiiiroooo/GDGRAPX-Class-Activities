#version 330 core

uniform vec3 newColor;
uniform sampler2D tex0;

out vec4 FragColor;
in vec2 texCoord;

void main()
{
	//FragColor = vec4(newColor.x, newColor.y, newColor.z, 1.f);

	FragColor = texture(tex0, texCoord);
}