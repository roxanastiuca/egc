#version 330

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

//uniform vec3 object_color;

// Output value to fragment shader
out vec2 texcoord;
out vec3 world_position;
out vec3 world_normal;

void main()
{
	// Pass texture_coord to Fragment Shader
	texcoord = vec2(v_texture_coord.x, v_texture_coord.y);
	world_position = vec3(Model * vec4(v_position, 1));
	world_normal = normalize (vec3(Model * vec4(v_normal, 0)));

	gl_Position = Projection * View * Model * vec4(v_position, 1.0);
}
