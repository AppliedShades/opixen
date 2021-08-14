#version 330 core
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec4 aColor;

out vec4 vertColor;
uniform mat4 projection;

void main()
{
	gl_Position = projection * aPos;
	vertColor = aColor;
}