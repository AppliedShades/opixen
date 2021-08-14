#version 330 core
layout (location = 0) in vec4 aPos;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
uniform mat4 projection;

void main()
{
	gl_Position = projection * aPos;
	TexCoord = vec2(aTexCoord);
}