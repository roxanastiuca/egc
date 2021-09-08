#version 330

uniform sampler2D texture;

// From vertex shader
in vec2 texcoord;
in vec3 world_position;
in vec3 world_normal;

// Uniforms for light properties
uniform int lights_count;

uniform vec3 light_position1;
uniform vec3 light_color1;
uniform int is_spot1;

uniform vec3 light_position2;
uniform vec3 light_color2;
uniform int is_spot2;

uniform vec3 light_position3;
uniform vec3 light_color3;
uniform int is_spot3;

uniform vec3 light_position4;
uniform vec3 light_color4;
uniform int is_spot4;

uniform vec3 light_position5;
uniform vec3 light_color5;
uniform int is_spot5;

uniform vec3 light_direction;
uniform vec3 eye_position;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

layout(location = 0) out vec4 out_color;

void main()
{
	vec3 color = texture2D(texture, texcoord).xyz;

	vec3 light;

	// TODO: define ambient light component
	float ambient_light = material_kd * 0.25;

	// N,V (don't depend on light position)
	vec3 N = normalize (world_normal);
	vec3 V = normalize (eye_position - world_position);

	for (int i = 0; i < lights_count; i++) {
		vec3 light_position;
		int is_spot;
		vec3 light_color;

		if (i == 0) { light_position = light_position1; is_spot = is_spot1; light_color = light_color1; }
		else if (i == 1) { light_position = light_position2; is_spot = is_spot2; light_color = light_color2; }
		else if (i == 2) { light_position = light_position3; is_spot = is_spot3; light_color = light_color3; }
		else if (i == 3) {light_position = light_position4; is_spot = is_spot4; light_color = light_color4; }
		else { light_position = light_position5; is_spot = is_spot5; light_color = light_color5; }

		// L,V,H
		vec3 L = normalize (light_position - world_position);
		vec3 H = normalize (L + V);

		// TODO: compute diffuse light component
		float diffuse_light = material_kd * max (dot(N, L), 0);

		// phd: light attenuation with regard to distance
		float dilution_factor = 100 / (1 + pow(distance(light_position, world_position), 2)); // +1 pt ca numitorul sa nu fie <1

		// TODO: compute specular light component
		float specular_light = 0;

		if (diffuse_light > 0)
		{
			// specular_light = material_ks * pow (max(dot(V, L), 0), material_shininess); // in varianta de baza
			specular_light = material_ks * pow (max(dot(N, H), 0), material_shininess); // in varianta folosind vectorul median
		}

		if (is_spot == 1) {
			float cut_off = radians(30);
			float spot_light = dot(-L, light_direction);
			float spot_light_limit = cos(cut_off);

			if (spot_light > spot_light_limit) {
				float linear_att = (spot_light - spot_light_limit) / (1 - spot_light_limit);
				float light_att_factor = pow(linear_att, 2);

				light += vec3(light_color * (light_att_factor * dilution_factor) * (specular_light + diffuse_light));
			}
		} else {
			light += vec3(light_color * dilution_factor * (specular_light + diffuse_light));
		}
	}

	light += vec3(ambient_light);

	out_color = vec4(color * light, 1);
}