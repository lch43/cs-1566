#version 120

attribute vec4 vPosition;
attribute vec4 vColor;
attribute vec4 vNormal;
attribute vec2 vTexCoord;
varying vec2 texCoord;
varying vec4 color;
varying vec4 normal;
varying vec4 position;

uniform mat4 model_view_matrix;
uniform mat4 projection_matrix;
uniform mat4 ctm;
uniform vec4 light_position;
uniform int is_shadow;

void main()
{
	texCoord = vTexCoord;
	normal = ctm * vNormal;
	position = ctm *vPosition;
	if (is_shadow == 0)
	{
		gl_Position = projection_matrix * model_view_matrix * ctm *vPosition;
		color = vColor;
	}
	else
	{
		//Create shadow
		float x = light_position.x - light_position.y*(light_position.x-position.x)/(light_position.y-position.y);
		float z = light_position.z - light_position.y*(light_position.z-position.z)/(light_position.y-position.y);
		gl_Position = projection_matrix * model_view_matrix * vec4(x,0.001,z,1);
		color = vec4(0,0,0,1);
	}
	
}
