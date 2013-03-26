#version 130
precision highp float;

in vec3 InPosition;
in vec2 InTextureCoordinate;

out vec2 TextureCoordinate;

void main()
{
	TextureCoordinate = InTextureCoordinate;
	gl_Position = vec4(InPosition.xy,0,1);
}  