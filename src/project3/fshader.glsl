#version 120

varying vec2 texCoord;
varying vec4 color;
varying vec4 normal;
varying vec4 position;

uniform sampler2D texture;
uniform int use_texture;
uniform int is_shadow;
uniform vec4 light_position;
uniform vec4 eye_position;

void main()
{
	vec4 lightSource;
	lightSource.x = light_position.x;
	lightSource.y = light_position.y;
	lightSource.z = light_position.z;
	lightSource.w = 1.0;
	vec4 the_color = color;
	if (use_texture == 1 && is_shadow == 0)
	{
		the_color = texture2D(texture, texCoord);
	}

	float distance = sqrt( (lightSource.x - position.x)*(lightSource.x - position.x) + (lightSource.y - position.y)*(lightSource.y - position.y) + (lightSource.z - position.z)*(lightSource.z - position.z));

	vec4 diffuse = 1.2 * max(dot(normalize(lightSource-position), normalize(normal)), 0) * normalize(lightSource-position) / (0 + .2 * distance + .5 * distance * distance);
	diffuse = vec4(diffuse.y, diffuse.y, diffuse.y, 0.0);
	vec4 specular;

	gl_FragColor = (the_color * 0.2) + the_color * diffuse;
}