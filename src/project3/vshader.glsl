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

void main()
{
	texCoord = vTexCoord;
	gl_Position = projection_matrix * model_view_matrix * ctm *vPosition;
	color = vColor;
	normal = ctm * vNormal;
	position = ctm *vPosition;
}
