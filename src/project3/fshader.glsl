#version 120

varying vec2 texCoord;
varying vec4 color;
varying vec4 normal;
varying vec4 position;

uniform sampler2D texture;
uniform int use_texture;
uniform int is_shadow;
uniform float lightX;
uniform float lightY;
uniform float lightZ;

void main()
{
	vec4 lightSource;
	lightSource.x = lightX;
	lightSource.y = lightY;
	lightSource.z = lightZ;
	lightSource.w = 1.0;
	vec4 the_color = color;
	if (use_texture == 1 && is_shadow == 0)
	{
		the_color = texture2D(texture, texCoord);
	}
	
	//diffuse = the_color * something;
	//specular = vec4(1.0, 1.0, 1.0, 1.0) * something_else;

	float distance = sqrt( (lightSource.x - position.x)*(lightSource.x - position.x) + (lightSource.y - position.y)*(lightSource.y - position.y) + (lightSource.z - position.z)*(lightSource.z - position.z));

	vec4 diffuse = 1.2 * max(dot(normalize(lightSource-position), normalize(normal)), 0) * normalize(lightSource-position) / (0 + .7 * distance + .1 * distance * distance);

	gl_FragColor = (the_color * 0.2) + the_color * diffuse.y;//+ diffuse + specular;
}