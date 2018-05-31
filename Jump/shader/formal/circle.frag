#version 330 core

in vec2 TexCoords;
in vec3 TexColor;

out vec4 color;

uniform sampler2D texture_diffuse1;
uniform float transparency=1.0f;
void main()
{    
    color = vec4(texture(texture_diffuse1, TexCoords));
	color.r=192.0f/256.0f;
	color.g=color.r;
	color.b=color.r;
	color.a=transparency;
	//color=vec4(1.0f,1.0f,0.0f,1.0f);
	//color=vec4(TexColor,1.0f);
}
