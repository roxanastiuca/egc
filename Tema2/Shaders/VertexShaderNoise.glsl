#version 330

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform vec3 object_color;
uniform float noise_time;

// Output value to fragment shader
out vec3 color;

// Function for modulo 289 for a vec3
vec3 mod289(vec3 x)
{
	return x - floor(x * (1.0 / 289.0)) * 289.0;
}

// Function for modulo 289 for a vec4
vec4 mod289(vec4 x)
{
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

// (34 x^2 + x) % 289
vec4 permute(vec4 x)
{
	return mod289(((x*34.0)+1.0)*x);
}

// Taylor expansion for the inverse square root
vec4 taylorInvSqrt(vec4 r)
{
	return 1.79284291400159 - 0.85373472095314 * r;
}

// Fade function for Perlin noise is 6t^5 - 15t^4 + 10t^3
vec3 fade(vec3 t) {
	return t*t*t* (t* (t*6.0 - 15.0) + 10.0);
}

// Classic Perlin noise
float cnoise(vec3 normal)
{
	// Integer part:
	vec3 normal_floor = floor(normal);
	vec3 p1_i = mod289(normal_floor);
	vec3 p2_i = mod289(normal_floor + vec3(1));

	// Fractional part:
	vec3 normal_fract = fract(normal);
	vec3 p1_f = normal_fract;
	vec3 p2_f = normal_fract - vec3(1);

	vec4 x_i = vec4(p1_i.x, p2_i.x, p1_i.x, p2_i.x);
	vec4 y_i = vec4(p1_i.yy, p2_i.yy);
	vec4 z1_i = p1_i.zzzz;
	vec4 z2_i = p2_i.zzzz;

	vec4 xy_i = permute(permute(x_i) + y_i);
	vec4 xy1_i = permute(xy_i + z1_i);
	vec4 xy2_i = permute(xy_i + z2_i);

	// Computations for P1 (normal):
	vec4 gx1 = fract(xy1_i * (1.0 / 7.0));
	vec4 gy1 = fract(floor(xy1_i * (1.0 / 7.0)) * (1.0 / 7.0)) - 0.5;
	vec4 gz1 = vec4(0.5) - abs(gx1) - abs(gy1);
	vec4 sz1 = step(gz1, vec4(0.0));
	gx1 -= sz1 * (step(0.0, gx1) - 0.5);
	gy1 -= sz1 * (step(0.0, gy1) - 0.5);

	vec3 g1_x = vec3(gx1.x, gy1.x, gz1.x);
	vec3 g1_y = vec3(gx1.y, gy1.y, gz1.y);
	vec3 g1_z = vec3(gx1.z, gy1.z, gz1.z);
	vec3 g1_w = vec3(gx1.w, gy1.w, gz1.w);

	vec4 norm1 = taylorInvSqrt(vec4(dot(g1_x, g1_x), dot(g1_y, g1_y), dot(g1_z, g1_z), dot(g1_w, g1_w)));
	g1_x *= norm1.x;
	g1_z *= norm1.y;
	g1_y *= norm1.z;
	g1_w *= norm1.w;

	float n1_x = dot(g1_x, p1_f);
	float n1_y = dot(g1_y, vec3(p2_f.x, p1_f.yz));
	float n1_z = dot(g1_z, vec3(p1_f.x, p2_f.y, p1_f.z));
	float n1_w = dot(g1_w, vec3(p2_f.xy, p1_f.z));

	// Computations for P2 (normal+/-vec3(1)) (similar to above):
	vec4 gx2 = fract(xy2_i * (1.0 / 7.0));
	vec4 gy2 = fract(floor(xy2_i * (1.0 / 7.0)) * (1.0 / 7.0)) - 0.5;
	vec4 gz2 = vec4(0.5) - abs(gx2) - abs(gy2);
	vec4 sz2 = step(gz2, vec4(0.0));
	gx2 -= sz2 * (step(0.0, gx2) - 0.5);
	gy2 -= sz2 * (step(0.0, gy2) - 0.5);

	vec3 g2_x = vec3(gx2.x, gy2.x, gz2.x);
	vec3 g2_y = vec3(gx2.y, gy2.y, gz2.y);
	vec3 g2_z = vec3(gx2.z, gy2.z, gz2.z);
	vec3 g2_w = vec3(gx2.w, gy2.w, gz2.w);

	vec4 norm2 = taylorInvSqrt(vec4(dot(g2_x, g2_x), dot(g2_y, g2_y), dot(g2_z, g2_z), dot(g2_w, g2_w)));
	g2_x *= norm2.x;
	g2_z *= norm2.y;
	g2_y *= norm2.z;
	g2_w *= norm2.w;

	float n2_x = dot(g2_x, vec3(p1_f.xy, p2_f.z));
	float n2_y = dot(g2_y, vec3(p2_f.x, p1_f.y, p2_f.z));
	float n2_z = dot(g2_z, vec3(p1_f.x, p1_f.yz));
	float n2_w = dot(g2_w, p2_f);

	vec3 fade_xyz = fade(p1_f);
	vec4 n_z = mix(vec4(n1_x, n1_y, n1_z, n1_w), vec4(n2_x, n2_y, n2_z, n2_w), fade_xyz.z);
	vec2 n_yz = mix(n_z.xy, n_z.zw, fade_xyz.y);
	float n_xyz = mix(n_yz.x, n_yz.y, fade_xyz.x); 
	return 1.1 * n_xyz;
}

void main()
{
	// Send color to fragment shader.
	color = object_color;

	// New position: (distorsion along the normal which oscilates in time)
	vec3 new_position = v_position + v_normal * sin(noise_time) * cnoise(v_normal);

	gl_Position = Projection * View * Model * vec4(new_position, 1.0);
}
