#version 120

varying vec2 texCoord;
varying vec4 color;
varying vec4 normal;
varying vec4 position;

uniform sampler2D texture;
uniform int use_texture;
uniform int is_shadow;
uniform int is_light;
uniform vec4 light_position;
uniform vec4 eye_position;

void main()
{
	vec4 the_color = color;
	if (use_texture == 1 && is_shadow == 0)
	{
		the_color = texture2D(texture, texCoord);
	}

	//Lights distance from the vertex
	float distance = sqrt( (light_position.x - position.x)*(light_position.x - position.x) + (light_position.y - position.y)*(light_position.y - position.y) + (light_position.z - position.z)*(light_position.z - position.z));

	//Diffuse
	vec4 diffuse = .9 * max(dot(normalize(light_position-position), normalize(normal)), 0) * normalize(light_position-position) / (0 + .3 * distance + .03 * distance * distance);
	diffuse = vec4(abs(diffuse.y), abs(diffuse.y), abs(diffuse.y), 0.0);

	//Specular has not been implemented yet.

	gl_FragColor = (the_color * 0.2) + the_color * diffuse;
	if (is_light == 1)
	{
		gl_FragColor = vec4(1.0,1.0,1.0,1.0);
	}
}