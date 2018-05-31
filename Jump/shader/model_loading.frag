#version 330 core

in vec2 TexCoords;
in vec3 TexColor;

out vec4 color;

uniform sampler2D texture_diffuse1;
uniform
void main()
{    
    color = vec4(texture(texture_diffuse1, TexCoords));
	color.a
	//color=vec4(1.0f,1.0f,0.0f,1.0f);
	//color=vec4(TexColor,1.0f);
}
