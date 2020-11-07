#version 120

varying vec2 texCoord;
varying vec4 color;

uniform sampler2D texture;
uniform int use_texture;

void main()
{
	
	//gl_FragColor = vec4(texCoord.x, texCoord.y, 0.0, 1.0);
	//gl_FragColor = texture2D(texture, texCoord);
	//gl_FragColor = color;

	vec4 the_color = color;

	if (use_texture == 1)
	{
		the_color = texture2D(texture, texCoord);
	}


	/*if (the_color.w != 1.0) //Last resort.
	{
		the_color = texture2D(texture, texCoord);
	}*/
	gl_FragColor = the_color;
}