#version 330

// get value from vertex shader
in vec3 color;

layout(location = 0) out vec4 out_color;

void main()
{
	out_color = vec4(color, 1);
}