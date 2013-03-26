#version 130
precision highp float;

uniform mat4 World;
uniform mat4 View;
uniform mat4 Projection;

in vec4 InPosition;
in vec3 InSurfacePos;

out vec3 SurfacePos;
out vec3 WorldPos;

void main()
{
	SurfacePos = InSurfacePos;
	gl_Position = (Projection * View * World) * InPosition;
	WorldPos = InPosition.xyz;
}  